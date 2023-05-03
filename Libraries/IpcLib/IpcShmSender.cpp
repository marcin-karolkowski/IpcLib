#include "IpcShmSender.hpp"

namespace IpcLib
{
    IpcResult IpcShmSender::Create()
    {
        ReceiverNotRespondingCounter = 0;
        return Shm.OpenAsSender(TargetMetadata->UniqueName);
    }

    IpcResult IpcShmSender::Send(uint32_t millisecondsTimeout, const std::vector<uint8_t> &data)
    {
        IpcResult ret;

        if (!Shm.Memory->IsInitialized)
            IPC_RET_FAIL_C(IpcOperationStatus::ShmSender_UninitializedShmMemory)            

        if (!Shm.SenderWaitForQueue(millisecondsTimeout))
            IPC_RET_TIMED_OUT_C(IpcOperationStatus::ShmSender_CannotPreemptShmMemory)            

        //auto timeStart = Utils::Time::GetCurrentMillisecondTimestamp();
        uint32_t dataSize = data.size();
        uint8_t* dataPos = (uint8_t*)data.data();
        uint8_t* dataEnd = dataPos + dataSize;

        bool initialWaitForSendPossibility = true;

        while (dataPos < dataEnd)
        {
            /* if whole operation time limit is needed
            if (Utils::Time::GetCurrentMillisecondTimestamp() - timeStart >= millisecondsTimeout)
            {
                IPC_MK_TIMED_OUT_C(ret, IpcOperationStatus::ShmSender_SendTimeExceeded)                
                goto return_result;
            }*/

            if (!Shm.SenderWaitForSendPossibility(millisecondsTimeout))
            {
                ReceiverNotRespondingCounter++;
                if (ReceiverNotRespondingCounter >= ReceiverNotRespondingLimit)
                {
                    if (!IPC_IS_OK(TargetMetadata->HasAssociation()))
                    {
                        IPC_MK_DAMAGED_C(ret, IpcOperationStatus::ShmSender_ReceiverNoLongerExistsForThisChannel)                        
                        goto return_result;
                    }
                    IPC_MK_FAIL_C(ret, IpcOperationStatus::ShmSender_ReceiverNotAnnouncingReadiness)                    
                    goto return_result;
                }
                IPC_MK_TIMED_OUT_C(ret, IpcOperationStatus::ShmSender_ReceiverWasNotReady)                
                goto return_result;
            }
            ReceiverNotRespondingCounter = 0;

            if (initialWaitForSendPossibility)
            {
                Shm.Memory->Memory.MessageSize = dataSize;
                Shm.Memory->Memory.BytesReceived = 0;
                initialWaitForSendPossibility = false;
            }

            uint32_t partSize = dataEnd - dataPos;
            partSize = partSize > sizeof(Shm.Memory->Memory.Buffer) ? sizeof(Shm.Memory->Memory.Buffer) : partSize;

            if (partSize == 0) // CANNOT EVER HAPPEN
            {
                IPC_MK_FATAL_C(ret, IpcOperationStatus::ShmSender_TriedToSend0Bytes)                
                goto return_result;
            }

            std::memcpy(Shm.Memory->Memory.Buffer, dataPos, partSize);
            dataPos += partSize;
            Shm.SenderSetDataIsAvailable();
        }
        IPC_MK_OK(ret)        
    return_result:
        Shm.SenderLeaveQueue();
        return ret;
    }

    IpcResult IpcShmSender::Close()
    {
        return Shm.Close(false);
    }

    IpcChannelType IpcShmSender::GetChannelType()
    {
        return IpcChannelType::SharedMemory;
    }
}