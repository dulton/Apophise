INCLUDE_DIRECTORIES(../SIPproto)
SET(siplib                                                                                                                                  
    ../SIPproto/SIPManager.cpp                                                  
    ../SIPproto/SIPMD5.cpp                                                      
    ../SIPproto/SIPSDP.cpp                                                      
    ../SIPproto/SIPMediaServer.cpp                                              
    ../SIPproto/SIPParser.cpp                                                   
    ../SIPproto/SIPUABase.cpp                                                   
    ../SIPproto/SIPClient.cpp                                                   
    ../SIPproto/SIPBuilder.cpp) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")
add_library(sipndsl SHARED ${siplib})
set_target_properties(sipndsl PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/SIPlib)
target_link_libraries(sipndsl
    ${siplib_EXTRA_LDFLAGS}
    "-Wl,--whole-archive"
    "-Wl,--no-whole-archive")
