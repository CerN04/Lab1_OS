cmake_minimum_required(VERSION 3.10)
project(lab1_OS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
include_directories(src)


if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message("Build for Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

    set(LIB_SOURCES
        src/ProcessManager.cpp
    )
    set(LIB_HEADERS
        include/ProcessManager.h
    )
    
    add_library(processlib STATIC ${LIB_SOURCES} ${LIB_HEADERS})
    target_include_directories(processlib PUBLIC include)
endif ()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message("Build for Windows")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /WX")
    
    set(LIB_SOURCES
        src/Win_ProcessManager.cpp
    )
    set(LIB_HEADERS
        include/Win_ProcessManager.h
    )
    
    add_library(processlib STATIC ${LIB_SOURCES} ${LIB_HEADERS})
    target_include_directories(processlib PUBLIC include)
endif ()

add_executable(parent parent.cpp)
add_executable(child1 child1.cpp)
add_executable(child2 child2.cpp)

target_link_libraries(parent processlib)
target_link_libraries(child1 processlib)
target_link_libraries(child2 processlib)


target_include_directories(parent PRIVATE include)
target_include_directories(child1 PRIVATE include)
target_include_directories(child2 PRIVATE include)
