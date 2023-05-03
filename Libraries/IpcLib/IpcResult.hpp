#ifndef IPCLIB_IPCRESULT_HPP
#define IPCLIB_IPCRESULT_HPP

#include <cerrno>
#include <cstring>
#include <string>

namespace IpcLib
{
    enum class IpcOperationResult : uint16_t
    {
        Unknown,
        Ok,
        Fail,
        TimedOut,
        ChannelDamaged,
        FatalError
    };

    enum class IpcOperationStatus : uint16_t
    {
        Undefined=0,
        Metadata_NoAssociation,
        Metadata_BadStatFile,
        Metadata_StartTimeNotMatching,
        Manager_MultipleMetadataFound,
        Manager_MetadataWithOwnerNameNotFound,
        Manager_SenderNotFound,
        Manager_Cache_MetadataWithOwnerNameNotFound,
        Manager_UnimplementedCreationOfReceiverChannelType,
        Manager_UnimplementedCreationOfSenderChannelType,
        Manager_ReceiverHasNotBeenCreatedOrOpenedYet,
        Manager_ReceiveMessageParseError,
        ReceiverChannel_ReceiverAlreadyOpenedForTarget,
        SenderChannel_SenderAlreadyOpenedForTarget,
        ShmHelper_FailedToOpenShm,
        ShmHelper_FailedToSetShmSize,
        ShmHelper_FailedToMapShmMemory,
        ShmHelper_FailedToInitializeSenderQueueSemaphore,
        ShmHelper_FailedToInitializeReceiverDataWaitSemaphore,
        ShmHelper_FailedToInitializeSenderSendPossibilitySemaphore,
        ShmHelper_FailedToRetrieveShmStat,
        ShmHelper_MemWasNotMapped,
        ShmHelper_MemUnlinkError,
        ShmReceiver_BlockedQueueToShm,
        ShmReceiver_TimedOut1,
        ShmReceiver_TriedToSend0Bytes,
        ShmReceiver_ReceivedTooMuchData,
        ShmReceiver_UnexpectedResult,
        ShmSender_UninitializedShmMemory,
        ShmSender_CannotPreemptShmMemory,
        ShmSender_SendTimeExceeded,
        ShmSender_ReceiverNoLongerExistsForThisChannel,
        ShmSender_ReceiverNotAnnouncingReadiness,
        ShmSender_ReceiverWasNotReady,
        ShmSender_TriedToSend0Bytes,
        IpcThread_AlreadyStarted,
        IpcThread_NotRunning,
    };

    #define IPC_IS_OK(x) (x.Result == IpcLib::IpcOperationResult::Ok)
    #define IPC_IS_FAIL(x) (x.Result == IpcLib::IpcOperationResult::Fail)
    #define IPC_IS_TIMED_OUT(x) (x.Result == IpcLib::IpcOperationResult::TimedOut)
    #define IPC_IS_DAMAGED(x) (x.Result == IpcLib::IpcOperationResult::ChannelDamaged)
    #define IPC_IS_FATAL(x) (x.Result == IpcLib::IpcOperationResult::FatalError)   

    struct IpcResult
    {
        IpcOperationResult Result;
        IpcOperationStatus Status;
    };    

    #define IPC_MK_RESULT(x, result, status) x = IpcLib::IpcResult {.Result = result, .Status = status };
    #define IPC_RET_RESULT(result, status) return IpcLib::IpcResult { .Result = result, .Status = status};

    #define IPC_RET_OK_C(statusCode) IPC_RET_RESULT(IpcLib::IpcOperationResult::Ok, statusCode)
    #define IPC_RET_OK IPC_RET_OK_C(IpcLib::IpcOperationStatus::Undefined);
    #define IPC_MK_OK_C(x, statusCode) IPC_MK_RESULT(x, IpcLib::IpcOperationResult::Ok, statusCode)
    #define IPC_MK_OK(x) IPC_MK_OK_C(x, IpcLib::IpcOperationStatus::Undefined)

    #define IPC_RET_FAIL_C(statusCode) IPC_RET_RESULT(IpcLib::IpcOperationResult::Fail, statusCode)
    #define IPC_RET_FAIL IPC_RET_FAIL_C(IpcLib::IpcOperationStatus::Undefined)
    #define IPC_MK_FAIL_C(x, statusCode) IPC_MK_RESULT(x, IpcLib::IpcOperationResult::Fail, statusCode)
    #define IPC_MK_FAIL(x) IPC_MK_FAIL_C(x, IpcLib::IpcOperationStatus::Undefined)

    #define IPC_RET_TIMED_OUT_C(statusCode) IPC_RET_RESULT(IpcLib::IpcOperationResult::TimedOut, statusCode)
    #define IPC_RET_TIMED_OUT IPC_RET_TIMED_OUT_C(IpcLib::IpcOperationStatus::Undefined)
    #define IPC_MK_TIMED_OUT_C(x, statusCode) IPC_MK_RESULT(x, IpcLib::IpcOperationResult::TimedOut, statusCode)
    #define IPC_MK_TIMED_OUT(x) IPC_MK_TIMED_OUT_C(x, IpcLib::IpcOperationStatus::Undefined)

    #define IPC_RET_DAMAGED_C(statusCode) IPC_RET_RESULT(IpcLib::IpcOperationResult::ChannelDamaged, statusCode)
    #define IPC_RET_DAMAGED IPC_RET_DAMAGED_C(IpcLib::IpcOperationStatus::Undefined)
    #define IPC_MK_DAMAGED_C(x, statusCode) IPC_MK_RESULT(x, IpcLib::IpcOperationResult::ChannelDamaged, statusCode)
    #define IPC_MK_DAMAGED(x) IPC_MK_DAMAGED_C(x, IpcLib::IpcOperationStatus::Undefined)
    
    #define IPC_RET_FATAL_C(statusCode) IPC_RET_RESULT(IpcLib::IpcOperationResult::FatalError, statusCode)
    #define IPC_RET_FATAL IPC_RET_FATAL_C(IpcLib::IpcOperationStatus::Undefined)
    #define IPC_MK_FATAL_C(x, statusCode) IPC_MK_RESULT(x, IpcLib::IpcOperationResult::FatalError, statusCode)
    #define IPC_MK_FATAL(x) IPC_MK_FATAL_C(x, IpcLib::IpcOperationStatus::Undefined)
    
}

#endif