#ifndef IPCLIB_IPCMETADATAMANAGER_HPP
#define IPCLIB_IPCMETADATAMANAGER_HPP

#include <vector>
#include <list>
#include <memory>
#include <mutex>

#include "IpcMetadata.hpp"

#include "IpcSenderChannel.hpp"
#include "IpcReceiverChannel.hpp"
#include "IpcShmReceiver.hpp"
#include "IpcShmSender.hpp"
#include "IpcMessage.hpp"

namespace IpcLib
{
    class IpcManager
    {        
        std::string IpcRootPath;
        IpcReceiverPtr Receiver;

        IpcSenderPtr LastUsedSender;
        std::list<IpcSenderPtr> SenderCache;
        std::mutex mutex_receiver_resources;

        public:
        IpcChannelType ReceiverType;
        std::string ReceiverName;

        explicit IpcManager(const std::string& ipcRootPath);

        private:

        inline IpcMtdtVecPtr GetAllAssociatedMetadata(bool removeNotAssociated = true);

        inline IpcResult FindMetadataByOwnerName(const std::string& ownerName, IpcMtdtPtr& outIpcMetadata);

        inline IpcResult CacheFindTargetSenderByName(const std::string& targetName, IpcSenderPtr& outTargetSender);

        inline IpcResult CacheFindTargetMetadataByName(const std::string& targetName, IpcMtdtPtr& outTargetMetadata);

        IpcMtdtPtr CreateReceiverMetadata();

        IpcResult CreateReceiver();

        IpcResult CreateSender(const IpcMtdtPtr& targetMetadata, IpcSenderPtr& outSender);

        inline IpcResult GetTargetSender(const std::string& targetName, IpcSenderPtr& outSender);

        public:

        IpcResult RecoverReceiver();

        IpcResult CloseReceiver();

        IpcResult SendBytes(const std::string& targetName, const std::vector<uint8_t>& message);

        IpcResult SendString(const std::string& targetName, const std::string& message);

        IpcResult ReceiveBytes(uint32_t timeoutMs, IpcBufferPtr& outMsg);

        IpcSenderPtr GetSender(const std::string& targetName);

        bool IsReceiverReady();


    };
}

#endif