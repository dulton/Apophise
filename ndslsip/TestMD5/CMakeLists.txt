INCLUDE_DIRECTORIES(../SIPproto)

SET(md5_test_src_list test_md5.cpp
    ../SIPproto/SIPManager.cpp
    ../SIPproto/SIPMD5.cpp
    ../SIPproto/SIPSDP.cpp
    ../SIPproto/SIPMediaServer.cpp
    ../SIPproto/SIPParser.cpp
    ../SIPproto/SIPUABase.cpp
    ../SIPproto/SIPBuilder.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -O2")
ADD_EXECUTABLE(test_md5 ${md5_test_src_list})
target_link_libraries(test_md5 eXosip2 osip2 osipparser2)

