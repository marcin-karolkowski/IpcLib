#ifndef IPCLIB_IPCSHMSENDER_HPP
#define IPCLIB_IPCSHMSENDER_HPP

#include "Utils/Time.hpp"
#include "IpcShm.hpp"
#include "IpcSenderChannel.hpp"

namespace IpcLib
{

    class IpcShmSender : public IpcSenderChannel
    {
        ShmHelper Shm;
        uint32_t ReceiverNotRespondingLimit = 3;
        uint32_t ReceiverNotRespondingCounter = 0;

        IpcResult Create() final;

    public:
        IpcResult Send(uint32_t millisecondsTimeout, const std::vector<uint8_t> &data) final;
        IpcResult Close() final;
        IpcChannelType GetChannelType() final;
    };
    using IpcShmSndrPtr = std::shared_ptr<IpcShmSender>;
}

#endif