include_directories(${TNET_SOURCE_DIR}/core)
include_directories(${TNET_SOURCE_DIR}/http)

add_executable(httpserver_test httpserver_test.cpp)
target_link_libraries(httpserver_test tnet_http tnet_core)


add_executable(httpurl_test httpurl_test.cpp)
target_link_libraries(httpurl_test tnet_http tnet_core)
