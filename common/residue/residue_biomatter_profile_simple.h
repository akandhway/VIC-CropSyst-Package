#ifndef residue_biomatter_profile_simpleH
#define residue_biomatter_profile_simpleH
//______________________________________________________________________________
#include "common/soil/layering.h"
#if defined(unix) || defined(linux)
// UNIX includes works a little different than DOS/Windows includes
#  include "CropSyst/source/options.h"
#else
#  include "options.h"
#endif
#include "corn/const.h"
#include "common/residue/residue_const.h"
#include "soil/layers_I.h"
class Biomatter_residue_V41;                                                     //090416

//______________________________________________________________________________
class Residue_biomatter_profile_simple   // This is used only in version 4.1
{  bool      standing_stubble;
   mutable Biomatter_residue_V41 *surface;  // Index 0 is the surface horizon
   mutable Biomatter_residue_V41 *subsurface[MAX_soil_sublayers_alloc];
           Biomatter_residue_V41 *initial;                                       //060215
   const Soil::Layers_interface &soil_layers;                                    //150424
public:
   Residue_biomatter_profile_simple
      (const Soil::Layers_interface &soil_layers);                               //150424
   Residue_biomatter_profile_simple
      (const Biomatter_residue_V41  subsurface[]
      ,nat8 horizons[]
      ,const Soil::Layers_interface &soil_layers);                               //150424

      ///< This constructor is more general and used by DairySyst
   Residue_biomatter_profile_simple
      (bool    standing_stubble
      //200220 ,float64 decomposition_time_63_deprecated                                  //040802
      ,float64 halflife                                                          //040826
      ,float64 surface_dry_weight
      ,soil_sublayer_array_64(subsurface_dry_weight)
#ifdef NITROGEN
      ,float64 i_surface_N_content
      ,soil_sublayer_array_64(i_subsurface_N_content)
#endif
      ,nat8 horizons[]
      ,const Soil::Layers_interface &_soil_layers);                              //150424

      /// This constructure is CropSyst specific
      //040517 check where used, might not need standing stubble parameter
   virtual ~Residue_biomatter_profile_simple();                                  //060202
   virtual void initialize
      (bool    _standing_stubble                                                 //040517
      ,float64 _decomposition_constant                                           //060122
      ,float64  _default_carbon_nitrogen_ratio                                   //060225
      // Used when we don't have a N mass (or for initial residue)
      ,float64             _residue_carbon_fraction                              //060227
      ,soil_sublayer_array_64(i_subsurface_dry_weight)
#ifdef NITROGEN
      ,soil_sublayer_array_64(i_subsurface_N_content)
#endif
      ,nat8 horizons[]);
      ///< This initialize is CropSyst specific //011011
   void redistribute
      (float64 fract_shallow_to_deep
      ,float64 fract_surface_to_deep
      ,float64 fract_surface_to_shallow
      ,float64 shallow_horizon_thickness                                         //990217
      ,float64 deep_horizon_thickness                                            //990217
      ,const nat8 horizons[]);
   float64 totalize(Biomatter_residue_V41 &total,Residue_horizons horizon
      , const nat8 horizons[], float64 &N_this_pool)                       const;//050116
   Biomatter_residue_V41 &sum_subsurface_to_layer
      (Biomatter_residue_V41 &total,nat8 to_sublayer = 0)                  const;//011005
   float64 get_surface_mass()                                              const;//011004
   float64 set_surface_mass(float64 new_surface_mass)              modification_;//020324
   float64 set_subsurface_mass(nat8 sublayer, float64 new_surface_mass) modification_;//020324
   float64 get_subsurface_mass(nat8 sublayer)                              const;//011004
#ifdef NITROGEN
   float64 get_surface_N_content()                                         const;//011004
   float64 get_subsurface_N_content(nat8 sublayer)                         const;//011004
   float64 set_surface_N_content(float64 new_surface_N_content)    modification_;//020324
   float64 set_subsurface_N_content
      (nat8 sublayer, float64 new_subsurface_N_content)            modification_;//011004
#endif
   void exhaust();                                                               //040523
 private:
   bool no_such_horizon
      (Residue_horizons search_for_horizon,const nat8 horizons[])          const;//981018
 public:                                                                         //011005
   inline bool is_standing_stubble()          const { return standing_stubble; } //041108
   inline virtual Biomatter_residue_V41 *create_biomatter_residue                //060202
      ( float64 _carbon_fraction = default_straw_residue_fraction_carbon) const;
   virtual Biomatter_residue_V41 *create_biomatter_residue                       //060214
      (const Biomatter_residue_V41 &from_copy)                            const;
   virtual Biomatter_residue_V41 *provide_biomatter_residue(nat8 layer) provision_;//060214
   inline virtual Biomatter_residue_V41 &get_initial()      { return *initial; } //060215
   inline virtual Biomatter_residue_V41 *mod_surface()             modification_ //060214
      { return surface; }
   inline virtual const     Biomatter_residue_V41 *get_surface_constant()  const //060214
      { return surface; }
   inline virtual modification_ Biomatter_residue_V41 *mod_subsurface
      (nat8 layer)                  modification_ { return subsurface[layer]; }  //060214
   inline virtual const Biomatter_residue_V41 *get_subsurface(nat8 layer)  const
      { return subsurface[layer]; }                                              //060214
   virtual bool delete_subsurface(nat8 layer);                                   //060214
};
//_Residue_biomatter_profile_simple_________________________________2001-10-04_/
#endif
