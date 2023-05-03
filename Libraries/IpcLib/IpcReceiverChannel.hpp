#ifndef IPCLIB_IPCRECEIVERCHANNEL_HPP
#define IPCLIB_IPCRECEIVERCHANNEL_HPP

#include <string>
#include <vector>
#include <cinttypes>
#include <memory>

#include "Utils/String.hpp"
#include "Utils/Utils.hpp"

#include "IpcMetadata.hpp"
#include "IpcResult.hpp"
#include "IpcBuffer.hpp"

namespace IpcLib
{
    class IpcReceiverChannel
    {
        bool is_open = false;
        protected:

        virtual IpcResult Create() = 0;
        virtual IpcResult Destroy() = 0;
        
        public:
        IpcMtdtPtr ReceiverMetadata;
        IpcResult Open(const IpcMtdtPtr& receiverMetadata);
        virtual IpcResult Receive(uint32_t millisecondsTimeout, IpcBufferPtr& outData) = 0;        
        IpcResult Close();
        virtual IpcChannelType GetChannelType() = 0;
        bool IsOpen();
    };

    using IpcReceiverPtr = std::shared_ptr<IpcReceiverChannel>;
}


#endif