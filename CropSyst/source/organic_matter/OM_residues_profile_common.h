#ifndef OM_residues_profile_commonH
#define OM_residues_profile_commonH
#include "organic_matter/OM_residues_profile_abstract.h"
#include "common/residue/residues_cycling.h"
#include "CropSyst/source/organic_matter/OM_pools_common.h"                               //201129LML

//______________________________________________________________________________
namespace Soil { class Abiotic_environment_profile; }                            //181206
class Organic_matter_pools_common;
class Organic_matter_initialization_parameters;                                  //111004
class Organic_matter_decomposition_parameters;                                   //111004
class Air_temperature_average;                                                   //140507
//______________________________________________________________________________
class Organic_matter_residues_profile_common    // rename because this is a concrete intantiable class
: public Organic_matter_residues_profile_abstract
{protected:                                                                      //070707
   const Organic_matter_decomposition_parameters &organic_matter_decomposition_parameters; //120912
   Soil::Abiotic_environment_profile &soil_abiotic_environment_profile;           //060216
      // must be listed first
   Organic_matter_pools_common   *organic_matter_pools_profile_owned;            //070707
   Residues_cycling               residues;                                      //111004
 public:
   Organic_matter_residues_profile_common                                        //070707
      (Soil::Soil_interface                     *soil
      ,Soil::Abiotic_environment_profile   &soil_abiotic_environment_profile      //160412
      ,modifiable_ CORN::Dynamic_array<float64> &evaporation_potential_remaining_ //160717
      ,const Air_temperature_average      &temperature_air_avg
      ,const Wind_speed                   &wind_speed                            //150427

      ,soil_horizon_array32               (horizon_percent_organic_matter)       //111004

      ,Organic_matter_pools_common        *organic_matter_pools_profile_given    //111004
      ,const Organic_matter_initialization_parameters &organic_matter_initialization_parameters  //111004_060215
      ,const Organic_matter_decomposition_parameters  &organic_matter_decomposition_parameters  //111004_060215
      ,const CORN::date32                 &today);                               //140507
   inline virtual ~Organic_matter_residues_profile_common() {
       organic_matter_pools_profile_owned->delete_all();
       delete organic_matter_pools_profile_owned;                                //201128LML
   }
 public: // process
   virtual bool process_day();                                                   //150611_140507_060116
   virtual bool start_day()                                       modification_; //060817
   //181214 virtual bool start_year()                                      modification_; //070510
 public: // The following are for the Nitrogen model return 0 if not simulating nitrogen
   virtual float64 get_N_immobilization_profile()                         const;
   virtual float64 get_soil_organic_matter_mineralization_profile_N()     const;
   virtual float64 get_soil_organic_matter_immobilization_profile_N()     const; //110918
   virtual float64 get_soil_organic_matter_mineralization_N(nat8 layer)   const;
   virtual float64 sum_carbon_to_depth_kg_m2
      (nat8 include_OM
      ,float64 to_depth_m   // To depth of soil  defaults to the entire profile
      ,bool include_surface )                                             const;
   #if (defined(MICROBASIN_VERSION) || defined (VIC_CROPSYST_VERSION))
   virtual float64 sum_nitrogen_to_depth_kg_m2
      (nat8 include_OM
      ,float64 to_depth_m   // To depth of soil  defaults to the entire profile
      ,bool include_surface )                                             const; //150722LML
   #endif
   float64 sum_N_mineralization_to_depth                    //090520 Need to check why this is not virtual
      (nat8 include_OM
      ,float64 to_depth_m=ENTIRE_PROFILE_DEPTH_as_0    // To depth of soil  defaults to the entire profile
      ,bool include_surface=false )                                       const;//061116
   float64 sum_N_immobilization_to_depth
      (nat8 include_OM                                     // 090520 need to check why this is not virtual
      ,float64 to_depth_m =ENTIRE_PROFILE_DEPTH_as_0   // To depth of soil  defaults to the entire profile
      ,bool include_surface=false )                                       const;//061116
   virtual float64 sum_C_decomposition_to_depth
      (nat8 include_OM
      ,float64 to_depth_of_soil_m)                                        const;//090520
 public: // The following are depricated in this model we only have mineralization from organic matter
   inline virtual float64 get_total_straw_mineralization()                const  { return 0; }
   inline virtual float64 get_total_manure_mineralization()               const  { return 0; }
   virtual float64 get_NH4_mineralization()                               const;
 public: // Event processing
   virtual bool respond_to_field_operation                                       //060726
      (float32 tillage_op_depth_m
      ,float32 tillage_op_mixing_fraction
      ,USDA::NRCS::RUSLE2::Soil_disturbance_effect   operation_soil_disturbance_effect                           // VB Operation_Soil_Disturbance_Effect
      ,const Soil::Disturbance          *_soil_disturbance
      ,const float32 residue_flatten_ratio[RUSLE2_RESIDUE_TYPE_COUNT]
      ,const float32 residue_burial_ratio [RUSLE2_RESIDUE_TYPE_COUNT]) modification_;//060807
   virtual const Soil::Disturbance *know_soil_disturbance
      (const Soil::Disturbance *_soil_disturbance)                modification_; //070801
 protected: // accessors
   virtual float64 get_organic_matter_mass(nat8 layer)                    const;//060401
   virtual Pot_mineralizable_N_profile *get_pot_mineralizable_N_profile() ;
      // get_pot_mineralizable_N_profile is applies only to V 4.1.xx
      // everything else can simply return 0;
   virtual float64
      get_mass_from_layer_to_layer
      (Mass_total mass_total
      ,nat32   include_organic_matter_types
      ,nat8    from_layer
      ,nat8    to_layer
      ,float64 prorate_fraction_bottom_layer)                             const; //071214
   inline virtual const Residues_interface *ref_residues()                const  //160627
      { return &residues; }
   inline virtual Residues_interface *mod_residues()               modification_ //160627
      { return &residues; }
 public: // carbon accessors
   virtual float64 get_organic_carbon
      (nat8 layer, nat8 include_OM )                                      const;
   virtual float64 get_gaseous_CO2_C_loss_reported_daily
      (nat8 include_OM)                                                   const; //060426
   virtual float64 *get_CO2_C_loss_profile_residue_and_OM();                     //060504
   virtual float64 *get_CO2_C_loss_profile_residue();                            //080902
   virtual float64 *get_CO2_C_loss_profile_OM();                                 //080902
   virtual float64 get_C_decomposed_from_residue                                 //060817
      (Organic_matter_position from_position)                             const;
   virtual float64 get_C_incorporated_by_tillage()                        const; //060817
   #ifdef RECALIB_OM
   bool know_organic_matter_pools(Organic_matter_pools_common *_organic_matter_pools_profile);
   #endif
 protected: // Moved from Organic_matter_residues_profile_multiple_cycling       //111004
   virtual bool initialize                                                       //111004_060216
      (soil_horizon_array32(percent_organic_matter)
      ,const Organic_matter_initialization_parameters &_OM_init_parameters);     //060215
   virtual bool clear_residues()                                  modification_; //160126
   virtual bool write(std::ostream &)                                stream_IO_;
   virtual bool read (std::istream &stream)                          stream_IO_; //140213_120912
   RENDER_INSPECTORS_DECLARATION;                                                //150916
 private:
/*
  inline  Organic_matter_residues_profile_common *abstract_check
      (Soil_interface                     *_soil
      ,Soil_abiotic_environment_profile   &_soil_abiotic_environment_profile
      ,const Air_temperature_average      &_temperature_air_avg
      ,const Wind_speed                   &_wind_speed                           //150427
      ,soil_horizon_array32               (_horizon_percent_organic_matter)      //111004
      ,Organic_matter_pools_common        *_organic_matter_pools_profile_given   //111004
      ,const Organic_matter_initialization_parameters  &_organic_matter_initialization_parameters  //111004_060215
      ,const Organic_matter_decomposition_parameters  &_organic_matter_decomposition_parameters  //111004_060215
      ,const CORN::Date                   &_today)                              //140507
     { return new
      Organic_matter_residues_profile_common                                        //070707
      (_soil
      ,_soil_abiotic_environment_profile
      ,_temperature_air_avg
      ,_wind_speed                           //150427
      ,_horizon_percent_organic_matter      //111004
      ,_organic_matter_pools_profile_given   //111004
      ,_organic_matter_initialization_parameters  //111004_060215
      ,_organic_matter_decomposition_parameters  //111004_060215
      ,_today);                              //140507
   }
*/
};
//_Organic_matter_residues_profile_common___________________________2005-07-26_/
#endif

