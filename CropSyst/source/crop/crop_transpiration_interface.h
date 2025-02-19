#ifndef crop_transpiration_abstractH
#define crop_transpiration_abstractH
namespace CORN { class Enumeration_list; }                                       //130730
#ifndef primitiveH
#  include <corn/primitive.h>
#endif
#include "corn/const.h"
#ifndef mgmt_typesH
#  include "mgmt_types.h"
#endif

#ifndef MAX_soil_layers
// This is needed when compiling under V3 because
// V4 global soil types are confusing the V4 spreadsheet outputs. 
#  include "common/soil/layering.h"
#endif
#include "CS_suite/observation/CS_observation.h"
namespace Soil
{  class Soil_interface;
}
//______________________________________________________________________________
interface_ Crop_transpiration       // actually this is an interface
{
 public:
   inline Crop_transpiration()                                                {}
   virtual float64 calc_transpiration
      (float64 limited_pot_transpiration                                         //011117
      ,float64 fract_canopy_cover                                                //020712
      ,float64 param_max_water_uptake_at_growth_stage_m                          //091208
      ,modifiable_ float64 &interception_canopy_m                                //160414_160318
      ,const soil_sublayer_array_64(total_fract_root_length)                     //050331
      ,soil_sublayer_array_64(water_uptake)                                      //010724
      ,float64 CO2_transpiration_adjustment_factor )                    const=0;

         // Optional for salinity. If null, 0.0 will be used osmotic potential

   virtual float64 get_active_fract_root_length(nat8 sublayer)          const=0;
   virtual float64 get_leaf_water_pot()                                 const=0; //071114
   virtual float64 get_uptake_actual()                                  const=0; //160414
   #if ((DIRECTED_IRRIGATION>=2014) && (DIRECTED_IRRIGATION <= 2016))
   virtual bool know_directed_irrigation_soil_wetting                            //130313
      (const Directed_irrigation_soil_wetting *soil_wetting)       cognition_=0;
   #endif
 public: // observation
   RENDER_INSPECTORS_DECLARATION_PURE;                                           //160627
};
//______________________________________________________________________________
#endif

