INCLUDE_DIRECTORIES(../SIPproto)

SET(client_test_src_list test_MSClient.cpp
    ../SIPproto/SIPManager.cpp
    ../SIPproto/SIPMD5.cpp
    ../SIPproto/SIPSDP.cpp
    ../SIPproto/SIPMediaServer.cpp
    ../SIPproto/SIPParser.cpp
    ../SIPproto/SIPUABase.cpp
    ../SIPproto/SIPMSClient.cpp
    ../SIPproto/SIPDSClient.cpp
    ../SIPproto/SIPBuilder.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -O2")
ADD_EXECUTABLE(ms_client_test ${client_test_src_list})
target_link_libraries(ms_client_test osip2 osipparser2)

