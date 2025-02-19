#include "soil/soil_I.h"
#include "soil/structure_I.h"
namespace Soil {
//______________________________________________________________________________
float64 Soil_interface::get_mass_kg_m2(nat8 layer)                        const
{  float64 mass_kg_m2 = 0.0;
   const Layers_interface  *layers    = ref_layers();
   unmodifiable_ Structure *structure = ref_structure();                         //160412
   if (layers && structure)
   {  float64 thickness_layer_m       =layers->get_thickness_m(layer);
      float64 bulk_density_layer_kg_m3=structure->get_bulk_density_kg_m3(layer);
      mass_kg_m2 = bulk_density_layer_kg_m3 * thickness_layer_m;
   }
   return mass_kg_m2;
}
//______________________________________________________________________________
float64 Soil_interface::distribute_amount
(const soil_horizon_array32(from_horizon_amount)
,soil_sublayer_array_64(to_layer_amount))                                  const
{  float64 sum = 0;
   for (nat8 layer = 1; layer <= ref_layers()->count(); layer ++)
   {
      to_layer_amount[layer] = from_horizon_amount[layer];
      sum += to_layer_amount[layer];
   }
   return sum;
}
//_distribute_amount____________________________________________2015-04-21_RLN_/
}//_namespace_Soil______________________________________________________________
