#include "IpcMetadata.hpp"

namespace IpcLib
{
    const std::string IpcMetadata::FileExtension = ".ipcmeta";

    IpcMtdtPtr IpcMetadata::FromIpcMetadataFile(const std::string &path)
    {
        IpcMtdtPtr ret = std::make_shared<IpcMetadata>();
        auto lines = Utils::Utils::ReadTextFileLines(path);
        ret->CreationTimestampMs = std::stoll(lines[0]);
        ret->Name = lines[1];
        ret->UniqueName = lines[2];
        ret->Path = lines[3];
        ret->ProcessId = std::stoi(lines[4]);
        ret->StartTime = std::stoull(lines[5]);
        ret->ChannelType = (IpcChannelType)std::stoi(lines[6]);
        return ret;
    }

    IpcMtdtPtr IpcMetadata::CreateForCurrentProcess(const std::string &name, const std::string &ipcRootDir, IpcChannelType channelType)
    {
        IpcMtdtPtr ret = std::make_shared<IpcMetadata>();
        auto thisPid = getpid();
        auto thisPidStr = std::to_string(thisPid);
        auto stat = Utils::Utils::ReadTextFile("/proc/" + thisPidStr + "/stat");
        auto fields = Utils::String::StringSplit(stat, " ");
        ret->CreationTimestampMs = Utils::Time::GetCurrentMillisecondTimestamp();
        ret->Name = name;
        ret->UniqueName = name+"."+thisPidStr+"."+std::to_string(ret->CreationTimestampMs);
        ret->Path = std::filesystem::path(ipcRootDir).append(ret->UniqueName).string()+FileExtension;
        ret->ProcessId = std::stoi(fields[0]);
        ret->StartTime = std::stoll(fields[21]);
        ret->ChannelType = channelType;
        return ret;
    }

    std::string IpcMetadata::ToString()
    {
        std::string content =
            std::to_string(CreationTimestampMs)+"\n"+
            Name + "\n" +
            UniqueName + "\n" +
            Path + "\n" +
            std::to_string(ProcessId) + "\n" +
            std::to_string(StartTime) + "\n" +
            std::to_string((int)ChannelType);
        return content;
    }

    void IpcMetadata::SaveToFile()
    {
        auto content = ToString();        
        Utils::Utils::WriteTextFile(Path, content);
    }

    void IpcMetadata::RemoveFile()
    {        
        std::filesystem::remove(Path);
    }

    IpcResult IpcMetadata::HasAssociation()
    {        
        auto compareFile = Utils::Utils::ReadTextFile(Path);
        auto thisAsString = ToString();
        
        if(compareFile != thisAsString)
            IPC_RET_FAIL_C(IpcOperationStatus::Metadata_NoAssociation)

        auto stat = Utils::Utils::ReadTextFile("/proc/"+std::to_string(ProcessId)+"/stat");
        auto fields = Utils::String::StringSplit(stat, " ");
        if(fields.size() != 52)
            IPC_RET_FAIL_C(IpcOperationStatus::Metadata_BadStatFile)

        if(std::to_string(StartTime) != fields[21])
            IPC_RET_FAIL_C(IpcOperationStatus::Metadata_StartTimeNotMatching)

        IPC_RET_OK
    }

}