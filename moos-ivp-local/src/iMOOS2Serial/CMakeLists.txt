#--------------------------------------------------------
# The CMakeLists.txt for:                        iMOOS2Serial
# Author(s):                          Andrew Patrikalakis
#--------------------------------------------------------
set(EXECNAME iMOOS2Serial)

SET(SRC CiMOOS2Serial.cpp CiMOOS2SerialMain.cpp CSerMOOS.cpp)

FILE(GLOB SRC *.cpp)

ADD_EXECUTABLE(iMOOS2Serial ${SRC})
   
TARGET_LINK_LIBRARIES(iMOOS2Serial
   ${MOOS_LIBRARIES}
   anrp_util
   m
   pthread)
