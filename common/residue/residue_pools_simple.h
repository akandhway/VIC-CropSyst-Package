#ifndef residue_pools_simpleH
#define residue_pools_simpleH

// This options.h needs to be for the current program being compiled
// is should not necessarily be /CropSyst/cpp/options.h

// The units of the residue pools have been abstracted
// The instanciation of the residue pools now defines the units.
// When passing soils, new residue pools and otherwise getting
// data make sure that the base units sets of the respective
// objects match.
// I.e.   declare residues pools thusly
//    Residue_pools  residue_pools_kg_m2
//    To indicate that the residue biomass is in kg/m2
//
#include "corn/condcomp.h"
#if defined(unix) || defined(linux)
// UNIX includes works a little different than DOS/Windows includes
#  include "CropSyst/source/options.h"
#else
#  include "options.h"
#endif
#  include "common/residue/residues_common.h"
#  include "corn/container/unilist.h"
#  include "common/biomatter/biomass_abstract.h"
#  ifdef NITROGEN
#     include "mgmt_param.h"
#     include "organic_matter/simple/OM_simple.h"
#  endif
#include "corn/condcomp.h"
#include "common/biomatter/organic_biomatter_balance.h"
//______________________________________________________________________________
namespace Soil
{
// These classes are not units specific,
// They will either work with as mass or mass/area basis

// In CropSyst, the units are  kg/m2

