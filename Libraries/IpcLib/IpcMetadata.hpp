#ifndef IPCLIB_IPCMETADATA_HPP
#define IPCLIB_IPCMETADATA_HPP

#include <unistd.h>
#include <string>
#include <cinttypes>
#include <filesystem>
#include <memory>
#include <vector>

#include "Utils/String.hpp"
#include "Utils/Utils.hpp"
#include "Utils/Time.hpp"
#include "IpcResult.hpp"
#include "IpcChannelType.hpp"

namespace IpcLib
{
    class IpcMetadata;
    using IpcMtdtPtr = std::shared_ptr<IpcMetadata>;
    using IpcMtdtVec = std::vector<IpcMtdtPtr>;
    using IpcMtdtVecPtr = std::shared_ptr<IpcMtdtVec>;

    class IpcMetadata
    {
    public:
        static const std::string FileExtension;
                
        int64_t CreationTimestampMs;
        // Name of process owning channel described by this metadata;
        std::string Name;
        // Name which clearly determines receiver's instance;        
        std::string UniqueName;
        // Path to file defining this metadata;        
        std::string Path;
        // ID of process owning channel described by this metadata;
        int32_t ProcessId;
        // Start time since boot in clock ticks
        uint64_t StartTime;
        // Type of associated channel
        IpcChannelType ChannelType;

        static IpcMtdtPtr FromIpcMetadataFile(const std::string &path);

        static IpcMtdtPtr CreateForCurrentProcess(const std::string &name, const std::string &ipcRootDir, IpcChannelType channelType);

        std::string ToString();

        void SaveToFile();

        void RemoveFile();

        IpcResult HasAssociation();
    };


}

#endif