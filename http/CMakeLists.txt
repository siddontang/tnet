set(HTTP_SRCS
    http_parser.c
    httprequest.cpp
    httpresponse.cpp
    httpserver.cpp
    httputil.cpp
    httpconnection.cpp
    wsconnection.cpp
    )

include_directories(${TNET_SOURCE_DIR}/core)

add_definitions(-DHTTP_PARSER_STRICT=0)

add_library(tnet_http ${HTTP_SRCS})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

target_link_libraries(tnet_http tnet_core)

install(TARGETS tnet_http DESTINATION lib)

set(HEADERS
    httpconnection.h
    httpdefs.h
    http_parser.h
    httprequest.h
    httpresponse.h
    httpserver.h
    httputil.h
    wsconnection.h
    wsevent.h 
    )

install(FILES ${HEADERS} DESTINATION include/tnet/http)
