#ifndef generiec_datarecH
#define generiec_datarecH

/*
This class is used with Data_source::render_generic_data_record
*/

//170525 #include "corn/datetime/date.hpp"
#include "corn/data_source/datarec.h"

namespace CORN {
//______________________________________________________________________________
class Generic_data_record
: public Data_record
{
public: //temporarily public
   // Currently I am just reserving an quick and dirty abritrary  number of values for each type
   // I need to modify the constructor to specify the number and allocate
   // the memory dynamically
   std::string strings[20];nat16 string_count;
   float32  floats[200];   nat16 float_count;
   int32    int32s[200];   nat16  int_count;                                     //170511_111107
   bool     bools[20];     nat16 bool_count;
   // NYI other field types
public:
   Generic_data_record(const char *primary_section);
   inline nat16 get_floats_count()                                        const
      { return 200; }
   // Gets from the current section
   int16  get_as_int16(const char *entry_name)                            const;
   int32  get_as_int32(const char *entry_name)                            const;
   ///NYN nat32  get_as_nat32(const char *entry_name) const;
   // NYI need gets for all field types
   int16 set_as_int16(const char *entry_name, int16 value) ;
   int16 set_as_int32(const char *entry_name, int32 value) ;
   const std::string &get_as_string(const char *entry_name)               const;
};
//______________________________________________________________________________
}//_namespace_CORN_____________________________________________________________/

#endif

