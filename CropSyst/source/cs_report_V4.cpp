#include "cs_report_V4.h"
#if defined(XLS_OUTPUT)
#include "csvc.h"
#include "cs_accum.h"
#include "cs_mgmt.h"
#include "corn/measure/measures.h"
#include "land_unit_sim.h"
#include "soil/chemicals_profile.h"
#include "soil/erosion_RUSLE.h"
#include "soil.h"
#include "soil/soil_evaporator.h"
#include "common/residue/residues_interface.h"
#include "cs_scenario_directory.h"
#include "cs_ET.h"
#include "common/weather/weather.h"
#include "crop/crop_interfaced.h"
#include "soil/runoff.h"
#include "cs_mgmt.h"
#include "static_phrases.h"
#include "crop/crop_N_interface.h"
#include "UED/library/UED_codes.h"
#include "organic_matter/OM_residues_profile_abstract.h"
#include "soil/nitrogen_profile.h"
#include "crop/phenology_I.h"
#include "corn/chronometry/date_32.h"
//______________________________________________________________________________
CropSyst_report::CropSyst_report
(CropSyst::Land_unit_simulation_VX&  _simulation                                 //010202
,const char*               _report_filename                                      //030110
,const Report_options &     options_to_copy                                      //981203
,report_types              _report_type
,nat16                     max_detail_lines)                                     //981203
:Report_writer(_report_filename,options_to_copy, max_detail_lines + FIRST_DATA_LINE_ROW)
,simulation(_simulation)
,report_type(_report_type)
{  report_options.setup_write_enabled();                                         //981203
}
//_CropSyst_report:constructor_________________________________________________/
CropSyst_YLY_report::CropSyst_YLY_report
(CropSyst::Land_unit_simulation_VX &_simulation                                  //010202
,const Report_options &options_to_copy                                           //981203
,nat16 max_detail_lines)                                                         //981203
:CropSyst_report
(_simulation
,_simulation.scenario_directory->Output().annual_xls().c_str()
,options_to_copy                                                                 //981203
,YLY
,max_detail_lines                                                                //981203
)
{  report_options.accumulation_mode_clad.set(ANNUAL_ACCUM);
}
//_CropSyst_YLY_report:constructor_____________________________________________/
#ifdef NITROGEN
CropSyst_N_balance_report::CropSyst_N_balance_report
(  const char *_report_filename
  ,const Report_options &options_to_copy
  ,nat16 max_detail_lines )
