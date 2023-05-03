#include "IpcThread.hpp"

namespace IpcLib
{
    void IpcThread::IpcThreadRun(IpcThread* instance)
    {
        instance->IpcThreadRunInternal();
    }

    void IpcThread::IpcThreadRunInternal()
    {
        auto openResult = ChannelManager.RecoverReceiver();
        if(!IPC_IS_OK(openResult))
            throw std::runtime_error("Cannot open ipc receiver: result:"+std::to_string((int)openResult.Result)+", status: "+std::to_string((int)openResult.Status));

        while(Running)
        {
            IpcBufferPtr msg;
            auto receiveResult = ChannelManager.ReceiveBytes(2500, msg);
            switch(receiveResult.Result)
            {
                case IpcOperationResult::ChannelDamaged:
                {
                    auto recover = ChannelManager.RecoverReceiver();
                    if(!IPC_IS_OK(recover))
                        throw std::runtime_error("Cannot recover receiver after detection of damaged channel");                
                    break;
                }

                case IpcOperationResult::Fail:
                case IpcOperationResult::FatalError:
                {
                    std::cerr<<"Result: "<<(int)receiveResult.Result<<", status: "<<(int)receiveResult.Status<<std::endl;
                    break;
                }
                case IpcOperationResult::TimedOut:
                {
                    break;
                }
                case IpcOperationResult::Ok:
                {                    
                    if(OnMessageReceived != nullptr)                    
                        OnMessageReceived(msg);                    
                    break;
                }
                default:
                {
                    std::cerr<<"unexpected receiving result: "<<(int)receiveResult.Result<<", status: "<<(int)receiveResult.Status<<std::endl;
                    break;
                }
            }
        }

        auto closeResult = ChannelManager.CloseReceiver();
        if(!IPC_IS_OK(closeResult))
            throw std::runtime_error("Cannot close receiver, result:"+std::to_string((int)closeResult.Result)+", status: "+std::to_string((int)closeResult.Status));
    }

    IpcThread::IpcThread(const std::string& ipcRootDir) : ChannelManager(ipcRootDir)
    {

    }

    IpcResult IpcThread::Start(const std::string& receiverName, IpcChannelType receiverType)
    {
        if(Running)
            IPC_RET_FAIL_C(IpcOperationStatus::IpcThread_AlreadyStarted)
        ChannelManager.ReceiverName = receiverName;
        ChannelManager.ReceiverType = receiverType;
        Running = true;
        ipc_thread = std::thread(IpcThreadRun, this);

        while (!ChannelManager.IsReceiverReady())        
            std::this_thread::sleep_for(std::chrono::milliseconds(100));      

        IPC_RET_OK
    }

    IpcResult IpcThread::Stop()
    {
        if(!Running)
            IPC_RET_FAIL_C(IpcOperationStatus::IpcThread_NotRunning)            
        
        Running = false;
        if(ipc_thread.joinable())
            ipc_thread.join();
        IPC_RET_OK
    }

    IpcResult IpcThread::SendTo(const std::string& targetName, const std::vector<uint8_t>& data)
    {
        auto ret = ChannelManager.SendBytes(targetName, data);
        return ret;
    }

    IpcResult IpcThread::SendTo(const std::string& targetName, const std::string& message)
    {
        auto ret = ChannelManager.SendString(targetName, message);
        return ret;
    }



}