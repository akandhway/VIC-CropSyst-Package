#ifndef recorder_datarecH
#define recorder_datarecH

#define MAX_VARIABLES_TO_RECORD 1000
#define MAX_RECORDING_OBJECTS     10

#ifndef uedH
#  include "UED/library/UED.h"
#endif
#ifndef datarecH
#  include "corn/data_source/datarec.h"
#endif

namespace CORN {
class Recording_object; // forward decl.
class Recorder_float32;         // forward decl.
class Recorder_float64;         // forward decl.
//______________________________________________________________________________
class Desired_variables_list
{
protected:
   uint16               count;          // When 0 allow recorders to record all variables, otherwise just the desired variables
   UED::Variable_code    desired_vars[MAX_VARIABLES_TO_RECORD];
public:
   inline Desired_variables_list()
      : count(0)
      {}
   void desire(UED::Variable_code new_var);
   void desire(UED::Variable_code new_vars[]);
   bool is_desired(UED::Variable_code variable_code);
   // Searches the list of desired variables for variable_code and returns true if it is listed.
   inline bool desire_all_variables()                      { return count == 0;}
};
//_Desired_variables_list______________________________________________________/
class Recorder_data_record
: public Data_record
, public Desired_variables_list
{
// Recording objects are no longer needed, we simply add or delete the respective sections we are interested in
//   Recording_object  *recording_objs[MAX_RECORDING_OBJECTS];
public:
   Date_time_clad_64 date_time;
    // of the current record entry.
public:
   Recorder_data_record();
   // Wo expect_structure() setup structure as recorders are added.
   void remember(Recording_object *obj_to_remember);
   void forget  (Recording_object *obj_to_forget)               disassociation_;
   void expect_recorder(Recorder_float32 *recorder);
   void expect_recorder(Recorder_float64 *recorder);
   // Appends the recorder to the current section;
   inline datetime64 set_date_time(datetime64 date_time64_)
      { date_time.set_datetime64(date_time64_); return date_time64_; }
};
//_Recorder_data_record________________________________________________________/
}//_namespace CORN_____________________________________________________________/

#endif