   interface_ Interface;
   interface_ Layers_interface;
   interface_ Hydrology_interface;
   interface_ Hydraulic_properties_interface;
   interface_ Nitrogen;
}
class Organic_matter_V4_2;
class Soil_cropsyst_specific;
class Residue_pool_simple;
extern CORN::statistical::Descriptive_progressive residue_running_balance;       //200701
//______________________________________________________________________________
class Biomatter_residue_V41
: public Biomass_abstract                                                        //101012
{
 public:
   float32 N_content;                                                            //011021
 public:
   Biomatter_residue_V41();
   Biomatter_residue_V41(const Biomatter_residue_V41 &from_copy); // copy constructor //040820
   inline virtual Biomass_abstract  *clone()                               const { return new Biomatter_residue_V41(*this); } //090416
   inline virtual float32 set_biomass(float32 biomass)                           { return update_amount_in_preset_units(biomass);  } //150123
#ifdef NITROGEN
   inline virtual float32 get_N_content()                                  const { return N_content;  }
   inline virtual float32 set_N_content(float32 i_N_content)                     { return N_content = i_N_content ;  }
#endif
   inline virtual float32  get_moisture_percent()                          const {return 0.0; } // 0 because this is dry matter basis.
   void register_variables();
 public: // Biomatter arithmetic
   virtual float64 add(const Physical::Mass &addend)               modification_;//101012
   // If extraction is specified, the current values are not allowed to go negative and the actual amount extracted is set
   virtual float64 multiply_by(float64 multiplier)                 modification_;//101012
   virtual bool clear()                                            modification_;
   inline virtual float64 get_carbon_fraction()                            const { return default_straw_residue_fraction_carbon; } //040517
   inline virtual void set_carbon_fraction(float64 _fraction_carbon)             { /* will be overridden by V4.2*/ }   //060217
           void exhaust();
   inline virtual void set_contact_fraction(float64 i_contact_fraction)          { /* will be overridden by V4.2*/ }   //040818
   inline virtual float64 get_contact_fraction()                           const { return 1.0 ; /* will be overridden by V4.2 */}  //040818
};
//_class_Biomatter_residue_V41_________________________________________________/
class Residue_balancer_V41
{public: // all variables
   Biomatter_residue_V41 BM_applied;                                             //011005
   Biomatter_residue_V41 BM_exhausted;                                           //011005
   Biomatter_residue_V41 BM_removed;                                             //011005
   float64 DM_decomposed_carbon;                                                 //040207
   float64 residue_biomass_balance_yesterday;                                    //040128
public: // Dry matter
#ifdef NITROGEN
 public: // Nitrogen
   float64 N_mineralized;  // total for whole simulation                         //980718
   float64 N_OM;           // sent to organic matter                             //980718
   float64 N_to_atmosphere;                                                      //980908
   float64 N_immobilized;                                                        //040207
   float64 residue_N_balance_yesterday;                                          //040128
#endif
   inline Residue_balancer_V41()
      : BM_applied()                                                             //011005
      , BM_exhausted()                                                           //011005
      , BM_removed()                                                             //011005
      , DM_decomposed_carbon(0.0)                                                //040207
      , residue_biomass_balance_yesterday(0.0)                                   //040125
#ifdef NITROGEN
      , N_mineralized(0.0)
      , N_OM(0.0)
      , N_to_atmosphere(0.0)
      , N_immobilized(0.0)                                                       //040207
      , residue_N_balance_yesterday(0.0)                                         //040128
#endif
      {}
   void removal(const Biomatter_residue_V41 &_BM_removed);
          void add_BM_exhausted(const Biomatter_residue_V41 &_BM_exhausted);     //011005
          void add_BM_applied(const Biomatter_residue_V41 &_BM_applied);         //011005
   inline void add_DM_decomposed_carbon(float64 x)                               { DM_decomposed_carbon += x;}   //040207
#ifdef NITROGEN
   inline void add_N_to_organic_matter(float64 _to_OM)                           { N_OM            += _to_OM;}
   inline void add_N_to_atmosphere(float64 _N_to_atmosphere)                     { N_to_atmosphere += _N_to_atmosphere;}
   inline void add_N_mineralized(float64 _N_mineralized)                         { N_mineralized   += _N_mineralized;}
   inline void add_N_immobilized(float64 _N_immobilized)                         { N_immobilized   += _N_immobilized;} //040207
#endif
   void balance
      ( const Biomatter_residue_V41 &current_BM                                  //011005
      , bool show_balance_errors);                                               //050725
   bool reset();                                                                 //160126
};
//_class_Residue_balancer_V41_______________________________________2000-05-01_/
class Residue_pools_simple
: public Residues_common                                                         //060322
{private:
   friend class Residues_organic_matter;
 protected:
   // references to derived classes
   Biomatter_residue_V41 total_surface_biomatter;    // kg/m2 in CropSyst        //011005
   Biomatter_residue_V41 total_subsurface_biomatter;                             //011005
   Biomatter_residue_V41 total_all_biomatter;                                    //011005
   float64   total_surface_mass[OM_POSITION_COUNT];   // kg/m2                   //041108
   float64   biomass_output    [OM_POSITION_COUNT][RESIDUE_TYPE_COUNT];// kg/m2  //050116
   float64   N_output          [OM_POSITION_COUNT][RESIDUE_TYPE_COUNT];          //050116
   Unidirectional_list pool_list;                                                //971231
 private:
   //190124 float64   total_straw_mineralization;  // kg/m2?
   //190124 float64   total_manure_mineralization; // kg/m2?
   float64  mineralization_total[RESIDUE_TYPE_COUNT]; // kg/m2?                  //190124
   float64   total_N_immobilization;     // kg/m2                                //040209
   Residue_balancer_V41 residue_balancer;                                        //030628
   // Old residue balance, would like to change to the organic_matter_balance
 private: //decomposition is now handled by Residue_and_organic_matter
   Organic_matter_simple *organic_matter_pool;
      // May be 0 if not running nitrogen (but even when no nitrogen can now have OM)
 public: // constructor
   Residue_pools_simple
      (Soil::Soil_interface                     &soil                                  //060122
      ,modifiable_ CORN::Dynamic_array<float64> &evaporation_potential_remaining //160719
      ,const Air_temperature_average      &temperature_air_avg                   //150427
      ,const Wind_speed                   &wind_speed                            //150427
      ,Biomatter_residue_V41 &total_surface_biomatter_ref                        //060202
      ,Biomatter_residue_V41 &total_subsurface_biomatter_ref                     //060202
      ,Biomatter_residue_V41 &total_all_biomatter_ref                            //060202
      ,const CORN::date32                 &today_ref                             //170523
      ,std::ostream  *detail_log = 0); // Optional log file                      //020307
   Residue_pools_simple
      (Soil::Soil_interface                     &soil                                  //040520
      ,modifiable_ CORN::Dynamic_array<float64> &evaporation_potential_remaining //160719
      ,const Air_temperature_average      &temperature_air_avg                   //150427
      ,const Wind_speed                   &wind_speed                            //150427
#ifdef NITROGEN
      ,Organic_matter_simple *organic_matter_pool
#endif
      ,const CORN::date32                 &today_ref                             //170523
      ,std:: ostream  *detail_log = 0); // Optional log file                     //020307
 public:
   virtual void add_surface_plant_pool
      (Organic_matter_position position                                          //040517
      ,Organic_matter_cycling _om_cycling                                        //060219
      ,float64 new_surface
      ,float64 new_N_surface_conc
      ,const Residue_decomposition_parameters &residue_decomposition_params) modification_; //060219
   virtual void add_subsurface_plant_pool
      (soil_sublayer_array_64(new_subsurface)                                    //970421
      ,float64 new_N_subsurface_conc
      ,const Residue_decomposition_parameters &residue_decomposition_params) modification_;  //060219
#ifdef NITROGEN
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
   void add_manure_pool_original
      (//120316 Residue_source i_org_N_source
      Organic_matter_source OM_source                                            //120316
      //needed? ,nat8    injection_layer                                                   //160120
      ,float64 new_shallow_N_content
      ,float64 new_deep_N_content
      ,float64 new_decomposition_constant                                        //060122
      ,float64 default_carbon_nitrogen_ratio   // Used when we don't have a N mass (or for initial residue)   //060225
      ,float64 carbon_fraction                 // around 0.5                    //080225
      ,float64 new_area_per_mass)                                  modification_;
#endif
   inline virtual void add_manure_pool_cycled                                    //080829
      (const Biomass_decomposition_parameters &manure_decomp_params              //020324
      ,nat8    injection_layer                                                   //160120
      ,float64             _shallow_N_content      // kg N/m2? Currently goes in (sub)layer 1
      ,float64             _deep_N_content        // kg N/m2? currently goes in (sub)layer 2
      ) modification_ { }
         // Not applicable to these versions, or at least not yet implemented
#endif
   virtual Residue_pool_simple *create_straw_pool
      (bool standing_stubble                                                     //040517
      , float64 new_surface
      ,soil_sublayer_array_64(new_subsurface)                                    //970421P
#if (CROPSYST_VERSION < 5)
      ,float64 new_water_hold     // m3/kg
#endif
      ,float64 new_N_surface_conc
      ,float64 new_N_subsurface_conc
      ,float64 new_decomposition_constant                                        //060122
      ,float64 _default_carbon_nitrogen_ratio   // Used when we don't have a N mass (or for initial residue) //060225
      ,float64 _residue_carbon_fraction                                          //060227
      ,float64 new_area_per_mass
      ,nat8    horizons[]
      ,Soil::Soil_interface &soil);

#ifdef NITROGEN
   virtual Residue_pool_simple *create_manure_pool
      (//120316 Residue_source _org_N_source                                              //020324
      Organic_matter_source OM_source
      ,float64 _shallow_N_content
      ,float64 _deep_N_content
      ,float64 _decomposition_constant                                           //060122
      ,float64 _default_carbon_nitrogen_ratio   // Used when we don't have a N mass (or for initial residue)   //060225
      ,float64 _carbon_fraction_UNUSED // Not used in this model                 //080225
      ,float64 _area_per_mass
      ,nat8 horizons[]);
#endif

   virtual void update_environment()                                             //040518
      {  /* this is currently only for V4_2 */ }
   void process_all();
#ifdef NITROGEN
//050118 mineralization and immobilization is now done only in organic_matter class
   inline float64   get_mineralization_total(Organic_matter_type type)     const { return mineralization_total[type];  } // kg/m2? //190124_040525
   inline float64   get_total_N_immobilization()       const { return total_N_immobilization;      } // kg/m2  //040525
#endif
   void decompose_all();
   // We do not have decompose_all in V4.2 because there it
   // is handled by the residue organic matter

   virtual void redistribute_all
      (float64 fract_surface_to_surface
      ,float64 fract_surface_to_shallow);
   virtual float64 remove_from_surface                                           //060116
      (float64    fract_to_remove // 0-1
      #if ((CROPSYST_VERSION >0) && (CROPSYST_VERSION <= 4))
      // will be obsolete in version 5                                           //070627
      ,int16      SCS_code
      #endif
      );
   virtual void balance(bool run_nitrogen);                                      //990517
   void recalibrate_surface_residue                                              //000312
      (float64 surface_residue_param                                             //010202
      ,float64 incorp_residue_param                                              //010202
      ,const Residue_decomposition_parameters &residue_decomposition_params      //060219
      ,float64 residue_N_conc_param);
   float64 sum_carbon_to_sublayer_kg_m2(nat8 to_sublayer)                 const; //040206
   float64 sum_carbon_surface()                                           const; //060124
 public: // Accessors from Residue abstract
   virtual float64 get_biomass_output                                            //050116
      (nat32 include_positions, nat32 include_residue_types)              const;
   inline virtual float64 get_surface_all_biomass()                       const  { return total_surface_biomatter.kg(); }       //061103
   inline virtual float64 get_surface_plant_biomass(nat32 include_positions ) const //170107
      { return total_surface_biomatter.kg(); }       //090311

      //170107 Warning simple doesn't yet have suface biomatter distiguished between flat and stubble
      // need to add this .

      // this version only had plant residue, not manure
   virtual float64 get_subsurface_plant_biomass(soil_sublayer_array_64(plant_residues) = 0);                         //000810
      // Returns the sum of the layers;
      // If plant_residue array is provided, the values will be copied into the array
   virtual float64 get_subsurface_manure_biomass(soil_sublayer_array_64(manure_residues) = 0);                       //000810
      // Returns the sum of the layers;
      // If manure_residues array is provided, the values will be copied into the array
 public: // Accessors                                                                                                //990216
   inline virtual float64 get_total_surface_biomass()          const { return total_surface_biomatter.kg(); }        //011011
   // get_total_surface_biomass is depricated now use get_surface_all_biomass()
   inline virtual float64 get_total_subsurface_biomass()       const { return total_subsurface_biomatter.kg() ; }    //011011
   inline virtual float64 get_subsurface_all_biomass()         const { return total_subsurface_biomatter.kg() ; }    //060116
   virtual float64 get_N_output                                                                                      //060328
      (nat32 include_positions, nat32 include_types) const;                                                        //060328
 protected:
      virtual void update_totals_all()                            modification_;
      virtual void update_totals_sum_pools()                      modification_;
      virtual bool clear_totals()                                 modification_; //060228
      virtual bool clear_all()                                    modification_; //160126
 public: // now public so residue_and_organic_matter_profile can use             //040517
      void exhaust_all()                                          modification_;
 public:
   inline virtual Unidirectional_list &get_pool_list()      {return pool_list;}  //060202
 private:
   inline virtual void decompose_all_without_N()                             {}  //040608
        // I think this only applies to V4.2, but need to check
   inline virtual void debug_put_everything_in_layer2()                      {}
};
//_class_Residue_pools_simple__________________________________________________/
#endif
//060721 708 lines
//060730 423 lines
//070627 380 lines

