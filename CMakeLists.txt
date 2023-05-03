cmake_minimum_required(VERSION 3.18)
enable_testing()
project(solution)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install)
#set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
add_subdirectory(Libraries)
add_subdirectory(Programs)
add_subdirectory(Tests)

install(TARGETS IpcLib Utils ipc_test_receiver ipc_test_sender mathTool
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
)

