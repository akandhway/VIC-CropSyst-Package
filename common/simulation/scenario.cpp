#  include "common/simulation/scenario.h"
//______________________________________________________________________________
DECLARE_CONTAINER_ENUMERATED_SECTION(Common_event,comment_event_section_VV,true);
//150416 May want to put DECLARE_CONTAINER_ENUMERATED_SECTION under the name space
namespace CS {
//______________________________________________________________________________
bool Common_scenario::expect_structure(bool for_write)
{  bool expected = Common_parameters_data_record::expect_structure(for_write);   //161023
   structure_defined = false;                                                    //120314
   EXPECT_ENUMERATED_SECTION("event",comment_event_section_VV,parameter_event_list);
   set_current_section("simulation");
      expect_int32("start_date",start_date_raw);                                 //170628_170532_110101
      expect_int32("stop_date" ,stop_date_raw);                                  //170628_170532_110101
      #if (CS_VERSION <= 5)
      // Recognize or version 4 scenario files
      expect_int32("starting_date",start_date_raw);                              //170628_170532_110101
      expect_int32("ending_date" ,stop_date_raw);                                //170628_170532_110101
      #endif
   structure_defined = true;                                                     //120314
   return expected;
}
//______________________________________________________________________________
Common_scenario::Common_scenario()
//180626 (nat8  _major,nat8  _release,nat8  _minor)
: Common_parameters_data_record(OPTIONAL_ASSOCIATED_DIRECTORY,"section"
,CS_VERSION
//180626 ,_major, _release, _minor
)
, start_date_raw(0), start_date(start_date_raw)
, stop_date_raw(0),  stop_date (stop_date_raw)
,parameter_event_list()
{}
//______________________________________________________________________________
Common_scenario::~Common_scenario()
{  parameter_event_list.delete_all(); }
//______________________________________________________________________________
}//_namespace CS_______________________________________________________________/
// scenario.cpp
