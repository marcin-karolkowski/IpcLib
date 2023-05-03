#ifndef IPCLIB_IPCSENDERCHANNEL_HPP
#define IPCLIB_IPCSENDERCHANNEL_HPP

#include <string>
#include <vector>
#include <cinttypes>
#include <memory>

#include "Utils/String.hpp"
#include "Utils/Utils.hpp"

#include "IpcMetadata.hpp"
#include "IpcResult.hpp"

namespace IpcLib
{
    class IpcSenderChannel
    {
        protected:        

        virtual IpcResult Create() = 0;
        
        public:
        IpcMtdtPtr TargetMetadata;
        IpcResult Open(const IpcMtdtPtr& targetMetadata);
        virtual IpcResult Send(uint32_t millisecondsTimeout, const std::vector<uint8_t>& data) = 0;   
        virtual IpcResult Close() = 0;
        virtual IpcChannelType GetChannelType() = 0;
    };
    using IpcSenderPtr = std::shared_ptr<IpcSenderChannel>;
}

#endif