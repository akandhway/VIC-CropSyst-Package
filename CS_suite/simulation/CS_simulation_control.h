#ifndef CS_simulation_controlH
#define CS_simulation_controlH
#ifndef eventH
#  include "common/simulation/event.h"
#endif
#ifndef parameters_datarecH
#  include "corn/parameters/parameters_datarec.h"
#endif
#  include "corn/OS/directory_entry_name.h"
#define LABEL_scenario "scenario"
#include "CS_suite/simulation/CS_identification.h"
namespace CS {
#define INSPECTION_CONTINUOUS_BIT              1
#define INSPECTION_ANNUAL_BIT                  2
#define INSPECTION_CONTINUOUS_ANNUAL_RESET_BIT 4
//______________________________________________________________________________
class Simulation_control
: public Common_parameters_data_record                                           //051102 Simulation_parameters
{
 public:
   // These are the overall control parameters
   // These are applicable to all simulation units.
   CS::Identification ID;                                                        //160222
   CORN::date32      start_date_raw; CORN::Date_cowl_32 start_date;              //170525_110101
   CORN::date32      stop_date_raw;  CORN::Date_cowl_32 stop_date;               //170525_110101

   Common_event_list control_event_list;  // For event parameters
      // These events would include operations common between simulation units
      // overall output control options.
   bool rotation_cycling;                                                        //151008
      // Rotation cycling is used to get representive seasonal results
      // for every year of output, the model will create internally
      // and land unit simulation for each year of the rotation cycle
      // and generates a set of management shifting the dates by one each year.
      // This is provided as an option in CropSyst proper.
      // REACCH and OFoot models currently aways enable (override this option).
   //now in all versions #if (CS_VERSION>=5)
   nat32 inspection_levels;                                                      //150919
      // probably obsolete using seclusion
      // bitmask identifies inspection options
      // 0 no inspection
      // 1 continuous
      // 2 annual
      // 4 continuous annual reset (conceptual)
   bool UED_recording;                                                           //160621
      // what to record is specified using seclusion
 protected:
   nat32 examination_statistics_mask;                                            //180318
 public:
   Simulation_control();
   virtual ~Simulation_control();
 public: // Common_parameters_data_record implementations
   virtual bool expect_structure(bool for_write = false);                        //161023
   inline virtual const char *get_primary_section_name()                   const
      { return LABEL_scenario;};
   inline virtual Common_operation *get_operation                                //030808
      (const char *operation_type_str
      ,const std::string &operation_ID
      ,modifiable_ CORN::OS::File_name  &operation_filename                      //120913
         // the fully qualified operation filename may be returned (to be output to the schedule)
      ,int32 resource_set_ID,bool &relinquish_op_to_event)
      {  UNUSED_arg(operation_type_str); UNUSED_arg(operation_ID);
         UNUSED_arg(operation_filename);UNUSED_arg(resource_set_ID);
         UNUSED_arg(relinquish_op_to_event);
         return 0; }
    // Derived classes (I.e. CANMS) may store operation parameters in separate files.
};
//_Simulation_control_______________________________________________2003-02-03_/
}//_namespace_CS_______________________________________________________________/
#endif


