#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>
#include <math.h> //keyvan
#include <algorithm>
#include <irrigation_lib.h>

//190202RLN I removed most of these includes, the don't appear to be needed
// anymore using forward declarations

//170413LML #ifdef VIC_CROPSYST_VERSION
/*170413LML moved to VCS_Nl.h
#  include <iostream>
#  include "crop/VIC_soil_hydrology.h"
#  if (VIC_CROPSYST_VERSION < 3)
#     include "crop/growth_stages.h" //keyvan
#     include "crop/VIC_crop_C_interface.h"
#  else
#     include "agronomic/VIC_land_unit_C_interface.h"
#     include "agronomic/VIC_land_unit_simulation.h"
#     include "agronomic/VIC_soil.h"
#  endif
*/
//170413LML #else
//170413LML #  include <vicNl_def.h>
//170413LML #endif

#ifdef CHECK_WATER_BALANCE
#include "agronomic/balanceitem.h"                                               //160506LML
#endif
#if (VIC_CROPSYST_VERSION>=3)
#include "VCS_Nl.h"
#include <assert.h>
// Forward declarations                                                          //190202RLN
extern double cal_soil_evap_from_total_evap(double ET_reference_crop);           //190202RLN
extern soil_con_struct *soil_con_global;
#endif
//extern double evaporation_from_irrigation_systems(double ET0);
//extern double test(double ET_reference_crop);
extern double VIC_CropSyst_get_return;
static char vcid[] = "$Id: canopy_evap.c,v 5.3.2.4 2012/02/05 00:15:44 vicadmin Exp $";

/**********************************************************************
    canopy_evap.c    Dag Lohmann        September 1995

  This routine computes the evaporation, transpiration and throughfall
  of the vegetation types for multi-layered model.

  The value of x, the fraction of precipitation that exceeds the
  canopy storage capacity, is returned by the subroutine.

  UNITS:    moist (mm)
        evap (mm)
        prec (mm)
        melt (mm)

  VARIABLE TYPE        NAME          UNITS DESCRIPTION
  atmos_data_struct    atmos         N/A   atmospheric forcing data structure
  layer_data_struct   *layer         N/A   soil layer variable structure
  veg_var_struct      *veg_var       N/A   vegetation variable structure
  char                 CALC_EVAP     N/A   TRUE = calculate evapotranspiration
  int                  veg_class     N/A   vegetation class index number
  int                  month         N/A   current month
  global_param_struct  global        N/A   global parameter structure
  double               mu            fract wet (or dry) fraction of grid cell
  double               ra            s/m   aerodynamic resistance
  double               prec          mm    precipitation
  double               displacement  m     displacement height of surface cover
  double               roughness     m     roughness height of surface cover
  double               ref_height    m     measurement reference height

  Modifications:
  9/1/97    Greg O'Donnell
  4-12-98  Code cleaned and final version prepared, KAC
  06-25-98 modified for new distributed precipitation data structure KAC
  01-19-00 modified to function with new simplified soil moisture
           scheme                                                  KAC
  5-8-2001 Modified to close the canopy energy balance.       KAC

**********************************************************************/
//extern double **accessable_aero_resistance; ///121129 keyvan
double canopy_evap(const soil_con_struct   &soil_con,
                   const char               CALC_EVAP,
                   const int                veg_class,
                   const int                month,
                   const double             mu_orig,
                   const double             delta_t,      /*seconds*/
                   const double             rad,
                   const double             vpd,
                   const double             net_short,
                   const double             air_temp,
                   const double             ra,
                   const double             elevation,
                   const double            *prec,
#if SPATIAL_FROST
                   const double            *frost_fract,
#endif
                   const float             *root
#ifdef VIC_CROPSYST_VERSION
                   #if (VIC_CROPSYST_VERSION>=3)
                   ,const int               iveg
                   ,const int               band                                 //151001LML
                   ,/*190918LML const*/ veg_con_struct   *veg_con
                   ,const double            gross_shortwave                      //(W/m^2) 150624LML
                   ,const double            net_longwave                         //(W/m^2) 150624LML
                   ,const int               hidx                                 //170410LML
                   #endif
                   ,const int               rec
                   ,const atmos_data_struct *atmos
                   ,const dmy_struct       *dmy
                   ,const int               veg_class_code
                   ,const double          * const *aero_resist_ET
                   ,cell_data_struct       *cell_wet
                   ,cell_data_struct       *cell_dry
#endif
                   ,double                 *Wdew
                   ,layer_data_struct      *layer_wet
                   ,layer_data_struct      *layer_dry
                   ,veg_var_struct         *veg_var_wet
                   ,veg_var_struct         *veg_var_dry
           )
/*********************************************************************
  CANOPY EVAPORATION

  Calculation of evaporation from the canopy, including the
  possibility of potential evaporation exhausting ppt+canopy storage
  2.16 + 2.17
  Index [0] refers to current time step, index [1] to next one
  If f < 1.0 then veg_var->canopyevap = veg_var->Wdew + ppt
  and Wdew = 0.0

  DEFINITIONS:
  Wdmax - max monthly dew holding capacity
  Wdew - dew trapped on vegetation

  Modified
     04-14-98 to work within calc_surf_energy_balance.c  KAC
     07-24-98 fixed problem that caused hourly precipitation
              to evaporate from the canopy during the same
              time step that it falls (OK for daily time step,
              but causes oscilations in surface temperature
              for hourly time step)                      KAC, Dag
            modifications:
     6-8-2000 Modified to use spatially distributed soil frost if
              present.                                           KAC
     5-8-2001 Modified to close the canopy energy balance.       KAC
  2009-Jun-09 Moved computation of canopy resistance rc out of penman()
              and into separate function calc_rc().            TJB
  2012-Jan-16 Removed LINK_DEBUG code                    BN

**********************************************************************/

