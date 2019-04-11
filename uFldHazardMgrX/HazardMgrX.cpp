/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardMgrX.cpp                                       */
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

#include <cmath>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>

#include "MBUtils.h"
#include "NodeMessage.h"
#include "HazardMgrX.h"
#include "HazardSearch.h"
#include "XYFormatUtilsHazard.h"
#include "XYFormatUtilsPoly.h"
#include "ACTable.h"

using namespace std;


//---------------------------------------------------------
// Constructor

HazardMgrX::HazardMgrX()
{
  // Config variables
  m_swath_width_desired = 25;
  m_pd_desired          = 0.9;

  // State Variables 
  m_sensor_config_requested = false;
  m_sensor_config_set   = false;
  m_swath_width_granted = 0;
  m_pd_granted          = 0;
  m_pfa_granted         = 0;
  m_pc_granted          = 0;

  m_sensor_config_reqs = 0;
  m_sensor_config_acks = 0;
  m_sensor_report_reqs = 0;
  m_detection_reports  = 0;

  m_summary_reports    = 0;

  // number of times an obstacle is passed over
  // TODO make new variable posting when next lawnmower begins 
  m_num_passes         = 0;
}


//---------------------------------------------------------
// Procedure: OnNewMail

bool HazardMgrX::OnNewMail(MOOSMSG_LIST &NewMail)
{
  AppCastingMOOSApp::OnNewMail(NewMail);

  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
    string key   = msg.GetKey();
    string sval  = msg.GetString(); 
    
    if(key == "UHZ_CONFIG_ACK") 
      handleMailSensorConfigAck(sval);

    else if(key == "UHZ_OPTIONS_SUMMARY") 
      handleMailSensorOptionsSummary(sval);

    else if(key == "UHZ_DETECTION_REPORT") 
      handleMailDetectionReport(sval);

    else if(key == "UHZ_HAZARD_REPORT") 
      handleMailHazardReport(sval);

    else if(key == "HAZARDSET_REQUEST") 
      handleMailReportRequest();

    else if(key == "UHZ_MISSION_PARAMS") 
      handleMailMissionParams(sval);

    else if(key == "FINISHED_SEARCH") {
      if (sval == "true") 
        m_num_passes++;
    }

    else 
      reportRunWarning("Unhandled Mail: " + key);
  }
	
   return(true);
}


//---------------------------------------------------------
// Procedure: OnConnectToServer

bool HazardMgrX::OnConnectToServer()
{
   registerVariables();
   return(true);
}


//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool HazardMgrX::Iterate()
{
  AppCastingMOOSApp::Iterate();

  if(!m_sensor_config_requested)
    postSensorConfigRequest();

  if(m_sensor_config_set)
    postSensorInfoRequest();

  if(m_node_message_queue.size()>0) {
    NodeMessage msg;
    msg.setSourceNode(m_host_community);
    msg.setDestNode("all");
    msg.setVarName("HAZARD_SEARCH_UP");
    msg.setStringVal(m_hazard_search.GetHazardNodeMessage(m_node_message_queue.front()));
    Notify("NODE_MESSAGE_LOCAL", msg.getSpec());
  }

  AppCastingMOOSApp::PostReport();
  return(true);
}


//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool HazardMgrX::OnStartUp()
{
  AppCastingMOOSApp::OnStartUp();

  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(true);
  if(!m_MissionReader.GetConfiguration(GetAppName(), sParams))
    reportConfigWarning("No config block found for " + GetAppName());

  STRING_LIST::iterator p;
  for(p=sParams.begin(); p!=sParams.end(); p++) {
    string orig  = *p;
    string line  = *p;
    string param = tolower(biteStringX(line, '='));
    string value = line;

    bool handled = false;
    if((param == "swath_width") && isNumber(value)) {
      m_swath_width_desired = atof(value.c_str());
      handled = true;
    }
    else if(((param == "sensor_pd") || (param == "pd")) && isNumber(value)) {
      m_pd_desired = atof(value.c_str());
      handled = true;
    }
    else if(param == "report_name") {
      value = stripQuotes(value);
      m_report_name = value;
      handled = true;
    }
    else if(param == "region") {
      XYPolygon poly = string2Poly(value);
      if(poly.is_convex())
	m_search_region = poly;
      handled = true;
    }

    if(!handled)
      reportUnhandledConfigWarning(orig);
  }
  
  m_hazard_set.setSource(m_host_community);
  m_hazard_set.setName(m_report_name);
  m_hazard_set.setRegion(m_search_region);
  
  registerVariables();	
  return(true);
}


//---------------------------------------------------------
// Procedure: registerVariables

void HazardMgrX::registerVariables()
{
  AppCastingMOOSApp::RegisterVariables();
  Register("UHZ_DETECTION_REPORT", 0);
  Register("UHZ_HAZARD_REPORT", 0);
  Register("UHZ_CONFIG_ACK", 0);
  Register("UHZ_OPTIONS_SUMMARY", 0);
  Register("UHZ_MISSION_PARAMS", 0);
  Register("HAZARDSET_REQUEST", 0);
  Register("FINISHED_SEARCH", 0);
}


