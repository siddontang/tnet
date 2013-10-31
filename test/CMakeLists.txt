include_directories(${TNET_SOURCE_DIR}/core)
include_directories(${TNET_SOURCE_DIR}/http)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(DEP_LIB tnet_http tnet_core)

link_directories("/usr/local/lib")

if(CMAKE_USE_TCMALLOC)
    set(DEP_LIB ${DEP_LIB} tcmalloc_minimal)
    message(STATUS "use tcmalloc  ${DEP_LIB}")
endif()

add_executable(httpserver_test httpserver_test.cpp)
target_link_libraries(httpserver_test ${DEP_LIB})

add_executable(echoserver_test echoserver_test.cpp)
target_link_libraries(echoserver_test ${DEP_LIB})

add_executable(echoclient_test echoclient_test.cpp)
target_link_libraries(echoclient_test ${DEP_LIB})

add_executable(wsechoserver_test wsechoserver_test.cpp)
target_link_libraries(wsechoserver_test ${DEP_LIB})
