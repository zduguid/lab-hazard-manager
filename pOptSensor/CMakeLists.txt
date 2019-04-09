#--------------------------------------------------------
# The CMakeLists.txt for:                       pOptSensor
# Author(s):                              Kristen Railey
#--------------------------------------------------------

SET(SRC
  OptSensor.cpp
  OptSensor_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pOptSensor ${SRC})

TARGET_LINK_LIBRARIES(pOptSensor
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

