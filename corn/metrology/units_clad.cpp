#include "corn/metrology/units_clad.h"
#include "corn/metrology/units_symbology.h"
#  include <string.h>
#  include <math.h>
#  include <cstdlib>
   // cstdlib added for mingw compiler
#  include "corn/math/moremath.h"
#  include "corn/string/strconv.hpp"
namespace CORN {
//______________________________________________________________________________
const char *Units_clad::label_string(std::string &result)                  const
{  int16 index = has_definition();
   if (index >= 0)
      result += units_standard_definition_table[index].abbreviation;
   else
   {  std::string unused;
      compose_description_str(unused,result);
   }
   return result.c_str();
}
//_2017=04-23_02-01-26___________________________________________________________________
nat32 Units_clad::set_label(const char *abbreviation_or_description)
{  // Sets the units code given abbreviation or descrition.
   // returns 0 if not a recognized units code
   return set(CORN::units_identify_code(abbreviation_or_description));
}
//_set_label________________________________________________________2006-06-26_/
int16 Units_clad::has_definition()                                  affirmation_
{  for   (int16 unit_index = 0
         ;units_standard_definition_table[unit_index].description
         ; unit_index ++)
   if (code ==  units_standard_definition_table[unit_index].code)
         return unit_index;
   return -1;
}
//_has_definition___________________________________________________1999-01-15_/
float32 Units_component::time_to_seconds_factor(nat8  time_measure, int8 base_10_exponent) const
{  float result                                                                  //051024
         = pow((double)10.0,(double)base_10_exponent);                           //100719
   switch (time_measure) // From units
   {  case UM_second   : result *= 1;     break;
      case UM_minute   : result *= 60;    break;
      case UM_hour     : result *= 3600;  break;
      case UM_day      : result *= 86400; break;
      case UM_year     : result *= 31536000; break;
            // Warning not sure how to account for leap years.
            // It should be ok because most variables with units per year
            // are probably based on 365 day years.
   }
   return result;
}
//_time_to_seconds_factor______________________________________________________/
float32 Units_component::standard_conversion_to_factor
(const Units_component &to_general_sub_units)                              const
{  nat8  from_measure = get_measure_index();
   nat8  to_measure   = to_general_sub_units.get_measure_index();
   sint8 from_base10_exponent = get_metric_exponent();
   sint8 to_base10_exponent = to_general_sub_units.get_metric_exponent();
   int exponent_diff = 0;  // int because pow10 take int
   if ((from_measure == to_measure))
   {  exponent_diff = (int)(from_base10_exponent - to_base10_exponent);
      switch (from_measure)                                                      //150425
      {  case UM_area: exponent_diff *= 2; break;                                //150425
         case UM_volume: exponent_diff *= 3; break;                              //150425
      }
   }
   else // try some other units conversion
   {  if ((subcode == UC_square_meter_area)
           && (to_general_sub_units.subcode == UC_hectare))
         return (float32)0.0001;                                                 //070209
      if ((to_general_sub_units.subcode == UM_area)
           && (from_measure == UM_land_area))
        return 10000.0;
      // Time conversions (We only permit time to time conversions)
      if (  (from_measure >= UM_second) && (from_measure <= UM_year)
          &&(to_measure >= UM_second) && (to_measure <= UM_year))
      {  float from_time_to_seconds = time_to_seconds_factor(from_measure,from_base10_exponent);
         float to_time_to_seconds   = time_to_seconds_factor(to_measure  ,to_base10_exponent);
         float from_time_to_time = from_time_to_seconds / to_time_to_seconds;
         float from_time_to_time_factor = (is_denominator() ? (1 /from_time_to_time) : from_time_to_time);
         return from_time_to_time_factor;
      }
      // I think the following can do all length squared to ha but I am not sure
      if ((from_measure == UM_area)
           && (to_measure == UM_land_area))
      {  // WARNING need to check
          exponent_diff = CORN_sqr(from_base10_exponent) - to_base10_exponent - 2;
      }
      if ((to_measure == UM_area)                                                //040715
           && (from_measure == UM_land_area))
      {  // WARNING need to check
          exponent_diff = CORN_sqr(from_base10_exponent) + to_base10_exponent + 2;
      }
      // perhaps this is a time conversion:
//NYI                if (U_is_time(from_general_sub_units) && U_is_time(to_general_sub_units))
//NYI                   return time_conversion_factor(from_general_sub_units,to_general_sub_units);
      // try something else
   }
   float32 result = 1.0;
   if (exponent_diff)
   {
      double factor = pow((double)10.0,(double)exponent_diff);                   //130918_051024
      result = (float32)(is_denominator() ? (1.0 / factor) : factor);            //070209
   }
   return result; // no conversion.
}
//_standard_conversion_to_factor_______________________________________________/
const char *Units_component::get_measure_quantity_description()            const
{ return units_generalized_measure_def_table[get_measure_index()].quantity; }
//_get_measure_quantity_description_________________________________2005-07-07_/
bool  Units_component::compose_common_description
(char * component_description
,char * component_abbr)                                                    const
{  component_description[0] = 0;                                                 //061011
   component_abbr[0] = 0;                                                        //061011
   if (is_denominator()) // if in denominator
   {  strcat(component_description," per ");
      strcat(component_abbr,"/");
   }
   if (((subcode > 0) && (subcode <= 0x1EFF )) ||
       ((subcode >= 0x4000) && (subcode < 0x4100)) // Special non metric angles
       ) // dimensionless or time
   {  if ((subcode >= UT_day_of_month_1) &&
          (subcode <= UT_day_of_month_31))
      {  // compose month number
         // return xxx
      }
      // seconds
      // same for minutes
      // hours
      for (int i = 1
          ;units_standard_definition_table[i].code
          ;i++)
          if (units_standard_definition_table[i].code == (uint32)subcode)
          {
               strcat(component_description,units_standard_definition_table[i].description);
               strcat(component_abbr,units_standard_definition_table[i].abbreviation);
               return true;
          }
   }
   return false;
}
//_compose_common_description_______________________________________1999-05-10_/
bool  Units_component::compose_nonstandard_description
(char * component_description
,char * component_abbr)                                                    const
{  if (compose_common_description(component_description,component_abbr))
      return true; // was temporal or dimensionless
   for (int i = 1
      ;units_customary_def_table[i].subcode
      ;i++)
      if ((uint32)units_customary_def_table[i].subcode == (uint32)subcode)
      {  strcat(component_description,units_customary_def_table[i].units_name);
         strcat(component_abbr,units_customary_def_table[i].units_abbreviation);
         return true;
      }
   return false;
}
//_compose_nonstandard_description__________________________________1999-05-10_/
bool  Units_component::compose_standard_description
(char * component_description
,char * component_abbr)                                                    const
{  if (compose_common_description(component_description,component_abbr))
      return true;  // was temporal or dimensionless
   // times > 0x1600 are metric (i.e. 10ths 100ths etc of second minute hour etc.
   sint16 exponent = get_metric_exponent();
   int label_index = abs(exponent);
   if (label_index > 63) return false;
   if ((label_index > 0) && (label_index <= 24))
   {  strcat(component_description,
         (exponent >= 0)
         ? units_metric_system_label_table_pos[label_index].label
         : units_metric_system_label_table_neg[label_index].label
         );
      strcat(component_abbr,
         (exponent >= 0)
         ? units_metric_system_label_table_pos[label_index].abbreviation
         : units_metric_system_label_table_neg[label_index].abbreviation
         );
   }
   if (label_index > 24) // There are no metric prefixes for 10^25 and greater
   {  char exp_str[10];
      int16_to_cstr(exponent,exp_str,10);
      strcat(component_description,"10^");
      strcat(component_description,exp_str);
   }
   nat16  unit_quality = get_measure_index();
   strcat(component_description,units_generalized_measure_def_table[unit_quality].units_name);
   strcat(component_description," (");
   strcat(component_description,units_generalized_measure_def_table[unit_quality].quantity);
   strcat(component_description,")");
   strcat(component_abbr,units_generalized_measure_def_table[unit_quality].units_abbreviation);
   return true;
}
//_compose_standard_description_____________________________________1999-05-10_/
bool Units_clad::compose_description
(char *description
,char *abbreviation)                                                       const
{  char primary_descr[100];  primary_descr[0] = 0;
   char primary_abbr[100];   primary_abbr[0] = '-'; primary_abbr[1] = 0;
   if (get() == 0)                                                              //061011
   {  strcpy(primary_descr,"unitless");
      primary_abbr[0] = '-'; primary_abbr[1] = 0;
      return true;
   }
   Units_component primary(get_primary());
   bool composed = (primary.is_standard())                                      //030922
      ? primary.compose_standard_description(primary_descr,primary_abbr)
      : primary.compose_nonstandard_description(primary_descr,primary_abbr);    //030922
   strcpy(description,primary_descr);
   strcpy(abbreviation,primary_abbr);
   nat16  secondary = get_secondary();
   if (secondary)
   {  char secondary_descr[100]; secondary_descr[0] = 0;
      char  secondary_abbr[100]; secondary_abbr[0] = 0;
      Units_component secondary(get_secondary());
      composed = (secondary.is_standard())                                       //030922
      ? secondary.compose_standard_description(secondary_descr,secondary_abbr)
      : secondary.compose_nonstandard_description(secondary_descr,secondary_abbr);
      strcat(description,secondary_descr);
      strcat(abbreviation,secondary_abbr);
   }
   //first look in the standard definition table
   //perhaps it is a UED_STD_UD definition
   return composed;
}
//_compose_description______________________________________________1999-05-10_/
bool Units_clad::compose_description_str
(std::string &description
,std::string &abbreviation)                                                const
{  char primary_descr[100];  primary_descr[0] = 0;
   char primary_abbr[100];   primary_abbr[0] = '-'; primary_abbr[1] = 0;
   if (get() == 0)                                                               //061011
   {  strcpy(primary_descr,"unitless");
      primary_abbr[0] = '-'; primary_abbr[1] = 0;
      return true;
   }
   Units_component primary(get_primary());
   bool composed = (primary.is_standard())                                       //030922
      ? primary.compose_standard_description(primary_descr,primary_abbr)
      : primary.compose_nonstandard_description(primary_descr,primary_abbr);     //030922
   description.append(primary_descr);                                            //170114
   abbreviation.append(primary_abbr);                                            //170114
   nat16  secondary = get_secondary();
   if (secondary)
   {  char secondary_descr[100]; secondary_descr[0] = 0;
      char  secondary_abbr[100]; secondary_abbr[0] = 0;
      Units_component secondary(get_secondary());
      composed = (secondary.is_standard())                                       //030922
      ? secondary.compose_standard_description(secondary_descr,secondary_abbr)
      : secondary.compose_nonstandard_description(secondary_descr,secondary_abbr);
      description.append(secondary_descr);                                       //170114
      abbreviation.append(secondary_abbr);
   }
   //first look in the standard definition table
   //perhaps it is a UED_STD_UD definition
   return composed;
}
//_compose_description_str__________________________________________2017-01-14_/
bool Units_clad::is_temporal()                                      affirmation_
{  if (!get_secondary()) return false;
   Units_component primary_component(get_primary());
   return primary_component.is_temporal();
}
//_is_temporal______________________________________________________2017-05-20_/
sint8 Units_component::get_metric_exponent()                               const
{  nat8  exponent = (nat8 )(subcode & 0x003F);
   return (int8)((subcode & 0x0040) ? - exponent : exponent);
}
//______________________________________________________________________________
nat8  Units_component::set_metric_exponent(nat8  metric_exponent)
{  subcode = (nat16 )((subcode & 0xFF00) | metric_exponent);
   return (nat8 )subcode;
}
//_set_metric_exponent______________________________________________2005-07-06_/
void Units_component::clear_metric_exponent()
{  subcode = subcode & (nat16 )0xFF00;
}
//_2005-06-13___________________________________________________________________
bool Units_component::is_temporal()                                 affirmation_ //170520
{  nat8 measure_index = get_measure_index();
   return (measure_index >= 0x10) && (measure_index <= 0x1A);
}
//_is_temporal______________________________________________________2017-05-20_/
}//_namespace_CORN_____________________________________________________________/