//---------------------------------------------------------
// Procedure: postSensorConfigRequest

void HazardMgrX::postSensorConfigRequest()
{
  string request = "vname=" + m_host_community;
  
  request += ",width=" + doubleToStringX(m_swath_width_desired,2);
  request += ",pd="    + doubleToStringX(m_pd_desired,2);

  m_sensor_config_requested = true;
  m_sensor_config_reqs++;
}


//---------------------------------------------------------
// Procedure: postSensorInfoRequest

void HazardMgrX::postSensorInfoRequest()
{
  string request = "vname=" + m_host_community;

  m_sensor_report_reqs++;
  Notify("UHZ_SENSOR_REQUEST", request);
}


//---------------------------------------------------------
// Procedure: handleMailSensorConfigAck

bool HazardMgrX::handleMailSensorConfigAck(string str)
{
  // Expected ack parameters:
  string vname, width, pd, pfa, pclass;
  
  // Parse and handle ack message components
  bool   valid_msg = true;
  string original_msg = str;

  vector<string> svector = parseString(str, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];

    if(param == "vname")
      vname = value;
    else if(param == "pd")
      pd = value;
    else if(param == "width")
      width = value;
    else if(param == "pfa")
      pfa = value;
    else if(param == "pclass")
      pclass = value;
    else
      valid_msg = false;       
  }

  if((vname=="")||(width=="")||(pd=="")||(pfa=="")||(pclass==""))
    valid_msg = false;
  
  if(!valid_msg)
    reportRunWarning("Unhandled Sensor Config Ack:" + original_msg);

  
  if(valid_msg) {
    m_sensor_config_set = true;
    m_sensor_config_acks++;
    m_swath_width_granted = atof(width.c_str());
    m_config_id   = "pd=" + pd + ",pfa=" + pfa + ",pc=" + pclass;
    m_pd_granted  = atof(pd.c_str());
    m_pfa_granted = atof(pfa.c_str());
    m_pc_granted  = atof(pclass.c_str());
  }

  return(valid_msg);
}


//---------------------------------------------------------
// Procedure: handleMailDetectionReport
//      Note: The detection report should look something like:
//            UHZ_DETECTION_REPORT = vname=betty,x=51,y=11.3,label=12 

bool HazardMgrX::handleMailDetectionReport(string str)
{
  m_detection_reports++;

  XYHazard new_hazard = string2Hazard(str);
  new_hazard.setType("hazard");
  string hazlabel = new_hazard.getLabel();
  
  if(hazlabel == "") {
    reportRunWarning("Detection report received for hazard w/out label");
    return(false);
  }

  string event = "New Det, label=" + hazlabel;
  reportEvent(event);

  // TODO add to queue of items to continually request (add hazard itself to queue)
  string req = "vname=" + m_host_community + ",label=" + hazlabel;
  Notify("UHZ_CLASSIFY_REQUEST", req);

  m_hazard_search_set.insert(str);
  m_simple_hazard_set.insert(hazlabel);
  string vname     = m_host_community;
  string search_id = m_hazard_search.GetHazardSearchID(str, vname, m_config_id);

  // add detection for this hazard 
  if (m_hazard_search.m_simple_detect_map.count(hazlabel) > 0){
    m_hazard_search.m_simple_detect_map[hazlabel]++;
  }
  else {
    m_hazard_search.m_simple_detect_map[hazlabel]=1;
  }

  // check if node message is already on the queue
  // TODO change this to just sharing the final decision with other vehicle 
  // TODO take the set union between vehicles regarding what to send in the hazard request 
  m_node_message_queue.push_back(search_id);

  return(true);
}


//---------------------------------------------------------
// Procedure: handleMailHazardReport
//      Note: The hazard report should look something like:
//            UHZ_HAZARD_REPORT = vname=archie,x=51,y=11.3,hazard=true,label=12 

bool HazardMgrX::handleMailHazardReport(string str)
{
  m_detection_reports++;

  XYHazard new_hazard = string2Hazard(str);
  string   hazlabel = new_hazard.getLabel();
  string   haztype  = new_hazard.getType();
  
  if(hazlabel == "") {
    reportRunWarning("Hazard report received for hazard w/out label");
    return(false);
  }

  string event = "New Haz, label=" + hazlabel;
  event += ", type=" + haztype;
  reportEvent(event);

  string vname     = m_host_community;
  string search_id = m_hazard_search.GetHazardSearchID(str, vname, m_config_id);

  // add classification request for this hazard
  if (m_hazard_search.m_simple_request_map.count(hazlabel) > 0) {
    m_hazard_search.m_simple_request_map[hazlabel]++;
  }
  else {
    m_hazard_search.m_simple_request_map[hazlabel]=1;
  }

  // add classification for type hazard
  if (haztype == "hazard"){
    if (m_hazard_search.m_simple_classify_map.count(hazlabel) > 0) {
      m_hazard_search.m_simple_classify_map[hazlabel]++;
    }
    else {
      m_hazard_search.m_simple_classify_map[hazlabel]=1;
    }
  }

  // TODO check if node message is already on the queue
  m_node_message_queue.push_back(search_id);

  return(true);
}


