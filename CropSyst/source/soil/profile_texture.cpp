#include "soil/profile_texture.h"
#include "soil/layers_I.h"
#include "soil/soil_param_class.h"
namespace Soil {
//______________________________________________________________________________
Profile_texture::Profile_texture()
: Texture_interface()
{
   clear_layer_array64(sand);
   clear_layer_array64(clay);
   clear_layer_array64(silt);
}
//______________________________________________________________________________
void Profile_texture::set_sand_clay_silt
(nat8  layer, float64 sand_, float64 clay_, float64 silt_)
{  sand[layer] = sand_;
   clay[layer] = clay_;
   silt[layer] = silt_;
}
//______________________________________________________________________________
void Profile_texture::set_sand_clay(nat8 layer,float64 sand_,float64 clay_)
{  float64 silt_ = 100.0 - (sand_ + clay_);                                      //160830
   set_sand_clay_silt(layer,sand_,clay_,silt_);
}
//______________________________________________________________________________
Profile_texture::Profile_texture
(const Layers_interface &layers
,const Soil_parameters_class_common &parameters)
{
   for (nat8 layer = 1; layer <= layers.count(); layer ++)                       //160830
   {
      set_sand_clay_silt
         (layer
         ,parameters.get_horizon_sand(layer)
         ,parameters.get_horizon_clay(layer)
         ,parameters.get_horizon_silt(layer));
   }
}
//_Profile_texture:constructor______________________________________2015-09-26_/
}//_namespace_Soil______________________________________________________________
