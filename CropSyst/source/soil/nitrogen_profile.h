#ifndef nitrogen_profileH
#define nitrogen_profileH

#include "soil/chemical_profile.h"
#include "soil/soil_N_param.h"
#include "model_options.h"
#include "CS_suite/observation/CS_observation.h"
namespace Soil {
//______________________________________________________________________________
class Abiotic_environment_profile;
//______________________________________________________________________________
class NO3_Profile : public Chemical_uptake_profile
{
 public:
   CORN::Dynamic_array<float64>&denitrified_N_output; // kgN/m2 //170502 Need to check if daily or cumulative
 public:
   NO3_Profile
      (const CORN::date32 &today_                                                //170523
      #if (CROPSYST_VERSION == 4)
      ,float64 control_transformation_adjustment
      #endif
      ,soil_layer_array64(simulation_amount_E) // const                          //051120
      /*NYN
      #ifndef OLD_N_XFER
      ,Chemical_pool *receiver_emmission                                         //170502
      #endif
      */
      ,const Soil::Layers_interface               &soil_layers_                  //150925
      ,const Soil::Hydrology_interface            &soil_hydrology_               //150925
      ,const Soil::Hydraulic_properties_interface &soil_hydraulic_properties_    //150925
      ,const Soil::Structure                      &soil_structure_               //150925
      ,Infiltration_model infiltration_model                                     //080117
      ,Water_table_curve *water_table_curve_);                                   //000504
   virtual float64 water_content_correction
      (float64 sublayer_saturation_water_content
      ,float64 sublayer_water_content
      ,float64 sublayer_reference_water_content
      ,float64 sublayer_bulk_density);                                           //000515
   #if (CROPSYST_VERSION==4)
   virtual float64 transformation_rate
      (float64 soil_temperature
      ,float64 water_content_correct);
   #endif
   /*190909 unused
   virtual const char *get_transformation_name();
   */
   virtual float64 get_N_mass(nat8 layer)                                 const; //060503
   inline virtual float64 get_N2O_loss()                   const { return 0.0; } // This version does not produce this. 060607
   inline virtual void know_CO2_C_loss_subsurface(float64 *CO2_loss_subsurf)  {} // Do nothing, this version does not use CO2.
   #ifndef OLD_N_XFER
   inline float64 get_denitrified_kgN_m2(nat8 layer)                       const //170502
      { return layer == 0
         ? denitrified_N_output.sum()
         : denitrified_N_output.get(layer);
      }
   #endif
   RENDER_INSPECTORS_DECLARATION;                                                //150916
};
//______________________________________________________________________________
class NO3_with_CO2_Profile
: public NO3_Profile
{  // This is the denitrification tranformation done hourly with CO2 respiration
 private:
   CropSyst::Denitrification_parameters denitrification_parameters;              //150824
 private:
      bool denitrification_event           [MAX_soil_layers_alloc];
      bool denitrification_event_second_day[MAX_soil_layers_alloc];
 public:
      virtual void setup_for_transformations();                                  //060608
 private:
   float64 N2O_N_loss_daily;  // from denitrification
   Abiotic_environment_profile *abiotic_environment_profile;                     //060510
   float64                *CO2_C_loss_subsurface;
      // pointer to Organic_matter_pools_multiple::CO2_C_loss_subsurface 060504
      // if 0 then use NO3_profile_class transpiration
   Infiltration_model      infiltration_model;                                   //060503
   const float64          &ref_water_flux_in_m;                                  //150925
public:
   NO3_with_CO2_Profile
      (const CORN::date32                 &simdate_                                 //170523
      ,const CropSyst::Denitrification_parameters &_denitrification_parameters   //150824
       #if (CROPSYST_VERSION == 4)
       ,float64                control_transformation_adjustment_
       #endif
       ,soil_layer_array64     (simulation_amount_E)                             //051120
       /*NYN
      #ifndef OLD_N_XFER
      ,Chemical_pool &receiver_emmission                                         //170502
      #endif
      */
      ,const Layers_interface               &soil_layers_                        //150925
      ,const Hydrology_interface            &soil_hydrology_                     //150925
      ,const Hydraulic_properties_interface &soil_hydraulic_properties_          //150925
      ,const Structure                      &soil_structure_                     //150925
      ,Infiltration_model                    infiltration_model_                 //080117
      ,const float64                        &ref_water_flux_in_m_                //150925
      ,Abiotic_environment_profile          *abiotic_environment_profile_        //060510
      ,Water_table_curve                    *water_table_curve_);
   virtual void transformation
      (nat8 sublayer
      ,float64 sublayer_water_content
      ,float64 sublayer_saturation_water_content
      ,float64 sublayer_reference_water_content
      ,float64 sublayer_temperature
      ,float64 sublayer_bulk_density
      ,Seconds preferred_transformation_time_step                                //060510
      // The original transformation time step is 86400 seconds (1 day)
      // When using  Organic_matter_residues_profile_multiple_cycling,
      // the nitrification and denitrification transformation timestep is 1 hour
      // (or the timestep of the infiltration model).
      #ifdef OLD_N_XFER
      ,float64 &transformed_to_M
      #endif
      );
   inline virtual void know_CO2_C_loss_subsurface(float64 *_CO2_loss_subsurface)
      {CO2_C_loss_subsurface = _CO2_loss_subsurface;}
   inline virtual float64 get_N2O_loss()      const { return N2O_N_loss_daily; }
 private:
   virtual float64 moisture_function_daily (nat8 layer,bool for_N2_N2O_ratio)    const;
   virtual float64 moisture_function_hourly(nat8 layer,Hour hour,bool for_N2_N2O_ratio) const;
   float64 calc_N2_to_N2O_respiration_based_potential                            //080828
      (float64 respiration_KgC_ha_day) const;   //  kg C/ha/day  respiration is CO2_C_loss_per_unit_soil_mass
   float64 calc_NO3_based_potential_N2_N20_ratio                                 //080828
      (float64 NO3_N_ppm) const;   // Nitrate content must be in PPM NO3-N
   float64 calc_potential_denitrification_rate(float64 WFP)                const;
   RENDER_INSPECTORS_DECLARATION;                                                //150916
};
//_NO3_with_CO2_Profile_____________________________________________2006-05-03_/
class NH4_Profile : public Chemical_uptake_profile
{
 #ifndef OLD_N_XFER
 public:
   CORN::Dynamic_array<float64>&nitrified_N_output; // kgN/m2                    //170502
 #endif
 public: // 'structor
   NH4_Profile
      (const CORN::date32                 &simdate                               //170523
       #if (CROPSYST_VERSION==4)
       ,float64 i_control_transformation_adjustment
       #endif
      ,soil_layer_array64(simulation_amount_E)
      #ifndef OLD_N_XFER
      ,Chemical_pool &receiver_NO3                                               //170502
      #endif
      ,const Layers_interface               &_soil_layers                        //150925
      ,const Hydrology_interface            &_soil_hydrology                     //150925
      ,const Hydraulic_properties_interface &_soil_hydraulic_properties          //150925
      ,const Structure                      &_soil_structure                     //150925
      ,Infiltration_model infiltration_model                                     //080117
      ,Water_table_curve                *NH4_water_table_curve); // may be 0 if no water table consideration
 public: //
   void volatilized_application
      ( float64 non_volatilized_addition_element //  { <- need convert to molecular in caller }
      , float64 volatilized_addition_element      //  { <- need convert to molecular in caller }
      , uint8   sublayer);
   virtual float64 moisture_function                                             //960802
      (float64 water_filled_porosity                                             //060504
       ,float64 function_value_at_saturation);                                   //060504
   #if (CROPSYST_VERSION==4)
   virtual float64 transformation_rate
      (float64 soil_temperature
      ,float64 water_content_correct);
   #endif
   /*190909 unused
   inline virtual const char *get_transformation_name();
   */
   virtual float64 get_N_mass(nat8 layer)                                 const;   //060503
   inline virtual float64 get_N2O_loss()                   const { return 0.0; } // This version does not produce N2O loss  //090722
   #ifndef OLD_N_XFER
   inline float64 get_nitrified_kgN_m2(nat8 layer)                        const  //170502
      {  return layer == 0
            ? nitrified_N_output.sum()
            : nitrified_N_output.get(layer); }
   #endif
/*190404 Now using Langmuir class
 private:
   inline virtual float64 get_constantK()               const { return 2000.0; } // kg H20/kg chem_solute  In some source equations this is K not C //080115
   inline virtual float64 get_constantQ()               const { return 0.003;  } // kg chem_solute / kg soil  //080115
*/
};
//______________________________________________________________________________
class NH4_Profile_with_pH : public extends_ NH4_Profile
{  // As from the VB version
 private:
   NO3_Profile                      &NO3;  // this new nitrification needs the
   soil_layer_array64(pH_function); // Calculated one time at the beginning of the simulation
   Abiotic_environment_profile *abiotic_environment_profile;  // May 0, if 0 use the old transformation 060510
   float64 N2O_N_loss_daily;  // from nitrification                                //090722
 public:
   NH4_Profile_with_pH
      (const CORN::date32 &today                                                 //170523
      #if (CROPSYST_VERSION==4)
      ,float64             control_transformation_adjustment
      #endif
      ,soil_layer_array64  (simulation_amount_E)
      ,const Layers_interface                 &soil_layers                       //150925
      ,const Hydrology_interface              &soil_hydrology                    //150925
      ,const Hydraulic_properties_interface   &soil_hydraulic_properties         //150925
      ,const Structure                        &soil_structure                    //150925
      ,Infiltration_model infiltration_model                                     //080117
      ,Abiotic_environment_profile            *abiotic_environment_profile       //060510
      ,NO3_Profile                                 &NO3
      ,soil_layer_array64                          (pH)
      ,Water_table_curve                           *NH4_water_table_curve
         // may be 0 if no water table consideration
      ,float64                                      pH_min = 3.5
      ,float64                                      pH_max = 6.5);
public:
   virtual void setup_for_transformations();                                     //060608
   virtual void transformation
      (nat8 sublayer
      ,float64 sublayer_water_content
      ,float64 sublayer_saturation_water_content
      ,float64 sublayer_reference_water_content
      ,float64 sublayer_temperature
      ,float64 sublayer_bulk_density
      ,Seconds preferred_transformation_time_step                                //060510
      // The original transformation time step is 86400 seconds (1 day)
      // When using  Organic_matter_residues_profile_multiple_cycling,
      // the nitrification and denitrification transformation timestep is 1 hour
      // (or the timestep of the infiltration model).
      #ifdef OLD_N_XFER
      ,float64 &transformed_to_M
      #endif
      );
   inline virtual float64 get_N2O_loss()                                   const
      { return N2O_N_loss_daily; }                                               //090722
   RENDER_INSPECTORS_DECLARATION;                                                //150916
};
//_NH4_Profile_with_pH_________________________________________________________/
}//_namespace_Soil_____________________________________________________________/
#endif