//---------------------------------------------------------
// Procedure: handleMailReportRequest

void HazardMgrX::handleMailReportRequest()
{
  m_summary_reports++;

  m_hazard_set.findMinXPath(20);
  //unsigned int count    = m_hazard_set.findMinXPath(20);
  string summary_report = m_hazard_set.getSpec("final_report");
  
  Notify("HAZARDSET_REPORT", summary_report);
}


//---------------------------------------------------------
// Procedure: handleMailMissionParams
//   Example: UHZ_MISSION_PARAMS = penalty_missed_hazard=100,               
//                       penalty_nonopt_hazard=55,                
//                       penalty_false_alarm=35,                  
//                       penalty_max_time_over=200,               
//                       penalty_max_time_rate=0.45,              
//                       transit_path_width=25,                           
//                       search_region = pts={-150,-75:-150,-50:40,-50:40,-75}

void HazardMgrX::handleMailMissionParams(string str)
{
  vector<string> svector = parseStringZ(str, ',', "{");
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
  }
}


//------------------------------------------------------------
// Procedure: buildReport()

bool HazardMgrX::buildReport() 
{
  m_msgs << "============================================" << endl;
  m_msgs << "File: HazardMgrX." << m_host_community << " " << endl;
  m_msgs << "============================================" << endl;
  m_msgs << endl;

  // set up a table that summarizes the results of the hazard search
  //  + 'HazID' is the label of the hazard
  //  + 'NumPass' is the estimated possible number of detections (# lawnmowers)
  //  + 'ActDet' is the actual number of detections received
  //  + 'ClassReq' is the number of classification requests
  //  + 'ClassHaz' is the number of times the obstacle is classified as a hazard
  m_msgs << "--------------------------------------------"       << endl;
  m_msgs << "Detection/Classification Summary:"                  << endl;
  ACTable actab(7);
  string vname = m_host_community; 
  actab << "HazID | # | NumPass | ActDet | ClassReq | ClassHaz | # ";
  actab.addHeaderLines();

  // add a new line to the table for each element seen
  set<string>::iterator it = m_simple_hazard_set.begin();
  while (it != m_simple_hazard_set.end())
  { 
    // assemble strings to use in the AppCast table
    string act_det, class_req, class_haz;

    // extract the detection count
    if (m_hazard_search.m_simple_detect_map.count(*it) > 0) {
      act_det = to_string(m_hazard_search.m_simple_detect_map[*it]);
    }
    else {
      act_det = to_string(0);
    }

    // extract the classification request count
    if (m_hazard_search.m_simple_request_map.count(*it) > 0)
      class_req = to_string(m_hazard_search.m_simple_request_map[*it]);
    else 
      class_req = to_string(0);

    // get the hazard classification count 
    if (m_hazard_search.m_simple_classify_map.count(*it) > 0)
      class_haz = to_string(m_hazard_search.m_simple_classify_map[*it]);
    else 
      class_haz = to_string(0);

    // print out all the count information in a nice table format
    actab << *it 
          << "#"
          << to_string(m_num_passes)
          << act_det
          << class_req
          << class_haz
          << "#";
    it++;
  }
  m_msgs << actab.getFormattedString();

  m_msgs << endl << endl;
  m_msgs << "--------------------------------------------"       << endl;
  m_msgs << "Config Requested:"                                  << endl;
  m_msgs << "    swath_width_desired: " << m_swath_width_desired << endl;
  m_msgs << "             pd_desired: " << m_pd_desired          << endl;
  m_msgs << "   config requests sent: " << m_sensor_config_reqs  << endl;
  m_msgs << "                  acked: " << m_sensor_config_acks  << endl;
  m_msgs << "--------------------------------------------"       << endl;
  m_msgs << "Config Result:"                                     << endl;
  m_msgs << "       config confirmed: " << boolToString(m_sensor_config_set) << endl;
  m_msgs << "    swath_width_granted: " << m_swath_width_granted << endl;
  m_msgs << "             pd_granted: " << m_pd_granted          << endl;
  m_msgs << "            pfa_granted: " << m_pfa_granted         << endl;
  m_msgs << "             pc_granted: " << m_pc_granted          << endl;
  m_msgs << "--------------------------------------------"       << endl;
  m_msgs << "Sensor Result:"                                     << endl;
  m_msgs << "        sensor requests: " << m_sensor_report_reqs  << endl;
  m_msgs << "      detection reports: " << m_detection_reports   << endl; 
  m_msgs << "  Hazardset Reports Req: " << m_summary_reports     << endl;
  m_msgs << " Hazardset Reports Post: " << m_summary_reports     << endl;
  m_msgs << "            Report Name: " << m_report_name         << endl;

  return(true);
}












