#if (CROPSYST_VERSION < 4)
#ifdef USE_PCH
#include "simulation/CropSyst_sim_pch.h"
#else
#  include <corn/string/strconv.h>
#  include <common/weather/snow.h>
#  include "sim_param.h"
#     include <corn/measure/units.ph>
#     include <common/weather/units.ph>
#     include <CropSyst/phrases/simfile.ph>
#     include <CropSyst/phrases/cs_chem.ph>
#     include <CropSyst/phrases/watertab.ph>
#endif
#pragma hdrstop
#include "sim_param.h"

#ifdef CHEM_PROFILE
//______________________________________________________________________________
initial_chemical_profile::initial_chemical_profile
(const char *i_name)                                                             //980731
:chemical_properties(i_name)                                                     //990225
,enabled(true)                                                                   //981211
,adjustment_32(0.0)
,water_table_conc_32(0.0)
,v_half_life_override                (half_life                ,UT_day        ,LABEL_half_life_override              ,3,0.0, 0.0,9999.0,  -0.0001,9999.0 ,T_date_Days    )
,v_Freundlich_N_override             (Freundlich_N             ,UC_unitless   ,LABEL_Freundlich_N_override           ,3,0.0, 0.0,9999.0,  -0.0001,9999.0 ,TU_unitless)
,v_activation_energy_override        (activation_energy        ,UC_kJ_mole    ,LABEL_activation_energy_override      ,3,0.0, 0.0,9999.0,  -0.0001,9999.0 ,TU_kJ_mole ,TL_Activation_energy)
,v_soil_moisture_influence_override  (soil_moisture_influence  ,UC_unitless   ,LABEL_soil_moisture_influence_override,3,0.0, 0.0,9999.0,  -0.0001,9999.0 ,TU_unitless)
,v_linear_sorption_coef_override     (linear_sorption_coef     ,UC_ml_g       ,LABEL_linear_sorption_coef_override   ,3,0.0, 0.0,9999.0,  -0.0001,9999.0 ,TU_ml_gram )
,v_adjustment                        (adjustment_32            ,UC_adjustment ,LABEL_adjustment                      ,3,1.0, 0.0,2.0,         0.0,3.0    ,TU_0_2     ,TL_Transformation_rate_adjustment)
,v_water_table_conc                  (water_table_conc_32      ,UC_kg_m3     ,LABEL_water_table_conc                ,3,0.0, 0.0,9999.0,  -0.0001,9999.0 ,TU_kg_m3   ,TL_Water_table_concentration) //040715
{  initialize();
}
//_initial_chemical_profile_______________________________________________1999_/
void initial_chemical_profile::initialize()
{  char i_str[10];
   for (int8 i = 0; i <= MAX_soil_horizons; i++)
   {  horizon_chemical[i] = 0;
      CORN_int8_to_str(i,i_str,10);                                              //001116
      v_chemical[i] =  new Valid_float32
      (horizon_chemical[i],UC_kg_ha,i_str,3,0.0, 0.0,9999.0,-0.0001,9999.0,TU_kg_ha);
   }
}
//_initialize_______________________________________________________1999-02-25_/
const char *initial_chemical_profile::label_string(std::string &buffer)    const
{  std::string chem_prop_label;
   chemical_properties::label_string(chem_prop_label);
   buffer = enabled ? "+" : "-"
      + chem_prop_label;
   return buffer;
};
//_label_string_____________________________________________________2017-04-23_/
/*190718 obs
const char *initial_chemical_profile::label_cstr_deprecated(char *buffer) const
{  char chem_prop_label[255];
   chemical_properties::label_cstr(chem_prop_label);
   strcpy(buffer,enabled ? "+" : "-");
   strcat(buffer,chem_prop_label);
   return buffer;
};
//_1997-07-20_______________________________________________________label_cstr_/
*/
#endif

#ifdef PESTICIDE_DISABLED
#ifdef PESTICIDE
//______________________________________________________________________________
WARNING need to implement this for pesticides
void initial_chemical_profile::setup_structure_with_adjustments
(VV_File &vv_file,int chem_num
NITRO ,float32 adjust_denitrify,float32 adjust_nitrify
)
{  vv_file.expect_valid_float32(v_half_life_override);
   vv_file.expect_valid_float32(v_Freundlich_N_override);
   vv_file.expect_valid_float32(v_activation_energy_override);
   vv_file.expect_valid_float32(v_soil_moisture_influence_override);
   vv_file.expect_valid_float32(v_adjustment);
   vv_file.expect_valid_float32(v_linear_sorption_coef_override);
   vv_file.expect_bool(LABEL_enabled,Xenabled,CORN::VV_bool_entry::form_true );
   vv_file.expect_valid_float32(v_water_table_conc);
   for (int layer = 1; layer <= MAX_soil_horizons; layer++)
      vv_file.expect_valid_float32(*v_chemical[layer]);
};
//_setup_structure_with_adjustments_____________________________________________
#endif
#endif

#endif

