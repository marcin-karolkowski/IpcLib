#ifndef IPC_IPCBUFFER_HPP
#define IPC_IPCBUFFER_HPP

#include <cinttypes>
#include <memory>

namespace IpcLib
{
    class IpcBuffer;
    using IpcBufferPtr = std::shared_ptr<IpcBuffer>;

    class IpcBuffer
    {
    public:
        uint32_t Size=0;
        uint8_t* Buffer = nullptr;

        void Delete();

        ~IpcBuffer();
        
        static IpcBufferPtr Allocate(uint32_t size);
    };

}


#endif