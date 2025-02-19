#ifndef infiltration_IH
#define infiltration_IH
#include "mgmt_types.h"
#include "corn/chronometry/date_I.h"
#include "corn/dynamic_array/dynamic_array_T.h"
#include "soil/hydrology_I.h"
#include "soil/layers_I.h"
#include "corn/chronometry/time_types.hpp"

class Dynamic_water_entering_soil;
class Soil_interface;
class Soil_wetting;
namespace CropSyst {
   class Soil_runoff;                                                            //130308
   class Pond;                                                                   //141206
}
namespace Physical
{
   class Water_depth;                                                            //160412
}
namespace Soil {                                                                 //181206
   class Chemicals_profile;                                                      //181206

//______________________________________________________________________________
interface_ Infiltration_interface
{  // This infiltration model does nothing,
   // so neither infiltration nor evaporation
   // It is used when we need to test the model without
 public:  // 'structors and initialization
   inline virtual ~Infiltration_interface() {}                                   //170217
   // Interface needs no constructor
   virtual bool initialize()                                    modification_=0;
   virtual bool reinitialize(const soil_horizon_array32(H2O))   modification_=0;
   #ifdef DIRECTED_IRRIGATION
   virtual Infiltration_interface *copy(float64 fraction)               const=0; //140609
   #endif
 public: // Processes
   virtual bool infiltrate
      (float64               water_entering_soil_not_irrigation                  //070109
      ,float64               management_irrigation
      ,float64               management_auto_irrigation_concentration
      ,Dynamic_water_entering_soil *non_runoff_water_entering_in_intervals_mm    //990204
      ,Dynamic_water_entering_soil *irrigation_water_entering_in_intervals_mm    //optl//070119
      ,const CORN::Dynamic_array<float32> &runon_in_intervals                    //990409
      ,      CORN::Dynamic_array<float32> &runoff_in_intervals) modification_=0; //990409
 public: // Get accessor
   virtual float64 get_act_water_entering_soil()                        const=0; //150925
   virtual modifiable_ float64 &mod_act_water_entering_soil()   modification_=0; //160412_150925
   virtual float64 get_water_table_input()                              const=0;
   virtual float64 get_water_table_depth()                              const=0; //070309
   virtual float64 get_balance_drainage()                               const=0; //070118
   virtual float64 get_reported_drainage()                              const=0; //070118
   virtual float64 get_infiltration()                                   const=0; //070118
   virtual bool is_water_table_applicable()                             const=0;

   #if ((DIRECTED_IRRIGATION>=2014) && (DIRECTED_IRRIGATION <= 2016))
      virtual bool know_directed_irrigation_soil_wetting                            //130313
         (const Directed_irrigation_soil_wetting *_soil_wetting)   cognition_=0;
   #endif
   #if (DIRECTED_IRRIGATION == 2020)
      // Current implementation
   virtual bool know_directed_irrigation(float64 wetted_area)      cognition_=0; //200226
   #endif
   RENDER_INSPECTORS_DECLARATION_PURE;                                           //150929
};
//_Infiltration_interface___________________________________________2006-05-12_/
}//_namespace_Soil_____________________________________________________________
#endif