{
  extern option_struct options;
  double Evap = 0;                                                               /*(m/s)*/
  double tmp_Evap;
  double layerevap[MAX_LAYERS];
  double rc;
  int    Ndist = options.NDIST;
  #ifdef VIC_CROPSYST_VERSION
  double irrig_efficiency = 0;
  veg_lib_struct &veglib_specific = *veg_con[iveg].VCS.veg_lib[band];
  #else
  veg_lib_struct &veglib_specific = veg_lib[veg_class];
  #endif
  bool is_crop = false;
  double mu = mu_orig;
  #ifdef VIC_CROPSYST_VERSION
  int rotation_code = veg_con[iveg].VCS.veg_class_code;
  is_crop = iscrop
          #if (VIC_CROPSYST_VERSION>=3)
          (rotation_code);
          #else
          (veg_class_code); //keyvan 1207/2012
          #endif
  //printf("canopy_evap::VEG(%d)\tis_crop(%d)\n",veg_con[iveg].veg_class_code,is_crop);
  if (is_crop && mu < 1.0) assert(false);                                        //LML 150501 mu = 1 for crops
  //150413RLN  These declaration should all have comment indicating the units
  #if VIC_CROPSYST_VERSION>=3
  double pot_et[N_PET_TYPES];                                                    //(mm/delta_t) 150624LML = cell_wet->pot_evap;                                           //150616LML
  int rotation_or_crop_veg_class_code = -1;                                      //161212LML
  int rotation_cycle_index = -1;                                                 //161212LML
  DecomposeVegclassCode(veg_class_code,rotation_or_crop_veg_class_code,
                        rotation_cycle_index);                                   //161212LML
  #endif
  #endif
  /*************************************
  calculate the ET potential
  and Evaporation from irrigation system
  ***************************************/
  for (int dist = 0; dist < Ndist; dist++)
  {  // Initialize variables
     for (int i = 0; i < options.Nlayer; i++) layerevap[i] = 0;
     double canopyevap  = 0;                                                     //(mm/delta_t)
     double throughfall = 0;                                                     //(mm/delta_t)
     // Set parameters for distributed precipitation */
     layer_data_struct *tmp_layer;
     veg_var_struct    *tmp_veg_var;
     cell_data_struct  *tmp_cell_data;
     double             tmp_Wdew;
     double             ppt;                                                     // effective precipitation
     if (dist == 0) {
       tmp_layer   = layer_wet;
       tmp_veg_var = veg_var_wet;
       ppt         = prec[WET];
       tmp_Wdew    = Wdew[WET];
       #ifdef VIC_CROPSYST_VERSION
       tmp_cell_data = cell_wet;                                                 //150930LML
       #endif
     } else {
       tmp_layer   = layer_dry;
       tmp_veg_var = veg_var_dry;
       ppt         = prec[DRY];
       mu          = (1. - mu);
       tmp_Wdew    = Wdew[DRY];
       #ifdef VIC_CROPSYST_VERSION
       tmp_cell_data = cell_dry;                                                 //150930LML
       #endif
     }
     #if VIC_CROPSYST_VERSION>=3
     crop_data_struct *current_crop = is_crop ? tmp_veg_var->crop_state : 0;     //150929LML 160506
     #endif
     if (mu > 0) {
        #ifdef CHECK_WATER_BALANCE
        BalanceItem canopy_water;
        BalanceItem irrigation_water;
        canopy_water.SetInitPool(tmp_Wdew);
        #endif
        //****************************************************
        //Compute Evaporation from Canopy Intercepted Water
        //****************************************************
        //** Due to month changes ..... Wdmax based on LAI **
        tmp_veg_var->Wdew = tmp_Wdew;
        double Wdew_max = veglib_specific.Wdmax[month-1];
        if (tmp_Wdew > Wdew_max) {
           throughfall = tmp_Wdew - Wdew_max;
           tmp_Wdew    = Wdew_max;
        }
        //**************************************
        //applying the irrigation water 21/12/2012
        //**************************************
        #ifdef VIC_CROPSYST_VERSION
        double crop_potential_ET = 0;
        double crop_potential_ET_reminning = 0;
        double maximum_allowable_depletion = 1;
        double depletion_observe_depth_mm = 500;                                 //180502LML
        int irrigation_index = -1;                                               //160509LML array index for irrigation parameters initialized in "irrigation_lib.c"
        bool require_irrigation = false;                                         //160509LML true if soil moisture lower than MAD in irrigation season
        bool surface_or_drip = false;                                            //true: surface or drip; false: sprinkler etc.
        double total_irrig = 0;                                                  //Total irrigated water
        double evap_from_irrig_sys = 0;                                          //(mm/delta_t) evap from irrigation system
        double potevap_irrig_canopy = 0;                                         //potential evap from irrigated canopy
        double runoff_from_irrig_sys = 0;
        double deep_percolation_from_irrig_sys = 0;                              //180531LML
        double irrig_aft_evap = 0;                                               //irrigation after evap
        double irrig_over_soil = 0;                                              //irrigated water reached soil surface after evaporation and canopy interception
        double irrig_intcp_canopy_init = 0;                                      //160104LML intercepted irrigated water for canopy initial deficit
        double irrig_for_pot_infiltration = 0;                                   //irrigated water after losses from evaporation, canopy interception (for initial deficit), and runoff
                                                                                 //Some of it might be intercepted by canopy
        double canopy_maxdew_deficit = 0;                                       //irrigated water used to fit the deficit
        double canopy_evap_deficit = 0;                                          //intercepted irrigation for canopy evap
        Irrigation_Type irrigation_mode     = NO_IRRIGATION;
        General_Irrigation_Type girrig_type = GENERAL_NO_IRRIGATION;
        if (is_crop) {
            double out_soil_water_demand_mm = 0;                                 //160115LML
            bool isPerennial = VIC_land_unit_is_current_crop_fruit_tree();
            double water_stress_index = VIC_land_unit_get(VIC::Water_Stress_Index);
           //**first calculate the amount of potential ET**
           #if VIC_CROPSYST_VERSION==2
           int short_grass_number  = 109;
           int open_water_number   = 108;
           //LML 150501 #endif
           rs                      = veg_lib[short_grass_number].rmin;          //short grass
           double rs_open_w        = veg_lib[open_water_number].rmin;           //open water calculations 130426 keyvan
           rarc                    = veg_lib[short_grass_number].rarc;
           double rarc_open_w      = veg_lib[open_water_number].rarc;           //open water calculations 130426 keyvan
           double RGL              = veg_lib[short_grass_number].RGL;
           double RGL_open_w       = veg_lib[open_water_number].RGL;            //open water calculations 130426 keyvan
           double lai              = veg_lib[short_grass_number].LAI[dmy->month-1];
           double lai_open_w       = veg_lib[open_water_number].LAI[dmy->month-1]; //open water calculations 130426 keyvan
           double gsm_inv          = 1.0;
           char ref_crop           = ref_veg_ref_crop[PET_SHORT/*LML 150414 short_grass_number*/];
           char ref_crop_open_w    = ref_veg_ref_crop[PET_H2OSURF/*LML 150414 open_water_number*/];       //open water calculations 130426 keyvan
           rc                      = calc_rc(rs, net_short/*LML 150430 rad*/, RGL, air_temp, vpd, lai, gsm_inv, ref_crop);  //LML 150415 why use rad rather than netshort?
           rc_open_w               = 0.0; //LML 150430 calc_rc(rs, rad, RGL, air_temp, vpd, lai, gsm_inv, ref_crop_open_w);  //open water calculations 130426 keyvan
           //ra = accessable_aero_resistance[102][0];
           ra                      = aero_resist_ET[PET_SHORT/*LML 150414 2*/][0];
           ra_open_w               = aero_resist_ET[PET_H2OSURF/*LML 150414 1*/][0];                                   //open water calculations 130426 keyvan
           ET0_short               = penman(air_temp, elevation, rad, vpd, ra, rc, rarc)
                                     * delta_t / SEC_PER_DAY;                   //(mm/delta_t)
           double ET0_open_water   = penman(air_temp,elevation,rad, vpd, ra_open_w, rc_open_w, rarc_open_w)
                                     * delta_t / SEC_PER_DAY;                   //(mm/delta_t) 130426 keyvan
           ET_pot                          = VIC_calc_ET_pot(ET0_short);                                  //keyvan dec102012 -rename it
           double ET0_open_water_actual    = VIC_calc_ET_pot(ET0_open_water);   //has to be checked Keyvan
           MAD                             = return_MAD(veg_class_code);
           #else
           compute_pot_evap(veg_class, dmy, rec, delta_t / seconds_per_hour,
                            gross_shortwave, net_longwave, air_temp, vpd,
                            elevation, aero_resist_ET, pot_et
                            );                                                   //LML 150501 potential problems in radiation because of various albedo
           double et_coef_full_shading = VIC_land_unit_get(
                                                     VIC::ET_COEF_FULL_SHADING);
           crop_potential_ET = pot_et[PET_SHORT] * et_coef_full_shading;         //crop potential ET
           crop_potential_ET_reminning = pot_et[PET_SHORT];                      //180620LML crop_potential_ET;
           #ifdef MECHANISTIC_IRRIGATION
           double ET_potential = pot_et[PET_SHORT];
           double ET0_open_water_actual = pot_et[PET_H2OSURF];
           double crop_height           = VIC_land_unit_get(VIC::CANOPY_HEIGHT); //LML 150414
           #endif
           #ifdef USE_CROPSYST_CROP_IRRIGATION
           total_irrig = current_crop->total_daily_irrigation_from_CropSyst_mm
                                            * delta_t / (double)SECONDS_PER_DAY; //150718LML need talk with Roger!!!
           isInCropIrrigation         = total_irrig > 1e-6 ? 1 : 0;
           is_irrigation              = isInCropIrrigation;
           if (is_irrigation)
             irrigation_mode = VIC_land_unit_get_irrigation_type();              //150718LML need talk with Roger!!!
           else irrigation_mode = NO_IRRIGATION;
           #else
           const CropSyst::Irrigation_operation_struct *irrigation_param         //190128RLN
               = VIC_land_unit_get_irrigation_parameters();                      //150413RLN
           double CropSyst_target_irrigation_m = 0.0;                            //201028LML irrigation need from manual irrigation events
           CropSyst_target_irrigation_m = VIC_land_unit_get_irrigation_target();
           bool isInCropIrrigationSeason = irrigation_param == 0 ? false : true; //150413RLN
           if (CropSyst_target_irrigation_m > 1.e-6) isInCropIrrigationSeason = true; //201028LML
           if ((dmy->month < 3) || (dmy->month > 10)) isInCropIrrigationSeason = false; //180807LML hard codded!!!
           double table_irrigation_runoff_loss = 0.0;                            //180511LML
           double table_deep_percolation_loss = 0;                               //180531LML
           if (isInCropIrrigationSeason && options.VCS.do_irrigate_crop) {
              if (irrigation_param) {
                //201111LML&COS MAD depends on irrigation technology
                //maximum_allowable_depletion =
                //  irrigation_param->max_allowable_depletion;                       //150413RLN
                switch (irrigation_param->depletion_observation_depth_mode) {
                case constant_fixed_depth_mode:
                    depletion_observe_depth_mm = m_to_mm(irrigation_param->depletion_observe_depth_m);            //180502LML
                    break;
                case fraction_of_root_depth_mode:
                    depletion_observe_depth_mm = VIC_CropSyst_get(VIC::VC_root_depth_mm) * irrigation_param->depletion_observe_root_zone_fract;  //190919LML added fraction part, seems not right and should be fixed!!!
                    break;
                case soil_profile_depth_mode:
                    depletion_observe_depth_mm = get_total_soil_thickness_mm(&soil_con);
                    break;
                default:
                    depletion_observe_depth_mm = m_to_mm(irrigation_param->depletion_observe_depth_m);
                    break;
                }
                depletion_observe_depth_mm = depletion_observe_depth_mm < 0.3
                                                ? 0.3 : depletion_observe_depth_mm;
              } //irrigation_param
              //depletion_observe_depth_mm =
              //  m_to_mm(irrigation_param->depletion_observe_depth_m);            //180502LML
              #ifdef USE_IRRIGATION_PARAM_FILE
              bool full_irrigation = true;
              bool find_irrig_type = false;
              //180730LML bool find_irrig_type = find_irrigation_type(
              //180730LML   soil_con.VCS.irrigation_type_list,current_crop->code,
              //180730LML   irrigation_mode,full_irrigation);
              if (!find_irrig_type) find_irrig_type = find_irrigation_type(
                soil_con.VCS.irrigation_type_list,
                rotation_or_crop_veg_class_code,irrigation_mode,
                full_irrigation);                                                //161212LML If the irrigation parameter doesn't define the irrigation type for this crop type, try to find the defination for the rotation type.
              if (!find_irrig_type) {
#ifdef IRRIG_PARAMETERFILE_DECIDE_IRRIGATION
                  //180605LML if not defined irrigation in irrigation parameter file, then set NO irrigation over this type.
                  irrigation_mode = NO_IRRIGATION;
                  isInCropIrrigationSeason = false;
#else
#ifdef LIU_DEBUG
                  std::clog << "Using default irrigation: DEFAULT_CENTER_PIVOT!\n";
#endif
                  irrigation_mode = DEFAULT_CENTER_PIVOT;
#endif
              }
              girrig_type = identify_general_irrigation_type(irrigation_mode);
              #else
              if(irrigation_param) {
                irrigation_mode = static_cast<Irrigation_Type>(
                    irrigation_param->irrigation_type);
              }
              #endif
           }
           #endif
           #endif
           //*******************************************************
           //calculation of Irrigation water and evaporation from
           //irrigation systems                      Keyvan 130307
           //********************************************************
           //160506LML Outputs for crops
           current_crop->soil_moist = get_total_soil_moisture(tmp_layer);
           current_crop->crop_coef =
              #if (VIC_CROPSYST_VERSION >= 3)
              et_coef_full_shading;
              #else
              crop_coefficient();
              #endif
           //********************************************************
           //chck if today is an irrigation day
           //********************************************************
           #if (VIC_CROPSYST_VERSION == 2)
           //150413RLN CropSyst land unit will identify when (automatic) irrigation period is scheduled
           isInCropIrrigation = ((current_crop->CropSystHandle != 0)
              && (current_crop->TypeCropIrrigated > 0
                  && dmy->day_in_year > 60                                       //do not irrigate before March 1st.  This might change by a day in the leap year. 07292011 KJC
                  && dmy->day_in_year <= 304                                     //do not irigate after Septempber. This might change by a day in the leap year. 07292011 KJC
                  && current_crop->growth_stage < NGS_DORMANT_or_INACTIVE));     //Do not irrigate when growth stage is less than the dormancy.
           #endif
           if (isInCropIrrigationSeason && options.VCS.do_irrigate_crop && (delta_t == SEC_PER_DAY)) {      //07142022LML the irrigation only happens at daily step
             irrigation_index = get_irrig_lib_index(irrigation_mode);
             if (irrigation_index < 0) {
                std::cerr << "\n\nINPUT ERROR: irrigation_mode:"
                          << irrigation_mode
                          << " is not in the hard coded irrigation parameter library!\n";
                exit(0);
             }
             const Irrigation_Efficiency &irreff =
                     Irrigation_library[irrigation_index].irrigation_efficiency;
             table_irrigation_runoff_loss = irreff.table_irrigation_runoff_loss; //180511LML
             table_deep_percolation_loss  = irreff.table_irrigation_deep_percolation_loss; //180531LML
             irrig_efficiency = irreff.table_irrigation_efficiency;
             double irrigation_capacity_mm = irreff.maximum_capacity * delta_t
                                             / SEC_PER_DAY;                      //(mm/T) 170303LML corrected the unit 160104LML
             double soil_water_demand_mm = 0;                                    //160115LML
             #ifndef USE_CROPSYST_CROP_IRRIGATION
             bool fill_soil_deficit = false;
             double real_filled_water = 0;
             if (irrigation_mode == IrrigTP_Sub_surf_drip_perfect || irrigation_mode == IrrigTP_Sub_surf_drip_perfect_eliminate_top) {
                 fill_soil_deficit = true;  //190724LML directly fill soil moisture to field capacity (exclude top soil layer)
                 maximum_allowable_depletion = MAD_FOR_NETIRRIGATION_DEMAND; //190807LML 0.3;                             //190725LML&COS
             }
             //201111LML&Claudio the MAD depends on irrigation technology
             maximum_allowable_depletion = irreff.max_allowable_depletion;


             //if (veg_con[iveg].VCS.this_year_start_irrigated == false && (veg_con[iveg].VCS.veg_class_code == 7701 || veg_con[iveg].VCS.veg_class_code == 8205))
#ifdef POSTPONE_FIRST_IRRIGATION_PERENNIAL_CROPS
             if (veg_con[iveg].VCS.this_year_start_irrigated == false &&
                (veg_con[iveg].VCS.veg_class_code == 7701 || veg_con[iveg].VCS.veg_class_code == 8205 || veg_con[iveg].VCS.veg_class_code == 8704)) //veg_con[iveg].VCS.veg_class_code < PERENNUIAL_CROP_END))
                 maximum_allowable_depletion = 0.5;                          //190918LML postpone the first irrigation event for perennual alfalfa & pasture
#endif

             //std::clog << "maximum_allowable_depletion:" << maximum_allowable_depletion << std::endl;
             double top_layer_to_fc_fraction = (irrigation_mode == IrrigTP_Sub_surf_drip_perfect_eliminate_top) ? 0.2 : 1.0;   //190805LML

             //std::clog << "maximum_allowable_depletion = " << maximum_allowable_depletion << std::endl;
             if (irrigation_param) {
             require_irrigation = need_irrigation(maximum_allowable_depletion
                                                 ,depletion_observe_depth_mm
                                                 ,rec
                                                 ,irrigation_capacity_mm
                                                 ,tmp_layer                      //150701LML
                                                 ,soil_con                       //150701LML
                                                 ,soil_water_demand_mm
#ifndef FULL_IRRIGATION
                                                 ,rotation_code
#endif
                                                 ,fill_soil_deficit
                                                 ,irrigation_capacity_mm
                                                 ,&real_filled_water
                                                 ,top_layer_to_fc_fraction
                                                 ,water_stress_index             //06282022LML
                                                 ,WATER_STRESS_TOLERANCE_FOR_IRRIGATION   //06282022LML
                                                 );
             } else {                                                            //201028LML irrigation is trigered by manual irrigation events
                 require_irrigation = true;
                 soil_water_demand_mm = m_to_mm(CropSyst_target_irrigation_m);
                 real_filled_water = soil_water_demand_mm;
             }
             //if (require_irrigation) {
             //    std::clog << "hidx:" << hidx << "\treal_filled_water:" << real_filled_water << std::endl;
             //}
             #endif
             if (require_irrigation/* && ((hidx == 24))*/) {                         //** by looking at soil moisture and MAD**
               veg_con[iveg].VCS.this_year_start_irrigated = true;

               if (is_surface_irrigation(girrig_type) || girrig_type == DRIP)    //190131RLN
                    surface_or_drip = true;
               else surface_or_drip = false;
               out_soil_water_demand_mm = soil_water_demand_mm;
               // Total irrigation water

               if (irrigation_mode == IrrigTP_Sub_surf_drip_perfect || irrigation_mode == IrrigTP_Sub_surf_drip_perfect_eliminate_top) {
                   total_irrig = real_filled_water;

               } else {

               #ifndef USE_CROPSYST_CROP_IRRIGATION
               //#ifdef FULL_IRRIGATION
               #ifdef IRRIGATION_FROM_DEMAND
               total_irrig =
                 #ifdef MECHANISTIC_IRRIGATION
                 soil_water_demand_mm / (irreff.uniformity
                                         * irreff.presumed_efficiency);          //160104LML
                 #else
                 soil_water_demand_mm / irreff.table_irrigation_efficiency;
                 #endif
               if (total_irrig > irrigation_capacity_mm)
                 total_irrig = irrigation_capacity_mm;            //160104LML
               #else
               //Irrigation with capacity
               //190130RLN  Warning daily_irrigation_capacity_mm is not longer defined
               // I presume this is  irrigation_capacity_mm
               total_irrig = daily_irrigation_capacity_mm
                                            * delta_t / (double)SEC_PER_DAY;     //160509LML added time fraction
               #endif                                                            //IRRIGATION_FROM_DEMAND
               //#else                                                             //Deficit irrigation
               #ifndef FULL_IRRIGATION
               //200917LML
               //For proration irrigation, if the rotation is not listed in the
               //irrigation pattern table, using full irrigation amount
               double proration_irrigation = amount_of_deficit_irrigation(rec,rotation_code);
               if (proration_irrigation >= 0) total_irrig = proration_irrigation;
               #endif                                                            //FULL_IRRIGATION==TRUE
               #else                                                             //Irrigation from CropSyst simulation
               total_irrig =
                 current_crop->total_daily_irrigation_from_CropSyst_mm
                 * delta_t / (double)SECONDS_PER_DAY;
               #endif                                                            //USE_CROPSYST_CROP_IRRIGATION
               } //irrigation_mode == IrrigTP_Sub_surf_drip_perfect

               //Partition of irrigated water to evap & runoff loss
               // Ed (evap from irrigation systems)
               #ifdef MECHANISTIC_IRRIGATION


               /*std::cerr << "WARNNING: TESTING irrigation water loss!!!\n";
               total_irrig = 50.0;//27.045;
               ET_potential = 10.0;//0.359;
               ET0_open_water_actual = 10.0;//0.41777;
               crop_height = 0.1;
               isPerennial = false;
               */




               evap_from_irrig_sys = evaporation_from_irrigation_systems(
                    irrigation_index,
                    total_irrig,
                    ET_potential,
                    ET0_open_water_actual,
                    crop_height,
                    soil_con,
                    isPerennial
                    );                                                           //(mm/day)calculates the evaporation from irrigation system. 121130 Keyvan
               evap_from_irrig_sys *= delta_t / (double)SEC_PER_DAY;                      //150702LML

               /*
               std::cerr << "Irrigation type_code:"  << Irrigation_library[irrigation_index].IrrigationType_code
                         << "\tirrig_index:"    << irrigation_index
                         << "\ttotal_irrig:"    << total_irrig
                         << "\tevap_loss:"      << evap_from_irrig_sys
                         << "\tTable_eff:"      << irreff.table_irrigation_efficiency
                         << "\tcal_eff:"        << (irreff.uniformity * irreff.presumed_efficiency)
                         << "\tTable_evap_f:"   << irreff.table_irrigation_evap_loss
                         << "\tcal_evap_f:"     << (evap_from_irrig_sys/total_irrig)
                         << std::endl;
              */

               #else
               evap_from_irrig_sys = irreff.table_irrigation_evap_loss
                                     * total_irrig;                              //160104LML
                                                                                 //180524LML We put the deep_percolation loss into the soil profile, not into the baseflow part.
                                                                                 //Need talk with Keyvon & Claudio.
               #endif                                                            //MECHANISTIC_IRRIGATION
               irrig_aft_evap = total_irrig - evap_from_irrig_sys;
             }                                                                   // is_irrigation
           }                                                                     //if in irrig season

           // EC (evap from canopy intercepted water
           //170920LML double canopy_fract =
           //170920LML         #if (VIC_CROPSYST_VERSION >= 3)
           //170920LML         VIC_land_unit_get(VIC::CANOPY_COVER_FRACTION_TOTAL);
           //170920LML         #else
           //170920LML         canopy_fraction();                                            //keyvan added this to avoid overestimation of evaporation from canopy
           //170920LML         #endif
#ifdef ENABLE_IRRIGATION_CANOPY_INTERCEPTION
           if (Wdew_max > 0) {
               if (irrig_aft_evap > 0 && !surface_or_drip) {
                   double canopy_initial_deficit = std::max<double>(0,
                                                           Wdew_max - tmp_Wdew);
                   //160104LML added condition for minor irrigation ammount
                   if (irrig_aft_evap > canopy_initial_deficit) {
                       irrig_intcp_canopy_init = canopy_initial_deficit;
                       tmp_Wdew = Wdew_max;
                   } else {
                       irrig_intcp_canopy_init = irrig_aft_evap;
                       tmp_Wdew += irrig_aft_evap;
                   }
//180522LML//#if MECHANISTIC_IRRIGATION
//180522LML//               //180522LML Check the following code. This evap is never used
//180522LML//               //excepted for counting the canopyevap loss.
//180522LML//               potevap_irrig_canopy =                                            //170920LML canopy_fract *
//180522LML//                 crop_potential_ET * pow((tmp_Wdew / Wdew_max),(2.0/3.0))
//180522LML//                 * delta_t / SEC_PER_DAY;                                        //keyvan dec102012
//180522LML//#endif
               }
           }
#endif

           canopyevap = crop_potential_ET * pow((tmp_Wdew / Wdew_max),(2.0/3.0))
                                     * delta_t / SEC_PER_DAY;                    //180522LML
           canopyevap = CORN::must_be_between(canopyevap, 0.0, tmp_Wdew);
#ifndef MECHANISTIC_IRRIGATION
           if (irrig_aft_evap > 0) canopyevap = 0;                               //180511LML the irrigation loss will be counted as evaporation loss as in the parameter
                                                                                 //so here will not be double counted
                                                                                 //180524LML: if use prescipt irrigation efficiency approach, should we specifically intercept the irrigated water on the canopy?
                                                                                 //Need some furture consideration on the CropSyst side.
#endif
#ifdef CROPSYST_HANDLE_CANOPY_EVAP
           canopyevap = 0;                                                       //180523LML the canopy intecepted water will be transfered to crop object as "intercepted_precipitation_m_stored"
#endif
           //tmp_Wdew -= evap_wet_canopy;
           //canopyevap += evap_wet_canopy;
           //std::clog << "evap_wet_canopy:" << evap_wet_canopy
           //          << std::endl;

           irrig_over_soil = irrig_aft_evap - irrig_intcp_canopy_init;
           //Ro (runoff from irrigation)
           if (total_irrig > 0) {
#if MECHANISTIC_RUNOFF==TRUE
             runoff_from_irrig_sys = calc_irrigation_runoff(irrigation_index
               ,irrig_over_soil
               ,tmp_layer[0].moist
               ,maximum_allowable_depletion
               ,soil_con
#ifndef FULL_IRRIGATION
               ,global_param.VCS.basin_wide_proration
#endif
                     );
             //180531LML how to handle the deep percolation?
#else
             runoff_from_irrig_sys = total_irrig
                                     * table_irrigation_runoff_loss;             //180511LML irreff.table_irrigation_runoff_loss;
#endif //MECHANISTIC_RUNOFF==TRUE

#ifndef MECHANISTIC_IRRIGATION
             //180813LML for mechanical irrigation, shouldn't seperate total runoff loss into deep percolation and surface runoff?
             deep_percolation_from_irrig_sys = total_irrig * table_deep_percolation_loss; //180531LML
             if (deep_percolation_from_irrig_sys > (total_irrig - runoff_from_irrig_sys)) {
                 std::clog << "deep_percolation_from_irrig_sys > (total_irrig - runoff_from_irrig_sys)! and set maximum \n";
                 deep_percolation_from_irrig_sys = total_irrig - runoff_from_irrig_sys;
             }
#endif

           }                                                                     //if
           irrig_for_pot_infiltration = irrig_over_soil - runoff_from_irrig_sys;
           irrig_for_pot_infiltration -= deep_percolation_from_irrig_sys;        //180531LML
           tmp_cell_data->VCS.irrigation_runoff = runoff_from_irrig_sys;
           tmp_cell_data->VCS.irrigation_netdemand = out_soil_water_demand_mm;
           tmp_cell_data->VCS.irrigation_water  = total_irrig;
           tmp_cell_data->VCS.table_irrigation_efficiency = irrig_efficiency;
           //tmp_cell_data->VCS.net_irrigation = irrig_for_pot_infiltration;       //210129
           tmp_cell_data->VCS.evap_from_irrigation_syst = evap_from_irrig_sys;
        } else                                                                   // if not a crop
        #endif //VIC_CROPSYST_VERSION
        {  //cropsyst is on but we do not have crop on land keyvan 01032013
           //LML 141015 shared code for non-crops or VropSyst is off
           rc = calc_rc(0.0, net_short, veglib_specific.RGL,
                        air_temp, vpd, veglib_specific.LAI[month-1],
                        1.0, FALSE);                                             //150501LML it's always zero!!
           canopyevap = pow((tmp_Wdew / Wdew_max),(2.0/3.0))
                        * penman(air_temp, elevation, rad, vpd, ra, rc,
                                 veglib_specific.rarc)
                        * delta_t / SEC_PER_DAY;
        }
        double f = 0;                                                            /* fraction of time step used to fill canopy */
        if (canopyevap > 0.0 && is_approximately(delta_t,SEC_PER_DAY)) {
            /** If daily time step, evap can include current precipitation **/
        bool test = is_approximately(delta_t,SEC_PER_DAY);
          f = std::min<double>(1.0,((tmp_Wdew + ppt
                                      #if VIC_CROPSYST_VERSION==2
                                      + amount_of_irrigation                     //LML 150415 tmp_Wdew alreay set to Wdewmax if irrigated (!surface_or_drip)
                                      #endif
                                     ) / canopyevap));
        } else if (canopyevap > 0.0) {
            //** If sub-daily time step, evap can not exceed current storage **
            f = std::min<double>(1.0,((tmp_Wdew) / canopyevap));
        } else {
            f = 1.0;
        }
        canopyevap *= f;
        //LML 150430 according to VIC5.0 beta, this tmp_Wdew should be used for calculate transpiration
        #if VIC_CROPSYST_VERSION>=3
        double Wdew_for_transpiration = tmp_Wdew;
        #endif
        tmp_Wdew += ppt - canopyevap;
        if (is_approximately(tmp_Wdew,0)) tmp_Wdew = 0;
        if (tmp_Wdew < 0) {
           #ifdef VIC_CROPSYST_VERSION
           if(is_crop && (irrig_for_pot_infiltration > 0) && !surface_or_drip
              && CALC_EVAP) {                                                    //150710LML added CALC_EVAP
#ifdef MECHANISTIC_IRRIGATION
              //rainfall has the priority to get evaporated otherwise irrigation water will meet the canopy evap demand
              canopy_evap_deficit = fabs(tmp_Wdew);
              canopy_maxdew_deficit = Wdew_max;
              double total_deficit = canopy_evap_deficit +
                                     canopy_maxdew_deficit;
              if (irrig_for_pot_infiltration > total_deficit) {
                 tmp_Wdew = Wdew_max;                                            //130318 RLN
                 irrig_for_pot_infiltration -= total_deficit;
              } else {
                 if (irrig_for_pot_infiltration > canopy_evap_deficit) {
                    canopy_maxdew_deficit = irrig_for_pot_infiltration -
                                                 canopy_evap_deficit;
                    tmp_Wdew = canopy_maxdew_deficit;
                 } else {
                    canopyevap -= canopy_evap_deficit -
                                  irrig_for_pot_infiltration;
                    canopy_evap_deficit = irrig_for_pot_infiltration;
                    canopy_maxdew_deficit = 0.0;
                    tmp_Wdew = 0;
                 }
                 irrig_for_pot_infiltration = 0;
              }
#endif
           } else {
             canopyevap += tmp_Wdew;                                             //**130521 keyvan added to fix this bug**
             tmp_Wdew    = 0.0;
           }
           #else
             tmp_Wdew = 0.0;
           #endif //VIC_CROPSYST_VERSION
        } else if (tmp_Wdew <= Wdew_max) {
           #ifdef VIC_CROPSYST_VERSION
           if (is_crop && require_irrigation && !surface_or_drip && CALC_EVAP) { //150710LML added CALC_EVAP
              //precipitation water is enough to meet Evap demand but not to fill the canopy
              canopy_maxdew_deficit = Wdew_max - tmp_Wdew;
              if (irrig_for_pot_infiltration > canopy_maxdew_deficit) {
                 /***for drip and surface irrigation interception is going to be zero  **/
                 tmp_Wdew = Wdew_max;
              } else {
                 canopy_maxdew_deficit = irrig_for_pot_infiltration;             //160509LML
                 tmp_Wdew += canopy_maxdew_deficit;                              //160104LML
              }
           }
           #endif
        } else { //precipitation is enough to meet canopy interception and evap
           throughfall += tmp_Wdew - Wdew_max;
           tmp_Wdew     = Wdew_max;
        }
        irrig_for_pot_infiltration -= canopy_maxdew_deficit
                                      + canopy_evap_deficit;

        #ifdef VIC_CROPSYST_VERSION
        //180705LML & Claudio crop_potential_ET_reminning -= evap_from_irrig_sys;                      //180620LML + canopyevap;


        #ifndef CROPSYST_HANDLE_CANOPY_EVAP
        crop_potential_ET_reminning -= canopyevap;
        #endif
        crop_potential_ET_reminning  = std::max<double>(
                                               crop_potential_ET_reminning,0.0);
        #endif
        #if (defined(CROPSYST_PROVIDED_SOIL_HYDROLOGY) && (VIC_CROPSYST_VERSION==2))
        double infiltration_wet = throughfall;
        #endif
        //*******************************************
        //Compute transpiration (T) and evaporation from soil (Es)
        //*******************************************
        double E_soil_actual = 0;
       #ifdef VIC_CROPSYST_VERSION
       if (is_crop && CALC_EVAP) {//150710LML added CALC_EVAP
           double potential_soil_evap = 0.0;                                     //(mm/delta_t)
           //*************   Es (evaporation from soil)   ********************
           #if VIC_CROPSYST_VERSION==2
           VIC_CropSyst_object_activate(current_crop->CropSystHandle);           //130718 RLN
           #endif
           double crop_LAI = VIC_CropSyst_get(VIC::GAI); //current_crop ? current_crop-> crop_leaf_area_index : 0;
           double canapy_cover = VIC_CropSyst_get(
            #if (VIC_CROPSYST_VERSION>=3)
               //190726LML VIC::CANOPY_COVER_FRACTION_TOTAL
               VIC::CANOPY_COVER_FRACTION_GREEN                    //190726LML suppose to be total canopy cover but the CropSyst returns wrong number at this moment
            #else
               VC_canopy_cover_fraction_total
            #endif
               );

           //std::clog <<  "canapy_cover:" << canapy_cover << std::endl;

           double land_pet = 0;                                                  //over entire land (crop + baresoil)
           if (crop_LAI > 0) land_pet = crop_potential_ET_reminning;
           else land_pet =
             #if VIC_CROPSYST_VERSION==2
             ET0_short
             #else
             pot_et[PET_SHORT]
             #endif
             ;
           potential_soil_evap = cal_soil_evap_from_total_evap(land_pet);
           if (crop_LAI > 0)
               //current_crop->potential_transpir = crop_potential_ET_reminning;   //180704LML & Claudio - potential_soil_evap;
               current_crop->potential_transpir = crop_potential_ET_reminning - potential_soil_evap;


           //180620LML set the value of potential ET for CropSyst
           tmp_cell_data->VCS.potential_transpir = crop_potential_ET_reminning;


           //std::clog << "VCS.potential_transpir:" << tmp_cell_data->VCS.potential_transpir
           //          << "\tpotential_soil_evap:" << potential_soil_evap
           //          << "\tpot_et[PET_SHORT]:" << pot_et[PET_SHORT]
           //          << std::endl;

           #if (VIC_CROPSYST_VERSION>=3)
           //LML 150505 added for calculating soil evaporation that doesn't covered by canopy or residue
           //180523LML the rad and ra in the canopy gap should be less than the baresoil without any canopies.
           //Here the soil evaporation might be overestimagted.

           //190718 V5 parameter 
           double SOIL_RARC = 100.0;
           
           double temp_soil_evap = (1.0 - canapy_cover) * delta_t * m_to_mm(calc_soil_evap(*tmp_layer, soil_con,air_temp,
               soil_con.elevation, rad, vpd, ra, 0, SOIL_RARC
               #if SPATIAL_FROST
               ,frost_fract
               #endif
               ));
           //double temp_soil_evap = (1.0 - canapy_cover) * delta_t * m_to_mm(soil_evap_m); //(mm/delta_t) //190121RLN_160509LML
           potential_soil_evap = std::min<double>(temp_soil_evap, potential_soil_evap);
           #endif //VIC_CROPSYST_VERSION>=3
           E_soil_actual =
           #if (VIC_CROPSYST_VERSION>=3)
             std::min<double>((1.0 - canapy_cover) * (tmp_layer[0].moist - 0.5 * soil_con_global->Wpwp[0]),
                              potential_soil_evap);                              //LML 150318 180522LML: the soil evaporation may get to the 1/2 of wiltpoint (FAO 56).
           #else
             VIC_calc_soil_evaporation_actual_mm(potential_soil_evap ,
                                                 tmp_layer[0].moist);            //calculate the actual
           #endif


           E_soil_actual = std::max<double>(0.0,E_soil_actual);                  //LML 150318
                                                                                 //LML 150504 note: why do not use arno_evap() to calculte actural soil evaporation?
                                                                                 //i.e. considering the effect of soil moisture?

           //if (E_soil_actual > 10) {
           //  std::clog << "E_soil_actual:" << E_soil_actual << std::endl;
           //}


           #if (UPDATE_T_VPD==TRUE)
           /***********************************************
           updating Temperature and Humidity
           ***********************************************/
           int crop_ret=0;
           current_crop->delta_T=0;
           current_crop->delta_VPD=0;
           if (is_irrigation){
               double T_new;
               double vpd_new;
               double  wet_bulb_T;
               double low_T=atmos->air_temp[0]-SURF_DT-5.0;
               double upper_T= atmos->air_temp[0]+SURF_DT+5.0;
               char ErrorString[MAXSTRING]; //ErrorString[0]=0;
               if((Ed>0 ||  current_crop->evap_intercepted_irrig_water>0) && atmos->wind[24]>1.0)
               {
                   double T_old=(atmos->tmax+atmos->tmin)/2;
                   double vp_old;
                   for(i=0; i<23 ;i++){
                       vp_old += (atmos->vp[i])/24;
                   }
                   double r_c_arc=rc + rarc;
                   //double error_strg; ERR*/

                   /*********   penman(air_temp, elevation, rad, vpd, ra, rc, rarc); **********/

                   T_new = root_brent(low_T,
                           upper_T,
                           ErrorString,
                           solve_penman_for_T,
                           air_temp,
                           vpd,
                           //atmos->vp,
                           current_crop->evap_intercepted_irrig_water, //Ec,
                           Ed, //current_crop->irrigation_evap,
                           ET_pot,
                           //rs,
                           rc,
                           rarc,
                           ra,
                           rad,
                           soil_con->elevation,
                           current_crop                                         //150702LML crops,
                           //150702LML current_crop_counter
                           );
                   /*********************
                   calculate wet bulb T
                   to compare the new_T to the theoritical lowest T can be reached by
                   increasing hunmidity which is wet bulb T
                   ********************/

                   wet_bulb_T=root_brent(low_T,
                                     upper_T,
                                     ErrorString,
                                     solve_wet_bulb,
                                     air_temp,
                                     vpd,
                                     elevation
                               );

                   /****************updating the vpd*******************/

                   if(T_new<wet_bulb_T)
                   {
                       T_new=air_temp;
                       vpd_new= vpd;
                   }/// if T_new < wet bulb
                   else {
                       vpd_new=current_crop->new_vpd;
                   }

               } ///if Ec or Ed >0
               else {
                   T_new=air_temp;
                   vpd_new= vpd;
               }
               //air_temp=T_new;
               //vpd=vpd_new;
               current_crop->delta_T=current_crop->new_Temp-air_temp;
               current_crop->delta_VPD=current_crop->new_vpd-vpd;
               if (current_crop->delta_T>0.0 && current_crop->delta_T<-4){ ///keyvan put these condition they are not very important
                   current_crop->delta_T=0.0;
                   current_crop->delta_VPD=0.0;
               }
               /************ updating tmax, tmin, MAXVPD  **************************/
               current_crop->new_tmax=atmos->tmax+current_crop->delta_T;
               current_crop->new_tmin=atmos->tmin+current_crop->delta_T;
               current_crop->new_max_vpd= svp(current_crop->new_tmax)-current_crop->new_ea;

               /***********    updating the atmos struct         *************/
               atmos->tmax=current_crop->new_tmax;
               atmos->tmin=current_crop->new_tmin;
               atmos->relative_humidity_max=current_crop->new_ea/svp(current_crop->new_tmin)*100;
               atmos->relative_humidity_min=current_crop->new_ea/svp(current_crop->new_tmax)*100;
               if (atmos->relative_humidity_max>100){
               current_crop->new_ea=svp(current_crop->new_tmin);
               atmos->relative_humidity_max=current_crop->new_ea/svp(current_crop->new_tmin)*100;
               atmos->relative_humidity_min=current_crop->new_ea/svp(current_crop->new_tmax)*100;

               }
           }
           #endif //UPDATE_T_VPD==TRUE

           /****************  calling CropSyst to calculate Transpiration  ***********************/
           #if (VIC_CROPSYST_VERSION == 2)
           if (CALC_EVAP)                                                       //150710LML
           crop_ret = call_crop_model(current_crop/*LML 141015 crop_counter*/
                                           //#ifdef CROPSYST_PROVIDED_ET
                                           ,soil_con
                                           // #endif
                                           ,atmos
                                           ,&layer_wet[0]
                                           ,dmy
                                           ,rec
                                           ,veg_class_code
                                           // &ppt, Transpiration_max,
                                           ,tmp_cell_data
                                           ,is_irrigation
                                           #ifdef CROPSYST_PROVIDED_SOIL_HYDROLOGY
                                           ,infiltration_wet
                                           #endif
                                           #if (VIC_CROPSYST_VERSION>=3)
                                           ,current_crop_counter
                                           ,veg_con
                                           #endif
                                           ); ///passing Tmax instead of PET_short, Keyvan Dec112012
           //LML 150423 #else
           //LML 150423 crop_ret = VIC_CropSyst_get_transpiration(layerevap);
           //           the Soil_hydrology::extract_water() has been called aready
           //           by Crop_common::process_transpiration_m
           #endif //VIC_CROPSYST_VERSION == 2
        } // if crops
        else
        #endif //VIC_CROPSYST_VERSION
        {
           if(CALC_EVAP)
               transpiration(tmp_layer, soil_con, veglib_specific, month, rad,
                             vpd, net_short,air_temp, ra, f, delta_t,
                             #if VIC_CROPSYST_VERSION>=3
                             Wdew_for_transpiration                              //LML 150430
                             #else
                             tmp_veg_var->Wdew
                             #endif
                             ,layerevap
                             #if SPATIAL_FROST
                             ,frost_fract
                             #endif
                             ,root
                             );
        }
        tmp_veg_var->canopyevap    = canopyevap;


        //std::clog << "canopyevap:" << canopyevap << std::endl;


        tmp_veg_var->throughfall   = throughfall;
        tmp_veg_var->Wdew          = tmp_Wdew;
        tmp_Evap                   = canopyevap;
        //LML 150504 Need talk with Roger: how CropSyst could use potential ET, i.e.
        //           ET_potential control to limite the actual ET??
        for (int i = 0; i < options.Nlayer; i++)
        {
            #ifdef VIC_CROPSYST_VERSION
            tmp_layer[i].VCS.evap_bare_soil = 0;                                 //150625LML
            #endif
            if (is_crop) {
              #ifdef VIC_CROPSYST_VERSION
              tmp_layer[i].evap    = tmp_layer[i].VCS.transpiration_daily
                                     * delta_t / SEC_PER_DAY;                    //150615LML
              tmp_Evap            += tmp_layer[i].evap;
              if (i == 0) {
                tmp_layer[i].VCS.evap_bare_soil = E_soil_actual;                 //150624LML
                tmp_Evap          += evap_from_irrig_sys;
              }
              #endif
            } else {
              tmp_layer[i].evap  = layerevap[i];
              tmp_Evap          += layerevap[i];
            }
        }

        #ifdef VIC_CROPSYST_VERSION
        if (is_crop && CALC_EVAP) {                                              //150710LML added CALC_EVAP
          tmp_cell_data->VCS.intercepted_irrigation = irrig_intcp_canopy_init
            + canopy_maxdew_deficit + canopy_evap_deficit;
          tmp_cell_data->VCS.actual_irrigation_reach_ground_amount =
            irrig_for_pot_infiltration;
          tmp_cell_data->VCS.deep_percolation_from_irrigation =
            deep_percolation_from_irrig_sys;                                     //180531LML this water will added to the bottom soil layer for baseflow process
          tmp_cell_data->VCS.evap_from_irrigation_intercept =
              canopy_evap_deficit;
        }
        #endif
        Evap = tmp_Evap / (1000. * delta_t);                                     //(m/s)
        #ifdef CHECK_WATER_BALANCE
        //double final_canopy = tmp_Wdew;
        canopy_water.SetFinalPool(tmp_Wdew);
        canopy_water.AddFluxIn(ppt
                              #ifdef VIC_CROPSYST_VERSION
                              + tmp_cell_data->VCS.intercepted_irrigation
                              #endif
                              );
        canopy_water.AddFluxOut(throughfall + canopyevap);
        if (!canopy_water.IsMassBalance()) {
           canopy_water.PrintMassBalanceTerms("Canopy water balance error");
        }
        #ifdef VIC_CROPSYST_VERSION
        irrigation_water.AddFluxIn(total_irrig);
        irrigation_water.AddFluxOut(
            tmp_cell_data->VCS.actual_irrigation_reach_ground_amount
            + tmp_cell_data->VCS.deep_percolation_from_irrigation
            + tmp_cell_data->VCS.intercepted_irrigation
            + tmp_cell_data->VCS.irrigation_runoff
            + tmp_cell_data->VCS.evap_from_irrigation_syst);
        if (!irrigation_water.IsMassBalance())
            irrigation_water.PrintMassBalanceTerms(
                  "Irrigation water balance error");
        #endif
        #endif

        //190724LML for perfect irrigation, the irrigation water already filled into the soil, so empty the actual_irrigation_reach_ground_amount
        #ifdef VIC_CROPSYST_VERSION
        if (tmp_cell_data->VCS.actual_irrigation_reach_ground_amount > 0
            && (irrigation_mode == IrrigTP_Sub_surf_drip_perfect || irrigation_mode == IrrigTP_Sub_surf_drip_perfect_eliminate_top))
            tmp_cell_data->VCS.actual_irrigation_reach_ground_amount = 0;
        #endif

     } // if mu
  } //for dist
  return (Evap);
}
///**********************************************************************
//    EVAPOTRANSPIRATION ROUTINE
//**********************************************************************
void transpiration(layer_data_struct *layer
                   ,const soil_con_struct &soil_con
                   ,const veg_lib_struct &veglib
                   //160510LML ,int veg_class
                   ,int month
                   ,double rad                                                           //net radiation
                   ,double vpd
                   ,double net_short
                   ,double air_temp
                   ,double ra
                   //160510LML ,double ppt
                   ,double f
                   ,double delta_t
                   ,double Wdew
                   //160510LML double elevation
                   //160510LML ,double *depth
                   //160510LML,double *Wcr
                   //160510LML,double *Wpwp
                   //160510LML ,double *new_Wdew    /*LML 150501 not used*/
                   //160510LML ,double *canopyevap  /*LML 150501 not used*/
                   ,double *layerevap   /*LML 150430 note: mm/delta_t*/
                   #if SPATIAL_FROST
                   ,double *frost_fract
                   #endif
                   ,const float  *root
                   )
