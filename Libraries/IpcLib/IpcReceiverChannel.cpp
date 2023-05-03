#include "IpcReceiverChannel.hpp"

namespace IpcLib
{
    IpcResult IpcReceiverChannel::Open(const IpcMtdtPtr& receiverMetadata)
    {
        if(ReceiverMetadata != nullptr)
            IPC_RET_FAIL_C(IpcOperationStatus::ReceiverChannel_ReceiverAlreadyOpenedForTarget)

        ReceiverMetadata = receiverMetadata;
        ReceiverMetadata->SaveToFile(); 
        auto ret = Create();        
        is_open = IPC_IS_OK(ret);
        return ret;
    }

    IpcResult IpcReceiverChannel::Close()
    {
        auto result = Destroy();
        if(IPC_IS_OK(result))
            ReceiverMetadata->RemoveFile();
        ReceiverMetadata = nullptr;
        is_open=false;
        return result;
    }

    bool IpcReceiverChannel::IsOpen()
    {
        return is_open;
    }

}