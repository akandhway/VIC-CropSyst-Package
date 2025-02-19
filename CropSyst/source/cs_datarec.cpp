// Not used in V6

#include "crop/crop_cropsyst.h"
#include "soil.h"
#include "soil/chemicals_profile.h"
#include "soil/runoff.h"
#include "soil/erosion_RUSLE.h"
#include "soil/temperature_profile.h"
#include "soil/salinity_profile.h"
#include "cs_ET.h"
#include "cs_mgmt.h"
#include "csvc.h"
#include "common/weather/weather.h"
#include "UED/library/database_file.h"
#include "corn/data_source/datarec.h"
#include "CS_suite/observation/CS_optation.h"
#include "crop/canopy_growth.h"
#  include "soil/soil_evaporator.h"
#include "crop/crop_N_interface.h"

#define desired_variables CS::optation_global
#define desired_vars CS::optation_global

//______________________________________________________________________________
#if (CROPSYST_VERSION==4)
bool CropSyst::Soil_components::setup_structure
(Data_record &data_rec,bool for_write)
{
   bool satup = Soil_base::setup_structure(data_rec,for_write);
   #ifdef CHEM_PROFILE
   if (chemicals) chemicals->setup_structure(data_rec,for_write);
   #endif
   #ifdef EROSION
   if (erosion)   erosion  ->setup_structure(data_rec,for_write);
   #endif
   #ifdef SOIL_TEMPERATURE
   if (temperature_profile) temperature_profile->setup_structure(data_rec,for_write);
   #endif
   //NYI FREEZE         soil_freezing_profile::add_desired_variables(data_rec,for_write,desired_variables);
   structure_defined = true;
   return satup;
}
//_Soil_components::setup_structure____________________________________________/
#endif
#if (CROPSYST_VERSION==4)

// NYI In version 5 we will use inspection

bool Soil_base::setup_structure(Data_record &data_rec,bool for_write)
{  UNUSED_arg(for_write);
   nat8 number_layers = layers->count();                                         //061005
   data_rec.set_current_section("soil_base");
   //190119  if (desired_vars.is_desired(CSVC_soil_base_evap_pot_time_step_sum))
   if (CS::optation_global.is_desired(CSVC_soil_base_evap_pot_time_step_sum))    //190119
      data_rec.expect_float64
      ("soilevappot",
         mod_evaporator()->output_evaporation_potential_daily                    //160719
      );
   if (CS::optation_global.is_desired(CSVC_soil_base_evap_act_time_step_sum))
      data_rec.expect_float64("soilevapact"
         ,mod_evaporator()->output_evaporation_actual_daily);                    //160719


   #ifdef SOIL_INFILTRATION
   if (CS::optation_global.is_desired(CSVC_soil_base_water_entering_time_step_sum))  data_rec.expect_float64("waterenter",infiltration_submodel->mod_act_water_entering_soil()); //160412
   if (CS::optation_global.is_desired(CSVC_soil_base_water_drainage_time_step_sum))	data_rec.expect_float64("drainage",		infiltration_submodel->infiltration);
   #endif
   if (CS::optation_global.is_desired(CSVC_soil_base_water_content))           data_rec.expect_float64_array("WC",		record_all_water_content,true,(uint16)number_layers,true);
   if (CS::optation_global.is_desired(CSVC_soil_base_plant_avail_water))
      data_rec.expect_float64_array("PAW" ,hydrology->ref_plant_avail_water(),true,(uint16)number_layers,true);

   structure_defined = true;
   return true;
}
//_setup_structure__________________________________________________2003-07-15_/
bool CropSyst::Soil_runoff_common::setup_structure(Data_record &data_rec,bool for_write)
{  UNUSED_arg(for_write);
   data_rec.set_current_section("soil_runoff");
   if (CS::optation_global.is_desired
      (CSVC_soil_runoff_surface_water_runoff_time_step_sum))
         data_rec.expect_float64("waterrunoff",estimated_runoff);
   return true;
}
//_Soil_runoff_common::setup_structure_________________________________________/
bool Soil::Temperature_profile::setup_structure(Data_record &data_rec,bool for_write) //170225
{  UNUSED_arg(for_write);
   data_rec.set_current_section("soil_temperature_profile");
   if (desired_vars.is_desired(CSVC_soil_base_temperature_time_step_avg))  data_rec.expect_float64_array("soil_temperature",temperatures,true,(uint16)soil_layers.count(),true);
   return true;
}

