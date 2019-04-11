/************************************************************/
/*    NAME: Zach & Kristen                                  */
/*    ORGN: MIT                                             */
/*    FILE: HazardSearch.cpp                                */
/*    DATE: 09 April 2019                                   */
/************************************************************/

#include <cmath>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <set>
#include <string>
#include <stdint.h>

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

HazardSearch::HazardSearch(){
  // query conventions for storing information in the above maps
  setExpectedDetectionQuery();
  setDetectionQuery();
  setRequestedClassificationQuery();
  setClassificationQuery();
}


//---------------------------------------------------------
// GetHazardSearchStringID

string HazardSearch::GetHazardSearchID(string hazard, string vname, string config_id)
{
  return(hazard + ":" + vname  + ":" + config_id);
}


//---------------------------------------------------------
// GetHazardSearchStringID

vector<string> HazardSearch::ParseHazardSearchID(std::string hazard_search_id)
{
  return(parseString(hazard_search_id, ':'));
}


//---------------------------------------------------------
// AddCount (protected)

void HazardSearch::addCount(string hazard_search_id,
                            string query_string){

  // parse the hazard search id string 
  vector<string> hazard_search_vector = HazardSearch::ParseHazardSearchID(hazard_search_id);
  string hazard, vname, config_id;
  hazard    = hazard_search_vector[0];
  vname     = hazard_search_vector[1];
  config_id = hazard_search_vector[2];

  // a count has been updated, this hazard is no longer consistent across vehicles
  m_consistent_map[hazard_search_vector[0]] = "false";
  bool count_added = false;
  if (m_query_map[query_string].count(hazard) > 0) {
    if (m_query_map[query_string][hazard].count(vname) > 0) {
      if (m_query_map[query_string][hazard][vname].count(config_id) > 0) {
        m_query_map[query_string][hazard][vname][config_id][query_string]++;
        count_added = true;
      }
    }
  }
  // hazard has not been seen before with this exact configuration
  if (!count_added) {
    m_query_map[query_string][hazard][vname][config_id][query_string]=1;
  }
}


//---------------------------------------------------------
// GetCount (protected)

string HazardSearch::getCount(string hazard_search_id,
                              string query_string){

  // parse the hazard search id string 
  vector<string> hazard_search_vector = HazardSearch::ParseHazardSearchID(hazard_search_id);
  string hazard, vname, config_id;
  hazard    = hazard_search_vector[0];
  vname     = hazard_search_vector[1];
  config_id = hazard_search_vector[2];

  // abstract function for extracting the count from a particular map
  if (m_query_map[query_string].count(hazard) > 0) {
    if (m_query_map[query_string][hazard].count(vname) > 0) {
      if (m_query_map[query_string][hazard][vname].count(config_id) > 0) {
        return(to_string(m_query_map[query_string][hazard][vname][config_id][query_string]));
      }
    }
  }
  return("");
}


//---------------------------------------------------------
// AddExpectedDetection

void HazardSearch::AddExpectedDetection(string hazard_search_id){
  HazardSearch::addCount(hazard_search_id, "num_expected_detections");
}


//---------------------------------------------------------
// AddDetection

void HazardSearch::AddDetection(string hazard_search_id){
  HazardSearch::addCount(hazard_search_id, "num_detections");
}


//---------------------------------------------------------
// AddRequestedClassification

void HazardSearch::AddRequestedClassification(string hazard_search_id)
{
  HazardSearch::addCount(hazard_search_id, "num_requested_classifications");
}


//---------------------------------------------------------
// AddClassification

void HazardSearch::AddClassification(string hazard_search_id){
  HazardSearch::addCount(hazard_search_id, "num_classifications");
}


//---------------------------------------------------------
// GetExpectedDetectionCount

string HazardSearch::GetExpectedDetectionCount(string hazard_search_id){
  return(HazardSearch::getCount(hazard_search_id, "num_expected_detections"));
}


//---------------------------------------------------------
// GetDetectionCount

string HazardSearch::GetDetectionCount(string hazard_search_id){
  return(HazardSearch::getCount(hazard_search_id,"num_detections"));
}


//---------------------------------------------------------
// GetClassificationRequestCount

string HazardSearch::GetRequestedClassificationCount(string hazard_search_id){
    return(HazardSearch::getCount(hazard_search_id, "num_requested_classifications"));
}


//---------------------------------------------------------
// GetClassificationCount

string HazardSearch::GetClassificationCount(string hazard_search_id){
    return(HazardSearch::getCount(hazard_search_id, "num_classifications"));
}


//---------------------------------------------------------
// GetConsistency

string HazardSearch::GetConsistency(string hazard){
  if (m_consistent_map.count(hazard) > 0) {
    return(m_consistent_map[hazard]);
  }
  return("");
}


//---------------------------------------------------------
// setExpectedDetectionQuery

void HazardSearch::setExpectedDetectionQuery()
{
  m_query_map["num_expected_detections"] = m_detect_map;
}


//---------------------------------------------------------
// setDetectionQuery

void HazardSearch::setDetectionQuery()
{
  m_query_map["num_detections"] = m_detect_map;
}


//---------------------------------------------------------
// setRequestedClassificationQuery

void HazardSearch::setRequestedClassificationQuery()
{
  m_query_map["num_requested_classifications"] = m_classify_map;
}


//---------------------------------------------------------
// setClassificationQuery

void HazardSearch::setClassificationQuery()
{
  m_query_map["num_classifications"] = m_classify_map;
}


