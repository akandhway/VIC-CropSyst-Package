#include "options.h"
#include "corn/data_source/vv_file.h"
#include "output.h"
/*200813
#ifndef LADSS_UNIX
#include "UED/convert/ED_tabular_format.h"
#endif
*/
#include "CS_suite/observation/CS_optation.h"
#include "corn/tabular/tabular_format.h"

using namespace CORN;

#define DEFAULT_CONVERSION_COUNT 12
namespace CropSyst
{
//______________________________________________________________________________
struct UED_XLS_conversion
{  const char *UED_file
;  const char *xTDF_file
;  const char *XXX_file
;} default_UED_XLS_conversions[DEFAULT_CONVERSION_COUNT] =
{  {"daily.ued"         ,"\\Simulation\\Database\\Output\\default\\daily.tdf"             ,"daily.xls"      }
,  {"harvest.ued"       ,"\\Simulation\\Database\\Output\\default\\harvest.tdf"           ,"harvest.xls"    }
,  {"season.ued"        ,"\\Simulation\\Database\\Output\\default\\season.tdf"            ,"season.xls"     }
,  {"annual.ued"        ,"\\Simulation\\Database\\Output\\default\\annual.tdf"            ,"annual.xls"     }
,  {"daily.ued"         ,"\\Simulation\\Database\\Output\\default\\water_content.tdf"     ,"soil\\water_content.xls"}
,  {"daily.ued"         ,"\\Simulation\\Database\\Output\\default\\temperature.tdf"       ,"soil\\temperature.xls"}
,  {"daily.ued"         ,"\\Simulation\\Database\\Output\\default\\nitrate.tdf"           ,"soil\\nitrate.xls"}
,  {"daily.ued"         ,"\\Simulation\\Database\\Output\\default\\ammonium.tdf"          ,"soil\\ammonium.xls"}
,  {"daily.ued"         ,"\\Simulation\\Database\\Output\\default\\mineralization.tdf"    ,"soil\\NH4_mineralization.xls"}
,  {"daily.ued"         ,"\\Simulation\\Database\\Output\\default\\nitrification.tdf"     ,"soil\\nitrification.xls"}
,  {"daily.ued"         ,"\\Simulation\\Database\\Output\\default\\denitrification.tdf"   ,"soil\\denitrification.xls"}
,  {"daily.ued"         ,"\\Simulation\\Database\\Output\\default\\salinity.tdf"          ,"soil\\salinity.xls"}
//,{"daily.ued"         ,"soil\\water_potential.tdf"   ,"water_potential.xls"}
//,{"daily.ued"         ,"soil\\Root_fraction.tdf"     ,"root_fraction.xls"}
//,{"daily.ued"         ,"soil\\plant_residue.tdf"     ,"plant_residue.xls"}
//,{"daily.ued"         ,"soil\\manure_residue.tdf"    ,"manure_residue.xls"}
//,{"daily.ued"         ,"soil\\organic_matter.tdf"    ,"organic_matter.xls"}
};
//_default_UED_XLS_conversions_________________________________________________/
DECLARE_CONTAINER_ENUMERATED_SECTION(Scenario_output_options ::Conversion,conversion_vv,true);
Scenario_output_options::Scenario_output_options()
: Common_parameters_data_record(REQUIRED_ASSOCIATED_DIRECTORY,"output",CS_VERSION)
   // Warning need to get this from common simulation paths
, conversions()
{}
//_DECLARE_CONTAINER_ENUMERATED_SECTION________________________________________/
bool Scenario_output_options::expect_structure(bool for_write)
{  bool expected = Common_parameters_data_record::expect_structure(for_write);
   structure_defined = false;                                                    //120314
   EXPECT_ENUMERATED_SECTION("conversion",conversion_vv,conversions);
   structure_defined = true;                                                     //120314
   return expected;
}
//_expect_structure____________________________________________________________/
bool Scenario_output_options::get_end()
{  // load the default conversions
   if (conversions.count() ==0 )
      for (int i = 0; i < DEFAULT_CONVERSION_COUNT; i++)
      {  Conversion *conversion = new Conversion;
         conversion->always_convert = true;;
         conversion->xxx_filename.set_cstr(default_UED_XLS_conversions[i].XXX_file);
         conversion->ued_filename.set_cstr(default_UED_XLS_conversions[i].UED_file );
         conversion->tdf_filename.set_cstr(default_UED_XLS_conversions[i].xTDF_file);
         conversions.append(conversion);
      }
   return true;
}
//_get_end__________________________________________________________2001-01-09_/
const char *Scenario_output_options::Conversion::label_string(std::string &buffer) const
{
   ued_filename.append_to_string(buffer);
   buffer += " ";
   tdf_filename.append_to_string(buffer);
   buffer += " ";
   xxx_filename.append_to_string(buffer);
   return buffer.c_str();
}
//_label_string_____________________________________________________2017-04-24_/
bool  Scenario_output_options::Conversion::setup_structure
(Data_record &data_rec,bool for_write)                            modification_
{  data_rec.expect_bool("enabled",always_convert);
   data_rec.expect_file_name("xxx_filename",xxx_filename);
   data_rec.expect_file_name("ued_filename",ued_filename);
   data_rec.expect_file_name("tdf_filename",tdf_filename);
   return true;
}
//_list_required_variables_____________________________________________________/
void Scenario_output_options::list_required_variables
(/*190204 CS::Desired_variables &required_variables*/)                     const
{  // for each of the conversions open the TDF file and list_required_variables
#ifndef LADSS_UNIX
// Not currently needed for LADSS
   FOR_EACH_IN(conversion,Scenario_output_options::Conversion,conversions,each_conversion)
   {
      Tabular_format tdf;                                                        //200813
      //200812 ED_tabular_file_format tdf;
      VV_File tdf_file(conversion->tdf_filename.c_str());
      tdf_file.get(tdf);
      tdf.list_required_variables(/*190204 required_variables*/); // should append the variables
   } FOR_EACH_END(each_conversion);
#endif
}
//_list_required_variables__________________________________________2004-10-21_/
}//_namespace CropSyst_________________________________________________________/
/*
Scenario output manager with conversion

output manager file contains:


There may multiple conversions for the same UED file
Option to make the output filenames prepended with project and scenario name to make them unique.

The output manager has a parameter file in database directory. (this will replace the .fmt file)
it has an associated directory to store TDS files.
There will be a TDS editor.

Scenario run should put a short cut to output manager in output directory
(using this as current working directory)

Command line args:
output options filename.  If not specified, then user Simulation database default

Options

A table shows CropSyst
UED file name - TDS - output filename associations

List of variables to record in UED file.
This list should show all variables listed in .TDF file plus any additional requested by the user.
(for annual, harvest, season, daily, fast
*/

