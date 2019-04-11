/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardMgrX.h                                         */
/*    DATE: Oct 26th 2012                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef UFLD_HAZARD_MGR_HEADER
#define UFLD_HAZARD_MGR_HEADER

#include <cmath>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "NodeMessage.h"
#include "HazardSearch.h"
#include "XYHazard.h"
#include "XYHazardSet.h"
#include "XYPolygon.h"


class HazardMgrX : public AppCastingMOOSApp
{
 public:
   HazardMgrX();
   ~HazardMgrX() {}

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected: // Registration, Configuration, Mail handling utils
   void registerVariables();
   bool handleMailSensorConfigAck(std::string);
   bool handleMailSensorOptionsSummary(std::string) {return(true);}
   bool handleMailDetectionReport(std::string);
   bool handleMailHazardReport(std::string);
   void handleHazardMsgReady();
   void handleMailReportRequest();
   void handleMailMissionParams(std::string);
   void handleNodeMessage(std::string);

 protected: 
   void postSensorConfigRequest();
   void postSensorInfoRequest();
   void postHazardSetReport();

 protected:
   bool calcHazardBelief(std::string label);
   unsigned long long int factorial(long int n);
   double binom_distribution(double p,int k,int n);
   
 private: // Configuration variables
   double       m_swath_width_desired;
   double       m_pd_desired;
   std::string  m_report_name;

 private: // State variables
   bool         m_sensor_config_requested;
   bool         m_sensor_config_set;
   bool         m_compile_hazard_set_now;
   bool         m_hazard_sharing_complete;
   bool         m_waiting_for_ack;
   bool         m_ready_to_send_msg;
   bool         m_send_ack_msg_now;

   unsigned int m_ack_messages_received;
   unsigned int m_sensor_config_reqs;
   unsigned int m_sensor_config_acks;

   unsigned int m_sensor_report_reqs;
   unsigned int m_detection_reports;
   unsigned int m_num_passes;
   unsigned int m_self_haz_reported;
   unsigned int m_collab_haz_reported;

   unsigned int m_summary_reports;

   double       m_transit_path_width;
   double       m_swath_width_granted;
   double       m_pd_granted;
   double       m_pfa_granted;
   double       m_pc_granted;

   XYHazardSet  m_hazard_set;
   XYHazardSet  m_hazard_set_shared;
   XYPolygon    m_search_region;

   HazardSearch m_hazard_search;
   std::string  m_config_id;
   std::string  m_latest_received_node_msg;
   std::set<std::string> m_hazard_search_set;
   std::set<std::string> m_simple_hazard_set;
   std::list<std::string> m_node_message_queue;
};

#endif 








