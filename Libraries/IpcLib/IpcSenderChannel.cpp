#include "IpcSenderChannel.hpp"

namespace IpcLib
{
    IpcResult IpcSenderChannel::Open(const IpcMtdtPtr& targetMetadata)
    {
        if(TargetMetadata!= nullptr)
            IPC_RET_FAIL_C(IpcOperationStatus::SenderChannel_SenderAlreadyOpenedForTarget)            

        auto result = targetMetadata->HasAssociation();
        if(IPC_IS_OK(result))
        {
            TargetMetadata = targetMetadata;
            result = Create();
        }
        return result;
    }

}