//_Temperature_profile::setup_structure________________________________________/
#ifdef NYI
//NYI  In order to get all the UED outputs currently provided in soil profile spreadsheet
// I need to output residue profile plant (straw) incorporation and  manure incorporation
   if (desired_vars.is_desired(CSVC_residue_plant_incorporated))           data_rec.expect_float64_array("plant_incorp"    ,	XXXXXXXXXXXX,true,(uint16)get_number_sublayers(),true);
   if (desired_vars.is_desired(CSVC_residue_manure_incorporated))          data_rec.expect_float64_array("manure_incorp"   ,	XXXXXXXXXXXX,true,(uint16)get_number_sublayers(),true);
   if (desired_vars.is_desired(CSVC_organic_matter_percent_time_step_sum)) data_rec.expect_float64_array("organic_matter"  ,	XXXXXXXXXXXX,true,(uint16)get_number_sublayers(),true);
#endif
//______________________________________________________________________________
#ifdef CHEM_PROFILE
bool Soil::Chemicals_profile::setup_structure(Data_record &data_rec,bool for_write)
{  UNUSED_arg(for_write);
   data_rec.set_current_section("soil_chemical_profile");
   nat16 number_sublayers = soil.ref_layers()->count();                          //150424_041002
   if (desired_vars.is_desired(CSVC_soil_N_N_amount))                      data_rec.expect_float64_array("N_amount"          ,total_N_E                     ,true,number_sublayers,true);
   if (desired_vars.is_desired(CSVC_soil_N_NO3_amount))                    data_rec.expect_float64_array("N_NO3"             ,NO3_N_E_output                ,true,number_sublayers,true);
   if (desired_vars.is_desired(CSVC_soil_N_NH4_amount))                    data_rec.expect_float64_array("N_NH4"             ,NH4_N_E_output                ,true,number_sublayers,true);


   //NYI denitrification_N_E_output and nitrification_N_E_output  170503
   // have been replaced with dynamic arrays
   // soil->chemicals->NO3->denitrified_N_output
   // soil->chemicals->NH4->nitrified_N_output
   // I need to have data_rec expect dynamic array,
   // (actually new inspector array idiom should work)
   // then denitrification_N_E_output and nitrification_N_E_output are obviated.

   if (desired_vars.is_desired(CSVC_soil_N_denitrification_time_step_sum)) data_rec.expect_float64_array("N_denitrification" ,denitrification_N_E_output    ,true,number_sublayers,true);
   if (desired_vars.is_desired(CSVC_soil_N_nitrification_time_step_sum))   data_rec.expect_float64_array("N_nitrification"   ,nitrification_N_E_output      ,true,number_sublayers,true);


   if (desired_vars.is_desired(CSVC_soil_N_mineralization_time_step_sum))  data_rec.expect_float64_array("N_mineralization"  ,mineralization_N_E_output     ,true,number_sublayers,true);
   if (desired_vars.is_desired(CSVC_soil_N_mineralization_time_step_sum))  data_rec.expect_float64_array("N_mineralization"  ,mineralization_N_E_output     ,true,number_sublayers,true);
   #ifdef SALINITY
   if (desired_vars.is_desired(CSVC_soil_base_salinity) && salinity_dSm)   data_rec.expect_float64_array("salinity"          ,salinity_dSm->salinity_profile->salinity_ECw_dSm_input_output ,true,number_sublayers,true);  //051201
   #endif
   return true;
}
//_Chemicals_profile::setup_structure__________________________________________/
#endif
#ifdef EROSION
bool RUSLE::Soil_erosion::setup_structure(Data_record &data_rec,bool for_write)                                      //170225
{  UNUSED_arg(for_write);
   data_rec.set_current_section("soil_erosion");
   if (desired_vars.is_desired(CSVC_soil_erosion_clod_rate))               data_rec.expect_float64("clodrate",clod_factor_rate);
   return true;
}
//_Soil_erosion:setup_structure________________________________________________/
#endif
bool CropSyst_ET::setup_structure(Data_record &data_rec,bool for_write)
{  UNUSED_arg(for_write);
   data_rec.set_current_section("ET");
   if (desired_vars.is_desired(CSVC_weather_ET_ref_time_step_sum))
      data_rec.expect_float64("ETref"       ,amount);                            //180404
   if (desired_vars.is_desired(CSVC_weather_temperature_time_step_min))    data_rec.expect_float64("airtempmin"  ,air_temperature_min.amount);  //151022
   if (desired_vars.is_desired(CSVC_weather_temperature_time_step_max))    data_rec.expect_float64("airtempmax"  ,air_temperature_max.amount);  //151022
   if (desired_vars.is_desired(CSVC_weather_solar_radiation))              data_rec.expect_float64("solarrad"    ,solar_radiation.amount );     //151022
   structure_defined = true;
   return true;
}
//_setup_structure_____________________________________________________________/
#endif

