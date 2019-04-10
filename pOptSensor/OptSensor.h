/************************************************************/
/*    NAME: Kristen Railey                                              */
/*    ORGN: MIT                                             */
/*    FILE: OptSensor.h                                          */
/*    DATE:                                                 */
/************************************************************/

#ifndef OptSensor_HEADER
#define OptSensor_HEADER

#include "MOOS/libMOOS/MOOSLib.h"

class OptSensor : public CMOOSApp
{
 public:
   OptSensor();
   ~OptSensor();

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected:
   void RegisterVariables();
   //From HazardMgr
   void handleMailSensorOptionsSummary(std::string);
   void handleMailMissionParams(std::string);
   double calcSearchTime(double num_passes,double sensor_width,double search_area_width,double search_area_height);
   double calcPd(double current_Pd);
   double calcOptPd(double exp);
 private: // Configuration variables
   double m_current_Pd;
   std::string m_finished_search;
   double m_penalty_missed_hazard;
   double m_penalty_nonopt_hazard; //?                                                                       
   double m_penalty_false_alarm;
   double m_penalty_max_time_over;
   double m_max_time;
   double m_penalty_max_time_rate;
   double m_transit_path_width; //?
   double m_search_reg_x_min;
   double m_search_reg_x_max;
   double m_search_reg_y_min;
   double m_search_reg_y_max;
   std::vector<double> m_sensor_width;
   bool m_sensor_options_received;
   bool m_update_lawnmower;
   bool m_search_config_received;
   std::string m_vname; 
   bool m_name_received;
   double m_num_passes;
   std::vector<double> m_sensor_exp;

   double m_time_buffer;
   double m_height_buffer;
 private: // State variables
};

#endif 
