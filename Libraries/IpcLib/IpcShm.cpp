#include <iostream>
#include "IpcShm.hpp"

namespace IpcLib
{
    std::string ShmHelper::GetName()
    {
        return Name;
    }

    IpcShm_t* ShmHelper::GetMemory()
    {
        return Memory;
    }

    IpcResult ShmHelper::OpenAsReceiver(const std::string &name)
    {
        std::string resultMessage;       
        IpcOperationStatus operationStatus = IpcOperationStatus::Undefined;

        std::string validName = "/" + name;
        int fd = shm_open(validName.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
        if (fd == -1)
        {
            operationStatus = IpcOperationStatus::ShmHelper_FailedToOpenShm;            
            goto exit_fail;
        }

        if (ftruncate(fd, sizeof(IpcShm_t)) == -1)
        {
            operationStatus = IpcOperationStatus::ShmHelper_FailedToSetShmSize;            
            goto exit_fail;
        }

        Memory = (IpcShm_t*)mmap(nullptr, sizeof(IpcShm_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (Memory == MAP_FAILED)
        {
            operationStatus = IpcOperationStatus::ShmHelper_FailedToMapShmMemory;            
            goto exit_fail;
        }

        // initialize sem to 1 to allow immediate use by sender
        if(sem_init(&Memory->SndQueue, 1, 1) != 0)
        {
            operationStatus = IpcOperationStatus::ShmHelper_FailedToInitializeSenderQueueSemaphore;            
            goto exit_fail;
        }

        // initialize sem to 0 to initial block receiver
        if(sem_init(&Memory->Sem_ReceiverDataWait, 1, 0))
        {            
            operationStatus = IpcOperationStatus::ShmHelper_FailedToInitializeReceiverDataWaitSemaphore;
            goto exit_fail;
        }

        if(sem_init(&Memory->Sem_SenderSendPossibility, 1, 1))
        {
            operationStatus = IpcOperationStatus::ShmHelper_FailedToInitializeSenderSendPossibilitySemaphore;            
            goto exit_fail;
        }


        Name = validName;        
        Memory->IsInitialized = true;
        close(fd);
        IPC_RET_OK

    exit_fail:
        if (fd != -1)
            close(fd);

        if(Memory!= MAP_FAILED)
        {
            if(munmap(Memory, sizeof(IpcShm_t))!=0)
            {

            }
        }

        fd = -1;
        Memory = (IpcShm_t*)MAP_FAILED;

        IPC_RET_FAIL_C(operationStatus)
    }

    IpcResult ShmHelper::OpenAsSender(const std::string& name)
    {
        std::string resultMessage;
        struct stat info{};
        IpcOperationStatus operationStatus = IpcOperationStatus::Undefined;
        std::string validName = "/" + name;
        int fd = shm_open(validName.c_str(), O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
        if (fd == -1)
        {
            operationStatus = IpcOperationStatus::ShmHelper_FailedToOpenShm;            
            goto exit_fail;
        }

        if(fstat(fd, &info) != 0)
        {
            operationStatus = IpcOperationStatus::ShmHelper_FailedToRetrieveShmStat;            
            goto exit_fail;
        }

        Memory = (IpcShm_t*)mmap(nullptr, sizeof(IpcShm_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

        if (Memory == MAP_FAILED)
        {
            operationStatus = IpcOperationStatus::ShmHelper_FailedToMapShmMemory;            
            goto exit_fail;
        }

        Name = validName;
        close(fd);
        IPC_RET_OK

    exit_fail:
        if (fd != -1)
            close(fd);

        IPC_RET_FAIL_C(operationStatus)
    }

    IpcResult ShmHelper::Close(bool unlink)
    {        
        IpcResult ret;
        IPC_MK_OK(ret)
        if (Memory != MAP_FAILED)
        {
            int result = munmap(Memory, sizeof(IpcShm_t));
            Memory = (IpcShm_t*)MAP_FAILED;            
        }
        else
        {
            IPC_MK_FAIL_C(ret, IpcOperationStatus::ShmHelper_MemWasNotMapped)
        }

        if (unlink)
        {
            if (shm_unlink(Name.c_str()) != 0)
            {
                IPC_MK_FAIL_C(ret, IpcOperationStatus::ShmHelper_MemUnlinkError)
            }
        }
        return ret;
    }
}