:Report_writer(_report_filename,options_to_copy, max_detail_lines + FIRST_DATA_LINE_ROW)
,NO3_accums_M(0)
,NH4_accums_M(0)
,accums(0)
{  //DLY accumulation mode for daily report is derived from report format option}
}
//_CropSyst_N_balance_report:constructor____________________________1999-03-18_/
void CropSyst_N_balance_report::set_reporters
(Chemical_balance_accumulators_molecular *NO3_accums_                            //041001
,Chemical_balance_accumulators_molecular *NH4_accums_                            //041001
,CropSyst::Period_accumulators *accums_)
{  NO3_accums_M   = NO3_accums_;                                                 //041001
   NH4_accums_M   = NH4_accums_;                                                 //041001
   accums = accums_;
}
#endif
//______________________________________________________________________________
CropSyst_DLY_report::CropSyst_DLY_report
(CropSyst::Land_unit_simulation_VX &i_simulation                                 //010202
,const Report_options &options_to_copy                                           //981203
,const char *DLY_report_filename                                                 //030110
,nat16 max_detail_lines)                                                         //981203
:CropSyst_report
(i_simulation
,DLY_report_filename                                                             //990312
,options_to_copy                                                                 //981203
,DLY
,max_detail_lines)                                                               //981203
{  //DLY accumulation mode for daily report is derived from report format option}
}
//_CropSyst_DLY_report:constructor_____________________________________________/
CropSyst_GS_report::CropSyst_GS_report
(CropSyst::Land_unit_simulation_VX &_simulation                                  //010202
,const Report_options &options_to_copy                                           //981203
,nat16 max_detail_lines)                                                         //981203
:CropSyst_report
(_simulation
,_simulation.scenario_directory->Output().season_xls().c_str()                   //051019
,options_to_copy                                                                 //981203
,GS
,max_detail_lines)                                                               //981203
{  report_options.accumulation_mode_clad.set(GROWTH_PERIOD_ACCUM);
}
//_CropSyst_GS_report:constructor______________________________________________/
CropSyst_report::~CropSyst_report()
{
  //Not needed because we can use references in c++
  //simulation = 0; //{ Don't delete simulation, only the reference!}
}
//_CropSyst_GS_report:destructor_______________________________________________/
#ifdef NITROGEN
void CropSyst_N_balance_report::write_headers(bool with_crop_description)
{  write_column_header(&DLY_report_var_opts[1],5,""); //DLY_year
   write_column_header(&DLY_report_var_opts[2],5,""); //DLY_month
   write_column_header(&DLY_report_var_opts[3],5,""); //DLY_day
   write_column_header(&DLY_report_var_opts[4],5,""); //DLY_day_of_year
   for (nat16 hdr = 1; hdr < MAX_NB_entries; hdr++)                              //020416
      write_grouped_column_header(NB_report_var_opts[hdr],FIRST_COLUMN_HEADER_LINE_ROW);
   if (with_crop_description)                                                    //160326
      write_column_header(&GS_report_var_opts[1],5,""); // crop name             //160326
   endl();
}
//_write_headers_______________________________________________________________/
void CropSyst_N_balance_report::write_detail_line
(const CORN::Date_const &today
,const char *crop_description)                                                   //160326
{
   CORN::Year  year;
   CORN::Month month;
   CORN::DOM   dom;
   today.gregorian(year,month,dom);                                              //990105
   CORN::DOY   doy = today.get_DOY();
   write_integer((int16)year,5);
   write_integer((int16)doy,4);                                                  //080222
   write_integer((int16)month,3);
   write_integer((int16)dom,3);
   if (NO3_accums_M && NH4_accums_M)
   {
      float64 ha_NO3_balance                    = over_m2_to_ha * NO3_accums_M->get_balance_error(true);
      float64 ha_NH4_balance                    = over_m2_to_ha * NH4_accums_M->balance_error;  //190909

      /*190810
            // don't actually need to covert to elemental
      float64 ha_NH4_balance                    = over_m2_to_ha * NH4_accums_M->molecular_to_elemental*(NH4_accums_M->balance_error);
      */

      float64 ha_NO3_original_profile_content   = over_m2_to_ha * NO3_accums_M->get_original_profile_content(true);
      float64 ha_NH4_original_profile_content   = over_m2_to_ha * NH4_accums_M->get_original_profile_content(true);
      float64 ha_initial_N_content  = ha_NO3_original_profile_content + ha_NH4_original_profile_content;
      float64 ha_NO3_final_profile_content      = over_m2_to_ha * NO3_accums_M->get_current_profile_content(true);
      float64 ha_NH4_final_profile_content      = over_m2_to_ha * NH4_accums_M->get_current_profile_content(true);
      float64 ha_final_N_content    = ha_NO3_final_profile_content + ha_NH4_final_profile_content;
      float64 ha_NO3_recalibration              = over_m2_to_ha * NO3_accums_M->get_recalibration(true);    //011119
      float64 ha_NH4_recalibration              = over_m2_to_ha * NH4_accums_M->get_recalibration(true);    //011119
      float64 ha_N_recalibration    = ha_NO3_recalibration + ha_NH4_recalibration;                          //011119
      float64 ha_balance_NO3_leached            = over_m2_to_ha * NO3_accums_M->get_balance_leached(true);  //981230
      float64 ha_balance_NH4_leached            = over_m2_to_ha * NH4_accums_M->get_balance_leached(true);  //981230

      float64 ha_NO3_applied                    = over_m2_to_ha * NO3_accums_M->get_applied(true);
      float64 ha_NH4_applied                    = over_m2_to_ha * NH4_accums_M->get_applied(true);

      float64 ha_NO3_uptake                     = over_m2_to_ha * NO3_accums_M->get_uptake(true);
      float64 ha_NH4_uptake                     = over_m2_to_ha * NH4_accums_M->get_uptake(true);

      float64 ha_NH3_volatilization_loss        = over_m2_to_ha * accums->NH3_volatilization_loss;          //021212
      float64 ha_inorganic_NH4_to_atmosphere    = over_m2_to_ha * accums->inorganic_NH4_to_atmosphere;
      float64 ha_organic_N_applied              = over_m2_to_ha * accums->organic_N_applied;

//Currently we only have the total organic matter mineralization (including OM and residues)
// But I think I can separate these again now.

      float64 ha_OM_mineralization_N   = over_m2_to_ha * accums->OM_N_mineralization;  // elemental 110918
      float64 ha_straw_mineralization         = 0.0; //over_m2_to_ha * accums->straw_mineralization_V4_1;
      float64 ha_manure_mineralization        = 0.0;//over_m2_to_ha * accums->manure_mineralization_V4_1;

      float64 ha_N_immobilization            = over_m2_to_ha * accums->N_immobilization; //040209

      float64 ha_denitrification   = over_m2_to_ha *
         accums->N_denitrification;                                              //170503
         // denitrified_N_output
         //NO3_accums->get_transformed_to(true)  * NO3_to_N_conv //170502

         ;
      float64 ha_nitrification     = over_m2_to_ha *
         accums->N_nitrification;                                                //170503
         //170503a NO3_accums->get_transformed_from(true)
         //accums->N_nitrification;
//            * NH4_to_N_conv; //170502
       ;
      // The input and output are special for Luca's VATE model.
      //040931  I want to create a soil chemical balance to output these

      float64 ha_N2O_loss_denitrification = over_m2_to_ha * accums->N2O_loss_denitrification; // This is not included in the balance because is a portion of the denitrification //060606
      float64 ha_N2O_loss_nitrification   = over_m2_to_ha * accums->N2O_loss_nitrification; // This is not included in the balance because is a portion of the nitrification     //090722

      float64 balance =  // kg/ha
        + ha_initial_N_content
        + ha_NO3_applied +ha_NH4_applied
//Not sure why this is not in the balance I think it was simply added to NO3 or NH4 /*090212_*/         + ha_organic_N_applied
        + ha_manure_mineralization + ha_straw_mineralization
        + ha_OM_mineralization_N   // 060717 Currently all mineralization is in total OM including straw  and manure residue
        - ha_N_immobilization                                                    //040207
        - ha_balance_NO3_leached - ha_balance_NH4_leached                        //981230
        - ha_denitrification
        - ha_NO3_uptake - ha_NH4_uptake
        - ha_inorganic_NH4_to_atmosphere
        - ha_NH3_volatilization_loss                                             //021212
        - ha_N_recalibration                                                     //011119
        - ha_N2O_loss_nitrification                                              //170502

//- ha_N2O_loss_denitrification

        - ha_final_N_content;

//cout << '\n' << ha_NH3_volatilization_loss << '\n';
//std::clog << "loss:" <<  ha_N2O_loss_denitrification << std::endl;
/*
if (ha_N2O_loss_denitrification > 0.0000000001)
std::clog << "N2Oloss:" << ha_N2O_loss_denitrification;
*/
/*
if (!CORN::is_zero<float64>(balance,0.01))
std::clog << "N balance:" << balance
<< "\tN2Oloss:" << (ha_N2O_loss_denitrification)
//<< "\tN2Odenit:" << N2O_loss_denitrification << "\tNO2nitr:" << N2O_loss_nitrification
<< std::endl;
*/

     write_real(ha_NH4_applied,8,3);
     write_real(ha_organic_N_applied,8,3);   // 060717 Currently all mineralization is in total OM including straw  and manure residue

     write_real(ha_straw_mineralization,8,3);
     write_real(ha_manure_mineralization,8,3);
     write_real(ha_OM_mineralization_N,8,3);
     write_real(ha_balance_NH4_leached,8,3);
     write_real(ha_NH4_uptake,8,3);
     write_real(ha_NH3_volatilization_loss,8,3);                                 //021212
     write_real(ha_inorganic_NH4_to_atmosphere,8,3);
     write_real(ha_NH4_original_profile_content,8,3);
     write_real(ha_NH4_final_profile_content,8,3);
     write_real(ha_NH4_balance,8,3);

      write_real(ha_NO3_applied,8,3);
      write_real(ha_nitrification,8,3);
      write_real(ha_balance_NO3_leached,8,3);                                    //981230
      write_real(ha_NO3_uptake,8,3);                                             //981230
      write_real(ha_denitrification,8,3);
      write_real(ha_NO3_original_profile_content,8,3);
      write_real(ha_NO3_final_profile_content,8,3);

/* This balance correction is just a hack because
 I broke the transformed_to amount.
ha_NO3_balance += ha_nitrification
- ha_denitrification         ;                                        //200522
*/
      write_real(ha_NO3_balance,8,3);

     // Total N values
      write_real(ha_NH4_applied + ha_NO3_applied,8,3);
      write_real(ha_organic_N_applied,8,3);
      write_real(ha_OM_mineralization_N
               +ha_straw_mineralization
               +ha_manure_mineralization
               ,8,3);
         //C urrently all mineralization is in total OM including straw  and manure residue. 060717
      write_real(ha_NH3_volatilization_loss                                      //021212
               +ha_inorganic_NH4_to_atmosphere
               +ha_denitrification,8,3);
      write_real(ha_balance_NH4_leached + ha_balance_NO3_leached,8,3);           //981230
      write_real(ha_NH4_uptake + ha_NO3_uptake,8,3);                             //981230
      write_real(ha_N_immobilization,8,3);                                       //040209
      write_real(ha_initial_N_content,8,3);
      write_real(ha_final_N_content,8,3);
      write_real(balance,8,3);
      write_real(ha_N2O_loss_denitrification,8,3);                               //060606
      write_real(ha_N2O_loss_nitrification,8,3);                                 //090722
   }
   if (crop_description)                                                         //160326
   {  std::string crop_desc(crop_description);                                   //160326
      write_string_aligned(CSS_justify_left,crop_desc);                          //160326
   }
   endl();
}
#endif
//_write_detail_line___________________________________________________________/
void CropSyst_report::write_headers()
{
   report_var_layout_with_graph *report_var_opts;
   std::string translated_report_name;                                           //020330
   if (report_options.write_enabled)
   {  switch (report_type)
      {  case DLY :translated_report_name.assign(TL_Daily_report);        break;
         case YLY :translated_report_name.assign(TL_Yearly_report);       break;
         case GS  :translated_report_name.assign("Growing season report");break; //030516
         case FLW :translated_report_name.assign(TL_Fallow);              break;
      }
     char LBF_ID_str[10];                                                        //131007
     std::string sim_desc(simulation.ID.code);                                   //151128
     sim_desc.append(" ");                                                       //131007
     sim_desc.append(simulation.scenario_control_and_model_options.description.brief); //150112_131007

     std::string sim_loc_desc (
       ""
         // simulation.ET_weather.description  // WARNING TEMPORARILY DISABLED
     );
     std::string sim_soil_desc(
       ""
         // simulation.environment.soil.description   // WARNING TEMPORARILY DISABLED
     );
     write_header(translated_report_name                                         //981211
                 ,sim_desc
                 ,sim_loc_desc
                 ,sim_soil_desc);
     for (nat16 i = 1; i <= report_options.num_vars_to_print; i++)
     {  switch (report_type)
        {  case DLY : report_var_opts = lookup_report_var_layout(DLY_report_var_opts,DLY_MAX_IDS,report_options.sel_var_code[i]);      break;      //000926
           case YLY : report_var_opts = lookup_report_var_layout(YLY_report_var_opts,MAX_YLY_entries,report_options.sel_var_code[i]);  break;      //000926
           // At this time the fallow report uses the harvest report format
           case GS  : case FLW : report_var_opts = lookup_report_var_layout(GS_report_var_opts,GS_MAX_IDS,report_options.sel_var_code[i]); break;  //030516
        }
        write_column_header(report_var_opts,5,"");
     }
     endl();
  }
}
//_write_headers_______________________________________________________________/
void CropSyst_report::write_detail_line()
{
   detail_types detail_type;
   std::string  value_str;
   int16        int_value_;
   float64      num_value ;
   int8         width;
   int8         precision_;
   // Static value used internal to this function
   //  This is special variable because denitrification is not kept as a chemical
   if (report_options.write_enabled)
   {
       for (nat16 dc = 1; dc <=  report_options.num_vars_to_print ;dc++)
       {
          get_detail(dc,value_str,int_value,num_value,detail_type,width,precision_);
          switch (detail_type)
          {  case detail_INTEGER : write_integer(int_value,width); break;
             case detail_REAL    : write_real(num_value,width,precision_); break;
             case detail_STRING  : write_string_aligned(CSS_justify_left,value_str); break;
          }
       }
      endl();
   }
}
//_write_detail_line___________________________________________________________/
void CropSyst_DLY_report::get_detail
(nat16    dc
,std::string &value_str
,int16    &int_value
,float64  &num_value
,detail_types &detail_type
,int8    &width
,int8    &precision_)
{
   CropSyst::Crop_model_interface *crop = (simulation.get_crop());               //200404_190621

   CropSyst::Period_accumulators *GP_accumulators         = simulation.GP_accumulators_active;         //030912
   CropSyst::Annual_accumulators *ANNL_accumulators       = simulation.ANNL_accumulators_active;       //030912
   CropSyst::Period_accumulators *CONTINUOUS_accumulators = simulation.CONTINUOUS_accumulators_active; //030912
   ET_Reference  &ET_ref = *simulation.ET_ref;                                   //150811_010202
   const CropSyst::Soil_components *soil = simulation.ref_soil();                //161008_060803
      // for local reference
   const CropSyst::Soil_runoff_common *runoff =
      dynamic_cast<const CropSyst::Soil_runoff_common *>                         //200316
      (simulation.runoff);                                                       //131101_130308
#ifdef NITROGEN
   const Soil::NO3_Profile *NO3 = (soil && soil->chemicals) ? soil->chemicals->NO3 : 0;
   const Soil::NH4_Profile *NH4 = (soil && soil->chemicals) ? soil->chemicals->NH4 : 0;
#endif
   const Residues_interface *residue_pools_kg_m2 = simulation.organic_matter_residues_profile_kg_m2 ? simulation.organic_matter_residues_profile_kg_m2->ref_residues() : 0;   //060115
   report_var_layout_with_graph *var_layout = lookup_report_var_layout(DLY_report_var_opts,DLY_MAX_IDS, report_options.sel_var_code[dc]);                               //000926
   width = var_layout ? var_layout->w : 5;                                       //000926
   precision_ = var_layout ? var_layout->d : 0;                                  //000926
   detail_type = detail_REAL;
   int_value = 0.0;                                                              //011127
   num_value = 0.0;                                                              //011031
   static float64 last_NO3_applied_E = 0;                                        //011127
   static float64 last_NH4_applied_E = 0;                                        //011127
   static float64 last_NO3_uptake_E = 0;                                         //011127
   static float64 last_NH4_uptake_E = 0;                                         //011127
   static float64 last_NO3_leached_E = 0;                                        //011127
   static float64 last_NH4_leached_E = 0;                                        //011127
   {  CORN::Year year; CORN::Month month; CORN::DOM dom;                         //030912
      CORN::Date_clad_32 formatted_today(simulation.get_today_date32());         //030912
      switch (report_options.sel_var_code[dc])                                   //030912
      {   // for date fields, format the  date only for fields as needed         //030912
        case DLY_year                                 :                          //030912
        case DLY_day_of_year                          :                          //030912
        case DLY_month                                :                          //030912
        case DLY_day                                  :                          //030912
            formatted_today.gregorian(year,month,dom);                           //030912
         break;                                                                  //030912
      }
      //WITH ANNL_accumulators^ DO
      if (crop)                                                                  //070227
      { switch (report_options.sel_var_code[dc])
        {
        #ifdef THERMAL_TIME_OBSOLETE_2020
        case CSVC_crop_base_growing_degree_days_period_accum: num_value= crop->get_accum_degree_days(true);    break; //040830
        case CSVC_crop_base_seasonal_thermal_time_period_sum : num_value= crop->get_accum_degree_days(false);   break; //071127
        #else
        case CSVC_crop_base_growing_degree_days_period_accum: num_value= crop->get_accum_degree_days();    break; //200211
        case CSVC_crop_base_seasonal_thermal_time_period_sum : num_value= crop->get_accum_degree_days();   break; //200211
        #endif
        case CSVC_crop_biomass_canopy                 : num_value= crop->get_dry_biomass_kg_ha() ;              break; //070207
        case CSVC_crop_base_green_biomass             : num_value= crop->get_live_green_canopy_biomass_kg_ha(); break; //070207
        case CSVC_crop_base_root_biomass_current      : num_value= crop->get_act_root_biomass_kg_ha();          break; //010716
        #ifdef CROP_ORCHARD
        case DLY_fruit_biomass                        : num_value= crop->get_fruit_dry_biomass_kg_ha();         break; //030529
        case DLY_fruit_canopy_biomass                 : num_value= crop->get_canopy_biomass_kg_ha();            break; //030529
        case CSVC_crop_base_fruit_reserves_biomass    : num_value= crop->get_fruit_reserves_biomass_kg_ha();    break; //200608

        case CSVC_crop_base_orchard_solid_canopy_interception  :  num_value=crop->get_orchard_solid_canopy_interception(); break; //081020
        case CSVC_crop_base_orchard_shaded_area_GAI            :  num_value=crop->get_orchard_shaded_area_GAI          (); break; //081020
        case CSVC_crop_base_orchard_canopy_porosity            :  num_value=crop->get_orchard_canopy_porosity          (); break; //081020
        #endif

        case DLY_leaf_area_index                      : num_value= crop->get_LAI(false);                  break;
        case DLY_green_area_index                     : num_value= crop->get_GAI(include_vital|include_effete); break;  // 060921 was only vital
        case DLY_root_depth                           : num_value= crop->update_root_depth(soil? soil->get_rooting_depth():0,soil? soil->ref_layers()->get_depth_profile_m()   /*061005 soil->ref_layers()->get_depth_m()*/:0);  break; // In meters not mm //010724
        case DLY_crop_water_stress  /* index */       : num_value= crop->get_water_stress_index();       break;   //010726
        case DLY_temperature_stress /* index */       : num_value= crop->get_temperature_stress_index(); break; //{ The only place we show stress as the inverse  //010726
         #ifdef OLD_YIELD
        case CSVC_crop_base_tuber_init_temp_stress_index  : num_value= crop->get_tuber_init_temperature_stress_index(); break; //{ The only place we show stress as the inverse  //081111
        case CSVC_crop_base_tuber_fill_temp_stress_index  : num_value= crop->get_tuber_fill_temperature_stress_index(); break; //{ The only place we show stress as the inverse  //081111
        #else
         //150825 evenually reimplement this with inspector idiom
        #endif
         case DLY_VPD_daytime                          : num_value= crop->get_VPD_daytime();              break;   //030530
         case DLY_intercepted_PAR                      : num_value= crop->get_intercepted_PAR_MJ_m2();    break;   //030530
         case DLY_intercepted_PAR_accum                : num_value= crop->get_intercepted_PAR_accum_MJ_m2(); break;//030530
                // This is now PAR or total depending on the mode (PAR) is becoming deprecated
         case DLY_canopy_ground_cover                  : num_value= crop->get_canopy_interception_global_green() /*200228 get_ fract_canopy_cover_green()*/ * 100.0;       break;  // report shows the units a percent not a fraction   //041031 //041031

//get_fract_canopy_cover_total
//
// Should output green and total canopy cover,  temporarily outputing green as total

         case CSVC_crop_base_leaf_water_pot            : num_value= crop->get_leaf_water_pot();     break;   //071115
         case DLY_crop_water_inter                     : num_value= m_to_mm(crop->get_intercepted_precipitation_m()); break; //160419
         case DLY_crop_name                            : detail_type = detail_STRING;      //000926
              value_str.assign(crop->get_description()); //200420 value_str.resize(width);
         break;
         case DLY_growth_stage                         :
         {
            detail_type = detail_STRING;
            value_str.clear();
            CORN::Text_list stage_descriptions;                                  //200404
            crop->describe_periods(stage_descriptions);                          //200404
            stage_descriptions.string_items(value_str,'&');                      //200404
            /*200420
             crop->describe_growth_stage(value_str); value_str.resize(width);    //190613
             */
         }break;
         case DLY_transp_pot                           : num_value= m_to_mm(crop->get_pot_transpiration_m(CO2_ADJUSTED));         break;

         // WARNING this is not adjusted transpiration I should
         // have seperate  output 160311

         //NYI case DLY_transp_pot_CO2_adjusted                         : num_value= m_to_mm(crop->get_pot_transpiration_m(true));         break;
         case DLY_transp_act                           : num_value
            = m_to_mm(crop->ref_reported().daily.transpiration_act_m); break;    //190705_000323
         }
       }
       switch (report_options.sel_var_code[dc])
       {
         case DLY_year                                 : detail_type = detail_INTEGER; int_value = year;           break;
         case DLY_day_of_year                          : detail_type = detail_INTEGER; int_value = formatted_today.get_DOY();  break;   //030714
         case DLY_month                                : detail_type = detail_INTEGER; int_value = month;          break;
         case DLY_day                                  : detail_type = detail_INTEGER; int_value = dom;            break;
         case DLY_residue_ground_cover                 : num_value= residue_pools_kg_m2 ? residue_pools_kg_m2->get_total_fract_ground_cover(INCLUDE_POSITION_FLAT_SURFACE| INCLUDE_POSITION_STUBBLE_SURFACE/*060328 both_flat_and_standing*/) * 100.0 : 0.0 ;       break;     // report shows the units a percent not a fraction //041031
         case DLY_residue_water_storage                : num_value= residue_pools_kg_m2 ?
             m_to_mm(
             residue_pools_kg_m2->get_curr_water_hold() *
              residue_pools_kg_m2->get_surface_plant_biomass(INCLUDE_POSITION_FLAT_SURFACE|INCLUDE_POSITION_STUBBLE_SURFACE)                    //170107_090312
             ) : 0;     break;
         case CSVC_residue_surface_biomass             : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_FLAT_SURFACE|INCLUDE_POSITION_STUBBLE_SURFACE,(INCLUDE_STRAW_RESIDUE_OM | INCLUDE_MANURE_RESIDUE_OM))):0.0;break;   //050116
         case CSVC_residue_plant_surface               : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_FLAT_SURFACE|INCLUDE_POSITION_STUBBLE_SURFACE,INCLUDE_STRAW_RESIDUE_OM))                              :0.0;break;   //050116
         case CSVC_residue_manure_surface              : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_FLAT_SURFACE|INCLUDE_POSITION_STUBBLE_SURFACE,INCLUDE_MANURE_RESIDUE_OM))                             :0.0;break;   //050116
         case CSVC_residue_incorporated                : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_SUBSURFACE,INCLUDE_STRAW_RESIDUE_OM|INCLUDE_MANURE_RESIDUE_OM))                                       :0.0;break;   //050116
         case CSVC_residue_plant_incorporated          : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_SUBSURFACE,INCLUDE_STRAW_RESIDUE_OM))                                                                 :0.0;break;   //050116
         case CSVC_residue_manure_incorporated         : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_SUBSURFACE,INCLUDE_MANURE_RESIDUE_OM))                                                                :0.0;break;   //050116
         case DLY_soil_clod_rate                       : if (soil && soil->erosion) num_value= soil->erosion->clod_factor_rate;                  break;
         case DLY_soil_loss                            : num_value= 0.0;                    break; //{over_m2_to_ha * daily_RUSLE.soil_loss not currently availabled}

         case DLY_ET_max                               : num_value= m_to_mm(simulation.output_crop_evapotransp_max_m); break; //190812_160601_150811_010910
         case DLY_ET_act                               : num_value= m_to_mm(simulation.output_act_crop_evapotransp_m); break; //160601_010910
         case CSVC_weather_ET_ref_time_step_sum        :
            num_value= ET_ref.get_mm();        break;  //130723
            //180416 num_value= m_to_mm(ET_ref.pot_ref_evapotransp_m);        break;  //130723
         case  DLY_evap_soil_pot                       : num_value=
            m_to_mm(soil?soil->ref_evaporator()->get_evaporation_potential_day():0); //160412
         break;
         case  DLY_evap_soil_act                       : num_value= m_to_mm(soil?soil->ref_evaporator()->get_evaporation_actual_day():0);            break;
         case  DLY_evap_residue_pot                    : num_value= residue_pools_kg_m2 ? m_to_mm(residue_pools_kg_m2->get_pot_evaporation()):0.0;   break;
         case  DLY_evap_residue_act                    : num_value= residue_pools_kg_m2 ? m_to_mm(residue_pools_kg_m2->get_act_evaporation()):0.0;   break;
         case  DLY_irrigation                          : num_value= m_to_mm(simulation.total_irrigation_today)       ;   break;
