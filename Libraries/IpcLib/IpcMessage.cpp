#include "IpcMessage.hpp"

namespace IpcLib
{
    IpcMsgPtr IpcMessage::Parse(std::vector<uint8_t>& data, std::string& outErrorMessage)
    {
        //SOH           1           xxx             )
        //MessageSize   4           --- )           )
        //HeaderSize    4           --- )           )
        //DataSize      4           --- ) HeaderSize)
        //SenderMetadataPath + ;    --- )           ) MessageSize
        //TargetMetadataPath + \0   --- )           )
        //STX           1           xxx             )
        // (data)       N           --- ) DataSize  )
        //ETX           1           xxx             )
        auto dataSize = data.size();
        auto dataPtr = data.data();
        if(dataSize < 17)
        {
            outErrorMessage = "Message does not contain control data";
            return nullptr;
        }

        if(data[0] != 0x01)
        {
            outErrorMessage = "No SOH byte at start";
            return nullptr;
        }

        if(data[dataSize-1] != 0x03)
        {
            outErrorMessage = "No ETX byte at end";
            return nullptr;
        }

        auto ret = std::make_shared<IpcMessage>();
        ret->MessageSize = *((uint32_t*)&dataPtr[1]);
        
        if(ret->MessageSize != dataSize)
        {
            outErrorMessage = "Bad size announced";
            return nullptr;
        }

        ret->HeaderSize = *((uint32_t*)&dataPtr[5]);
        ret->DataSize = *((uint32_t*)&dataPtr[9]);

        if(ret->HeaderSize + ret->DataSize + 3 != ret->MessageSize)
        {
            outErrorMessage = "Bad announcement of sizes of message fields";
            return nullptr;
        }

        if(dataPtr[ret->HeaderSize+1] != 0x02)
        {
            outErrorMessage = " No STX byte at expected position";
            return nullptr;
        }

        uint32_t stringLen = ret->HeaderSize - 13;
        std::string senderAndTarget = std::string((char*)&dataPtr[13], stringLen);
        auto senderAndTargetArr = Utils::String::StringSplit(senderAndTarget, ";");
        
        if(senderAndTargetArr.size() != 2)
        {
            outErrorMessage = "Bad sender and target fields";
            return nullptr;
        }

        ret->SenderMetadataPath = senderAndTargetArr[0];
        ret->TargetMetadataPath = senderAndTargetArr[1];

        ret->Data.resize(ret->DataSize);
        std::memcpy(ret->Data.data(), &dataPtr[ret->HeaderSize+2], ret->DataSize);
        
        return ret;
    }

    IpcMsgPtr IpcMessage::Create(const std::string& senderMetadataPath, const std::string& targetMetadataPath, const std::vector<uint8_t>& data)
    {
        auto msgPtr = std::make_shared<IpcMessage>();
        msgPtr->SenderMetadataPath = senderMetadataPath;
        msgPtr->TargetMetadataPath = targetMetadataPath;
        msgPtr->Data = data;
        return msgPtr;
    }

    std::vector<uint8_t> IpcMessage::Serialize()
    {
        DataSize = Data.size();
        HeaderSize = 14 + SenderMetadataPath.size() + TargetMetadataPath.size();
        MessageSize = HeaderSize + DataSize + 3;
        std::vector<uint8_t> ret = 
        {
            0x01,
            ((uint8_t*)&MessageSize)[0],
            ((uint8_t*)&MessageSize)[1],
            ((uint8_t*)&MessageSize)[2],
            ((uint8_t*)&MessageSize)[3],
            ((uint8_t*)&HeaderSize)[0],
            ((uint8_t*)&HeaderSize)[1],
            ((uint8_t*)&HeaderSize)[2],
            ((uint8_t*)&HeaderSize)[3],
            ((uint8_t*)&DataSize)[0],
            ((uint8_t*)&DataSize)[1],
            ((uint8_t*)&DataSize)[2],
            ((uint8_t*)&DataSize)[3]
        };

        std::vector<uint8_t> senderMetadataPathVec(SenderMetadataPath.size());
        std::vector<uint8_t> targetMetadataPathVec(TargetMetadataPath.size());
        std::memcpy(senderMetadataPathVec.data(), SenderMetadataPath.data(), senderMetadataPathVec.size());
        std::memcpy(targetMetadataPathVec.data(), TargetMetadataPath.data(), targetMetadataPathVec.size());
        ret.insert(ret.end(), senderMetadataPathVec.begin(), senderMetadataPathVec.end());
        ret.emplace_back(';');
        ret.insert(ret.end(), targetMetadataPathVec.begin(), targetMetadataPathVec.end());
        ret.emplace_back(0);
        ret.emplace_back(0x02);
        ret.insert(ret.end(), Data.begin(), Data.end());
        ret.emplace_back(0x03);
        return ret;
    }
}