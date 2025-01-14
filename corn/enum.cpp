#include "corn/enum.h"
#include <string.h>
#include <stdlib.h>
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#ifndef strconvHPP
#   include "corn/string/strconv.hpp"
#endif
// in UNIX, MSC and BC++ strtoul is in stdlib.h
// it might be in other header files for other compiler vendors
//namespace CORN {
//______________________________________________________________________________
Enum_abstract::Enum_abstract()
: simple_label_table(0)
, descriptive_label_table(0)
{}
// This constructor is for enumeration that are simply integer numbers
//_Enum_abstract:constructor________________________________________2005-09-29_/
Enum_abstract::Enum_abstract(const char **_label_table)
: simple_label_table(_label_table)
, descriptive_label_table(0)
{}
//_Enum_abstract:constructor________________________________________2005-09-29_/
Enum_abstract::Enum_abstract(Label_description *_descriptive_label_table )
: simple_label_table(0)
, descriptive_label_table(_descriptive_label_table)
{}
//_Enum_abstract:constructor________________________________________2005-09-29_/
bool Enum_abstract::is_label(const char *label)                     affirmation_
{  if (simple_label_table)
      for (int32 i = 0; simple_label_table[i] && strlen(simple_label_table[i]); i++)
        if (strcmp(label,simple_label_table[i]) == 0)
          return true;
   if (descriptive_label_table)
      for (int32 i = 0; descriptive_label_table[i].label != 0
           && strlen(descriptive_label_table[i].label); i++)
        if (strcmp(label,descriptive_label_table[i].label) == 0)
          return true;
   return false;
}
//_is_label_________________________________________________________2002-04-19_/
int32 Enum_abstract::set_label(const char *label)
{  int32 new_value = 0;
   if (simple_label_table)
   {  for (int32 i = 0; simple_label_table[i] && strlen(simple_label_table[i]); i++)
         if (strcmp(label,simple_label_table[i]) == 0)
          new_value = i;
      // If the label wasn't found in the table, try case insensitive search.
      if (!new_value)
         for (int32 i = 0; simple_label_table[i] && strlen(simple_label_table[i]); i++)
            if (stricmp(label,simple_label_table[i]) == 0)
               new_value = i;
   } else if (descriptive_label_table)
   {  for (int32 i = 0; descriptive_label_table[i].label && strlen(descriptive_label_table[i].label); i++)
         if (strcmp(label,descriptive_label_table[i].label) == 0)
          new_value = i;
      // If the label wasn't found in the table, try case insensitive search.
      if (!new_value)
         for (int32 i = 0; descriptive_label_table[i].label && strlen(descriptive_label_table[i].label); i++)
            if (stricmp(label,descriptive_label_table[i].label) == 0)
               new_value = i;
   } else // simple integer enumeration (I.e. soil layers
        new_value = atoi(label);
   set_int32(new_value);
   return new_value;
}
//_2002-03-21 __________________________________________________________________
const char *Enum_abstract::append_label(std::string &return_buffer)        const
{  int32 label_index = get_int32();
   if (simple_label_table)             return_buffer.append(simple_label_table[label_index]);
   else if (descriptive_label_table)   return_buffer.append(descriptive_label_table[label_index].label);
   else CORN::append_nat32_to_string(label_index,return_buffer);
   return return_buffer.c_str();
}
//_append_label_____________________________________________________2019-01-09_/
std::ostream &Enum_abstract::stream_label(std::ostream &stream)       stream_IO_
{  int32 label_index = get_int32();
   #ifdef __BCPLUSPLUS__
   if (simple_label_table)             stream << simple_label_table[label_index];
   else if (descriptive_label_table)   stream << descriptive_label_table[label_index].label;
   else stream << label_index;
   #endif

   // WARNING Qt is not compiling this

   return stream;
}
//_stream_label_____________________________________________________2019-01-15_/
const char *Enum_abstract::append_description(std::string &return_buffer)  const //190106
{  if (descriptive_label_table)
        return_buffer.append(descriptive_label_table[get_int32()].description);
   else append_label(return_buffer);
   return return_buffer.c_str();                                                 //190106
}
//_append_description__________________________________2019-01-09__2005-09-29_/
nat16 Enum_abstract::get_max_label_length()                                const
{  nat16 max_label_len  = 1; // Must be at least 1 character
   // Returns the longest label length.
   // This is used to determine the field size for Data_records enumeration attributes.
   if (simple_label_table)
       for (nat16 i = 0; simple_label_table[i] && strlen(simple_label_table[i]); i++)
           max_label_len = std::max<nat16>(max_label_len,(nat16)strlen(simple_label_table[i]));
   else if (descriptive_label_table)
       for (nat16 i = 0; descriptive_label_table[i].label && strlen(descriptive_label_table[i].label); i++)
           max_label_len = std::max<nat16>(max_label_len,(int)strlen(descriptive_label_table[i].label));
   else max_label_len  = 6;   // For enumerated label tabels
   return max_label_len;
}
//_get_max_label_length_____________________________________________2005-09-29_/
//}//_namespace_CORN_____________________________________________________________/

