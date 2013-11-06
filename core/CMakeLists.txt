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

set(POLARSSL_SRCS 
    ./polarssl/base64.c
    ./polarssl/md5.c
    ./polarssl/sha1.c)

set(SRCS ${CORE_SRCS} ${EV_SRCS} ${POLARSSL_SRCS})

#define some libev macro to proper use libev
add_definitions(-DEV_STANDALONE=1 -DEV_USE_FLOOR -DEV_USE_NANOSLEEP -DEV_MULTIPLICITY=1)

include_directories(".")
include_directories("./libev")

set(DEP_LIB pthread)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    add_definitions(-DEV_USE_EVENTFD -DEV_USE_EPOLL -DEV_USE_CLOCK_SYSCALL)

    set(DEP_LIB ${DEP_LIB} rt)
else()
    add_definitions(-DEV_USE_SELECT)
endif()

add_library(tnet_core ${SRCS})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

target_link_libraries(tnet_core ${DEP_LIB})

install(TARGETS tnet_core DESTINATION lib)

set(HEADERS
    address.h
    connection.h
    connevent.h
    coredefs.h
    log.h
    misc.h
    nocopyable.h
    sockutil.h
    stringutil.h
    tcpclient.h
    tcpserver.h
    threadutil.h
    timer.h
    )

install(FILES ${HEADERS} DESTINATION include/tnet)
