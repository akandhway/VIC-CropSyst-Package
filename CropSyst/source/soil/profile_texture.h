#ifndef profile_textureH
#define profile_textureH
#include "soil/texture_I.h"
#include "soil/layering.h"

class Soil_parameters_class_common;
namespace Soil {                                                                 //181206
class Layers_interface;                                                          //181206
//______________________________________________________________________________
class Profile_texture
: public Texture_interface
{
protected:
   soil_layer_array64(sand);
   soil_layer_array64(clay);
   soil_layer_array64(silt);
public:
   Profile_texture();
   Profile_texture                                                          //150926
      (const Layers_interface &_layers
      ,const Soil_parameters_class_common &parameters);
      // This constructure is used by scrutinize infiltration
      // where the soil layers match the soil horizon
public: // Accessors
   inline virtual float64 get_clay_percent(nat8 layer) const {return clay[layer];} //080313
   inline virtual float64 get_sand_percent(nat8 layer) const {return sand[layer];} //080313
   inline virtual float64 get_silt_percent(nat8 layer) const {return silt[layer];}
public:
   virtual void set_sand_clay_silt(nat8  layer, float64 _sand, float64 _clay, float64 _silt);
   virtual void set_sand_clay(nat8  layer, float64 _sand, float64 _clay);
};
//_Profile_texture__________________________________________________2006-07-24_/
}//_namespace_Soil______________________________________________________________
#endif
