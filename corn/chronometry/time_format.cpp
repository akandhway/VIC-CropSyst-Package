#  include "corn/chronometry/time_format.h"
#  include <string.h>

namespace CORN
{
//______________________________________________________________________________
Time_format::Time_format
(nat8       _ordering
,nat8       _styles                                                              //991220
,nat8       _separator)
: ordering (_ordering)                                                           //991220
, styles   (_styles)                                                             //991220
, separator(_separator)
{}
//_1999-11-15___________________________________________________________________
Time_format::Time_format
(const Time_format &/*to_copy_from*/)
{}
// Copy Constructor
//_2003-07-15___________________________________________________________________
Time_format::Time_format(const char *format_string)
: ordering(0)
, styles(T_24hour)
, separator(0)
{    set_string(format_string);
}
//_1999_________________________________________________________________________
bool Time_format::set_string(const std::string &format_string)contribution_
{   return set_cstr(format_string.c_str());
}
//_2014-011-20__________________________________________________________________
bool Time_format::set_cstr(const char *format_string)     contribution_
{  uint8 new_ordering = 0;
   //NYN uint8 new_styles = 0;
   uint8 new_separator = 0;
   if (!format_string) return false;
   if (format_string[0] == 0)                                                    //171017
   {  ordering = T_omit;                                                         //171017
      return true;                                                               //171017
   }
   bool has_hour = false;
   bool has_minute = false;
   bool has_second = false;
   for (int i = 0; i < (int)strlen(format_string); i++)
   {  switch (format_string[i])
      {  case 'H' :
            has_hour = true;
            if (format_string[i+1] == 'H')   styles = T_military;
         break;
         case 'M' :  if ((new_separator) || (format_string[i+2] == 'S')) // Make sure it isn't month from a date-time combined format
                     has_minute = true;
                     break;
         case 'S' :  has_second = true;            break;
         case ':' :  new_separator = ':';              break;
         case '.' :  new_separator = '.';              break;
      }
   }
   if (has_hour)
      if (has_minute)
         if (has_second)   new_ordering = T_HMS;
         else              new_ordering = T_HM;
      else                 new_ordering = T_H;
   else
      if (has_minute)
         if (has_second)   new_ordering = T_MS;
         else              new_ordering = T_M;
      else  if (has_second)new_ordering = T_S;  // else this doesn't appear to be a time format or date time format.
   ordering = new_ordering;
   //NYI   styles   = new_styles;  should scan for am/pm indicator
   styles = T_scientific; // currently assuming scientific 24 hour
   separator= new_separator;
   return true;
}
//_2005-04-12___________________________________________________________________
bool Time_format::copy_format(const Time_format &to_copy_from)     contribution_
{  ordering    =(to_copy_from.ordering);
   styles      =(to_copy_from.styles);
   separator   =(to_copy_from.separator);
   return true;
}
//_2005-04-12_1999-11-15________________________________________________________
bool Time_format::clear()                                          contribution_
{
   ordering = T_omit;
   styles = 0;
   separator = 0;
   return true;
}
//_2016-06-06___________________________________________________________________
Time_format ISO_8601_time_format;                                                //170428
}//_namespace_CORN__________________________________________________2016-06-06_/