//**********************************************************************
// Computes evapotranspiration for unfrozen soils
// Allows for multiple layers.
//
// modifications:
// 6-8-2000 Modified to use spatially distributed soil frost if
//          present.                                               KAC
// 2006-Oct-16 Modified to initialize ice[] for all soil layers
//         before computing available moisture (to avoid using
//         uninitialized values later on).                TJB
// 2009-Jun-09 Moved computation of canopy resistance rc out of penman()
//         and into separate function calc_rc().            TJB
//
// 141017 LML veg_class is veg index in veg_lib.
//*********************************************************************
{
  extern option_struct options;
  double avail_moist[MAX_LAYERS];       /* moisture available for trans */
  double ice[MAX_LAYERS];
  int bottom_layer = options.Nlayer - 1;
  //**********************************************************************
  //  EVAPOTRANSPIRATION
  //  Calculation of the evapotranspirations
  //  2.18
  //  First part: Soil moistures and root fractions of both layers
  //  influence each other
  //  Re-written to allow for multi-layers.
  //**************************************************
  // Set ice content in all individual layers
  // **************************************************
  for (int i = 0; i <= bottom_layer; i++) {
    #if SPATIAL_FROST
    ice[i] = 0;
    for (int frost_area = 0; frost_area < FROST_SUBAREAS; frost_area++ ) {
      ice[i] += layer[i].ice[frost_area] * frost_fract[frost_area];
    }
    #else
    ice[i]         = layer[i].ice;
    #endif
  }

  /**************************************************
    Compute moisture content in combined upper layers
    **************************************************/
  double moist1 = 0.0;
  double Wcr1   = 0.0;                                                           //* tmp holding of critical water for upper layers *
  for (int i = 0; i < bottom_layer; i++) {
    if (root[i] > 0.)
        avail_moist[i] = cal_liquid_moist_for_certain_layer(layer[i]
            #if SPATIAL_FROST
            ,frost_fract
            #endif
            );
    else avail_moist[i] = 0;
    moist1 += avail_moist[i];
    Wcr1   += soil_con.Wcr[i];
  }

  //*****************************************
  // Compute moisture content in lowest layer
  // *****************************************
  avail_moist[bottom_layer] = cal_liquid_moist_for_certain_layer(
      layer[bottom_layer]
      #if SPATIAL_FROST
      ,frost_fract
      #endif
      );
  double moist2 = avail_moist[bottom_layer];
  //******************************************************************
  // CASE 1: Moisture in both layers exceeds Wcr, or Moisture in
  // layer with more than half of the roots exceeds Wcr.
  //
  // Potential evapotranspiration not hindered by soil dryness.  If
  // layer with less than half the roots is dryer than Wcr, extra
  // evaporation is taken from the wetter layer.  Otherwise layers
  // contribute to evapotransipration based on root fraction.
  //*****************************************************************
  if ( (moist1>=Wcr1 && moist2>=soil_con.Wcr[bottom_layer] && Wcr1>0.) ||
       (moist1>=Wcr1 && (1-root[bottom_layer])>= 0.5)                  ||
       (moist2>=soil_con.Wcr[bottom_layer] && root[bottom_layer]>=0.5)) {
    double gsm_inv = 1.0;
    double rc = calc_rc(veglib.rmin, net_short, veglib.RGL,
                        air_temp, vpd, veglib.LAI[month-1], gsm_inv, FALSE);
    double evap    = penman(air_temp, soil_con.elevation, rad, vpd, ra, rc,
                            veglib.rarc)
                     * delta_t / SEC_PER_DAY
                     * (1.0-f*pow((Wdew/veglib.Wdmax[month-1]), (2.0/3.0)));

    //** divide up evap based on root distribution **
    //** Note the indexing of the roots **
    double root_sum = 1;                                                         //* proportion of roots in moist>Wcr zones *
    double spare_evap = 0;                                                       //* evap for 2nd distribution *
    for (int i = 0; i < options.Nlayer; i++) {
      double gsm_inv = cal_moist_stress_factor(avail_moist[i],i,soil_con);
      layerevap[i] = evap*gsm_inv*(double)root[i];
      if (avail_moist[i] < soil_con.Wcr[i]) {
        root_sum -= root[i];
        spare_evap += evap*(double)root[i]*(1.0-gsm_inv);                        //LML 150430 added +=
      }
    }
    //** Assign excess evaporation to wetter layer **
    if (spare_evap > 0.0) {
      for (int i = 0; i < options.Nlayer; i++) {
        if (avail_moist[i] >= soil_con.Wcr[i]) {
          layerevap[i] += (double)root[i]*spare_evap/root_sum;
        }
      }
    }
  } else {
  //*********************************************************************
  // CASE 2: Independent evapotranspirations
  // Evapotranspiration is restricted by low soil moisture. Evaporation
  // is computed independantly from each soil layer.
  //********************************************************************
    for (int i = 0; i < options.Nlayer; i++) {
      //** Set evaporation soil moisture restriction factor **
      double gsm_inv = cal_moist_stress_factor(avail_moist[i],i,soil_con);
      if(gsm_inv > 0.0){
        /** Compute actual evapotranspiration **/
        double rc = calc_rc(veglib.rmin, net_short, veglib.RGL,
             air_temp, vpd, veglib.LAI[month-1], gsm_inv, FALSE);
        layerevap[i] = penman(air_temp, soil_con.elevation, rad, vpd, ra, rc,
                              veglib.rarc)
                       * delta_t / SEC_PER_DAY * (double)root[i]
                       * (1.0-f*pow((Wdew/veglib.Wdmax[month-1]), (2.0/3.0)));
      } else {
        layerevap[i] = 0.0;
      }
    } //i
  }

  //****************************************************************
  // Check that evapotransipration does not cause soil moisture to
  // fall below wilting point.
  //***************************************************************
  for (int i = 0; i < options.Nlayer; i++ ) {
    if (ice[i] > soil_con.Wpwp[i] && layerevap[i] > avail_moist[i])
      layerevap[i] = avail_moist[i];
    else if ( layerevap[i] > (layer[i].moist - soil_con.Wpwp[i]) )
      layerevap[i] = layer[i].moist - soil_con.Wpwp[i];
    if ( layerevap[i] < 0.0 ) layerevap[i] = 0.0;
  }
}
/*170413LML moved to VCS_Nl.c
//**********************************************************************
//solver Keyvan Oct152013
//***********************************************************************
//***************************************
//solver
//****************************************
//LML 140812 added macro
#if VIC_CROPSYST_VERSION
double solve_penman_for_T(double Ts, va_list ap){
    double init_T;
    double vpd;
    //double vp;
    double Ec;
    double Ed;
    double ET_pot;
    double rc;
    double rarc;
    double ra;
    double Rn;
    double elev;
    //double cp;
    //LML 140812 added CROPSYST_ON
    //150702LML#ifdef VIC_CROPSYST_VERSION
    //150702LML crop_data_struct *current_crop;                                              //150702LML crops;
    //150702LML int current_crop_counter;
    //150702LML#endif


    ////////////////// Passing arguments through va_list/////////

    init_T              = (double) va_arg(ap,double ); /// has to be in C and will be converted to K
    vpd                 = (double) va_arg(ap,double ); /// has to be in Pascal
    //vp                  = (double) va_arg(ap,double );
    Ec                  = (double) va_arg(ap, double); /// mm
    Ed                  = (double) va_arg(ap, double); /// mm
    ET_pot              = (double) va_arg(ap, double); /// mm
    rc                  = (double) va_arg(ap, double);
    rarc                = (double) va_arg(ap, double);
    ra                  = (double) va_arg(ap, double);
    Rn                  = (double) va_arg(ap, double);
    elev                = (double) va_arg(ap, double);

    //LML 140812 added CROPSYST_ON
    #ifdef VIC_CROPSYST_VERSION
    crop_data_struct *current_crop = (crop_data_struct *) va_arg(ap, crop_data_struct *); ///crop structure
    //150702 current_crop_counter = (int) va_arg(ap, int);
    //150702 crop_data_struct *current_crop = &crops[current_crop_counter];
    #endif
    //cp                  = (double) va_arg(ap, double);
    double init_ea = 1000* 0.618*exp(17.27*init_T/(init_T+237.3))-vpd;

    //double P=101.3*pow((293.0-0.0065*elev)/293.0 ,5.26);
    //Ts=init_T;
    double C_gas_law        = 2.16679;
    double e                = 0.622; ///water_to_air_molecular_weight_ratio
    double cp               = 0.001013; ///specific heat of air
    double init_A           = C_gas_law*init_ea/(init_T+273.15);
    double resid_vertical_dist_coeff=0.8756; ///for 30 minutes of residence time and 10 cm above crop for 10 m log profile starting from 10cm below canopy level
    double new_A            = init_A +resid_vertical_dist_coeff*(Ed +Ec);

    double new_ea           = new_A*(Ts+273.15)/C_gas_law;
    double new_es           = 0.618*exp(17.27*Ts/(Ts+237.3))*1000.;
    double new_vpd          = new_es-new_ea;

    //LML 140812 added CROPSYST_ON
    #ifdef VIC_CROPSYST_VERSION
    current_crop->new_vpd  = new_vpd;
    current_crop->new_Temp = Ts;
    current_crop->new_ea   = new_ea;
    #endif

    //  penman(air_temp, elevation, rad, vpd, ra, rc, rarc); *********
    double ET = penman(Ts, elev, Rn, new_vpd, ra, rc, rarc);
    double kc =
        #if VIC_CROPSYST_VERSION >= 3
        VIC_land_unit_get(VIC::ET_COEF_FULL_SHADING);
        #else
        crop_coefficient();
        #endif
    double target_ET = (ET_pot - (Ed + Ec)) / kc;
    //Ts=init_T;
    //double check_exp=0.6108*exp(17.27*Ts/(Ts+237.3));
    return ET - target_ET;
}
#endif
//**********************************************************
//solve wet bulb T equation

//based campbell book page 46, chapter 3
//**********************************************************
double solve_wet_bulb(double Ts, va_list ap)
{
    double air_T;
    double vpd;
    double elev;
    //////////////// Passing arguments through va_list/

    air_T               = (double) va_arg(ap,double ); /// has to be in C and will be converted to K
    vpd                 = (double) va_arg(ap,double ); /// has to be in Pascal
    elev                = (double) va_arg(ap, double);

    double cp = 0.001013; ///specific heat of air
    double calc_air_T;
    double latent_heat_vaporization = 2.501-0.002361*air_T;
    double P = 101.3*pow((293.0-0.0065*elev)/293.0 ,5.26);
    double psychrometric_constant = cp*P/latent_heat_vaporization; ////
    double ea = (0.618*exp(17.27*air_T/(air_T+237.3)))- vpd/1000.0;

    double es_TW = 0.618*exp(17.27*Ts/(Ts+237.3));

    calc_air_T = Ts + (es_TW-ea)/psychrometric_constant;


    double error = air_T-calc_air_T;
    return error;
}
*/

