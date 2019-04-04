/************************************************************/
/*    NAME: Zach & Kristen                                  */
/*    ORGN: MIT                                             */
/*    FILE: HazardPath.h                                    */
/*    DATE: April 4th 2019                                  */
/************************************************************/

#ifndef HazardPath_HEADER
#define HazardPath_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class HazardPath : public AppCastingMOOSApp
{
 public:
   HazardPath();
   ~HazardPath(){};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables

 private: // State variables
};

#endif 
