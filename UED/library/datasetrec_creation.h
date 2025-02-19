#ifndef datasetrec_creationH
#define datasetrec_creationH

#ifndef UED_TYPES_H
#  include "UED/library/UED_types.h"
#endif
#include "UED/library/record.h"

#include "corn/format/binary/binary_record.h"
namespace UED {
//______________________________________________________________________________
// Template for literal constant creation record
// UED::Data_record_creation_parameters XXX_data_rec_creation {VariableCode, {RecType,UnitsCode,TimeStepUnits,TimeStampUnits,DefaultAttribute}};
//______________________________________________________________________________
struct Data_record_creation_options
{  CORN::Record_code       record_type;            //   (structs   cannot have objects in structs    Record_code_clad record_type;)
   UED_units_code          stored_units_code;      // This is the units that datum will be stored as in the UED file (was   preferred_units_code)
   UED_units_code          timestep_units_code;    // datum time step
   UED_units_code          time_stamp_step_units_code;                           //050415
      // can be UT_arbitrary_date for daily data where the simulation starting data is arbitrary
   // This indicates how the time series are records split into records
   // time_stamp_step_units_code = 0 indicate an arbitrary time stamp
   UED_quality_attribute   default_attribute_code;
   // Normally data records are timestamped such that the date or time stamp
   // corresponds to the first time index for the time series time step.
   // (I.e.  Jan 1,  midnight etc..)
   // if num_values is 0, this means the user want records sized for the specified timestep
   // This is usually the normal situation,  for applications such as cropsyst crop variables
   // the records may be time stepped a some arbitrary time with an indeterminate number of values
   // In any case the Unum_values should be obsolete
};
//_Data_record_creation_options_____________________________________2007-02-14_/
struct Data_record_creation_parameters
{  UED::Variable_code           variable_code;
   Data_record_creation_options options;
   // I would prefer to use an anonymous class here, but it is not standard C++
};
//_Data_record_creation_parameters__________________________________2007-02-14_/
class Data_record_creation_layout   //  Rename this to UED_data_record_creator
: public Record_type_distinguisher_interface                                     //990107
, public Data_record_creation_parameters                                         //070214
{
 public:
   // When setting values in a UED, a time series record might
   // not exist in the database file for the specified variable
   // encompassing the specified date, thus it will be necessary
   // to create a new record.
   // The creation layout specifies the time step, preferred Units,
   // variable code etc. to be used when creating the new record.
 public: // 'structors
   Data_record_creation_layout
      (CORN::Record_code   record_type_code_
      ,UED::Variable_code   variable_code_
      ,UED_units_code      preferred_units_code_
      ,UED_units_code      time_step_units_code_
      ,UED_units_code      time_stamp_step_units_code_                           //050415
      ,CORN::Quality_code  default_attribute_code_);
   Data_record_creation_layout
      (const Data_record_creation_parameters &creation_parameters);
   inline virtual ~Data_record_creation_layout() {}                              //170216
   // Conceptual I should add a constructor that takes a data record
   // and clones its creationg
 public:
   Data_record_creation_layout(); // default constructor (Don't use)             //970909
   inline virtual nat16 get_code()         const { return options.record_type ;} //990105
   Record_code_clad &get_record_type()                                    const; //070214
   inline void allow_arbitrary_time_stamp()                        modification_
      { options.time_stamp_step_units_code = 0 ; }                               //020530
      // I think this is prettymuch obsolete because we now have time_stamp_step_units
 private: // This is just a return buffer
   mutable Record_code_clad record_type_clad;                                   //070214
};
//_Data_record_creation_layout_class_____________________________________1997?_/
}//_namespace UED______________________________________________________________/
#endif

