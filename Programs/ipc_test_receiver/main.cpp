#include "IpcLib/IpcThread.hpp"


IpcLib::IpcThread *ipc_thread = nullptr;
std::string testType;

int64_t last_log = 0;

uint32_t messagesReceived = 0;
uint32_t bytesReceived = 0;

uint32_t msgsDiff = 0;
uint32_t bytesDiff = 0;

void test_bandwidth()
{
    ipc_thread->OnMessageReceived = [](IpcLib::IpcBufferPtr &msg)
    {
        auto now = Utils::Time::GetCurrentMillisecondTimestamp();
        messagesReceived++;
        bytesReceived += msg->Size;
        if (now - last_log >= 1000)
        {
            uint32_t m = messagesReceived - msgsDiff;
            uint32_t b = bytesReceived - bytesDiff;
            last_log = now;
            std::cout << "Msgs: " << messagesReceived << "| MiB: " << bytesReceived / 1024.0 / 1024.0
                      << "| Msgs/sec: " << m << "| MiB/s: " << b / 1024.0 / 1024.0 << std::endl;
            msgsDiff = messagesReceived;
            bytesDiff = bytesReceived;
        }
    };
    std::cout<<"bandwidth test started"<<std::endl;
}

void test_delay()
{
    ipc_thread->OnMessageReceived = [](IpcLib::IpcBufferPtr &msg)
    {
        ipc_thread->SendTo("ipc_test_sender", "echo");
    };
    std::cout<<"delay test started"<<std::endl;
}

void test_preempt()
{
    ipc_thread->OnMessageReceived = [](IpcLib::IpcBufferPtr &msg)
    {
        std::string str = std::string((char*)msg->Buffer, 40);
        std::cout<<"Message received: ["+str+"]"<<std::endl;;
    };
    std::cout<<"preempt test started"<<std::endl;
}

int main(int argc, char *argv[])
{
    IpcLib::IpcResult result;
    std::thread test;
    
    if(argc!=2)    
        goto throw_error;

    testType = std::string(argv[1]);

    if(testType!="bandwidth" && testType!="delay" && testType!="preempt")
        goto throw_error;
    
    ipc_thread = new IpcLib::IpcThread(getenv("HOME"));
    result = ipc_thread->Start("ipc_test_receiver", IpcLib::IpcChannelType::SharedMemory);

    if(testType == "delay")
        test = std::thread(test_delay);
    else if(testType == "bandwidth")
        test = std::thread(test_bandwidth);
    else
        test = std::thread(test_preempt);

    std::cin.get();
    test.join();
    result = ipc_thread->Stop();
    delete ipc_thread;

    return EXIT_SUCCESS;
throw_error:
    throw std::runtime_error("Required 1 argument: test type: {bandwidth, delay, preempt}");    
    return EXIT_FAILURE;
}