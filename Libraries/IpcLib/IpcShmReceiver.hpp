#ifndef IPCLIB_IPCSHMRECEIVER_HPP
#define IPCLIB_IPCSHMRECEIVER_HPP

#include "Utils/Time.hpp"
#include "IpcShm.hpp"
#include "IpcReceiverChannel.hpp"

namespace IpcLib
{
    class IpcShmReceiver : public IpcReceiverChannel
    {
        ShmHelper Shm;
        uint32_t NoDataReceivedLimit = 3;
        uint32_t NoDataReceivedCounter = 0;

        IpcResult Create() final;
        IpcResult Destroy() final;
    public:

        IpcResult Receive(uint32_t millisecondsTimeout, IpcBufferPtr &outData) final;
        IpcChannelType GetChannelType() final;
    };
    using IpcShmRcvPtr = std::shared_ptr<IpcShmReceiver>;
}

#endif