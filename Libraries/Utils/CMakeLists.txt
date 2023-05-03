project(Utils CXX)
set(CMAKE_CXX_STANDARD 20)
add_library(${PROJECT_NAME} SHARED
    String.hpp String.cpp
    Utils.hpp Utils.cpp
    Time.hpp Time.cpp
)