//NYI here I could output biomass_grazed
         case  DLY_precipitation                       :
            num_value=
               simulation.meteorology.                                           //151130
               ref_precipitation_actual().get_mm();   break;                     //151028_990105
         case  UED_STD_VC_Tavg                         : num_value=
               simulation. meteorology                                           //151130
               .ref_air_temperature_avg().Celcius(); break;                      //081111
         case  DLY_residue_water_inter                 : num_value= residue_pools_kg_m2 ? m_to_mm(residue_pools_kg_m2->get_interception() ):0.0;      break;
         case  DLY_surface_water_runoff                : if (runoff) num_value= m_to_mm(runoff->estimated_runoff);            break; //130308
         case  DLY_water_entering_soil                 : num_value= m_to_mm(soil ? soil->get_act_water_entering_soil() : 0);  break; //990716
         case  DLY_soil_water_drainage                 : num_value= m_to_mm(soil ? soil->get_reported_drainage(): 0);         break; //990126
                // eventually we will have a water balance report which will use balance_drainage
                // we will restore here the reported drainage.
         case  DLY_snow_storage                        : num_value= m_to_mm
            (simulation.meteorology.get_snow_water_equivelent_depth()); break;   //151129
         case  CSVC_soil_base_water_depletion_balance_accum_to_date_time                  : num_value= m_to_mm(ANNL_accumulators->water_depletion);         break;   //990311
         case  CSVC_soil_base_water_depletion_seasonal_period_sum                : num_value= GP_accumulators ? m_to_mm(GP_accumulators->water_depletion) : 0.0;         break;   //131007
         case  DLY_soil_water_balance                  : num_value= m_to_mm
            (ANNL_accumulators->water_balance);          break;                  //990311
