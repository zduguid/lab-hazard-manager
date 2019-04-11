#--------------------------------------------------------
# The CMakeLists.txt for:                       uFldHazardMgr
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  HazardSearch.cpp
  HazardMgrX.cpp
  HazardMgrX_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(uFldHazardMgrX ${SRC})

TARGET_LINK_LIBRARIES(uFldHazardMgrX
   ${MOOS_LIBRARIES}
   ufield
   ufld_hazards
   geometry
   apputil
   mbutil
   ${SYSTEM_LIBS}
)