/*170413LML moved to VCS_Nl.c
#ifdef VIC_CROPSYST_VERSION
#ifndef USE_SIMPLIFIED_IRRIGATION_TYPES
double evaporation_from_irrigation_systems(
                                           double ET0,
                                           double ET0_open_water,
                                           int irrigation_index,
                                           double crop_h = 0.5
                                           )
{
    double Ed = 0;
    VIC_Irrigation_library *current_irrigation = &Irrigation_library[irrigation_index];
    int irrigation_code = current_irrigation->IrrigationType_code;
    //  double irrigation_runoff = Irrigation_library[irrigation_index].irrigation_efficiency[0];
    #if (VIC_CROPSYST_VERSION>=3)
    double cd               = current_irrigation->irrigation_efficiency.cd;                      //1.1; //nuzzle coeficient
    double h                = current_irrigation->irrigation_efficiency.h_op*100;                   //207.0; //irrigation system operating pressure kpa
    double g                = GRAVITY;//9.81;

    double D                = current_irrigation->irrigation_efficiency.D;                          //3.0; //average droplet size mm
    double teta             = current_irrigation->irrigation_efficiency.q_nuzzle*0.0174532925;   //35.0*0.0174532925; //angle of the nuzzle it should be converted to radian by a function
    double x0               = current_irrigation->irrigation_efficiency.x_dm_spk;                  //10.0; //diameter of sprinkler
    double y0_0             = current_irrigation->irrigation_efficiency.y0_spk;                  //2.0; //height of the irrigation sprinkler from ground m
    double time_of_irrig    = current_irrigation->irrigation_efficiency.tirrig;
    double actual_drop_size = current_irrigation->irrigation_efficiency.actual_droplet_size;
    double Ap               = current_irrigation->irrigation_efficiency.A_Ap;                      //0.07;//percentage of area which is covered by irrigation system at a time
    #else
    double cd               = current_irrigation->irrigation_efficiency[4];                      //1.1; //nuzzle coeficient
    double h                = current_irrigation->irrigation_efficiency[2]*100;                     //207.0; //irrigation system operating pressure kpa
    double g                = 9.81;

    double D                = current_irrigation->irrigation_efficiency[1];                         //3.0; //average droplet size mm
    double teta             = current_irrigation->irrigation_efficiency[8]*0.0174532925;         //35.0*0.0174532925; //angle of the nuzzle it should be converted to radian by a function
    double x0               = current_irrigation->irrigation_efficiency[6];                        //10.0; //diameter of sprinkler
    double y0_0             = current_irrigation->irrigation_efficiency[5];                      //2.0; //height of the irrigation sprinkler from ground m
    double time_of_irrig    = current_irrigation->irrigation_efficiency[11];
    double actual_drop_size = current_irrigation->irrigation_efficiency[12];
    double Ap               = current_irrigation->irrigation_efficiency[10];                       //0.07;//percentage of area which is covered by irrigation system at a time
    #endif

    const double K_t = 0.6425;
    const double k_D = -0.0108;
    // static double crop_h=0.5; //crop height m it should be impelemented from CropSyst
    // static double K_t=-0.07527; //empirical coefficient of the t term
    // static double k_D=0.31866;
    if (irrigation_code < IrrigTP_drip_0_0) {
        double v0 = cd*pow((2.0*g*h/100.0),0.5); //initial velocity
        //
        //calcualte the evaporation from
        //irrigation system   keyvan 121130
        //
        double t_term = pow((v0*sin(teta)/g+pow(pow(v0,2.0)*pow(sin(teta),2)*g*y0_0,0.5)/g),K_t);//pow(((tan(teta)-(y0_0-crop_h)/x0)*v0*cos(teta)/g),K_t);
        double D_term = pow(h/(actual_drop_size*10.), k_D);
        Ed = ET0 * D_term * t_term;
    }
    else{
        Ed = ET0_open_water; //it's just a fixed number and later we can define it for diferent irrigation systems
    }
    //printf("Ed(%.5f)\tAp(%.5f)\ttime_of_irrig(%.5f)\n",Ed,Ap,time_of_irrig);
    return Ed * Ap * time_of_irrig / 24.0 / 3.0; //*LML 150501 I don't know where 3.0 come from.*

};
#endif
//*LML 150501__________________________________________________________________*
void clear_cell_irrigation_water_fluxes(cell_data_struct *current_cell)
{
  //Initialize water fluxes from current crop in current time step
    current_cell->VCS.evap_from_irrigation_syst      = 0;
    current_cell->VCS.total_irrigation_water         = 0;
    current_cell->VCS.actual_irrigation_reach_ground_amount = 0;                     //150714LML
    current_cell->VCS.irrigation_runoff              = 0;
    current_cell->VCS.intercepted_irrigation         = 0;
    current_cell->VCS.evap_from_irrigation_intercept = 0;
    current_cell->VCS.potential_transpir             = 0;
}
#endif //VIC_CROPSYST_VERSION
*/
//160510LML
double cal_liquid_moist_for_certain_layer(const layer_data_struct &layer
    #if SPATIAL_FROST
    ,double *frost_fract
    #endif
    )
{
  double moist = 0;
  #if SPATIAL_FROST
  for (int frost_area = 0; frost_area < FROST_SUBAREAS; frost_area++)
    moist += ((layer.moist - layer.ice[frost_area])
           * frost_fract[frost_area]);
  #else
  moist = layer.moist - layer.ice;
  #endif
  return moist;
}
//160510LML
double cal_moist_stress_factor(const double liq_moist
                               ,const int layer_index
                               ,const soil_con_struct &soil_con
                               )
{
   double gsm_inv = 0;
   if(liq_moist >= soil_con.Wcr[layer_index])
     gsm_inv = 1.0;
   else if(liq_moist >= soil_con.Wpwp[layer_index])
     gsm_inv = (liq_moist - soil_con.Wpwp[layer_index]) /
               (soil_con.Wcr[layer_index] - soil_con.Wpwp[layer_index]);
  return gsm_inv;
}
/*170413LML moved to pubtools.cpp
//160510LML copied from CropSyst
bool is_approximately(const double &value,const double &target,const double &tolerance)
{ return ((value) < ((target) + (tolerance))) && ((value) > ((target) - (tolerance)));}
//160526LML____________________________________________________________________/
*/
double overstory_canopy_evap(layer_data_struct *layer_wet,
                             layer_data_struct *layer_dry,
                             veg_var_struct    *veg_var_wet,
                             veg_var_struct    *veg_var_dry,
                             int                veg_class,
                             int                month,
                             double             mu,
                             double            *Wdew,
                             double             delta_t,
                             double             rad,
                             double             vpd,
                             double             net_short,
                             double             air_temp,
                             double             ra,
                             double             elevation,
                             double            *prec,
                             double            *dryFrac)
