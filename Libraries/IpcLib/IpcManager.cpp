#include "IpcManager.hpp"

namespace IpcLib
{

    IpcManager::IpcManager(const std::string& ipcRootPath)
    {
        IpcRootPath = ipcRootPath;
        
    }    

    IpcMtdtVecPtr IpcManager::GetAllAssociatedMetadata(bool removeNotAssociated)
    {
        IpcMtdtVecPtr ret = std::make_shared<IpcMtdtVec>();
        std::filesystem::directory_iterator dir(IpcRootPath);
        for(auto& dirEntry : dir)
        {            
            auto path = dirEntry.path(); 
            auto ext = path.extension().string();
            if(ext != IpcMetadata::FileExtension)
                continue;                                
            auto metadata = IpcMetadata::FromIpcMetadataFile(dirEntry.path().string());
            if(IPC_IS_OK(metadata->HasAssociation()))
                ret->emplace_back(metadata);
            else if(removeNotAssociated)
                metadata->RemoveFile();
        }        
        return ret;        
    }

    IpcResult IpcManager::FindMetadataByOwnerName(const std::string& ownerName, IpcMtdtPtr& outIpcMetadata)
    {
        uint32_t found=0;        
        auto all = GetAllAssociatedMetadata();
        auto allSize = all->size();
        for(uint32_t i=0; i<allSize; i++)
        {
            auto& m = all->operator[](i);
            if(m->Name == ownerName)
            {
                outIpcMetadata = m;
                found++;
                if(found > 1)
                    IPC_RET_FAIL_C(IpcOperationStatus::Manager_MultipleMetadataFound)                    
            }
        }        
        if(found == 1)
            IPC_RET_OK
        IPC_RET_FAIL_C(IpcOperationStatus::Manager_MetadataWithOwnerNameNotFound)
    }

    IpcResult IpcManager::CacheFindTargetSenderByName(const std::string& targetName, IpcSenderPtr& outTargetSender)
    {
        if(LastUsedSender!= nullptr && LastUsedSender->TargetMetadata->Name == targetName)
        {
            outTargetSender = LastUsedSender;
            IPC_RET_OK
        }

        auto it = std::find_if(SenderCache.begin(), SenderCache.end(), [targetName](IpcSenderPtr& sndr)
        {
            return sndr->TargetMetadata->Name == targetName;
        });

        if(it != std::end(SenderCache))
        {
            //std::cout<<"Target ["<<targetName<<"] sender was found"<<std::endl;
            outTargetSender = (*it);
            LastUsedSender = outTargetSender;
            IPC_RET_OK
        }
        IPC_RET_FAIL_C(IpcOperationStatus::Manager_SenderNotFound)        
    }

    IpcResult IpcManager::CacheFindTargetMetadataByName(const std::string& targetName, IpcMtdtPtr& outTargetMetadata)
    {
        IpcSenderPtr ptr;
        auto sndrFound = CacheFindTargetSenderByName(targetName, ptr);
        if(IPC_IS_OK(sndrFound))
        {
            //std::cout<<"Metadata for target ["<<targetName<<"] found"<<std::endl;
            outTargetMetadata = ptr->TargetMetadata;
            return sndrFound;
        }
        IPC_RET_FAIL_C(IpcOperationStatus::Manager_Cache_MetadataWithOwnerNameNotFound)        
    }

    IpcMtdtPtr IpcManager::CreateReceiverMetadata()
    {
        auto ret = IpcMetadata::CreateForCurrentProcess(ReceiverName, IpcRootPath, ReceiverType);
        return ret;
    }
    
    IpcResult IpcManager::CreateReceiver()
    {
        switch(ReceiverType)
        {
            case IpcChannelType::SharedMemory:
            {
                auto rcv = std::make_shared<IpcShmReceiver>();
                auto ret = rcv->Open(CreateReceiverMetadata());
                if(IPC_IS_OK(ret))
                    Receiver = rcv;
                return ret;                
            }
            default:    
                IPC_RET_FAIL_C(IpcOperationStatus::Manager_UnimplementedCreationOfReceiverChannelType)                
        }
    }

