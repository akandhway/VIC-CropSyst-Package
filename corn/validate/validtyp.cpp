// #error This is obsolete  200204
// Instead used parameter classes or ontologies


#ifndef compareHPP
#  include <corn/math/compare.hpp>
#endif
#include "string.h"
#include "corn/validate/validtyp.h"
#include "corn/string/strconv.hpp"
#include "corn/validate/validtyp.pc"
namespace CORN {
//______________________________________________________________________________
Valid_type::Valid_type
(const char     *i_key
, Phrase_label i_description1
, Phrase_label i_description2)
: key(0)                                                                         //020129
,description1(i_description1)
,description2(i_description2)
,status(ready_status)                                                            //000305
{  set_key(i_key);                                                               //020129
}
//______________________________________________________________________________
//          This constructor is for use with common translator
Valid_type::Valid_type(const Valid_type &to_copy)
: key(0)                                                                         //020129
,description1(to_copy.description1)
,description2(to_copy.description2)
,status(ready_status)                                                            //000305
{  set_key(to_copy.key);                                                         //020129
}
//______________________________________________________________________________
Valid_type::~Valid_type()
{  if (key) delete [] key; key = 0;                                              //020329
}
//_~Valid_type______________________________________________________1998-05-08_/
void Valid_type::set_key(const char *i_key)
{  int key_length = i_key ? strlen(i_key)+1 : 1;
   //201129LML if (key) delete key;
   if (key) delete [] key; key = 0;
   key = new char[key_length];
   key[0] = 0;
   if (i_key) strcpy(key,i_key);
}
//_set_key__________________________________________________________2002-01-29_/
void Valid_number::compose_range_message
(char *message_text
,bool HTML_format
,bool must
,double low
,double high
,double value
,Phrase_label units
,Phrase_label //label1
,Phrase_label //label2
,Phrase_label label3)
{  if (!HTML_format)                                                             //010307
   {  if (strlen(label3))
         strcat(message_text," ("); strcat(message_text,label3); strcat(message_text,")");
      strcat(message_text,"\n = ");
      strcat(message_text,float64_to_cstr(value,nat8_decimals));
      strcat(message_text," "); strcat(message_text,units);strcat(message_text," ");
      strcat(message_text,TL_is_not_a_reasonable_value);
      strcat(message_text,", \n");
   }
   if (!CORN::is_approximately<double>(low, std::numeric_limits<float32>::min(),0.0000001) && !CORN::is_approximately<double>(high, std::numeric_limits<float32>::max(),0.0000001)) //010307
   {  const char * should_must = (must) ? TL_must_range_from : TL_should_range_from;
      strcat(message_text,should_must);
      strcat(message_text," ");
      strcat(message_text,float64_to_cstr(low,nat8_decimals));
      strcat(message_text," - ");
      strcat(message_text,float64_to_cstr(high,nat8_decimals));
      strcat(message_text,"\n");
   }
   else if (! CORN::is_approximately<double>(high, std::numeric_limits<float32>::max(),0.0000001))          //990310
   { const char * should_must = (must) ? TL_must_be_less_than : TL_should_be_less_than;
      strcat(message_text,should_must);
      strcat(message_text," ");
      strcat(message_text,float64_to_cstr(high,nat8_decimals));
      strcat(message_text,"\n");
   }
   else
   { const char * should_must = (must) ? TL_must_be_greater_than : TL_should_be_greater_than;
      strcat(message_text,should_must);
      strcat(message_text," ");
      strcat(message_text,float64_to_cstr(low,nat8_decimals));
      strcat(message_text,"\n");
   }
}
// I haven't found strstream in UNIX yet!
//_compose_range_message_______________________________________________________/
const char *HTML_validation_colors[] =
{"LIGHTBLUE"   // unavailable_status
,"MAGENTA"     // missing_status
,"RED"         // error_low_status
,"RED"         // error_status
,"RED"         // error_high_status
,"ORANGE"      // pot_error_low_status
,"ORANGE"      // pot_error_status
,"ORANGE"      // pot_error_high_status
,"YELLOW"      // warning_low_status
,"YELLOW"      // warning_status
,"YELLOW"      // warning_high_status
,"limegreen"   // ready_status
,"lightgreen"  // good_status
,"GRAY"        // disabled_status
};
//_HTML_validation_colors___________________________________________2001-09-17_/
int RUI_colored_resource_index[] =
{0
,1
,2,2,2
,3,3,3
,4,4,4
,5
,6
,0
};
//______________________________________________________________________________
Validation_status Valid_number::validate_value
(char *message_text,bool HTML_format, float64 i_value)                           //020129
{  status = good_status;
   char range_message[MESSAGE_TEXT_SIZE]; range_message[0] = 0;                  //051227
   if (HTML_format)
         strcat(message_text,"<TR><TD>");
   else  { strcat(message_text,(TL_Validating)); strcat(message_text,":"); }
   strcat(message_text,description1?description1: "");
   if (HTML_format)
         strcat(message_text,"<BR>");
   else  strcat(message_text," ");
   strcat(message_text,description2? description2 : "");
   if (!CORN::is_approximately<float64>(min_warning, std::numeric_limits<float32>::min(),0.0000001) && (i_value < min_warning))
      status = warning_low_status;
   if (!CORN::is_approximately<float64>(max_warning,std::numeric_limits<float32>::max(),0.0000001) && (i_value > max_warning))
      status = warning_high_status;
   if (!CORN::is_approximately<float64>(min_error,std::numeric_limits<float32>::min(),0.0000001) && (i_value < min_error))
      status = error_low_status;
   if (!CORN::is_approximately<float64>(max_error,std::numeric_limits<float32>::max(),0.0000001) && (i_value > max_error))
      status = error_high_status;
   if ((status == error_low_status) || (status == error_high_status))
      compose_range_message(range_message,HTML_format,true,min_error,max_error,i_value,units_description,description1,description2,"");
   else
   if ((status == warning_low_status) || (status == warning_high_status))
      compose_range_message(range_message,HTML_format,false,min_warning,max_warning,i_value,units_description,description1,description2,"");
   if (HTML_format)
         { strcat(message_text,"<TD BGCOLOR=\""); strcat(message_text,HTML_validation_colors[status]); strcat(message_text,"\">"); }
   else  strcat(message_text," = ");
   strcat(message_text,float64_to_cstr(i_value,nat8_decimals));
   if (HTML_format)
      strcat(message_text,"<TD>");
   if (units_description)
      {strcat(message_text," "); strcat(message_text,units_description); }
   if (HTML_format)                                                              //010307
   {
      strcat(message_text,"<TD BGCOLOR=YELLOW>");
      strcat(message_text,float64_to_cstr(min_warning,nat8_decimals));
      strcat(message_text,"<TD BGCOLOR=YELLOW>");
      strcat(message_text,float64_to_cstr(max_warning,nat8_decimals));
      strcat(message_text,"<TD BGCOLOR=RED>");
      strcat(message_text,float64_to_cstr(min_error,nat8_decimals));
      strcat(message_text,"<TD  BGCOLOR=RED>");
      strcat(message_text,float64_to_cstr(max_error,nat8_decimals));
      strcat(message_text,"<TD>");
   }
   if (status < ready_status)
      strcat(message_text,range_message);
   if (!HTML_format)                                                             //010307
      strcat(message_text,":");
   strcat(message_text,(TL_Valid)); strcat(message_text,"\n");
   return status;
}
//_validate_value___________________________________________________1998-04-01_/
const char *Valid_type::log_cstr(char *buffer)                             const
{  strcpy(buffer,key);
   strcat(buffer,"=");
   strcat(buffer,get_value_c_str());
   strcat(buffer," ");
   if (description1)
      strcat(buffer,description1);
   strcat(buffer," ");
   if (description2)
      strcat(buffer,description2);
   strcat(buffer,"\n");
   return buffer;
}
//_log_cstr_________________________________________________________2003-01-23_/
const char *Valid_number::log_cstr(char *buffer)                           const
{  Valid_type::log_cstr(buffer);
   strcat(buffer,key);
   strcat(buffer,"_units=");
   strcat(buffer,units_description);
   strcat(buffer,"\n");
   return buffer;
}
//_log_cstr_________________________________________________________2003-01-23_/
char c_str_buffer[30];
const char *Valid_float32::get_value_c_str()                               const
{  return float32_to_cstr(value,nat8_decimals);
}
//_get_value_c_str__________________________________________________2002-08-12_/
const char *Valid_float64::get_value_c_str()                               const
{  return float64_to_cstr(value,nat8_decimals);
}
//_get_value_c_str__________________________________________________2002-08-12_/
const char *Valid_int16::get_value_c_str()                                 const
{  int16_to_cstr(value,c_str_buffer,10);
   return c_str_buffer;
}
//_get_value_c_str__________________________________________________2002-08-12_/
const char *Valid_int32::get_value_c_str()                                 const
{  int32_to_cstr(value,c_str_buffer,10);
   return c_str_buffer;
}
//_get_value_c_str__________________________________________________2002-08-12_/
}//_namespace_CORN_____________________________________________________________/