//             (CONTINUOUS_accumulators->water_balance);          break; //990311
#ifdef NITROGEN
         case  DLY_nitrogen_applied                    :
         if (NO3 && NH4 && NO3->CONT_bal_accums_M)                               //000606
         {                                                                       //000606
            float64 curr_NO3_applied_E = NO3->CONT_bal_accums_M->get_applied(true);//000606
            float64 curr_NH4_applied_E = NH4->CONT_bal_accums_M->get_applied(true);//000606
             num_value= over_m2_to_ha * ( (curr_NO3_applied_E - last_NO3_applied_E)   +  (curr_NH4_applied_E - last_NH4_applied_E) );
            last_NO3_applied_E = curr_NO3_applied_E;                             //011127
            last_NH4_applied_E = curr_NH4_applied_E;                             //011127
         }                            
         break;
         case  DLY_nitrogen_uptake                     :
          if (NO3 && NH4 && NO3->CONT_bal_accums_M)                              //000606
          {                                                                      //000606
             float64 curr_NO3_uptake_E = NO3->molecular_to_elemental*(NO3->CONT_bal_accums_M->uptake); //040206
             float64 curr_NH4_uptake_E = NH4->molecular_to_elemental*(NH4->CONT_bal_accums_M->uptake); //040206
             num_value= over_m2_to_ha *                                          //000606
                           ((curr_NO3_uptake_E - last_NO3_uptake_E)  +           //040206
                            (curr_NH4_uptake_E - last_NH4_uptake_E)  );          //040206
             last_NO3_uptake_E = curr_NO3_uptake_E;                              //040206
             last_NH4_uptake_E = curr_NH4_uptake_E;                              //040206
          }
         break;
         case  DLY_nitrogen_leached                    :
          if (NO3 && NH4 &&NO3->CONT_bal_accums_M)                               //000606
          {                                                                      //000606
             float64 curr_NO3_leached_E = NO3->molecular_to_elemental*(NO3->CONT_bal_accums_M->reported_leached); //040206
             float64 curr_NH4_leached_E = NH4->molecular_to_elemental*(NH4->CONT_bal_accums_M->reported_leached); //040206
             num_value= over_m2_to_ha *                                          //000606
                           ( (curr_NO3_leached_E - last_NO3_leached_E)           //040206
                            +(curr_NH4_leached_E - last_NH4_leached_E) );        //040206
             last_NO3_leached_E = curr_NO3_leached_E;                            //040206
             last_NH4_leached_E = curr_NH4_leached_E;                            //040206
          }
          break;
         case  DLY_organic_N_applied : num_value= over_m2_to_ha *
            simulation.nitrogen_daily_output.applied_organic;                    //131004
         break;
#endif
      }
#ifdef NITROGEN
       Organic_matter_residues_profile_abstract *organic_matter_residues_profile_kg_m2 = simulation.organic_matter_residues_profile_kg_m2;
       if (organic_matter_residues_profile_kg_m2)                                //080306
       switch(report_options.sel_var_code[dc])                                   //080306
       {  case  DLY_N_immobilization       : num_value= over_m2_to_ha * organic_matter_residues_profile_kg_m2->get_N_immobilization_profile()    ;break; //080306
          case  DLY_mineralizated_residue  : num_value= over_m2_to_ha * organic_matter_residues_profile_kg_m2->get_total_straw_mineralization()  ;break; // Will be 0.0 for V4.2 and greater  //080306
          case  DLY_mineralizated_manure   : num_value= over_m2_to_ha * organic_matter_residues_profile_kg_m2->get_total_manure_mineralization() ;break; // Will be 0.0 for V4.2 and greater  //080306
          case  DLY_mineralizated_OM       :                                     //060122
          {                                                                      //060122
             float64 OM_mineralization_kg_m2 =                                   //060122
                (simulation.scenario_control_and_model_options.organic_matter_and_residue_model_clad.get() == V4_1_ORGANIC_MATTER_AND_RESIDUE) //130827_090714
                ? organic_matter_residues_profile_kg_m2->get_NH4_mineralization() //090714
                : organic_matter_residues_profile_kg_m2->get_soil_organic_matter_mineralization_profile_N();  // 060122 need to check this option
             num_value= over_m2_to_ha * OM_mineralization_kg_m2;                 //090714
          }break;
       }                                                                         //080306
       const CropSyst::Crop_nitrogen_interface *crop_N_kg_m2 = crop ? crop->get_nitrogen() : 0; //050722
       if (crop_N_kg_m2)                                                         //020505
      switch (report_options.sel_var_code[dc])                                   //000926
       {
         case  DLY_nitrogen_stress_index               : num_value= crop_N_kg_m2->get_stress_index();  break;      //010726
         case  DLY_avg_nitrogen_stress_index           : num_value= crop_N_kg_m2->get_stress_index_mean();  break; //010726
         case  DLY_nitrogen_uptake_accum               : num_value=
            CONTINUOUS_accumulators ? over_m2_to_ha * (CONTINUOUS_accumulators->N_uptake) : 0.0;
            break;
         case  CSVC_crop_N_mass_canopy_current         : num_value= over_m2_to_ha * crop_N_kg_m2->get_mass_canopy_current();      break; //131007
         case  CSVC_crop_N_mass_canopy_production_accum_to_date_time: num_value= over_m2_to_ha * crop_N_kg_m2->get_mass_canopy_production();      break; //131006_070209_040914
         case  CSVC_crop_N_conc_canopy                 : num_value= crop_N_kg_m2->get_canopy_concentration(include_vital);   break;  // Note this is only looking at the current growth
         case  DLY_automatic_N_in_uptake               : num_value= over_m2_to_ha * crop_N_kg_m2->get_fertilization_requirements();  break;
         case  DLY_N_Fixation                          : num_value= over_m2_to_ha * crop_N_kg_m2->get_fixation();  break;


         case  CSVC_crop_N_canopy_gaseous_loss         : num_value= over_m2_to_ha * crop_N_kg_m2->get_gaseous_loss(); break; //1310625
#define CSVC_crop_N_canopy_gaseous_loss                              0x8023000b
#define CSVC_crop_N_canopy_gaseous_loss_accum_to_date_time           0x8623000b
#define CSVC_crop_N_canopy_gaseous_loss_period_sum                   0x8d23000b
       }
