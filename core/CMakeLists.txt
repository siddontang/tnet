set(CORE_SRCS
    acceptloop.cpp
    acceptor.cpp
    address.cpp
    connection.cpp
    ioloop.cpp
    ioloopthread.cpp
    ioloopthreadpool.cpp
    log.cpp
    misc.cpp
    signaler.cpp
    sockutil.cpp
    tcpclient.cpp
    tcpserver.cpp
    threadutil.cpp
    timer.cpp
    connchecker.cpp
    iolooppooltimer.cpp
    stringutil.cpp
    )

set(EV_SRCS ./libev/ev.c)

#define some libev macro to proper use libev
add_definitions(-DEV_STANDALONE=1 -DEV_USE_FLOOR -DEV_USE_CLOCK_SYSCALL
-DEV_USE_NANOSLEEP -DEV_MULTIPLICITY=1 -DEV_USE_NAANOSLEEP)

include_directories("./libev")

set(DEP_LIB pthread)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DEV_USE_EVENTFD -DEV_USE_EPOLL)

    set(DEP_LIB ${DEP_LIB} rt)
else()
    add_definitions(-DEV_USE_KQUEUE)
endif()

add_library(tnet_core ${CORE_SRCS} ${EV_SRCS})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

target_link_libraries(tnet_core ${DEP_LIB})
