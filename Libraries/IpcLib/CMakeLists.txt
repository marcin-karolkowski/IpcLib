project(IpcLib CXX)
set(CMAKE_CXX_STANDARD 20)
add_library(${PROJECT_NAME} SHARED
IpcBuffer.cpp IpcBuffer.hpp
IpcChannelType.hpp
IpcManager.cpp IpcManager.hpp
IpcMessage.cpp IpcMessage.hpp
IpcMetadata.cpp IpcMetadata.hpp
IpcReceiverChannel.cpp IpcReceiverChannel.hpp
IpcResult.hpp
IpcSenderChannel.cpp IpcSenderChannel.hpp
IpcShm.cpp IpcShm.hpp
IpcShmReceiver.cpp IpcShmReceiver.hpp
IpcShmSender.cpp IpcShmSender.hpp
IpcThread.cpp IpcThread.hpp
)

target_link_libraries(${PROJECT_NAME} Utils pthread rt)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/Libraries)