#endif
      switch (report_options.accumulation_mode_clad.get())
      {  case GROWTH_PERIOD_ACCUM :
            //WITH GP_accumulators^ DO
          if (GP_accumulators)                                                    //990311
            switch(report_options.sel_var_code[dc])                               //000926
            {
            case  DLY_ET_max_accum                     : num_value= m_to_mm(GP_accumulators->evapotranspiration_max); break;
            case  DLY_ET_act_accum                     : num_value= m_to_mm(GP_accumulators->evapotranspiration_act); break;
            case  CSVC_weather_ET_ref_accum_to_date_time:num_value= m_to_mm(GP_accumulators->evapotranspiration_ref); break; //130723
            case  DLY_transp_pot_accum                 : num_value= m_to_mm(GP_accumulators->pot_transpiration);      break;
            case  DLY_transp_act_accum                 : num_value= m_to_mm(CORN::must_be_0_or_greater<float64>(GP_accumulators->act_transpiration));break;
            case  DLY_evap_soil_pot_accum              : num_value= m_to_mm(GP_accumulators->pot_soil_evaporation);   break;
            case  DLY_evap_soil_act_accum              : num_value= m_to_mm(GP_accumulators->act_soil_evaporation);   break;
            case  DLY_evap_residue_pot_accum           : num_value= m_to_mm(GP_accumulators->pot_residue_evaporation);break;
            case  DLY_evap_residue_act_accum           : num_value= m_to_mm(GP_accumulators->act_residue_evaporation);break;
            case  DLY_irrig_accum                      : num_value= m_to_mm(GP_accumulators->irrigation);             break;
            case  DLY_precip_accum                     : num_value= m_to_mm(GP_accumulators->precip_m);               break;
            case  DLY_crop_water_intrcpt_accum         : num_value= m_to_mm(GP_accumulators->crop_intercept);         break; //was crop_intercept //160419
            //160419 case  DLY_crop_water_intrcpt_accum         : num_value= m_to_mm(GP_accumulators->crop_intercept_stored);         break; //was crop_intercept
            case  DLY_residue_water_intrcpt_accum      : num_value= m_to_mm(GP_accumulators->residue_interception);   break;
            case  DLY_surface_water_runoff_accum       : num_value= m_to_mm(GP_accumulators->runoff);                 break;
            case  DLY_ponded_water                     : num_value= m_to_mm(simulation.pond.get_depth());              break;
            case  DLY_water_enter_soil_accum           : num_value= m_to_mm(GP_accumulators->act_water_entering_soil);break;   //990716
            case  DLY_soil_water_drain_accum           : num_value= m_to_mm(GP_accumulators->reported_drainage);      break;   //990106
                // eventually we will have a water balance report which will use balance_drainage
                // we will restore here the reported drainage.
#ifdef NITROGEN
            case  DLY_nitrogen_applied_accum           : num_value= NO3 && NH4 && NO3->GS_bal_accums_M
                                                       ?
                                                          over_m2_to_ha *
                            ( NO3->GS_bal_accums_M->get_applied(true)
                            + NH4->GS_bal_accums_M->get_applied(true))
                                                       : 0.0 ;
             break;
            case  DLY_nitrogen_leached_accum           : num_value= NO3 && NH4 && NO3->GS_bal_accums_M
                                                        ? over_m2_to_ha *
                            (NO3->GS_bal_accums_M->get_reported_leached(true)     //981230
                            +NH4->GS_bal_accums_M->get_reported_leached(true) )   //981230
                                                        : 0.0;
             break;
            case  DLY_mineralizated_residue_accum      : num_value= 0.0; break;
            case  DLY_mineralizated_manure_accum       : num_value= 0.0; break;
            case  CSVC_organic_matter_mineralized_period_sum : num_value= over_m2_to_ha * GP_accumulators->OM_N_mineralization; break; //170503 NH4->molecular_to_elemental*(NH4->GP_bal_report->transformed_from_M);break;  //110918
            case  DLY_N_immobilization_accum           : num_value= over_m2_to_ha * GP_accumulators->N_immobilization; break;  //041022
            case  DLY_automatic_N_in_uptake_accum      : num_value= over_m2_to_ha * GP_accumulators->N_fert_requirements; break;
            case  DLY_organic_N_applied_accum          : num_value= over_m2_to_ha * GP_accumulators->organic_N_applied;   break;
            case  DLY_N_Fixation_accum                 : num_value= over_m2_to_ha * GP_accumulators->N_fixation;          break;
#endif
            }
         break;
          default: // continuous is the default mode case CONTINUOUS_ACCUM :
            switch(report_options.sel_var_code[dc])
            {
//190812             case  DLY_ET_pot_accum                     : num_value= m_to_mm(CONTINUOUS_accumulators->pot_evapotranspiration); break;
            case  DLY_ET_max_accum                     : num_value= m_to_mm(CONTINUOUS_accumulators->evapotranspiration_max); break;
            case  DLY_ET_act_accum                     : num_value= m_to_mm(CONTINUOUS_accumulators->evapotranspiration_act); break;
            case  CSVC_weather_ET_ref_accum_to_date_time: num_value= m_to_mm(CONTINUOUS_accumulators->evapotranspiration_ref); break; //130723
            case  DLY_transp_pot_accum                 : num_value= m_to_mm(CONTINUOUS_accumulators->pot_transpiration);      break;

/*200129  continue here!

                  transpiration deman pot
*/

            case  DLY_transp_act_accum                 : num_value= m_to_mm(CORN::must_be_0_or_greater<float64>(CONTINUOUS_accumulators->act_transpiration));  break;
            case  DLY_evap_soil_pot_accum              : num_value= m_to_mm(CONTINUOUS_accumulators->pot_soil_evaporation);   break;
            case  DLY_evap_soil_act_accum              : num_value= m_to_mm(CONTINUOUS_accumulators->act_soil_evaporation);   break;
            case  DLY_evap_residue_pot_accum           : num_value= m_to_mm(CONTINUOUS_accumulators->pot_residue_evaporation);break;
            case  DLY_evap_residue_act_accum           : num_value= m_to_mm(CONTINUOUS_accumulators->act_residue_evaporation);break;
            case  DLY_irrig_accum                      : num_value= m_to_mm(CONTINUOUS_accumulators->irrigation);             break;
            case  DLY_precip_accum                     : num_value= m_to_mm(CONTINUOUS_accumulators->precip_m);               break;
            case  DLY_crop_water_intrcpt_accum         : num_value= m_to_mm(CONTINUOUS_accumulators->crop_intercept);         break; //160419
            case  DLY_residue_water_intrcpt_accum      : num_value= m_to_mm(CONTINUOUS_accumulators->residue_interception);   break;
            case  DLY_surface_water_runoff_accum       : num_value= m_to_mm(CONTINUOUS_accumulators->runoff);                 break;
            case  DLY_ponded_water                     : num_value= m_to_mm(simulation.pond.get_depth());                     break; //010202
            case  DLY_water_enter_soil_accum           : num_value= m_to_mm(CONTINUOUS_accumulators->act_water_entering_soil);break; //990716
            case  DLY_soil_water_drain_accum           : num_value= m_to_mm(CONTINUOUS_accumulators->reported_drainage);      break; //990106
                // eventually we will have a water balance report which will use balance_drainage
                // we will restore here the reported drainage.
#ifdef NITROGEN
            case  DLY_nitrogen_applied_accum           : num_value=  NO3 && NH4 && NO3->CONT_bal_accums_M
                  ? over_m2_to_ha *
                       ( NO3->molecular_to_elemental*(NO3->CONT_bal_accums_M->applied)
                       + NH4->molecular_to_elemental*(NH4->CONT_bal_accums_M->applied))
                  : 0.0;
            break;
            case  DLY_nitrogen_leached_accum           : num_value= NO3 && NH4 && NO3->CONT_bal_accums_M
                   ?   over_m2_to_ha *
                            (NO3->molecular_to_elemental*(NO3->CONT_bal_accums_M->reported_leached)    //981230
                            +NH4->molecular_to_elemental*(NH4->CONT_bal_accums_M->reported_leached) )  //981230
                   : 0.0 ; break;
            case  DLY_mineralizated_residue_accum      : num_value= 0.0;  break;
            case  DLY_mineralizated_manure_accum       : num_value= 0.0;  break;
//NYI /*040225_*/            case  DLY_N_immobilization_accum      : num_value= over_m2_to_ha * CONTINUOUS_accumulators->N_immobilization;   break;
            case  DLY_mineralizated_OM_accum           :
                {                                                                //020328
                   if (residue_pools_kg_m2)                                      //090714
                     // this is a new single pool or multiple pool model.
                      num_value = over_m2_to_ha * CONTINUOUS_accumulators->
                        N_OM_mineralization[0];  // index 0 is profile           //160706
                   else
                   if (NH4)  // this is the older single pool mode..             //020328
                      num_value= over_m2_to_ha * NH4->CONT_bal_accums_M->get_transformed_from(true);
                } break;
            case  DLY_automatic_N_in_uptake_accum      : num_value= over_m2_to_ha * CONTINUOUS_accumulators->N_fert_requirements;    break;
            case  DLY_organic_N_applied_accum          : num_value= over_m2_to_ha * CONTINUOUS_accumulators->organic_N_applied;      break;
            case  DLY_N_Fixation_accum                 : num_value= over_m2_to_ha * CONTINUOUS_accumulators->N_fixation;             break;
#endif
         } break;
      }
   }
}
//_DLY_get_detail__________________________________________________________________/
void CropSyst_YLY_report::get_detail
(nat16    dc
,std::string &//value_str  unused
,int16    &int_value
,float64  &num_value
,detail_types &detail_type
,int8    &width
,int8    &precision_)
{  const CropSyst::Soil_components *soil = simulation.ref_soil();  // for local reference   //060803
   CropSyst::Annual_accumulators *ANNL_accumulators =                            //030912
         simulation.ANNL_accumulators_active; // In this case the simulation ended on a date before end of year
   const Residues_interface *residue_pools_kg_m2 = simulation.organic_matter_residues_profile_kg_m2 ? simulation.organic_matter_residues_profile_kg_m2->ref_residues() : 0;   //060115
   #ifdef NITROGEN
   Soil::NO3_Profile *NO3 = soil && soil->chemicals ? soil->chemicals->NO3 : 0;
   Soil::NH4_Profile *NH4 = soil && soil->chemicals ? soil->chemicals->NH4 : 0;
   #endif
    report_var_layout_with_graph *var_layout = lookup_report_var_layout
      (YLY_report_var_opts,MAX_YLY_entries, report_options.sel_var_code[dc]);    //000926
    width = var_layout ? var_layout->w : 5;                                      //000926
    precision_ = var_layout ? var_layout->d : 0;                                 //000926
    detail_type = detail_REAL;
    int_value = 0.0;                                                             //011127
    num_value = 0.0;                                                             //011031
       switch(report_options.sel_var_code[dc])
      {
         case YLY_year                           : detail_type = detail_INTEGER;
               int_value = (simulation.get_today_date32()/1000);                 //030714
             break;
         case YLY_residue_ground_cover           : num_value= residue_pools_kg_m2 ? residue_pools_kg_m2->get_total_fract_ground_cover(INCLUDE_POSITION_FLAT_SURFACE| INCLUDE_POSITION_STUBBLE_SURFACE)  *100.0 : 0.0;        break;  //041031
         case YLY_soil_loss                      : if (soil && soil->erosion) num_value= soil->erosion->total_soil_loss;break; //020529
         case YLY_ET_max                         : num_value= m_to_mm(ANNL_accumulators->evapotranspiration_max);  break;
         case YLY_ET_act                         : num_value= m_to_mm(ANNL_accumulators->evapotranspiration_act);  break;
         case CSVC_weather_ET_ref_time_step_sum                         : num_value= m_to_mm(ANNL_accumulators->evapotranspiration_ref);  break;  //130723
         case YLY_transp_pot                     : num_value= m_to_mm(ANNL_accumulators->pot_transpiration);       break;

/*200129  continue here!
            transpIration demand pot
*/

         case YLY_transp_act                     : num_value= m_to_mm(CORN_must_be_greater_or_equal_to(ANNL_accumulators->act_transpiration,0.0));break;
         case YLY_evap_soil_pot                  : num_value= m_to_mm(ANNL_accumulators->pot_soil_evaporation);    break;
         case YLY_evap_soil_act                  : num_value= m_to_mm(ANNL_accumulators->act_soil_evaporation);    break;
         case YLY_evap_residue_pot               : num_value= m_to_mm(ANNL_accumulators->pot_residue_evaporation); break;
         case YLY_evap_residue_act               : num_value= m_to_mm(ANNL_accumulators->act_residue_evaporation); break;
         case YLY_irrigation                     : num_value= m_to_mm(ANNL_accumulators->irrigation);              break;
         case YLY_precipitation                  : num_value= m_to_mm(ANNL_accumulators->precip_m);                break;
         case YLY_crop_water_intrcpt             : num_value= m_to_mm(ANNL_accumulators->crop_intercept);          break; // crop_intercept // remove _ today 160419
         case YLY_residue_water_intrcpt          : num_value= m_to_mm(ANNL_accumulators->residue_interception);    break;
         case xYLY_surface_water_runoff          : num_value= m_to_mm(ANNL_accumulators->runoff);                  break;
         case YLY_ponded_water                   : num_value= m_to_mm(simulation.pond.get_depth());                break;   //010202
         case YLY_water_enter_soil               : num_value= m_to_mm(ANNL_accumulators->act_water_entering_soil); break;   //990716
         case YLY_soil_water_drainage            : num_value= m_to_mm(ANNL_accumulators->reported_drainage);       break;   //990106
                // eventually we will have a water balance report which will use balance_drainage
                // we will restore here the reported drainage.
         case YLY_soil_water_balance             : num_value= m_to_mm(ANNL_accumulators->water_balance);           break;
         case YLY_biomass_grazed                 : num_value= per_m2_to_per_ha(ANNL_accumulators->biomass_grazed);break;
         case CSVC_residue_surface_biomass             : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_FLAT_SURFACE|INCLUDE_POSITION_STUBBLE_SURFACE ,INCLUDE_STRAW_RESIDUE_OM|INCLUDE_MANURE_RESIDUE_OM)):0.0; break;  //050116
         case CSVC_residue_plant_surface               : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_FLAT_SURFACE|INCLUDE_POSITION_STUBBLE_SURFACE ,INCLUDE_STRAW_RESIDUE_OM))                          :0.0; break;  //050116
         case CSVC_residue_manure_surface              : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_FLAT_SURFACE|INCLUDE_POSITION_STUBBLE_SURFACE ,INCLUDE_MANURE_RESIDUE_OM))                         :0.0; break;  //050116
         case CSVC_residue_incorporated                : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_SUBSURFACE                                    ,INCLUDE_STRAW_RESIDUE_OM|INCLUDE_MANURE_RESIDUE_OM)):0.0; break;  //050116
         case CSVC_residue_plant_incorporated          : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_SUBSURFACE                                    ,INCLUDE_STRAW_RESIDUE_OM))                          :0.0; break;  //050116
         case CSVC_residue_manure_incorporated	      : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_SUBSURFACE                                    ,INCLUDE_MANURE_RESIDUE_OM))                         :0.0; break;  //050116
     }
