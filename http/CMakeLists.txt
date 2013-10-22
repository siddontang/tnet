set(HTTP_SRCS
    http_parser.c
    httpconnection.cpp
    httprequest.cpp
    httpresponse.cpp
    httpserver.cpp
    httputil.cpp)

include_directories(${TNET_SOURCE_DIR}/core)

add_definitions(-DHTTP_PARSER_STRICT=0)

add_library(tnet_http ${HTTP_SRCS})

target_link_libraries(tnet_http tnet_core)
