#include "IpcLib/IpcThread.hpp"

bool run = true;
IpcLib::IpcThread* ipc_thread = nullptr;
std::string name;
std::string testType;

void test_bandwidth()
{
   while(run)
   {
      auto result = ipc_thread->SendTo("ipc_test_receiver", "1234567890");
      if(!IPC_IS_OK(result))
      {
         std::string msg = "Message send result: "+std::to_string((int)result.Result)+", status: "+std::to_string((int)result.Status);         
         throw std::runtime_error(msg);
      }      
   }   
   std::cout<<"Bandwidth test ended"<<std::endl;
}


void test_delay()
{   
   bool msg_received = false;
   ipc_thread->OnMessageReceived = [&msg_received](IpcLib::IpcBufferPtr& msg){
      msg_received = true;
   };   

   for(int i=0; i<10 && run; i++)
   {
      auto startTime = Utils::Time::GetCurrentMillisecondTimestamp();
      msg_received = false;
      auto result = ipc_thread->SendTo("ipc_test_receiver", "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
      if(!IPC_IS_OK(result))
      {
         std::string message = "Message send result: "+std::to_string((int)result.Result)+", status: "+std::to_string((int)result.Status); 
         throw std::runtime_error(message);
      }
      while(!msg_received && run);
      if(msg_received)
         std::cout<<"delay= "<<Utils::Time::GetCurrentMillisecondTimestamp() - startTime<<"ms" << std::endl;
   }
   std::cout<<"delay test ended"<<std::endl;
}

void test_preempt()
{
   std::cout<<"Generating message"<<std::endl;
   std::string message = "Hello from "+name+"\n";
   for(uint32_t i=0; i<3 * 1024 * 1024; i++)
      message+='!'+i%126;
   std::cout<<"Starting"<<std::endl;
   while(run)
   {
      auto result = ipc_thread->SendTo("ipc_test_receiver", message);
      if(!IPC_IS_OK(result))
      {
         std::string msg = "Message send result: "+std::to_string((int)result.Result)+", status: "+std::to_string((int)result.Status);         
         throw std::runtime_error(msg);
      }      
   }   
   std::cout<<"preempt test ended"<<std::endl;
}

int main(int argc, char *argv[])
{
   IpcLib::IpcResult result;
   std::thread test;

   if(argc!=2)
      goto throw_error;      

   name = "ipc_test_sender";
   testType = std::string(argv[1]);

   if(testType!="delay" && testType!="bandwidth" && testType!="preempt")
      goto throw_error;

   if(testType=="preempt")
      name+="_"+std::to_string(Utils::Time::GetCurrentMillisecondTimestamp());

   ipc_thread = new IpcLib::IpcThread(getenv("HOME"));
   result = ipc_thread->Start(name, IpcLib::IpcChannelType::SharedMemory);
   if(testType == "delay")
      test = std::thread(test_delay);
   else if(testType == "bandwidth")
      test = std::thread(test_bandwidth);
   else
      test = std::thread(test_preempt);

   std::cin.get();   
   run = false;
   test.join();
   result = ipc_thread->Stop();
   delete ipc_thread;

   return EXIT_SUCCESS;
throw_error:
   throw std::runtime_error("Required 1 argument: test type: {delay,bandwidth,preempt}");
   return EXIT_FAILURE;
}