void Soil_base::output_layering
(UED::Database_file_indexed   &output_database)                            const
{  nat8 number_layers = layers->count();
   Dynamic_float_array thickness_array(number_layers,10,0);                      //020808
   for (uint8 sublayer = 0; sublayer <= number_layers; sublayer++)
       thickness_array.set(sublayer,(float32)layers->get_thickness_m(sublayer)); //150424_020808
   UED::Data_record_creation_layout soil_thickness_record_creation
      (UED::Record_type_distinguisher_interface::UED_data_vector_date_var_units
      ,UED_soil_thickness                                                        //010216
      ,UC_meter,UT_day ,UT_arbitrary_date,simulated_quality);
   soil_thickness_record_creation.allow_arbitrary_time_stamp();
   UED_units_code preferred_units_code = UC_meter;                               //991121
   bool record_created;
   datetime64 now(simdate.get_datetime64());                                     //030714
   CORN::Quality_clad simulated_qual(simulated_quality);                         //030103
   output_database.set_vector(thickness_array ,now                               //030103
      ,preferred_units_code                                                      //990516
      ,simulated_qual,soil_thickness_record_creation,record_created);
}
//_output_layering__________________________________________________1998-09-25_/
void Soil_base::output_hydraulic_properties
(UED::Database_file_indexed &output_database)                              const
{  nat8 number_layers = layers->count();
   Dynamic_float_array FC_array(number_layers,10,0);
   Dynamic_float_array PWP_array(number_layers,10,0);
   Dynamic_float_array WC_sat_array(number_layers,10,0);
   for (nat8 layer = 0; layer <= number_layers; layer++)
   {
       FC_array    .set(layer,(float32)hydraulic_properties->get_field_capacity_volumetric(layer));
       PWP_array   .set(layer,(float32)hydraulic_properties->get_permanent_wilt_point_volumetric(layer));
       WC_sat_array.set(layer,(float32)hydraulic_properties->get_saturation_water_content_volumetric(layer,0.0));
   }
   UED::Data_record_creation_layout FC_record_creation
   (UED::Record_type_distinguisher_interface::UED_data_vector_date_var_units,CSVC_soil_base_field_capacity
   ,UC_m3_per_m3,UT_day ,UT_arbitrary_date,simulated_quality);
   FC_record_creation.allow_arbitrary_time_stamp();
   UED::Data_record_creation_layout PWP_record_creation
      (UED::Record_type_distinguisher_interface::UED_data_vector_date_var_units
      ,CSVC_soil_base_permanent_wilt_point
      ,UC_m3_per_m3,UT_day ,UT_arbitrary_date,simulated_quality);
   PWP_record_creation.allow_arbitrary_time_stamp();
   UED::Data_record_creation_layout WC_sat_record_creation
      (UED::Record_type_distinguisher_interface::UED_data_vector_date_var_units
      ,CSVC_soil_base_saturation_water_content                                   //010216
      ,UC_m3_per_m3,UT_day ,UT_arbitrary_date,simulated_quality);
   WC_sat_record_creation.allow_arbitrary_time_stamp();
   UED_units_code preferred_units_code = UC_m3_per_m3;
   bool record_created;
   datetime64 now(simdate.get_datetime64());
   CORN::Quality_clad simulated_qual(simulated_quality);                         //090316
   output_database.set_vector(FC_array ,now
      ,preferred_units_code
      ,simulated_qual,FC_record_creation,record_created);
   output_database.set_vector(PWP_array ,now
      ,preferred_units_code
      ,simulated_qual,PWP_record_creation,record_created);
   output_database.set_vector(WC_sat_array ,now
      ,preferred_units_code
      ,simulated_qual,WC_sat_record_creation,record_created);
}
//_output_hydraulic_properties______________________________________2007-02-09_/
#if (CROPSYST_VERSION==4)
bool CropSyst::Crop_complete::setup_structure(Data_record &data_rec,bool for_write)
{  UNUSED_arg(for_write);
   data_rec.set_current_section("crop_base");  // note, section name should already be set

   #ifdef THERMAL_TIME_OBSOLETE_2020
   if (desired_vars.is_desired(CSVC_crop_base_growing_degree_days_period_accum)) data_rec.expect_float64("degdays"      ,thermal_time->accum_degree_days_clipped[0]); //040830
   if (desired_vars.is_desired(CSVC_crop_base_seasonal_thermal_time_period_sum)) data_rec.expect_float64("thermaltime"  ,thermal_time->accum_degree_days_normal[0]);  //071127
   #else
   if (desired_vars.is_desired(CSVC_crop_base_growing_degree_days_period_accum)) data_rec.expect_float64("degdays"      ,thermal_time->GDDs); //200211
   if (desired_vars.is_desired(CSVC_crop_base_seasonal_thermal_time_period_sum)) data_rec.expect_float64("thermaltime"  ,thermal_time->GDDs); //200211

   #endif
   if (desired_vars.is_desired(CSVC_crop_biomass_canopy))                        data_rec.expect_float64("canopybiomass",reported.daily.canopy_biomass); //070207
   if (desired_vars.is_desired(CSVC_crop_base_root_biomass_current))             data_rec.expect_float64("rootbiomass"  ,act_root_biomass);  // was CSVC_crop_base_root_biomass  //070207
   if (desired_vars.is_desired(CSVC_crop_base_leaf_area_index))                  data_rec.expect_float64("LAI"          ,reported.daily.LAI_for_self_shading_response);  // May want to be the canopy_LAI
   if (desired_vars.is_desired(CSVC_crop_base_green_area_index))                 data_rec.expect_float64("GAI"          ,reported.daily.GAI);
   if (desired_vars.is_desired(CSVC_crop_base_root_depth))                       data_rec.expect_float64("rootdepth"    ,reported.daily.root_depth);
   if (desired_vars.is_desired(CSVC_crop_base_water_stress_factor))              data_rec.expect_float64("waterstress"  ,water_stress_stats.ref_current()); //160803
   if (desired_vars.is_desired(CSVC_crop_base_temperature_stress_factor))        data_rec.expect_float64("tempstress"   ,report_temperature_stress_stats.ref_current());//160803

//NYI/*_______*/    if (desired_vars.is_desired(xxxx))data_rec.expect_float64(,total_stress                    ,"totalstress" ,UED_index                          ,UED_index         );
   if (desired_vars.is_desired(CSVC_crop_base_fract_canopy_cover) && canopy_leaf_growth)
      data_rec.expect_float64("canopycover",canopy_leaf_growth->reference_fract_canopy_cover()); //080801
   if (desired_vars.is_desired(CSVC_crop_base_transp_pot_time_step_sum))
      data_rec.expect_float64("transppot",pot_transpiration_m_unadj);            //190125

   // conceptual add CO2 adjusted if (desired_vars.is_desired(CSVC_crop_base_transp_pot_time_step_sum))         data_rec.expect_float64("transppot"    ,pot_transpiration_m_CO2_unadj);
   if (desired_vars.is_desired(CSVC_crop_base_transp_act_time_step_sum))         data_rec.expect_float64("transpact"    ,reported.daily.transpiration_act_m);
   if (desired_vars.is_desired(CSVC_crop_base_water_intrcpt_time_step_sum))      data_rec.expect_float64("canopyintercept",intercepted_precipitation_m_stored);  //160414_031120
// NYI this will be needed when I drop soil profile outputs
///*_______*/    if (desired_vars.is_desired(CSVC_crop_base_root_fraction))              data_rec.expect_float64_array("root_fraction"   ,	XXXXXXXXXXXX,true,(uint16)get_number_sublayers(),true);
   if (nitrogen)                                                                 //020504
      if (desired_vars.is_desired(CSVC_crop_N_stress_factor))	data_rec.expect_float32("Nstress" ,nitrogen->ref_stress());
   return true;
}
#endif
//_setup_structure__________________________________________________2003-08-01_/

