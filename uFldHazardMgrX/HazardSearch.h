/************************************************************/
/*    NAME: Zach & Kristen                                  */
/*    ORGN: MIT                                             */
/*    FILE: HazardSearch.h                                  */
/*    DATE: 09 April 2019                                   */
/************************************************************/

#ifndef HAZARD_SEARCH
#define HAZARD_SEARCH

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


class HazardSearch
{
public:
  HazardSearch();
  ~HazardSearch(){};

  // hazard_search_id is a shorthand for describing the important information
  // from when a report was retrieved, which includes: 
  //  + the hazard being sensed
  //  + the vehicle that sensed the hazard
  //  + the sensor configuration of the vehicle when it was sensed 
  std::string GetHazardSearchID(std::string hazard, 
                                std::string vname, 
                                std::string config_id);

  std::vector<std::string> ParseHazardSearchID(std::string hazard_search_id);

  void AddExpectedDetection(std::string hazard_search_id);
  void AddDetection(std::string hazard_search_id);
  void AddRequestedClassification(std::string hazard_search_id);
  void AddClassification(std::string hazard_search_id);

  std::string GetExpectedDetectionCount(std::string hazard_search_id);
  std::string GetDetectionCount(std::string hazard_search_id);
  std::string GetRequestedClassificationCount(std::string hazard_search_id);
  std::string GetClassificationCount(std::string hazard_search_id);
  std::string GetConsistency(std::string hazard);

  // maps unique hazard to counts of detect, classify, and request 
  std::map<std::string, int> m_simple_detect_map;
  std::map<std::string, int> m_simple_classify_map;
  std::map<std::string, int> m_simple_request_map;

protected:
  // abstract functions for add and retrieving information from the report maps
  void        addCount(std::string hazard_search_id,
                       std::string query_string);

  std::string getCount(std::string hazard_search_id,
                       std::string query_string);

  // map query strings to correct map structure for later usage
  void setExpectedDetectionQuery();
  void setDetectionQuery();
  void setRequestedClassificationQuery();
  void setClassificationQuery();

  std::list<std::string> m_hazard_node_message_queue;

  // m_detect_map represents results of all detection reports
  // -------------------------------------------
  // {hazard:
  //    {vname:
  //       {config_id:
  //          {num_detect_expected: int},
  //          {num_detect_recieved: int}}}}
  std::map<std::string, 
    std::map<std::string, 
       std::map<std::string, 
          std::map<std::string, unsigned int> > > > m_detect_map;

  // m_classify_map represents results of all classify reports
  // -------------------------------------------
  // {hazard:
  //    {vname:
  //       {config_id:
  //          {num_classify_requested: int},
  //          {num_classify_receieved: int}}}}
  std::map<std::string, 
    std::map<std::string, 
       std::map<std::string, 
          std::map<std::string, unsigned int> > > > m_classify_map;

  // m_consistent_map represents which hazards are consistent across vehicles
  // ------------------------------------------- 
  // {hazard: boolean}
  std::map<std::string, std::string> m_consistent_map;

  // notation used for mapping a query string to the relevant map
  std::string m_expected_detection_query;
  std::string m_detection_query;
  std::string m_requested_classification_query;
  std::string m_classification_query;

  std::map<std::string,
    std::map<std::string, 
      std::map<std::string, 
         std::map<std::string, 
            std::map<std::string, unsigned int> > > > > m_query_map; 

};

#endif 