/**********************************************************************
  CANOPY EVAPORATION
  Overstory canopy evaporation from intercepted water
**********************************************************************/
{
  /** declare global variables **/
  extern veg_lib_struct *veg_lib;
  extern option_struct options;
  /** declare local variables **/
  int                Ndist;
  int                dist;
  int                i;
  double             ppt;		/* effective precipitation */
  double             f;		/* fraction of time step used to fill canopy */
  double             throughfall;
  double             Evap;
  double             tmp_Evap;
  double             canopyevap;
  double             tmp_Wdew;
  double             layerevap[MAX_LAYERS];
  double             rc;
  layer_data_struct *tmp_layer;
  veg_var_struct    *tmp_veg_var;
  if(options.DIST_PRCP) Ndist = 2;
  else Ndist = 1;
  Evap = 0;
  for ( dist = 0; dist < Ndist; dist++ ) {
    /* Initialize variables */
    for ( i = 0; i < options.Nlayer; i++ ) layerevap[i] = 0;
    canopyevap = 0;
    throughfall = 0;
    /* Set parameters for distributed precipitation */
    if(dist==0) {
      tmp_layer   = layer_wet;
      tmp_veg_var = veg_var_wet;
      ppt         = prec[WET];
      tmp_Wdew    = Wdew[WET];
    }
    else {
      tmp_layer   = layer_dry;
      tmp_veg_var = veg_var_dry;
      ppt         = prec[DRY];
      mu          = (1. - mu);
      tmp_Wdew    = Wdew[DRY];
    }
    if(mu > 0) {
      /****************************************************
        Compute Evaporation from Canopy Intercepted Water
      ****************************************************/
      tmp_veg_var->Wdew = tmp_Wdew;
      if (tmp_Wdew > veg_lib[veg_class].Wdmax[month-1]) {
        throughfall = tmp_Wdew - veg_lib[veg_class].Wdmax[month-1];
        tmp_Wdew    = veg_lib[veg_class].Wdmax[month-1];
      }
      rc = calc_rc((double)0.0, net_short, veg_lib[veg_class].RGL,
           air_temp, vpd, veg_lib[veg_class].LAI[month-1],
           (double)1.0, FALSE);
      canopyevap = pow((tmp_Wdew / veg_lib[veg_class].Wdmax[month-1]),(2.0/3.0))
           * penman(air_temp, elevation, rad, vpd,
                    ra, 0, veg_lib[veg_class].rarc)
           * delta_t / SEC_PER_DAY;
      if (canopyevap > 0.0 && delta_t == SEC_PER_DAY)
        /** If daily time step, evap can include current precipitation **/
        f = std::min<double>(1.0,((tmp_Wdew + ppt) / canopyevap));
      else if (canopyevap > 0.0)
        /** If sub-daily time step, evap can not exceed current storage **/
        f = std::min<double>(1.0,((tmp_Wdew) / canopyevap));
      else
        f = 1.0;
      canopyevap *= f;
      /* compute fraction of canopy that is dry */


      double temp1 = veg_lib[veg_class].Wdmax[month-1];
      *dryFrac = 0;


      *dryFrac = 1.0 - f * pow((tmp_Wdew / veg_lib[veg_class].Wdmax[month-1]),
                          (2.0 / 3.0));
      tmp_Wdew += ppt - canopyevap;
      if (tmp_Wdew < 0.0)
        tmp_Wdew = 0.0;
      if (tmp_Wdew <= veg_lib[veg_class].Wdmax[month-1])
        throughfall += 0.0;
      else {
        throughfall += tmp_Wdew - veg_lib[veg_class].Wdmax[month-1];
        tmp_Wdew = veg_lib[veg_class].Wdmax[month-1];
      }
    }
    tmp_veg_var->canopyevap = canopyevap;
    tmp_veg_var->throughfall = throughfall;
    tmp_veg_var->Wdew = tmp_Wdew;
    tmp_Evap = canopyevap;
    Evap += tmp_Evap * mu / (1000. * delta_t);
  }
  return (Evap);
}
/*170413LML moved to VCS_Nl.c
//160609LML____________________________________________________________________/
#if (VIC_CROPSYST_VERSION>=3)
bool find_irrigation_type(const Irrigation_Types_In_Each_Cell &irrig_lib,
                          const int crop_code, Irrigation_Type &irrigation_type, bool &full_irrigation)
{
  //160609LML may need optimize later!
  bool found = false;
  for (int i = 0; i < MAX_NUM_CROP_TYPES_FOR_IRRIGATION_DEFINE; i++) {
    if (crop_code == irrig_lib.crop_code[i]) {
      found = true;
      irrigation_type = irrig_lib.irrigation_type[i];
      full_irrigation = irrig_lib.full_irrigation[i];
      return found;
    }
  }
  return found;
}
#endif //VIC_CROPSYST_VERSION>=3
*/
