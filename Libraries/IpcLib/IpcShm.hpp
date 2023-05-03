#ifndef IPCLIB_IPCSHM_HPP
#define IPCLIB_IPCSHM_HPP

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <semaphore.h>
#include <cinttypes>
#include <thread>
#include <string>
#include "IpcResult.hpp"

namespace IpcLib
{
    typedef struct IpcShmMem
    {
        uint32_t MessageSize;
        uint32_t BytesReceived;
        uint8_t Buffer[1048576]; // 1MiB
    } IpcShmMem_t;

    typedef struct IpcShm
    {
        bool IsInitialized;
        sem_t SndQueue;
        sem_t Sem_ReceiverDataWait;
        sem_t Sem_SenderSendPossibility;
        IpcShmMem_t Memory;
    } IpcShm_t;

    class ShmHelper
    {
    public:
        std::string Name;
        IpcShm_t *Memory;

        std::string GetName();

        IpcShm_t *GetMemory();

        IpcResult OpenAsReceiver(const std::string &name);

        IpcResult OpenAsSender(const std::string &name);

        IpcResult Close(bool unlink = true);

    private:
        
        inline bool TryLock(sem_t *sem, uint32_t millisecondsTimeout)
        {
            struct timespec timeLimit{};
            timespec_get(&timeLimit, TIME_UTC);

            uint64_t seconds = millisecondsTimeout / 1000;
            uint64_t total_nanoseconds = timeLimit.tv_nsec;
            total_nanoseconds += (millisecondsTimeout - (seconds * 1000)) * 1000000;

            uint32_t overflow_seconds = total_nanoseconds / 1000000000;
            seconds += overflow_seconds;
            timeLimit.tv_sec += seconds;
            timeLimit.tv_nsec = total_nanoseconds - overflow_seconds * 1000000000;

            int32_t waitResult = sem_timedwait(sem, &timeLimit);

            if (waitResult == -1 && errno != ETIMEDOUT)
            {
                std::cout << "Error on waiting on semaphore " << strerror(errno) << " " << errno << std::endl;
            }
            return waitResult == 0;
        }

        inline void Unlock(sem_t *sem)
        {
            sem_post(sem);
        }

    public:
        inline bool ReceiverWaitForData(uint32_t millisecondsTimeout)
        {
            return TryLock(&Memory->Sem_ReceiverDataWait, millisecondsTimeout);
        }
        inline bool IsSenderPreparingDataToReceive()
        {
            int semValue = 0;
            return sem_getvalue(&Memory->SndQueue, &semValue) == 0 && semValue == 0;
        }
        inline void ReceiverSetSendPossible()
        {
            Unlock(&Memory->Sem_SenderSendPossibility);
        }
        inline bool SenderWaitForSendPossibility(uint32_t millisecondsTimeout)
        {
            return TryLock(&Memory->Sem_SenderSendPossibility, millisecondsTimeout);
        }
        inline void SenderSetDataIsAvailable()
        {
            Unlock(&Memory->Sem_ReceiverDataWait);
        }

        inline bool SenderWaitForQueue(uint32_t millisecondsTimeout)
        {
            return TryLock(&Memory->SndQueue, millisecondsTimeout);
        }

        inline void SenderLeaveQueue()
        {
            Unlock(&Memory->SndQueue);
        }
    };
}
#endif