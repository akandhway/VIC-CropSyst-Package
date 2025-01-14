
#include "corn/parameters/number_keyed.h"
#include "corn/string/strconv.hpp"
#include <assert.h>
namespace CORN {
//______________________________________________________________________________
bool Number_keyed::set_str(const char *value_as_cstr)              modification_
{  bool result = false;
   if (value_as_cstr)   // Not sure if atof already checks for nul
   {  set_value_as_float64(atof(value_as_cstr));
      result = true;
   }
   return result;
   // Currently almost always returning true,
   // but may also want to return false if value_cstr does not represent a number
   // or atof returns positive or negative HUGE_VAL because the number is out of range.
}
//______________________________________________________________________________
bool Number_keyed::set_wstr(const wchar_t *value_as_wstr)          modification_
{  bool result = false;
   if (value_as_wstr)   // Not sure if atof already checks for nul
   {  set_value_as_float64(wstr_to_float64(value_as_wstr));
      result = true;
   }
   return result;
}
//_set_wstr_________________________________________________________2015-01-12_/
bool Number_keyed::is_float()                                              const
{  return (mode >= DT_float32) && (mode <= DT_float64);
}
//______________________________________________________________________________
float64 Number_keyed::get_value_as_float64()                               const
{  float64 value64 = 0.0;
   switch (mode)
   {  case DT_int8:   value64 = (float64)*(value.as_int8)   ; break;
      case DT_int16:  value64 = (float64)*(value.as_int16)  ; break;
      case DT_int32:  value64 = (float64)*(value.as_int32)  ; break;
      case DT_nat8:   value64 = (float64)*(value.as_nat8)   ; break;
      case DT_nat16:  value64 = (float64)*(value.as_nat16)  ; break;
      case DT_nat32:  value64 = (float64)*(value.as_nat32)  ; break;
      case DT_float32:value64 = (float64)*(value.as_float32); break;
      case DT_float64:value64 = (float64)*(value.as_float64); break;
   }
   return value64;
}
//______________________________________________________________________________
float32 Number_keyed::get_value_as_float32()                               const
{  return (float32) get_value_as_float64();
}
//_2015-01-11___________________________________________________________________
float64 Number_keyed::set_value_as_float64(float64 new_value64)modification_
{  switch (mode)
   {  case DT_int8:   *(value.as_int8)   = (int8)   new_value64; break;
      case DT_int16:  *(value.as_int16)  = (int16)  new_value64; break;
      case DT_int32:  *(value.as_int32)  = (int32)  new_value64; break;
      case DT_nat8:   *(value.as_nat8)   = (nat8)   new_value64; break;
      case DT_nat16:  *(value.as_nat16)  = (nat16)  new_value64; break;
      case DT_nat32:  *(value.as_nat32)  = (nat32)  new_value64; break;
      case DT_float32:*(value.as_float32)= (float32)new_value64; break;
      case DT_float64:*(value.as_float64)= (float64)new_value64; break;
   }
   return get_value_as_float64();
}
//______________________________________________________________________________
float32 Number_keyed::set_value_as_float32(float32 new_value)      modification_
{  return (float32)set_value_as_float64((float64) new_value);
}
//_2015-01-11___________________________________________________________________
const char *Number_keyed::append_value_in_radix_to
(std::string &buffer,nat8 precision,nat8 radix_preferred)                  const
{
   nat8 radix = (radix_preferred == 0) ? 10.0 : radix_preferred;
   switch (mode)
   {  case DT_int8:   append_int8_to_string  (*(value.as_int8) ,buffer,radix);break;
      case DT_int16:  append_int16_to_string (*(value.as_int16),buffer,radix);break;
      case DT_int32:  append_int32_to_string (*(value.as_int32),buffer,radix);break;
      case DT_nat8:   append_nat8_to_string  (*(value.as_nat8) ,buffer,radix);break;
      case DT_nat16:  append_nat16_to_string (*(value.as_nat16),buffer,radix);break;
      case DT_nat32:  append_nat32_to_string (*(value.as_nat32),buffer,radix);break;
      case DT_float32:append_float32_to_string(*(value.as_float32),precision,buffer);break;
      case DT_float64:append_float64_to_string(*(value.as_float64),precision,buffer);break;
   }
   return  buffer.c_str();
}
//______________________________________________________________________________
const char * Number_keyed::know_key_cstr(char const *_key_cstr)       cognition_
{  key_is_wide = false;
   return key.as_cstr = _key_cstr;
}
//______________________________________________________________________________
const wchar_t * Number_keyed::know_key_wcstr(wchar_t const *_key_wstr) cognition_
{  key_is_wide = true;
   return key.as_wstr = _key_wstr;
}
//______________________________________________________________________________
bool Number_keyed::is_key_string(const std::string &key_)           affirmation_
{
   assert(!key_is_wide);
   return key_.compare(key.as_cstr) == 0;
}
//_is_key_string____________________________________________________2018-08-20_/
bool Number_keyed::is_key_wstring(const std::wstring &key_)         affirmation_
{
   assert(key_is_wide);
   return key_.compare(key.as_wstr) == 0 ;
}
//_is_key_wstring___________________________________________________2018-08-20_/
Number_keyed::Number_keyed()
: mode(DT_float64)
{  value.as_int8 = 0;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(int8    &value_)
: mode                (DT_int8)
{  value.as_int8 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(nat8    &value_)
: mode                (DT_nat8)
{  value.as_nat8 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(int16   &value_)
: mode                (DT_int16)
{  value.as_int16 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(nat16   &value_)
: mode                (DT_nat16)
{  value.as_nat16 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(int32   &value_)
: mode                (DT_int32)
{  value.as_int32 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(nat32   &value_)
: mode                (DT_nat32)
{  value.as_nat32 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(float32 &value_)
: mode                (DT_float32)
{  value.as_float32 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(float64 &value_)
: mode                (DT_float64)
{  value.as_float64 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(const int8    &value_)
: mode                (DT_int8)
{  value.as_const_int8 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(const nat8    &value_)
: mode                (DT_nat8)
{  value.as_const_nat8 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(const int16   &value_)
: mode                (DT_int16)
{  value.as_const_int16 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(const nat16   &value_)
: mode                (DT_nat16)
{  value.as_const_nat16 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(const int32   &value_)
: mode                (DT_int32)
{  value.as_const_int32 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(const nat32   &value_)
: mode                (DT_nat32)
{  value.as_const_nat32= &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(const float32 &value_)
: mode                (DT_float32)
{  value.as_const_float32 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
Number_keyed::Number_keyed(const float64 &value_)
: mode                (DT_float64)
{  value.as_const_float64 = &value_;
   key.as_cstr = 0;
}
//______________________________________________________________________________
}//_namespace_CORN_____________________________________________________________/