    IpcResult IpcManager::CreateSender(const IpcMtdtPtr &targetMetadata, IpcSenderPtr &outSender)
    {
        switch (targetMetadata->ChannelType)
        {
        case IpcChannelType::SharedMemory:
        {
                outSender = std::make_shared<IpcShmSender>();
                return outSender->Open(targetMetadata);
        }
        default:
            IPC_RET_FAIL_C(IpcOperationStatus::Manager_UnimplementedCreationOfSenderChannelType)            
        }
    }

    IpcResult IpcManager::GetTargetSender(const std::string& targetName, IpcSenderPtr& outSender)
    {
        IpcResult ret = CacheFindTargetSenderByName(targetName, outSender);
        if(IPC_IS_OK(ret))
            return ret;

        IpcMtdtPtr targetMetadata;
        ret = FindMetadataByOwnerName(targetName, targetMetadata);

        if(IPC_IS_OK(ret))
        {
            ret = CreateSender(targetMetadata, outSender);
            if(IPC_IS_OK(ret))
                SenderCache.emplace_back(outSender);
        }
        return ret;
    }

    IpcResult IpcManager::RecoverReceiver()
    {
        mutex_receiver_resources.lock();
        IpcResult ret = {.Result = IpcOperationResult::Ok, .Status = IpcOperationStatus::Undefined};        
        if(Receiver)
            ret = Receiver->Close();
        if(IPC_IS_OK(ret))
            ret = CreateReceiver();    
        mutex_receiver_resources.unlock();        
        return ret;
    }

    IpcResult IpcManager::CloseReceiver()
    {
        mutex_receiver_resources.lock();
        IpcResult ret;
        IPC_MK_FAIL(ret);
        if(Receiver)
        {
            ret = Receiver->Close();
            Receiver = nullptr;
        }
        mutex_receiver_resources.unlock();
        return ret;
    }

    IpcResult IpcManager::SendBytes(const std::string& targetName, const std::vector<uint8_t>& message)
    {
        IpcSenderPtr sender;
        //std::vector<uint8_t> msg;
        IpcResult ret;

        mutex_receiver_resources.lock();

        if(Receiver == nullptr || !Receiver->IsOpen())        
        {
            IPC_MK_FAIL_C(ret, IpcOperationStatus::Manager_ReceiverHasNotBeenCreatedOrOpenedYet)            
            goto return_result;
        }

        ret = GetTargetSender(targetName, sender);
        if(!IPC_IS_OK(ret))
            goto return_result;;
        
        //msg = IpcMessage::Create(Receiver->ReceiverMetadata->Path, sender->TargetMetadata->Path, message)->Serialize();
        //ret = sender->Send(2500, msg);
        ret = sender->Send(2500, message);
        if(IPC_IS_DAMAGED(ret))
        {
            sender->Close();
            SenderCache.erase(std::remove_if(SenderCache.begin(), SenderCache.end(), [sender](IpcSenderPtr& sndr) {
                return sender==sndr;
            }));
        }

        return_result:
        mutex_receiver_resources.unlock();
        return ret;
    }

    IpcResult IpcManager::SendString(const std::string& targetName, const std::string& message)
    {
        auto msgSize = message.size();
        std::vector<uint8_t> data(msgSize);
        std::memcpy(data.data(), message.data(), msgSize);
        auto ret = SendBytes(targetName, data);
        return ret;
    }   

    IpcResult IpcManager::ReceiveBytes(uint32_t timeoutMs, IpcBufferPtr& outMsg)
    {         
        auto ret = Receiver->Receive(timeoutMs, outMsg);
        return ret;
    }  

    IpcSenderPtr IpcManager::GetSender(const std::string& targetName)
    {
        IpcSenderPtr ret;
        if(IPC_IS_OK(GetTargetSender(targetName, ret)))
            return ret;
        return nullptr;
    }

    bool IpcManager::IsReceiverReady()
    {
        bool ret = false;
        mutex_receiver_resources.lock();
        ret = Receiver != nullptr && Receiver->IsOpen();
        mutex_receiver_resources.unlock();
        return ret;
    }

}