#include "IpcShmReceiver.hpp"

namespace IpcLib
{    

    IpcResult IpcShmReceiver::Create()
    {
        NoDataReceivedCounter = 0;
        return Shm.OpenAsReceiver(ReceiverMetadata->UniqueName);
    }


    IpcResult IpcShmReceiver::Receive(uint32_t millisecondsTimeout, IpcBufferPtr& outData)
    {
        bool firstPartAvailable = true;
        uint8_t* bufferPos = nullptr;
        uint8_t* bufferEnd = nullptr;
        do
        {
            if (!Shm.ReceiverWaitForData(millisecondsTimeout))
            {
                if (Shm.IsSenderPreparingDataToReceive())
                {
                    NoDataReceivedCounter++;
                    if (NoDataReceivedCounter >= NoDataReceivedLimit)
                        IPC_RET_DAMAGED_C(IpcOperationStatus::ShmReceiver_BlockedQueueToShm)
                }
                IPC_RET_TIMED_OUT_C(IpcOperationStatus::ShmReceiver_TimedOut1)
            }
            if (firstPartAvailable)
            {
                outData = IpcBuffer::Allocate(Shm.Memory->Memory.MessageSize);
                firstPartAvailable = false;
                NoDataReceivedCounter = 0;
                bufferPos = outData->Buffer;
                bufferEnd = outData->Buffer + outData->Size;
            }

            uint32_t dataInBuffer = bufferEnd - bufferPos;
            if(dataInBuffer > sizeof(Shm.Memory->Memory.Buffer))
                dataInBuffer = sizeof(Shm.Memory->Memory.Buffer);            

            std::memcpy(bufferPos, Shm.Memory->Memory.Buffer, dataInBuffer);
            bufferPos += dataInBuffer;
            Shm.ReceiverSetSendPossible();

            if (bufferPos == bufferEnd)
                IPC_RET_OK
        } while(bufferPos < bufferEnd);

        IPC_RET_FAIL_C(IpcOperationStatus::ShmReceiver_ReceivedTooMuchData)
    }
    
    IpcChannelType IpcShmReceiver::GetChannelType()
    {
        return IpcChannelType::SharedMemory;
    }

    IpcResult IpcShmReceiver::Destroy()
    {
        return Shm.Close();        
    }

}