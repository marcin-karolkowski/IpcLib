#ifndef IPCLIB_IPCTHREAD_HPP
#define IPCLIB_IPCTHREAD_HPP

#include <thread>
#include <mutex>
#include <memory>
#include <queue>
#include <map>
#include <functional>

#include "IpcMessage.hpp"
#include "IpcManager.hpp"


namespace IpcLib
{
    class IpcThread
    {
        bool Running = false;
        std::thread ipc_thread;

        static void IpcThreadRun(IpcThread* instance);

        void IpcThreadRunInternal();

    public:
    
        IpcManager ChannelManager;

        std::function<void(IpcBufferPtr& msg)> OnMessageReceived = nullptr;

        explicit IpcThread(const std::string& ipcRootDir);

        IpcResult Start(const std::string& receiverName, IpcChannelType receiverType);

        IpcResult Stop();

        IpcResult SendTo(const std::string& targetName, const std::vector<uint8_t>& data);

        IpcResult SendTo(const std::string& targetName, const std::string& message);
    };
}

#endif