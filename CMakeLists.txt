cmake_minimum_required(VERSION 2.8)

project(TNET)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DLINUX)

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DOSX)
else()
    message(FATAL_ERROR, "only support linux and mac osx")
endif()

add_subdirectory(core)
add_subdirectory(http)
add_subdirectory(test)
