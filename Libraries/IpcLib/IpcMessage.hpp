#ifndef IPCLIB_IPCMESSAGE_HPP
#define IPCLIB_IPCMESSAGE_HPP

#include <string>
#include <cinttypes>
#include <vector>
#include <memory>
#include <cstring>

#include "Utils/String.hpp"

namespace IpcLib
{
    class IpcMessage;

    using IpcMsgPtr = std::shared_ptr<IpcMessage>;
    using IpcMsgVec = std::vector<IpcMsgPtr>;
    using IpcMsgVecPtr = std::shared_ptr<IpcMsgVec>;
    using IpcRawMsg = std::vector<uint8_t>;
    using IpcRawMsgPtr = std::shared_ptr<IpcRawMsg>;

    class IpcMessage
    {
        public:
        uint32_t MessageSize;
        uint32_t HeaderSize;
        uint32_t DataSize;
        std::string SenderMetadataPath;
        std::string TargetMetadataPath;
        std::vector<uint8_t> Data;

        static IpcMsgPtr Parse(std::vector<uint8_t>& data, std::string& outErrorMessage);
        static IpcMsgPtr Create(const std::string& senderMetadataPath, const std::string& targetMetadataPath, const std::vector<uint8_t>& data);
        std::vector<uint8_t> Serialize();
    };
}


#endif