#include "IpcBuffer.hpp"
#include <iostream>
namespace IpcLib
{
    void IpcBuffer::Delete()
    {
        if (Buffer != nullptr)
        {
            delete Buffer;
            Size = 0;
        }
    }
 
    IpcBufferPtr IpcBuffer::Allocate(uint32_t size)
    {
        IpcBufferPtr ret = std::make_shared<IpcBuffer>();
        
        ret->Size = size;
        ret->Buffer = new uint8_t[size];

        return ret;
    }    

    IpcBuffer::~IpcBuffer()
    {
        Delete();
    }
}