#ifdef NITROGEN
      if (NO3 && NO3->ANNL_bal_accums_M && NH4 && NH4->ANNL_bal_accums_M)             //990409
      switch(report_options.sel_var_code[dc])
      {
         case  YLY_nitrogen_applied_total        : num_value= over_m2_to_ha *
               (NO3->molecular_to_elemental*(NO3->ANNL_bal_accums_M->applied)
               +NH4->molecular_to_elemental*(NH4->ANNL_bal_accums_M->applied));break;
         case  YLY_nitrogen_leached_total        : num_value= over_m2_to_ha *
               (NO3->molecular_to_elemental*(NO3->ANNL_bal_accums_M->reported_leached) //981230
               +NH4->molecular_to_elemental*(NH4->ANNL_bal_accums_M->reported_leached));break;
         case  YLY_nitrogen_uptake_total         : num_value= over_m2_to_ha *    //981230
               (NO3->molecular_to_elemental*(NO3->ANNL_bal_accums_M->uptake)
               +NH4->molecular_to_elemental*(NH4->ANNL_bal_accums_M->uptake)); break;
         case  YLY_added_auto_N_in_uptake        : num_value= over_m2_to_ha * ANNL_accumulators->N_fert_requirements;         break;

         case  YLY_mineralized_from_residue      : num_value= 0.0;        break;
         case  YLY_mineralized_from_org_fert     : num_value= 0.0;        break;
         case  YLY_mineralized_OM                : num_value= over_m2_to_ha * ANNL_accumulators->OM_N_mineralization;         break; //000513
         case  YLY_N_immobilization              : num_value= over_m2_to_ha * ANNL_accumulators->N_immobilization;            break; //040209
         case  YLY_total_mineralization          : num_value= over_m2_to_ha *(ANNL_accumulators->OM_N_mineralization);        break; //970814
          // OM_mineralization
         case  YLY_organic_N_applied             : num_value= over_m2_to_ha * ANNL_accumulators->organic_N_applied;           break;
         case  YLY_organic_N_gaseous_loss        : num_value= over_m2_to_ha * ANNL_accumulators->NH3_volatilization_loss;     break; //021212
         case  YLY_inorganic_N_gaseous_loss      : num_value= over_m2_to_ha * ANNL_accumulators->inorganic_NH4_to_atmosphere; break;
         case xYLY_N_fixation                    : num_value= over_m2_to_ha * ANNL_accumulators->N_fixation;                  break;
         case CSVC_soil_C_all_profile :                                          //130918_040127
         {  num_value =                                                          //060202
               simulation.organic_matter_residues_profile_kg_m2 ?                //110718
                 over_m2_to_ha * (simulation.organic_matter_residues_profile_kg_m2->sum_carbon_to_depth_kg_m2(INCLUDE_RESIDUE_OM|INCLUDE_SOM_AND_MICROBIAL,ENTIRE_PROFILE_DEPTH_as_9999,false)) : 0.0;

          } break;
         case YLY_soil_carbon_30cm               :                               //040127
         {     num_value =                                                       //040206
             simulation.organic_matter_residues_profile_kg_m2                    //111018
             ? over_m2_to_ha * (simulation.organic_matter_residues_profile_kg_m2->sum_carbon_to_depth_kg_m2(INCLUDE_RESIDUE_OM|INCLUDE_SOM_AND_MICROBIAL,0.3,false))  //060202
             : 0.0 ;
         } break;
#endif
   }
}
//_YLY:get_detail______________________________________________________________/
void CropSyst_GS_report::get_detail
(nat16    dc
,std::string &value_str
,int16    &int_value
,float64  &num_value
,detail_types &detail_type
,int8    &width
,int8    &precision_)
{
   CropSyst::Crop_model_interface &crop = (*simulation.get_crop());              //200404
      //200404 (*simulation.get_crop_at_end_of_season());
   const CropSyst::Phenology &phenology = crop.ref_phenology();                  //190614
   Crop_mass_fate *biomass_fate_season = crop.get_biomass_fate_season();         //040922
   const CropSyst::Soil_components *soil = simulation.ref_soil(); // for local reference    //060803
   CropSyst::Period_accumulators *GP_accumulators = simulation.                  //030912
            GP_accumulators_active;                                              //051012
   #ifdef NITROGEN
   Soil::NO3_Profile *NO3 = soil && soil->chemicals ? soil->chemicals->NO3 : 0;
   Soil::NH4_Profile *NH4 = soil && soil->chemicals ? soil->chemicals->NH4 : 0;
   #endif
   report_var_layout_with_graph *var_layout = lookup_report_var_layout
      (GS_report_var_opts,GS_MAX_IDS, report_options.sel_var_code[dc]);          //000926
   width = var_layout ? var_layout->w : 5;                                       //000926
   precision_ = var_layout ? var_layout->d : 0;                                  //000926
   detail_type = detail_REAL ;
   const Residues_interface *residue_pools_kg_m2
      = simulation.organic_matter_residues_profile_kg_m2
      ? simulation.organic_matter_residues_profile_kg_m2->ref_residues():0;      //060115
   int_value = 0.0;                                                              //011127
   num_value = 0.0;                                                              //011031
   std::string date_buf;                                                         //020212
   switch(report_options.sel_var_code[dc])
   {  case /*  1*/ GS_crop_name                    : detail_type = detail_STRING; value_str = crop.get_description(); value_str.resize(width); break;
      case /*  2*/ GS_planting_date_YMD            : detail_type = detail_STRING; value_str = CORN::Date_cowl_const_32(phenology.ref_planting_date ()).as_string();       break; //041205
      case /*  3*/ GS_restart_date_YMD             : detail_type = detail_STRING; value_str = CORN::Date_cowl_const_32(phenology.ref_restart_date  ()).as_string();        break; //041205
      case /*  5*/ GS_emergence_date_YMD           : detail_type = detail_STRING; value_str = CORN::Date_cowl_const_32(phenology.ref_emergence_date()).as_string();      break; //030108
      case /*  6*/ GS_flowering_date_YMD           : detail_type = detail_STRING; value_str = CORN::Date_cowl_const_32(phenology.ref_anthesis_date ()).as_string();      break; //030108
      case CSVC_crop_base_tuber_init_date_YMD      : detail_type = detail_STRING; value_str = CORN::Date_cowl_const_32(phenology.ref_tuberization_date ()).as_string();     break; //081111
      case /*  7*/ GS_grain_filling_date_YMD       : detail_type = detail_STRING; value_str = CORN::Date_cowl_const_32(phenology.get_yield_formation_date ()).as_string(); //190710
            break;
      case GS_peak_LAI_date_YMD            : detail_type = detail_STRING; value_str = CORN::Date_cowl_const_32(phenology.ref_culminescence_date()).as_string(); break; //180413 040211
      case GS_maturity_date_YMD            : detail_type = detail_STRING; value_str = CORN::Date_cowl_const_32(phenology.ref_maturity_date     ()).as_string(); ; break; //030108
      case GS_harvest_date_YMD             : detail_type = detail_STRING; value_str = crop.get_harvest_date().as_string();        break; //030108
      case CSVC_crop_base_bud_break_date_YMD                        : detail_type = detail_STRING; value_str = CORN::Date_cowl_const_32(phenology.ref_restart_date()             ).as_string(); break; //190614
      case CSVC_crop_base_initial_fruit_growth_date_YMD             : detail_type = detail_STRING; value_str = CORN::Date_cowl_const_32(phenology.ref_fructescence_date()        ).as_string(); break; //190614
      case CSVC_crop_base_rapid_fruit_growth_date_YMD               : detail_type = detail_STRING; value_str = CORN::Date_cowl_const_32(phenology.ref_rapid_fruit_growth_date()  ).as_string(); break; //190614
      case CSVC_crop_base_inactive_begin_date_YMD                   : detail_type = detail_STRING; value_str = CORN::Date_cowl_const_32(phenology.ref_quiescence_init_date()     ).as_string(); break; //190614
      case CSVC_crop_base_inactive_end_date_YMD                     : detail_type = detail_STRING; value_str = CORN::Date_cowl_const_32(phenology.ref_quiescence_culm_date()     ).as_string(); break; //190614
      case CSVC_crop_base_base_chill_requirement_satisfied_date_YMD : detail_type = detail_STRING; value_str = "NYI" /*crop.get_chill_requirement_satisfied_date().as_string()*/; break; //081013

      case GS_planting_date_YD             : num_value= phenology.get_planting_DOY();       break; //041205
      case GS_restart_date_YD              : num_value= phenology.get_restart_DOY();        break; //041205
      case GS_emergence_date_YD            : num_value= phenology.get_emergence_DOY();      break;
      case GS_flowering_date_YD            : num_value= phenology.get_anthesis_DOY();       break;
      case CSVC_crop_base_tuber_init_date_YD       : num_value= phenology.get_tuberization_DOY();  break; //081111
      case GS_grain_filling_date_YD        :                                     //020809
         num_value = phenology.get_yield_formation_date()/ 1000.0;               //190614
         break;
      case /* 15*/ GS_peak_LAI_date_YD             : num_value= phenology.get_culminescence_DOY(); break; //040211
      case /* 16*/ GS_maturity_date_YD             : num_value= phenology.get_maturity_DOY();      break;
      case /* 17*/ GS_harvest_date_YD              : num_value= crop.get_harvest_date()   .get_date32() / 1000.0; break;
      case CSVC_crop_base_bud_break_date_YD                   : num_value= phenology.get_emergence_DOY();         break; //081013
      case CSVC_crop_base_initial_fruit_growth_date_YD        : num_value= phenology.get_fructescence_DOY();      break; //081013
      case CSVC_crop_base_rapid_fruit_growth_date_YD          : num_value= phenology.get_rapid_fruit_growth_DOY();break; //081013
      case CSVC_crop_base_inactive_begin_date_YD              : num_value= phenology.get_quiescence_init_DOY();   break; //081013
      case CSVC_crop_base_inactive_end_date_YD                : num_value= phenology.get_quiescence_culm_DOY();   break; //081013
      case CSVC_crop_base_chill_requirement_satisfied_date_YD : num_value= 0; /*190617 need to reimplement crop.get_chill_requirement_satisfied_date() / 1000.0;*/ break; //081013

      case  GS_peak_LAI                            : num_value= crop.get_peak_LAI(true);                       break;// for last day of year         //040920
      case  GS_yield                               : num_value= over_m2_to_ha * (biomass_fate_season
                                                         ? biomass_fate_season->yield
                                                         : crop.get_latest_yield_kg_m2());  break; //040728
      case  GS_used_biomass                        : num_value= biomass_fate_season ? (over_m2_to_ha * biomass_fate_season->useful) : 0.0;    break; //040728
      case  GS_grazed_biomass                      : num_value= biomass_fate_season ? (over_m2_to_ha * biomass_fate_season->get_removed_for_grazing()) : 0.0; break; //040728
      case  GS_residue_biomass                     : num_value= biomass_fate_season ? (over_m2_to_ha * (biomass_fate_season->chaff_or_leaf_litter + biomass_fate_season->stubble_dead)) : 0.0; break; //040728
      case  GS_disposed_biomass                    : num_value= biomass_fate_season ? (over_m2_to_ha * biomass_fate_season->disposal) : 0.0;  break; //040728
      case  CSVC_crop_biomass_produced_above_ground_period_sum  : num_value= over_m2_to_ha * crop.ref_reported().season.biomass_production; break; //200609_190705
      //200609 case  CSVC_crop_biomass_produced_above_ground_period_sum  : num_value= biomass_fate_season ? (over_m2_to_ha * crop.ref_reported().season.biomass_production) : 0.0; break; //190705
      case  CSVC_crop_base_fruit_biomass           : num_value= over_m2_to_ha * crop.get_latest_yield_kg_m2();   break;                                   //081014
      //200608 case  CSVC_crop_base_fruit_biomass           : num_value= biomass_fate_season ? (over_m2_to_ha * biomass_fate_season->yield) : 0.0;  /*assumuing the yield is the fruit biomass */   break;                                   //081014
      case  CSVC_crop_base_fruit_canopy_biomass    : num_value = crop.get_fruit_reserves_biomass_kg_ha() + crop.get_canopy_biomass_kg_ha() ;  break; //081014
      case CSVC_crop_base_fruit_reserves_biomass   : num_value = crop.get_fruit_reserves_biomass_kg_ha(); break; //200618
#ifdef NA
applies only to daily
      case CSVC_crop_base_orchard_solid_canopy_interception  :  num_value=crop.get_orchard_solid_canopy_interception(); break; //081020
      case CSVC_crop_base_orchard_shaded_area_GAI            :  num_value=crop.get_orchard_shaded_area_GAI();           break; //081020
      case CSVC_crop_base_orchard_canopy_porosity            :  num_value=crop.get_orchard_canopy_porosity ();          break; //081020
#endif
      case  GS_daytime_VPD_avg                     : num_value = crop.get_VPD_daytime_mean();                            break;                                                                                                                                                                 //030530
      case  GS_intercepted_PAR_accum               : num_value = crop.get_intercepted_PAR_season_accum_MJ_m2();          break;                                                                                                                                                                 //030602
               // This is now PAR or total depending on the mode (PAR) is becoming deprecated
      case  GS_root_depth                          :                                                                                                                                                                                                                                            //060811
            { num_value= crop.get_recorded_root_depth_m() + (soil ? soil->ref_layers()->get_thickness_m(1): 0);   //150424
            } break;
      case CSVC_crop_base_root_biomass_current     : num_value= crop.get_act_root_biomass_kg_ha()                      ;break; // 070207  GS_root_biomass                                                                                                                                       //060613
      case CSVC_residue_surface_biomass            : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_FLAT_SURFACE|INCLUDE_POSITION_STUBBLE_SURFACE,INCLUDE_STRAW_RESIDUE_OM|INCLUDE_MANURE_RESIDUE_OM)): 0.0 ;  break;               //050116
      case CSVC_residue_plant_surface              : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_FLAT_SURFACE|INCLUDE_POSITION_STUBBLE_SURFACE,INCLUDE_STRAW_RESIDUE_OM))                          : 0.0 ;  break;               //050116
      case CSVC_residue_manure_surface             : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_FLAT_SURFACE|INCLUDE_POSITION_STUBBLE_SURFACE,INCLUDE_MANURE_RESIDUE_OM))                         : 0.0 ;  break;               //050116
      case CSVC_residue_incorporated               : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_SUBSURFACE                                   ,INCLUDE_STRAW_RESIDUE_OM|INCLUDE_MANURE_RESIDUE_OM)): 0.0 ;  break;               //050116
      case CSVC_residue_plant_incorporated         : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_SUBSURFACE                                   ,INCLUDE_STRAW_RESIDUE_OM))                          : 0.0 ;  break;               //050116
      case CSVC_residue_manure_incorporated	      : num_value= residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_SUBSURFACE                                   ,INCLUDE_MANURE_RESIDUE_OM))                         : 0.0 ;  break;               //050116
      case  GS_erosion_index  // 091205 Warning this is the wrong variable code, this is the cover factor, not the erosion index                                                                                                                                                                //050505
                                          // I need to create a new variable for this
            : if (soil && soil->erosion) num_value= soil->erosion->C_factor_tally.get_sum() * 10.0 ;   break; // RLN not sure why mulitply by 10.0
      case  GS_soil_loss                           : if (soil && soil->erosion) num_value= soil->erosion->total_soil_loss;  break;
      case  GS_ET_max                              : num_value= m_to_mm(GP_accumulators->evapotranspiration_max);       break;
      case  GS_ET_act                              : num_value= m_to_mm(GP_accumulators->evapotranspiration_act);       break;
      case  GS_ET_ref                              : num_value= m_to_mm(GP_accumulators->evapotranspiration_ref);       break;   //130723
      case  GS_transp_pot                          : num_value= m_to_mm(GP_accumulators->pot_transpiration);            break;

