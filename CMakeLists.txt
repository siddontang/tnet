cmake_minimum_required(VERSION 2.8)

project(TNET)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -ggdb")

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DLINUX)

elseif(${CMAKE_SYSTEM_NAME} matches "Darwin")
    add_definitions(-DOSX)
else()
    message(FATAL_ERROR, "only support linux and mac osx")
endif()

add_subdirectory(core)
add_subdirectory(http)
add_subdirectory(test)
