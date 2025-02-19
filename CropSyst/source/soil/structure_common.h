#ifndef structure_commonH
#define structure_commonH
//______________________________________________________________________________
#include "soil/structure_I.h"
#include "corn/math/statistical/sample_dataset_T.h"
#include "common/soil/layering.h"
class Soil_parameters_class_common;
namespace Soil {                                                                 //181206
class Layers_interface;                                                          //181206
//______________________________________________________________________________
class Structure_common
: public Soil::Structure
{
protected:
   const Layers_interface    &layers;    // Provides convenient access to soil layering
   soil_layer_array64(bulk_density_g_cm3);
   CORN::statistical::Sample_dataset_clad<float32,float32,float32>                //180408
      bulk_density_stats_g_cm3;
   // using_statistical data set because because we need average for profile.
public: // structors
   Structure_common(const Layers_interface &_layers);
   Structure_common                                                              //150926
      (const Layers_interface &_layers
      ,const Soil_parameters_class_common &parameters);
   Structure_common(const Structure_common &from_copy);
   inline virtual ~Structure_common() {}                                         //170217
   void copy(const Structure_common &copy_from);
   virtual void expand_last_layer_for_boundary_conditions
      (nat8 number_layers, nat8 number_expanded_layers)           modification_; //080130
public:
   virtual float64 get_bulk_density_g_cm3(nat8 layer)                     const;
   virtual float64 set_bulk_density_g_cm3(nat8 layer,float64 bulk_dens_g_cm3);
   virtual float64 set_bulk_density_kg_m3(nat8 layer,float64 bulk_dens_kg_m3);   //141207RLN
   virtual float64 get_bulk_density_kg_m3(nat8 layer)                     const; //150526LML
   virtual float64 get_bulk_density_avg_g_cm3()                           const;
public: // Units of mass conversion
   virtual float64 kg_per_m2_to_micrograms_per_g
      ( nat8 layer, float64 amount_kg_ha)                                 const;
    // alias this to kg_per_m2_to_concentration
   virtual float64 micrograms_per_g_to_kg_per_m2
      ( nat8 layer, float64 concentration)                                const;
    // alias this to concentration_to_kg_per_m2
   virtual float64 get_mass_kg_m2(nat8 layer)                             const; //110927
};
//_Structure_common_________________________________________________2007-07-25_/
}//_namespace_Soil______________________________________________________________
#endif