/*200129  continue here!
               now transpration demand pot
*/

      case  GS_transp_act                          : num_value= m_to_mm(CORN_must_be_greater_or_equal_to(GP_accumulators->act_transpiration,0));break;
      case  GS_evap_soil_pot                       : num_value= m_to_mm(GP_accumulators->pot_soil_evaporation);         break;
      case  GS_evap_soil_act                       : num_value= m_to_mm(GP_accumulators->act_soil_evaporation);         break;
      case  GS_evap_residue_pot                    : num_value= m_to_mm(GP_accumulators->pot_residue_evaporation);      break;
      case  GS_evap_residue_act                    : num_value= m_to_mm(GP_accumulators->act_residue_evaporation);      break;
      case  GS_irrig                               : num_value= m_to_mm(GP_accumulators->irrigation);                   break;
      case  GS_precip                              : num_value= m_to_mm(GP_accumulators->precip_m);                     break;
      case  GS_crop_water_intrcpt                  : num_value= m_to_mm(GP_accumulators->crop_intercept);               break; // crop_intercept  remove _today 160419
      case  GS_residue_water_intrcpt               : num_value= m_to_mm(GP_accumulators->residue_interception);         break;
      case  GS_surface_water_runoff                : num_value= m_to_mm(GP_accumulators->runoff);                       break;
      case  GS_ponded_water                        : num_value= m_to_mm(simulation.pond.get_depth());                   break;               //010202
      case  GS_water_enter_soil                    : num_value= m_to_mm(GP_accumulators->act_water_entering_soil);      break;               //990716
      case  GS_soil_water_drainage                 : num_value= m_to_mm(GP_accumulators->reported_drainage);            break;               //990106
               // eventually we will have a water balance report which will use balance_drainage
               // we will restore here the reported drainage.
      // I think the following are actually indexes!!!
      case  GS_avg_water_stress_index              : num_value= crop.get_water_stress_index_mean();               break;                     //010724
      case  GS_avg_temperature_stress_index        : num_value= crop.get_temperature_stress_index_mean();         break;                     //011023
      #ifdef OLD_YIELD
      case CSVC_crop_base_flower_temperature_stress_index_period_avg  : num_value= crop.get_flower_temperature_stress_index_mean(); break;   //081125
      case CSVC_crop_base_tuber_init_temp_stress_index   : num_value= crop.get_tuber_init_temperature_stress_index_mean();  break;           //081114
      case CSVC_crop_base_tuber_fill_temp_stress_index   : num_value= crop.get_tuber_fill_temperature_stress_index_mean();  break;           //081114
      #else
      //150825 eventually reimplement this with listener idiom
      #endif
   }
