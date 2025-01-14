#ifndef residues_interfaceH
#define residues_interfaceH
#include "organic_matter/OM_types.h"
#include "options.h"
#include "corn/const.h"
#include "common/residue/residue_const.h"
#include "N_sources.h"
#include "corn/primitive.h"
#include "common/soil/layering.h"
#include "residue/residue_const.h"
#include "common/evaporator.h"
namespace CORN
{  class Unidirectional_list;
   class Date;
}
//______________________________________________________________________________
namespace Soil                                                                   //181206
{
   class Layers_interface;
   class Soil_interface;
}
class Residue_decomposition_parameters;
class Biomass_decomposition_parameters;
//class Organic_biomatter_common; //190215
//______________________________________________________________________________
interface_  Residues_interface
{
 public:
   virtual bool start_day()                                        modification_ =0;//160523
   virtual bool end_day()                                          modification_ =0;//160628
 public:
   virtual float64 get_pot_evaporation()                                   const = 0;
   virtual float64 get_act_evaporation()                                   const = 0;
   virtual float64 get_interception()                                      const = 0;
   virtual float64 get_curr_water_hold()                                   const = 0;
   virtual float64 get_water_storage()                                     const = 0; //170104
   virtual float64 get_surface_depth()                                     const = 0;
   virtual float64 get_surface_all_biomass()                               const = 0;
   virtual float64 get_surface_plant_biomass(nat32 include_positions)      const = 0; //170107_090311
   virtual float64 get_subsurface_all_biomass()                            const = 0; //060116 plant or manure biomass
   virtual float64 get_subsurface_plant_biomass(float64 *plant_residues = 0)     = 0; //000810
      // Returns the sum of the layers;
      // If plant_residue array is provided, the values will be copied into the array
   virtual float64 get_subsurface_manure_biomass(float64 *manure_residues = 0)   = 0; //000810
      //  Returns the sum of the layers;
      //  If manure_residues array is provided, the values will be copied into the array
   virtual float64 get_total_fract_ground_cover                                  //050726
      (nat32 include_position)                                             const = 0;//060328
   virtual float64 get_total_area_index                                          //060328_041108
      (nat32 include_position )                                            const = 0;
   virtual void add_surface_plant_pool
      (Organic_matter_position position                                          //040517
      ,Organic_matter_cycling _om_cycling                                        //060219
      ,float64 new_surface
      ,float64 new_N_surface_conc
      ,const Residue_decomposition_parameters &residue_decomposition_params      //060219
      )                                                            modification_ = 0;
   virtual void add_subsurface_plant_pool                                        //040517
      (soil_sublayer_array_64(new_subsurface)                                    //970421
      ,float64 new_N_subsurface_conc
      ,const Residue_decomposition_parameters &residue_decomposition_params      //060219
      )                                                            modification_ = 0;
#ifdef NITROGEN
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <=4))
   virtual void add_manure_pool_original
           (Organic_matter_source OM_source
           //Residue_source org_N_source
           //needed? ,nat8    injection_layer                                    //160120
           ,float64 new_shallow_N_content
           ,float64 new_deep_N_content
           ,float64 residue_decomposition_constant
           ,float64 default_carbon_nitrogen_ratio   // Used when we don't have a N mass (or for initial residue)
           ,float64 carbon_fraction
           ,float64 new_area_per_mass )                            modification_ = 0;
#endif
   virtual void add_manure_pool_cycled
      (const Biomass_decomposition_parameters &manure_decomp_params              //020324
      ,nat8    injection_layer                                                   //160120
      ,float64 shallow_N_content      // kg N/m2? Currently goes in (sub)layer 1
      ,float64 deep_N_content        // kg N/m2? currently goes in specified layer //160120 (sub)layer 2
      )                                                            modification_ = 0; //080829
#endif
   virtual float64 water_interception
      (float64 water_entering_residues_m) = 0;
   virtual float64 take_pool_water(float64 pool_water)            appropriation_ = 0; //170104
      // returns the updated water storage
   virtual float64 know_fract_canopy_cover
      (float64 fract_canopy_cover)                                    cognition_ = 0; //160523
      // Eventually provide reference to this in constructor
   virtual float64 evaporate_day()                                 modification_ = 0; //010910
   virtual float64 evaporate_hour(CORN::Hour hour)                 modification_ = 0; //160603
   virtual void balance(bool run_nitrogen)= 0;                                   //990517
   virtual void identify_residue_horizons() = 0;                                 //981001
   virtual void recalibrate_surface_residue                                      //000312
      (float64 surface_residue_param                                             //010202
      ,float64 incorp_residue_param                                              //010202
      ,const Residue_decomposition_parameters &residue_decomposition_params      //060219
      ,float64 residue_N_conc_param
      )                                                            modification_ = 0;
   virtual void redistribute_all
      (float64 fract_surface_to_surface
      ,float64 fract_surface_to_shallow)                           modification_ = 0;
   virtual float64 remove_from_surface
      (float64    fract_to_remove // 0-1
      #if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <=4))
      ,int16      SCS_code
      #endif
      )                                                            modification_ = 0; //060116
   virtual void update_environment()                                             = 0; //040518
   virtual float64 get_biomass_output
      (nat32 include_positions, nat32 include_residue_types)               const = 0; //050116
   virtual CORN::Unidirectional_list &get_pool_list()                            = 0; //060202
 public:
   virtual void decompose_all_without_N()                          modification_ = 0; //040608
   virtual void exhaust_all()                                      modification_ = 0;
   virtual bool clear_all()                                        modification_ = 0;//160126
 public: // observation
   RENDER_INSPECTORS_DECLARATION_PURE;                                               //160627
};
//_2005-07-26___________________________________________________________________
#endif

