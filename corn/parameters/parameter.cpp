#include "corn/parameters/parameter.h"
#include "corn/measure/units.h"
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <string.h>
#include <float.h>
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include "corn/validate/validtyp.ph"
#include "corn/parameters/parameter.h"
#include "corn/data_source/datarec.h"

namespace CORN {
//______________________________________________________________________________
Parameter_properties::~Parameter_properties()
{}
//______________________________________________________________________________
Parameter_abstract::Parameter_abstract
(const Parameter_properties_literal &properties_
,const Parameter_format             &format_)                                    //140131
:properties(properties_)
,format(format_)                                                                 //140131
,status(ready_status)
{}
//______________________________________________________________________________
Parameter_number::Parameter_number
(int8                               &value_
,const Parameter_properties_literal &properties_
,const Parameter_range              &range_
,const Parameter_format             &format_)                                    //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_int8 = &value_;
   initial_value = value_;
   mode          = DT_int8;                                                      //150103
   know_key_cstr(properties_.name_long);                                         //150103
//assert(format_.format.numeric_type != NT_float);
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(nat8    &value_,const Parameter_properties_literal &properties_
,const Parameter_range &range_,const Parameter_format &format_)                  //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_nat8 = &value_;
   initial_value = value_;
   mode          = DT_nat8;                                                      //150103
   know_key_cstr(properties_.name_long);                                         //150103
//assert(format_.format.numeric_type != NT_float);
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(int16   &value_,const Parameter_properties_literal &properties_
,const Parameter_range &range_, const Parameter_format &format_)                 //140131

:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_int16 = &value_;
   mode            = DT_int16;
   initial_value = value_;
   mode          = DT_int16;                                                     //150103
   know_key_cstr(properties_.name_long);                                         //150103
//assert(format_.format.numeric_type != NT_float);
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(nat16   &value_
,const Parameter_properties_literal &properties_
,const Parameter_range &range_ ,const Parameter_format &format_)                 //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_nat16 = &value_;
   initial_value = value_;
   mode          = DT_nat16;                                                     //150103
   know_key_cstr(properties_.name_long);                                         //150103
//assert(format_.format.numeric_type != NT_float);
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(int32   &value_
,const Parameter_properties_literal &properties_
,const Parameter_range &range_, const Parameter_format &format_)                  //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_int32 = &value_;
   initial_value = value_;
   mode          = DT_int32;                                                     //150103
   know_key_cstr(properties_.name_long);                                         //150103
//assert(format_.format.numeric_type != NT_float);
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(nat32   &value_,const Parameter_properties_literal &properties_
,const Parameter_range &range_,const Parameter_format &format_)                  //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_nat32 = &value_;
   initial_value = value_;
   mode          = DT_nat32;                                                     //150103
   know_key_cstr(properties_.name_long);                                         //150103
//assert(format_.format.numeric_type != NT_float);
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(float32 &value_,const Parameter_properties_literal &properties_
,const Parameter_range &range_,const Parameter_format &format_)                  //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_float32 = &value_;
   initial_value = value_;
   mode          = DT_float32;                                                   //150103
   know_key_cstr(properties_.name_long);                                         //150103
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(float64 &value_,const Parameter_properties_literal &properties_
,const Parameter_range &range_,const Parameter_format &format_)                  //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_float64 = &value_;
   initial_value = value_;
   mode          = DT_float64;                                                   //150103
   know_key_cstr(properties_.name_long);                                         //150103
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(const int8    &value_
,const Parameter_properties_literal &properties_
,const Parameter_range &range_,const Parameter_format &format_)                  //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_const_int8 = &value_;
   initial_value = value_;
   mode          = DT_int8;                                                      //150103
   know_key_cstr(properties_.name_long);                                         //150103
//assert(format_.format.numeric_type != NT_float);
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(const nat8    &value_,const Parameter_properties_literal &properties_
,const Parameter_range &range_,const Parameter_format &format_)                  //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_const_nat8 = &value_;
   initial_value = value_;
   mode          = DT_nat8;                                                      //150103
   know_key_cstr(properties_.name_long);                                         //150103
//assert(format_.format.numeric_type != NT_float);
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(const int16   &value_,const Parameter_properties_literal &properties_
,const Parameter_range &range_, const Parameter_format &format_)                 //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_const_int16 = &value_;
   initial_value = value_;
   mode          = DT_int16;                                                     //150103
   know_key_cstr(properties_.name_long);                                         //150103
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(const nat16   &value_
,const Parameter_properties_literal &properties_
,const Parameter_range &range_, const Parameter_format &format_)                 //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_const_nat16 = &value_;
   initial_value = value_;
   mode          = DT_nat16;                                                     //150103
   know_key_cstr(properties_.name_long);                                         //150103
//assert(format_.format.numeric_type != NT_float);
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(const int32   &value_
,const Parameter_properties_literal &properties_
,const Parameter_range  &range_, const Parameter_format &format_                 //140131
)
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_const_int32 = &value_;
   initial_value = value_;
   mode          = DT_int32;                                                     //150103
   know_key_cstr(properties_.name_long);                                         //150103
//assert(format_.format.numeric_type != NT_float);
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(const nat32   &value_,const Parameter_properties_literal &properties_
,const Parameter_range &range_,const Parameter_format &format_)                  //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_const_nat32 = &value_;
   initial_value = value_;
   mode          = DT_nat32;                                                     //150103
   know_key_cstr(properties_.name_long);                                         //150103
//assert(format_.format.numeric_type != NT_float);
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(const float32 &value_,const Parameter_properties_literal &properties_
,const Parameter_range &range_,const Parameter_format &format_)                  //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_const_float32 = &value_;
   initial_value = value_;
   mode          = DT_float32;                                                   //150103
   know_key_cstr(properties_.name_long);                                         //150103
}
//______________________________________________________________________________
Parameter_number::Parameter_number
(const float64 &value_,const Parameter_properties_literal &properties_
,const Parameter_range &range_,const Parameter_format &format_)                  //140131
:Parameter_abstract  (properties_,format_)
,range               (range_)                                                    //140131
,shown_units_code    (properties_.units_code)
,alt_units_codes(0)
{  value.as_const_float64 = &value_;
   initial_value = value_;
   mode          = DT_float64;                                                   //150103
   know_key_cstr(properties_.name_long);                                         //150103
}
//______________________________________________________________________________
void Parameter_number::reset_to_initial_value()                    modification_
{  switch (mode)
   {  case DT_int8:   *(value.as_int8)   = (int8)   initial_value; break;
      case DT_int16:  *(value.as_int16)  = (int16)  initial_value; break;
      case DT_int32:  *(value.as_int32)  = (int32)  initial_value; break;
      case DT_nat8:   *(value.as_nat8)   = (nat8)   initial_value; break;
      case DT_nat16:  *(value.as_nat16)  = (nat16)  initial_value; break;
      case DT_nat32:  *(value.as_nat32)  = (nat32)  initial_value; break;
      case DT_float32:*(value.as_float32)= (float32)initial_value; break;
      case DT_float64:*(value.as_float64)= (float64)initial_value; break;
   }
}
//______________________________________________________________________________
void Parameter_number::compose_range_message
(std::string             &message_text
,bool HTML_format
,bool must
,float64 low
,float64 high
,float64 value
,const char *  units
,const char *  //label1
,const char *  //label2
,const char *  label3)
{  if (!HTML_format)
   {  if (strlen(label3))
         message_text.append(" ("); message_text.append(label3); message_text.append(")");
      message_text.append("\n = ");
      message_text.append(float64_to_cstr(value,format.format.precision));
      message_text.append(" "); if (units) message_text.append(units);message_text.append(" ");
      message_text.append(TL_is_not_a_reasonable_value);
      message_text.append(", \n");
   }
   if (!CORN::is_approximately<float64>(low,DBL_MIN,0.0000001)
    && !CORN::is_approximately<float64>(high, DBL_MAX,0.0000001))
   {  const char * should_must = (must) ? TL_must_range_from : TL_should_range_from;
      message_text.append(should_must);
      message_text.append(" ");
      message_text.append(float64_to_cstr(low,format.format.precision));
      message_text.append(" - ");
      message_text.append(float64_to_cstr(high,format.format.precision));
      message_text.append("\n");
   }
   else if (! CORN::is_approximately<float64>(high,DBL_MAX,0.0000001))
   { const char * should_must = (must) ? TL_must_be_less_than : TL_should_be_less_than;
      message_text.append(should_must);
      message_text.append(" ");
      message_text.append(float64_to_cstr(high,format.format.precision));
      message_text.append("\n");
   }
   else
   { const char * should_must = (must) ? TL_must_be_greater_than : TL_should_be_greater_than;
      message_text.append(should_must);
      message_text.append(" ");
      message_text.append(float64_to_cstr(low,format.format.precision));
      message_text.append("\n");
   }

   // NYI want to add parameter explaination/description URL
}
//_compose_range_message_______________________________________________________/
const char *HTML_quality_colors[] =
{"limegreen"   // inviolate ready_status
,"lightgreen"  // ready_status
,"green"       // reserved
,"green"       // reserved
,"green"       // reserved
,"green"       // reserved
,"green"       // reserved
,"green"       // reserved
,"green"       // reserved
,"green"       // reserved
,"green"       // reserved
,"green"       // reserved
,"green"       // reserved
,"green"       // reserved
,"GRAY"        // disabled_status
,"BLUE"   // unavailable_status
,"RED"         // not valid
,"RED"         // RESERVED
,"MAGENTA"     // missing_status
,"YELLOW"      // warning_low_status
,"YELLOW"      // warning_status
,"YELLOW"      // warning_high_status
,"ORANGE"      // pot_error_low_status
,"ORANGE"      // pot_error_status
,"ORANGE"      // pot_error_high_status
,"RED"         // error_low_status
,"RED"         // error_status
,"RED"         // error_high_status
};
//_HTML_quality_colors_________________________________________________________/
const char *Parameter_number::get_units_label(std::string &units_description) const
{  char component_description [256];
   char component_abbr        [100];
   CORN::Units_clad smart_units(properties.units_code);
   smart_units.compose_description(component_description, component_abbr);
   units_description.assign(component_abbr);
   return  units_description.c_str();
}
//_get_units_label_____________________________________________________________/
Parameter_abstract::Validation_status Parameter_number::validate
(std::string &message_text,bool HTML_format)
{  return validate_value(message_text,HTML_format);
}
//_validate____________________________________________________________________/
Parameter_abstract::Validation_status Parameter_number::validate_value
(std::string &message_text,bool HTML_format )
{  status = Parameter_abstract::inviolate_status;
   std::string range_message;
   float32 value_ = (float32)get_value_as_float64();
   if (HTML_format)
      message_text.append("<TR><TD>");
//   else
//      { message_text.append((TL_Validating)); message_text.append(":"); } ;
   if (properties.caption)
        message_text.append(properties.caption);
   if (HTML_format)
         message_text.append("<BR>");
   else  message_text.append(" ");
//   message_text.append(properties.hint);
   if (!CORN::is_approximately<float32>(range.min_warning, std::numeric_limits<float32>::min(),0.0000001) && (value_ < range.min_warning))
      status = Parameter_abstract::warning_low_status;
   if (!CORN::is_approximately<float32>(range.max_warning,std::numeric_limits<float32>::max(),0.0000001) && (value_ > range.max_warning))
      status = Parameter_abstract::warning_high_status;
   if (!CORN::is_approximately<float32>(range.min_error,std::numeric_limits<float32>::min(),0.0000001) && (value_ < range.min_error))
      status = Parameter_abstract::error_low_status;
   if (!CORN::is_approximately<float32>(range.max_error,std::numeric_limits<float32>::max(),0.0000001) && (value_ > range.max_error))
      status = Parameter_abstract::error_high_status;
   if ((status == Parameter_abstract::error_low_status) || (status == Parameter_abstract::error_high_status))
    compose_range_message(range_message,HTML_format,true,range.min_error,range.max_error,value_,properties.prefered_units_caption,properties.caption,properties.hint,"");
   else
      if ((status == Parameter_abstract::warning_low_status) || (status == Parameter_abstract::warning_high_status))
         compose_range_message(range_message,HTML_format,false,range.min_warning,range.max_warning,value_,properties.prefered_units_caption,properties.caption,properties.hint,"");
   if (HTML_format)
      { message_text.append("<TD BGCOLOR=\""); message_text.append(HTML_quality_colors[status]); message_text.append("\">"); }
   else
      message_text.append(" = ");
   message_text.append(float64_to_cstr(value_,format.format.precision));
   if (HTML_format)
      message_text.append("<TD>");
   if (properties.prefered_units_caption!=NULL) {message_text.append(" ");
   if (properties.prefered_units_caption) message_text.append(properties.prefered_units_caption); }
   if (HTML_format)
   {  message_text.append("<TD BGCOLOR=YELLOW>");
      message_text.append(float32_to_cstr(range.min_warning,format.format.precision));
      message_text.append("<TD BGCOLOR=YELLOW>");
      message_text.append(float32_to_cstr(range.max_warning,format.format.precision));
      message_text.append("<TD BGCOLOR=RED>");
      message_text.append(float32_to_cstr(range.min_error,format.format.precision));
      message_text.append("<TD  BGCOLOR=RED>");
      message_text.append(float32_to_cstr(range.max_error,format.format.precision));
      message_text.append("<TD>");
   }
   if (status >= Parameter_abstract::not_valid_status)
         message_text.append(range_message);
   else  message_text.append((TL_Valid));
   if (!HTML_format)
      message_text.append(":");
   message_text.append("\n");
   return status;
}
//_validate_value______________________________________________________________/
const char *Parameter_abstract::append_VV_to(std::string &buffer)          const
{  buffer.append(properties.name_long);
   buffer.append("=");
   append_value(buffer);
   if (properties.prefered_units_caption)
   {  buffer.append(" ");
      buffer.append(properties.prefered_units_caption);
   }
  return buffer.c_str();
}
//_append_VV_to________________________________________________________________/
const char *Parameter_abstract::log(std::string &buffer)                   const
{  buffer.append(properties.name_long);
   buffer.append("=");
   append_value(buffer);
   buffer.append(" ");
   if (properties.prefered_units_caption)
      buffer.append(properties.prefered_units_caption);
   buffer.append(" ");
   if (properties.hint)
      buffer.append(properties.hint);
//NYI add here explaination
  buffer.append("\n");
  return buffer.c_str();
}
//_log_________________________________________________________________________/
const char *Parameter_number::log(std::string &buffer)                     const
{ Parameter_abstract::log(buffer);
  buffer.append(properties.name_long);
  buffer.append("_units=");
  if (properties.prefered_units_caption)buffer.append(properties.prefered_units_caption);
  buffer.append("\n");
  return buffer.c_str();
}
//_log_________________________________________________________________________/
const char *Parameter_number::append_value(std::string &buffer)            const
{  return append_value_in_radix_to(buffer,(nat8)format.format.precision,(nat8)format.format.radix);
}
//______________________________________________________________________________
bool Parameter_properties_string::setup_structure(Data_record &data_rec,bool for_write )
{  data_rec.expect_string("name"                ,name_long     ,255);
   data_rec.expect_string("name_short"          ,name_short    ,255);
   data_rec.expect_string("caption"             ,caption       ,255);
   data_rec.expect_string("hint"                ,hint          ,1023);
   data_rec.expect_string("ontology"            ,ontology      ,1023);           //190106
   data_rec.expect_enum  ("units"               ,units);
   data_rec.expect_string("units_caption"       ,units_caption ,255);
   #if ((CS_VERSION >= 1) && (CS_VERSION <= 5))
   data_rec.expect_nat32("variable_code",UED_variable,16);                       //200808
   #endif
   return true;
}
//_setup_structure_____________________________________________________________/
Parameter_number_properties::Parameter_number_properties()
{  min_warning  =(-999999);
   max_warning  =( 999999);
   min_error    =(-999999);
   max_error    =( 999999);
   format.precision    =(3);
   format.radix        =(10);
   delineation.width  =(0);                                                      //150405_141014
      //150405 was 8 but using 0 to allow free form
}
//_Parameter_number_properties_________________________________________________/
bool Parameter_number_properties::setup_structure
(Data_record &data_rec,bool for_write )
{  data_rec.expect_float32("min_warning",min_warning);
   data_rec.expect_float32("max_warning",max_warning);
   data_rec.expect_float32("min_error"  ,min_error);
   data_rec.expect_float32("max_error"  ,max_error);
   data_rec.expect_nat8   ("precision"  ,format.precision,10);
   data_rec.expect_nat8   ("radix"      ,format.radix,10);
   data_rec.expect_nat16  ("width"      ,delineation.width,10);                  //141014
   return true;
}
//_setup_structure_____________________________________________________________/
Units_code  Parameter_properties_string::get_units_code()                  const
{  Units_code U_code = units.units_clad.get();                                   //190106
   if (U_code == 0)  // it could be unit less, but maybe we can derive it from the caption
   {  units.units_clad.set_label(units_caption.c_str());                         //190106
      U_code = units.units_clad.get();                                           //190106
   }
   return U_code;
}
//_get_units_code___________________________________________________2011-12-06_/
bool Parameter_properties::is_key_string(const std::string &key_)   affirmation_  //180820
{  const char *key  = get_name_long();
   if (!strlen(key))
   {  key = get_name_long();
      if (!strlen(key))
         key = get_caption_SDF();
   }
   return key_ == key;
}
//_is_key_string____________________________________________________2018-08-20_/
const char* Parameter_properties::get_key()                                const
{  const char *key  = get_name_long();                                           //130915
   if (!strlen(key))
   {  key = get_name_long();
      if (!strlen(key))
         key = get_caption_SDF();
   }
   return key;
}
//_get_key_____________________________________________________________________/
const Date_format *Parameter_properties_abstract::provide_format_date() provision_
{  if (!format_temporal.date)
   {  const char *format_template = get_date_time_format();
      // if no format specified instanciate ISO format
      if (format_template)
         format_temporal.date = new Date_format(format_template);
   }
   return format_temporal.date;
}
//_Parameter_properties_abstract::provide_format_date_______________2019-01-10_/
const Time_format *Parameter_properties_abstract::provide_format_time() provision_
{  if (!format_temporal.time)
   {  const char *format_template = get_date_time_format();
      // if no format specified instanciate ISO format
      if (format_template)
         format_temporal.time = new Time_format(format_template);
   }
   return format_temporal.time;
}
//_Parameter_properties_abstract::provide_format_time_______________2019-01-10_/
Parameter_range  parameter_range_arbitrary = {-999999.99,999999.99,-999999.99,999999.99};

