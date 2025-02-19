#include "CS_suite/observation/CS_optation.h"
#include "corn/OS/directory_entry_name.h"
#include "corn/string/strconv.hpp"
//200813 #include "UED/convert/ED_tabular_format.h"
#include "corn/tabular/tabular_format.h"
#include "corn/data_source/vv_file.h"

namespace CS {
//______________________________________________________________________________
bool Optation_temporal::is_desired(const std::string &ontology)     affirmation_
{  return ontologies.find_string(ontology);
}
//______________________________________________________________________________
bool Optation_temporal::is_desired(const char *ontology)            affirmation_
{  return ontologies.find_cstr(ontology);
}
//______________________________________________________________________________
bool Optation_temporal::know_ontology(const std::string &ontology) modification_
{  ontologies.append(new CORN::Item_string(ontology));
   return true;
}
//______________________________________________________________________________
#ifdef USE_VAR_CODES
bool Optation_temporal::is_desired(nat32 variable_code)             affirmation_
{  nat32 occurs_at = 0;
   return variable_codes.occurs(variable_code,occurs_at);
}
//______________________________________________________________________________
bool Optation_temporal::know_variable_code(nat32 variable_code)    modification_
{  nat32 occurs_at = 0;
   if (!variable_codes.occurs(variable_code,occurs_at))
      variable_codes.append(variable_code);
   return !occurs_at;
}
#endif
//______________________________________________________________________________
//==============================================================================
bool Optation::is_desired
(const std::string &ontology, CORN::Units_code timestep)           affirmation_
{
   bool desired = false;
   if (timestep)
   {  const Optation_temporal *opt_temp = dynamic_cast<const Optation_temporal *>
         (temporals.find_nat32(timestep));
      if (opt_temp) desired = opt_temp->ontologies.find_string(ontology);
   } else
   {  FIND_FOR_EACH_IN(found_opt_temp,opt_temp,Optation_temporal,temporals,true,iter_opt_temp)
      {  if (opt_temp->ontologies.find_string(ontology))
         {  found_opt_temp = opt_temp;
            desired = true;
         }
      } FOR_EACH_END(iter_opt_temp)
   }
   return desired;
}
//_Optation::is_desired________________________________________________________/
bool Optation::is_desired
(const char *ontology, CORN::Units_code timestep)                   affirmation_
{  return is_desired(std::string(ontology),timestep);
}
//_Optation::is_desired________________________________________________________/
#ifdef USE_VAR_CODES
bool Optation::is_desired
(nat32 variable_code, CORN::Units_code timestep)                    affirmation_
{
   bool desired = false;
   if (timestep)
   {  const Optation_temporal *opt_temp =dynamic_cast<const Optation_temporal *>
         (temporals.find_nat32(timestep));
      if (opt_temp)
      {  nat32 occurs_index;
         desired = opt_temp->variable_codes.occurs(variable_code,occurs_index);
      }
   } else
   {  FIND_FOR_EACH_IN(found_opt_temp,opt_temp,Optation_temporal,temporals,true,iter_opt_temp)
      {  nat32 occurs_at = 0xFFFFFFFF;
         if (opt_temp->variable_codes.occurs(variable_code,occurs_at))
         {  found_opt_temp = opt_temp;
            return desired = true;
         }
      } FOR_EACH_END(iter_opt_temp)
   }
   return desired;
}
//_Optation::is_desired________________________________________________________/
bool Optation::desire(nat32 variable_code, CORN::Units_code timestep)
{  Optation_temporal &opt_temp = provide_temporal(timestep);
   return opt_temp.know_variable_code(variable_code);
}
//_Optation::desire____________________________________________________________/
#endif
//______________________________________________________________________________
nat32 Optation::optate_TDF(const CORN::OS::File_name &TDF_filename)
{  nat32 optated = 0;
   /*200813 replaced with tabular
   ED_tabular_file_format tdf;
   */
   CORN::Tabular_format tdf;                                                     //200813
   CORN::VV_File tdf_file(TDF_filename.c_str());
   tdf_file.get(tdf);
   nat32 timestep = tdf.detail.timestep_units_enum.get();
   Optation_temporal &opt_temp  =provide_temporal(timestep);
   //200813 FOR_EACH_IN(column,ED_tabular_file_format::Column,tdf.columns,iter_column)
   FOR_EACH_IN(column,CORN::Tabular_format::Column,tdf.columns,iter_column)
   {  optated++;
      if (column->ontology.length())
         opt_temp.know_ontology(column->ontology);
      #if ((CS_VERSION > 0) &&(CS_VERSION < 6))
      if (column->UED_variable)                                                  //200813
         opt_temp.know_variable_code(column->UED_variable);                      //200813
      /*200813
      if (column->variable_code_clad.get())
         opt_temp.know_variable_code(column->variable_code_clad.get());
      */
      #endif
   } FOR_EACH_END(iter_column)
   return optated;
}
//_Optation::optate_TDF________________________________________________________/
nat32 Optation::optate_TDFs(const CORN::Container &TDF_filenames)
{  nat32 optated = 0;
   FOR_EACH_IN(TDF_filename,CORN::OS::File_name,TDF_filenames,iter_TDF_filename)
      optated += optate_TDF(*TDF_filename);
   FOR_EACH_END(iter_TDF_filename)
   return optated;
}
//_Optation::optate_TDFs_______________________________________________________/
Optation_temporal &Optation::provide_temporal(CORN::Units_code timestep) provision_
{  Optation_temporal *provided
   = dynamic_cast<Optation_temporal *>(temporals.find_nat32(timestep));
   if (!provided)
   {  provided = new Optation_temporal(timestep);
      temporals.take(provided);
   }
   return *provided;
}
//_Optation::provide_temporal__________________________________________________/
bool Optation::has_any_daily()                                      affirmation_
{  Optation_temporal *daily_optation
   = dynamic_cast<Optation_temporal *>(temporals.find_nat32(UT_day));
   return daily_optation;
}
//_has_any_daily_______________________________________________________________/
Optation optation_global;
}//_namespace CropSyst_________________________________________________________/

