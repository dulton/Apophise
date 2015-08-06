INCLUDE_DIRECTORIES(../SIPproto)

SET(media_test_src_list test_DSclient.cpp
    ../SIPproto/SIPManager.cpp
    ../SIPproto/SIPMD5.cpp
    ../SIPproto/SIPSDP.cpp
    ../SIPproto/SIPMediaServer.cpp
    ../SIPproto/SIPParser.cpp
    ../SIPproto/SIPUABase.cpp
    ../SIPproto/SIPDSClient.cpp
    ../SIPproto/SIPBuilder.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -O2")
ADD_EXECUTABLE(ds_client_test ${media_test_src_list})
target_link_libraries(ds_client_test osip2 osipparser2)

