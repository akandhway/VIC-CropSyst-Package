#error obsolete
150429 Now the complete detailed organic matter profile is saved

#include "OM_historic_datarec.h"
//______________________________________________________________________________
bool Historical_organic_matter_data_record::expect_structure(bool for_write)
{
  set_current_section(LABEL_POM);
   // we don't use recalibration date here (this is the recalibration)
   expect_float32_array_with_units_code_fixed(LABEL_POM,UC_percent,horizon_percent_organic_matter,1,MAX_soil_horizons,false);
   // we don't use_soil_parameter_organic_matter (we are defining it here)
  return organic_matter_initialization_parameters.setup_structure(*this,for_write);
}
//_expect_structure_________________________________________________2012-09-09_/
