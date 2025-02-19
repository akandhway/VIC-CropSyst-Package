#ifndef crop_N_commonH
#define crop_N_commonH
#include "options.h"

#include "corn/math/statistical/stats.h"
#include "crop/crop_cropsyst.h"                                                  //170220LML
#include "crop/crop_N_interface.h"
#include "crop/crop_param.h"
#include "CS_suite/simulation/CS_simulation_element.h"
#include "CS_suite/observation/CS_observation.h"

//#define OLD_N_STORED

//Forward declarations__________________________________________________________
namespace Soil {
class Soil_interface;                                                            //050721
class Nitrogen;                                                                  //181206_050721
}
class Slope_saturated_vapor_pressure_plus_psychrometric_resistance;
namespace CropSyst
{
   #ifdef DEBUG_CROP_N_FATE
   extern  bool N_fate_headers_dumped; // =false;
   #endif
}
//Forward declarations_________________________________________________________/
namespace CropSyst
{
//______________________________________________________________________________
class Crop_nitrogen_common
: public implements_ Crop_nitrogen_interface                                     //150601_050722
{protected:
   class Amounts     // This should become obsolete, replaced by balancer        //061213
   {  // all units are in (kg N)/(m� soil)                                       //061213
   public:                                                                       //061213
      float64 uptake;                                                            //070209
      float64 fixation;       //                                                 //960808
      float64 gaseous_loss;   // N loss to atmosphere from plant
      float64 removed;        // removed as a result of trimming                 //970107
      float64 fertilization_requirements;
         // for automatic N allocated directly to tissues mode
   public: //                                                                    //061213
      Amounts();                                                                 //061213
      void clear()                                         modification_;        //061213
      void accumulate(const Amounts &augend)               modification_;        //061213
   };                                                                            //061213
   //________________________________________________________Amounts
 protected:
   Crop_complete         &crop;                                                  //020505
   const Crop_parameters &crop_parameters;                                       //020409
   Crop_parameters_struct::Nitrogen  &parameters;                                //110218_070209
   Photosynthetic_pathway photosynthetic_pathway;                                //110517
 private:
   Soil::Soil_interface             *soil_properties;    // Optional. If not specified, we assum full water content (no limitations) 050721_
   Soil::Nitrogen /*Soil_nitrogen_interface*/    *soil_chemicals;     // Optional. If not specified, then nitrogen is not simulated 050721_
 protected:
   const Slope_saturated_vapor_pressure_plus_psychrometric_resistance            //referenced
      *slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference;   //150601
   #if ((PHENOLOGY_VERSION==2018))
   const float64 &accum_deg_days_adjusted_for_clipping;                          //181118
   #endif
 public: // State
   float64 N_limited_growth;                                                     //151105
   float64 N_limited_pot_transpiration;                                          //151106
      // adjusted when CO2 is enabled otherwise unadjusted
   float64 stress_factor;                                                        //151031
   float64 canopy_resistance_under_N_stress;                                     //150601
      // RLN now member so can be referenced by
      // slope_saturated_vapor_pressure_plus_psychrometric_resistance_stressed

   Amounts daily;                                                                //061213
   Amounts seasonal;                // This is usually used to get seasonal totals at the end of the season, during the season, this is to-date 061213_
   float64 seedling_N; // (kg N)/(m� soil)                                       //960802

   float64 reported_uptake_accum;                                                //160628
   float64 reported_canopy_mass_current;                                         //160628
   float64 reported_root_conc;                                                   //160629

   float64 canopy_concentration_vital; // kg N/kg biomass      rename this to canopy_concentration
   float64 root_concentration_vital     ; // kg N/kg biomass
   float64 canopy_concentration_effete  ; // kg N/kg biomass      rename this to canopy_concentration 070321
   float64 root_concentration_effete    ; // kg N/kg biomass                     //070321

   float64 root_N_mass_vital;    // kg/m�
   float64 canopy_N_mass_vital;  // kg/m�  total N content in the plant          //070326
   float64 root_N_mass_effete;   // kg/m�                                        //070321
   float64 canopy_N_mass_effete; // kg/m�  total N content in the plant          //070326
   float64 store_persistent;     // kg/m�
   float64 max_N_uptake_per_unit_root_length; // kg/m root/day                   //061129
   Crop_mass_fate  *N_fate_today;// kg N/^2 soil N removed due to various operations (may be 0 if no fate processed processed) may be sum of multiple operations today. 040625_
   mutable CORN::statistical::Sample_cumulative<float32,float32>
      stress_factor_stats;                  //011117
//   mutable CORN::statistical::Sample_cumulative<float32,float32>stress_factor_stats; //160803
   bool automatic_nitrogen_to_plant; // parameter option // was automatic_nitrogen               //160329

 private: // Uptake rates and adjustments
   float64 water_availability_coef;                                              //070209
   float64 NO3_N_availability_adj;                                               //070209
   float64 NH4_N_availability_adj;                                               //070209
 private: // These concentrations always apply to vital only
   float64 N_max_conc_at_change ;   // N max conc. at begin of flowering (was grain filling at one time)//960119
   float64 N_crit_conc_at_change ;  // N crit conc. at begin of (was grain filling at one time)         //960119
   float64 N_min_conc_at_change ;   // N min conc. at begin of (was grain filling at one time)          //960119
   float64 N_max_conc_at_maturity;                                               //070129
   float64 N_crit_conc_at_maturity;                                              //070129
   float64 N_min_conc_at_maturity;                                               //070129
 private: // These are today's state values they used to be local to nitrogen_depend_growth
        // and were recomputed every day, but we find                            //041206
        // that in some instances the calculated values cannot be                //041206
        // calculated for a day, so we simply preserve the previous day's values //041206
   float64 N_max_concentration ;    // these are vital only                      //041206
   float64 N_crit_concentration;                                                 //041206
   float64 N_min_concentration ;                                                 //041206
 protected: // Added for new find_concentration_range
   float64 N_max_conc_at_emergence;   // vital only                              //070202
   float64 concentration_slope;                                                  //070202
   float64 N_crit_conc_at_emergence;                                             //070202
   float64 N_min_conc_at_emergence;                                              //070202
   bool    before_change;                                                        //130406
   bool    start_change_today;                                                   //070126_121221(was start_flowering_today)
/*181118 unused
 private:
   bool    is_in_senescence;                                                     //121221
*/
 private:  // The following are output to the N parameter calibration report
   bool    after_start_of_maturity;                                              //061206
 private: // Crop nitrogen balance
   mutable Crop_nitrogen_mass_balancer *mass_balance;  // Optional  //owned by simulation 070219_
 public:
   Crop_nitrogen_common
      (Crop_parameters::Nitrogen &_parameters
      ,Crop_complete             &_crop                                          //050721
      ,Soil::Soil_interface      *_soil                                          //181206_050721
      ,Soil::Nitrogen            *_chemicals                                     //181206_050721
      ,const Slope_saturated_vapor_pressure_plus_psychrometric_resistance        //150601
         *_slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference//150601
         // although _slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference
         // is optional, if it is not specified, there will be no correction for transpiration
      #if ((PHENOLOGY_VERSION==2018))
      ,const float64 &accum_deg_days_adjusted_for_clipping_                      //181118
      #endif
      );
   virtual ~Crop_nitrogen_common();
 public: // Simulation_element implementations
   virtual bool initialize()                                     initialization_;//150601
   virtual bool start()                                           modification_ { return true; } //150601
   virtual bool start_year()                                      modification_ { return true; } //150601
   virtual bool start_day()                                       modification_;//040206
   virtual bool process_day()                                     modification_ { return true; } //150611_150601
   virtual bool end_day()                                         modification_;//061214
   virtual bool end_year()                                        modification_ { return true; } //150601
   virtual bool stop()                                            modification_ { return true; } //150601

 public: // processing
   inline virtual bool track_mass_balance                                        //070220
      (Crop_nitrogen_mass_balancer *_mass_balance)                      mutates_ {mass_balance = _mass_balance; return true;}
   virtual void restart
      (float64 initial_biomass
      ,float64 restart_GAI                                                       //070320
      ,bool use_clumping_factor
      , bool after_dormancy)                                      modification_;
   virtual bool start_season()                                    modification_; //061214
   virtual void start_dormancy()                                  modification_; //070327
   virtual Crop_mass_fate *render_N_fate
      (Biomass_fated_cause cause,float64 canopy_before
      ,float64 root_before)                                          rendition_; //131005
   virtual Crop_mass_fate     *apportion_fate                                    //070625
      (const Crop_mass_fate   &biomass_fate_now
      ,Crop_mass_fate *N_fate_now                                                //131005
      ,const Crop_biomass     &canopy_biomass_removed                            //120514
      ,bool plant_part_N_concentrations_specified
      ,nat8 vital_or_effete )                                     modification_;
   virtual float64 process_any_gaseous_losses()                   modification_; //070202
   /*181118 unused
   virtual void know_start_senescence()                           modification_; //121221
   */
   float64 nitrogen_depend_growth
      (float64 root_activity
      ,float64 daily_crop_root_mass
      ,float64 daily_crop_root_growth_pot
      ,float64 daily_crop_top_growth_pot         //<- mustn't be 0! }
      ,float64 FND)                                               modification_;
   virtual float64 update_limited_growth                                         //151030
      (float64 attainable_growth_transp_or_RUE
      ,float64 root_activity_factor)                              modification_;
 public: // operations
   virtual float64 remove_canopy_nitrogen_for                                    //070412
      (float64 respective_biomass,nat8 include_vital_or_effete);
 private:
   void find_concentration_range();                                              //070202
   void /*180822 float64*/ compute_dilution()                      computation_; //121221
 public: // accessors
   inline virtual float64 get_limited_pot_transpiration()                 const { return N_limited_pot_transpiration; }
   inline virtual float64 get_limited_growth()                            const { return N_limited_growth; } //151105
   inline virtual void set_automatic_mode(bool enable)            modification_ { automatic_nitrogen_to_plant = enable; } //160329_050722
   inline virtual bool get_automatic_mode()                               const { return automatic_nitrogen_to_plant; }   //160329_050722
   virtual float64 get_root_concentration(nat8 vital_effete_or_dead)      const; //070327
   virtual float64 get_canopy_concentration(nat8 vital_effete_or_dead)    const;
//NYI/*07032__*/    inline virtual float64 get_canopy_concentration_effete() const { return canopy_concentration_effete; };
//NYI/*070131_*/    inline virtual float64 get_root_concentration_vital()    const { return root_concentration_vital; };
//NYI/*070321_*/    inline virtual float64 get_root_concentration_effete()   const { return root_concentration_effete; };
   inline virtual float64 get_concentration_min()                         const { return N_min_concentration; }                 //070126
   inline virtual float64 get_concentration_max()                         const { return N_max_concentration; }                 //070126
   inline virtual float64 get_concentration_critical()                    const { return N_crit_concentration; }                //070126
          virtual float64 get_residue_concentration()                     const;                                                //061215
          virtual float64 get_expected_root_concentration()               const;                                                //061215
   inline virtual float64 get_stress()                                    const { return stress_factor_stats.get_current(); }   //050722
      // rename to get_stress_factor()
   inline virtual float32 &ref_stress()                                   const { return stress_factor_stats.ref_current(); }   //060929
      // rename to ref_stress_factor
   inline virtual float64 get_stress_index()                              const { return 1.0- stress_factor_stats.get_current(); } //160803 { return stress_factor_stats.index ; }
   inline virtual const Crop_mass_fate *get_fate()                        const { return N_fate_today; }                        //040625
   inline virtual float64 get_mass_in_plant()                             const { return root_N_mass_vital + canopy_N_mass_vital + root_N_mass_effete + canopy_N_mass_effete ; }                //070327
   inline virtual float64 get_mass_of_seedling()                          const { return seedling_N; }                          //050722
   inline virtual float64 get_mass_removed_seasonal()                     const { return seasonal.removed;}                     //061214
   inline virtual float64 get_gaseous_loss()                              const { return daily.gaseous_loss; }                  //050722
   inline virtual float64 get_fertilization_requirements()                const { return daily.fertilization_requirements; }    //050722
   inline virtual float64 get_fixation()                                  const { return daily.fixation ; }
   inline virtual const Crop_mass_fate *get_N_fate()                      const { return N_fate_today; }                        //040625
   inline virtual float64 get_stress_index_mean()                         const { return 1.0 - (float64)stress_factor_stats.get_mean(); }

   #if (CROPSYST_VERSION == 4)
   inline virtual float64 get_mass_canopy_current()                       const { return canopy_N_mass_vital+canopy_N_mass_effete; }
   #else
   inline virtual float64 get_mass_canopy_current()                       const { return reported_canopy_mass_current; }
   #endif
   inline virtual float64 get_mass_canopy_production()                    const { return canopy_N_mass_vital+canopy_N_mass_effete + get_mass_removed_seasonal(); }
   inline virtual float64 get_plant_N_kg_m2()                             const { return root_N_mass_vital + canopy_N_mass_vital + root_N_mass_effete + canopy_N_mass_effete;  }  //070327
   virtual float64 get_N_max_conc_at_emergence()                          const; //061212
   #ifdef DILUTION_CURVE_BASED_ON_N_MAX_CONC
   #else
   virtual float64 get_N_crit_conc_at_emergence()                         const; //121219
   #endif
 private:
   float64 calc_max_N_uptake_per_unit_root_length(float64 total_root_lengths)const;//080723
   bool allocate_available_to_plant_part_as_demanded
      (float64 &to_be_allocated
      ,float64 &plant_part_N_mass
      ,float64 &plant_part_N_demand)                                      const; //070131
   float64 Fract_N_demand();
   float64 find_plant_demand
      (float64 daily_crop_top_growth_pot
      ,float64 daily_crop_root_growth_pot
      ,float64 FND
      ,float64 &top_N_demand
      ,float64 &root_N_demand);
   float64 find_pot_uptake                                                       //061129
      (soil_sublayer_array_64(N_NO3_pot_uptake)  //returned (kg N) / (m^2 soil) elemental
      ,soil_sublayer_array_64(N_NH4_pot_uptake)  //returned (kg N) / (m^2 soil) elemental
      ,float64 root_activity
      ) const;      // kg N / m^2 soil
   float64 find_act_uptake
      (float64 pot_N_uptake
      ,float64 crop_N_demand
      ,float64 canopy_N_demand
      ,float64 root_N_demand
      ,soil_sublayer_array_64(N_NO3_pot_uptake)
      ,soil_sublayer_array_64(N_NH4_pot_uptake)
      ,float64 &N_uptake);
   float64 find_actual_growth(float64 pot_new_growth);
 public: // Accessors used for crop nitrogen parameter calibration.
   // These functions should only be called after the end of the season. //061213
   inline virtual float64 get_max_expected_concentration_at_change()      const { return N_max_conc_at_change; }    //061213
   inline virtual float64 get_max_expected_concentration_at_maturity()    const { return N_max_conc_at_maturity; }  //061213
   inline virtual float64 get_season_uptake()                             const { return seasonal.uptake; }         //070209
   inline virtual float64 get_daily_uptake()                              const { return daily.uptake; }            //150723LML
          virtual float64 calc_uptake_excess_or_deficit()                 const; //061213
            // Returns negative if deficit.
            // Normally should be near 0 or slightly positive.
   virtual float64 calc_leaf_stress_factor()                       calculation_; //200529_070224
      //200529 (Nitrogen_leaf_stress_mode calc_mode)                      const; //070224
   RENDER_INSPECTORS_DECLARATION;
//NYI   RENDER_MODIFIERS_DECLARATION;                                            //181015
};
//_Crop_nitrogen_common________________________________________________________/
}//_namespace_CropSyst_________________________________________________________/
#endif