#ifdef NITROGEN
      if (NO3 && NO3->GS_bal_accums_M && NH4 && NH4->GS_bal_accums_M)            //011117
      switch(report_options.sel_var_code[dc])
      {  case  GS_nitrogen_leached_total              : num_value= (NO3 && NH4) ?(over_m2_to_ha * ((NO3->GS_bal_accums_M->get_reported_leached(true))  //981230
                          +(NH4->GS_bal_accums_M->get_reported_leached(true)))): 0.0;   break;                                                        //981230
         case  GS_nitrogen_applied_total              : num_value= (NO3 && NH4) ?(over_m2_to_ha * ((NO3->GS_bal_accums_M->get_applied(true))
                           +(NH4->GS_bal_accums_M->get_applied(true)) )) : 0.0;  break;
         case  CSVC_crop_N_automatic_uptake_period_sum: num_value= over_m2_to_ha * GP_accumulators->N_fert_requirements;    break;
         case  CSVC_soil_N_available_period_sum       : num_value= over_m2_to_ha *
             (NO3->molecular_to_elemental*(NO3->GS_bal_accums_M->original_profile_content)
             +NH4->molecular_to_elemental*(NH4->GS_bal_accums_M->original_profile_content)
             +NH4->molecular_to_elemental*(NH4->GS_bal_accums_M->transformed_from)
             +NO3->molecular_to_elemental*(NO3->GS_bal_accums_M->applied)
             +NH4->molecular_to_elemental*(NH4->GS_bal_accums_M->applied)
             +GP_accumulators->N_fert_requirements);    break;
         case  CSVC_residue_mineralized_period_sum          : num_value= 0.0;      break;
         case  CSVC_residue_manure_mineralized_period_sum   : num_value= 0.0;     break;
         case  CSVC_organic_matter_mineralized_period_sum   : num_value= over_m2_to_ha * GP_accumulators->OM_N_mineralization;         break; //970814
         case  CSVC_soil_N_immobilization_period_sum        : num_value= over_m2_to_ha * GP_accumulators->N_immobilization;            break; //040209
         case  CSVC_soil_N_mineralization_period_sum        : num_value= over_m2_to_ha *                                                      //970814
            ( GP_accumulators->OM_N_mineralization); break; // OM_mineralization
      case  CSVC_mgmt_N_organic_applied_period_sum          : num_value= over_m2_to_ha * GP_accumulators->organic_N_applied;           break;
      case  CSVC_mgmt_N_organic_gaseous_loss_period_sum     : num_value= over_m2_to_ha * GP_accumulators->NH3_volatilization_loss;     break; //021212
      case  CSVC_mgmt_N_inorganic_gaseous_loss_period_sum   : num_value= over_m2_to_ha * GP_accumulators->inorganic_NH4_to_atmosphere; break;
      case  CSVC_crop_N_fixation_period_sum                 : num_value= over_m2_to_ha * GP_accumulators->N_fixation;                  break;
   }
   const CropSyst::Crop_nitrogen_interface *crop_N_kg_m2 = crop.get_nitrogen();  //050722
   if (crop_N_kg_m2)
      switch(report_options.sel_var_code[dc])
      {  case CSVC_crop_N_stress_index_period_avg        : num_value= crop_N_kg_m2->get_stress_index_mean();               break; //070209_010726
         case CSVC_crop_N_uptake_period_sum              : num_value = over_m2_to_ha * crop_N_kg_m2->get_season_uptake();  break; //070209
         case CSVC_crop_N_mass_canopy_production_accum_to_date_time : num_value= over_m2_to_ha *crop_N_kg_m2->get_mass_canopy_production();   break; //131007_110915
         case CSVC_crop_N_mass_canopy_current : num_value= over_m2_to_ha *crop_N_kg_m2->get_mass_canopy_current();   break; //131007
      }
#endif
}
//_GS::get_detail___________________________________________________2003-05-16_/
#endif