Parameter_format PPF_N ={{10,0},{0,10,NT_nat,false}};
Parameter_format PPF_F ={{10,0},{7,10,NT_float,false}};
/*
Parameter_format PPF_3N  ={{3,0},{0,10,NT_nat,false}};
Parameter_format PPF_3_0 ={{3,0},{0,10,NT_float,false}};
Parameter_format PPF_6N  ={{3,0},{0,10,NT_nat  ,false}};
Parameter_format PPF_6_1 ={{6,0},{1,10,NT_float,false}};
Parameter_format PPF_6_2 ={{6,0},{2,10,NT_float,false}};

Parameter_format PPF_8N  ={{8,0},{0,10,NT_nat  ,false}};
*/
Parameter_format PPF_8_1 ={{8,0},{1,10,NT_float,false}};
/*
Parameter_format PPF_8_2 ={{8,0},{2,10,NT_float,false}};
Parameter_format PPF_8_4 ={{8,0},{4,10,NT_float,false}};
*/
/*
Parameter_format parameter_format_arbitrary = {{10,0},{4,10,NT_float,false}};
Parameter_format PPF_8_4_float={{8,0}, {4,10,NT_float,false} };
Parameter_format PPF_8_4_float={{8,0}, {4,10,NT_float,false} };
Parameter_format PPF_8_4_float={{8,0}, {4,10,NT_float,false} };
Parameter_format PPF_8_4_float={{8,0}, {4,10,NT_float,false} };
*/

//______________________________________________________________________________
}//_namespace_CORN_____________________________________________________________/

