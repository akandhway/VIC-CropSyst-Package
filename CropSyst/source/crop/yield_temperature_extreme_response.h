#ifndef extreme_temperature_responseH
#define extreme_temperature_responseH
#include "corn/const.h"
#include "corn/primitive.h"
#include "corn/dynamic_array/dynamic_array_T.h"
#include "corn/container/unilist.h"
#include "corn/math/statistical/sample_dataset_T.h"
#include "crop/crop_param_struct.h"
namespace CORN
{  class Local_time;
}
namespace CropSyst
{
//______________________________________________________________________________
interface_ Harvest_temperature_stress_factor

//150825  Probably rename to Harvest_extreme_temperature_stress_factor

{public:
   virtual float64 get_factor()                                         const=0;
   virtual bool update()                                       rectification_=0;
   virtual bool add_period
      (const Crop_parameters_struct::Temperature_sensitivity &parameters)    =0;
};
//_Harvest_temperature_stress_factor________________________________2013-04-24_/
class  Yield_grain_temperature_extreme_response
//200422 was Harvest_temperature_stress_factor_grain

: public Harvest_temperature_stress_factor
{
/*
   Continue here need to reference the parameters
   also need to query the stress adjustment flag.
*/

   const CORN::Dynamic_array<float32> &stress_adj_hourly_temperature;
   //___________________________________________________________________________
   class Temperature_factors
   : public CORN::Item
   {public:
      const Crop_parameters_struct::Temperature_sensitivity &parameters;
      mutable CORN::statistical::Sample_dataset_clad<float32,float32,float32>
         cold;
         // original Statistical_data_set was float32 but could now be float64
      mutable CORN::statistical::Sample_dataset_clad<float32,float32,float32>
         heat;
    public:
      inline Temperature_factors
         (const Crop_parameters_struct::Temperature_sensitivity &_parameters)
         :parameters(_parameters)
         {}
      float64 effective_factor()                                           const
      {
         float64 factor = 1.0;
         if (cold.get_count()+heat.get_count())
         {  // Not all growth stages may have been applicable
            factor = std::min<float64>(cold.get_mean(),heat.get_mean());
            /*
            nat16 factor_0_occurances
               = cold.count_occurances(0.0,0.00001)
                +heat.count_occurances(0.0,0.00001);
            if (factor_0_occurances == 1)
               factor = std::min<float64>(factor,0.5);
            if (factor_0_occurances > 2)
               factor = 0.0;
            */
         }
         return factor;
      }
      float64 temperature_stress_function
      (float64 hour_temperature
      ,float32 no_stress_threshold_temperature
      ,float32 max_stress_threshold_temperature
      ,float32 Sensitivity)
      {  float64 TSF

            = (max_stress_threshold_temperature > no_stress_threshold_temperature)

               ? (hour_temperature <= no_stress_threshold_temperature) // for heat
                  ? 1.0
                  : (hour_temperature >= max_stress_threshold_temperature)
                     ? 0.0
                     : 1.0 - (hour_temperature - no_stress_threshold_temperature)
                      /(max_stress_threshold_temperature - no_stress_threshold_temperature)
               : (hour_temperature >= no_stress_threshold_temperature) // for cold
                  ? 1.0
                  : (hour_temperature <= max_stress_threshold_temperature)
                     ? 0.0
                     : 1.0 - (no_stress_threshold_temperature - hour_temperature)
                      /(no_stress_threshold_temperature - max_stress_threshold_temperature);
         TSF = CORN::is_zero<float64>
            ((float64)Sensitivity,(float64)0.00001)                              //131105
            ? 1.0 : pow(TSF,(float64)Sensitivity);                               //131105
         return TSF;
      }
      //_Harvest_temperature_stress_factor_grain____________________2013-04-16_/

   };
   CORN::Unidirectional_list period_temperature_factors; // list of Temperature_factors
   Temperature_factors *current_period_sensitivity;
   const CORN::Local_time &sun_rise_time; // reference to weather sun sun_rise_time    //130628
 public:
   inline Yield_grain_temperature_extreme_response
      (const CORN::Dynamic_array<float32>&_stress_adj_hourly_temperature
      ,const CORN::Local_time &_sun_rise_time)
      :stress_adj_hourly_temperature(_stress_adj_hourly_temperature)
      ,period_temperature_factors()
      ,current_period_sensitivity(0)
      ,sun_rise_time(_sun_rise_time)                                             //130628
      {}
   virtual float64 get_factor()                                                    const;
   virtual bool add_period(const Crop_parameters_struct::Temperature_sensitivity &parameters);
   virtual bool update()      rectification_;
         // Eventually I should just have a reference to the
         // current growth stage sequence index maintained by the crop
         // object. (This would be faster than passing the value every day)
};
//_Harvest_temperature_stress_factor_grain__________________________2013-04-24_/
}//_namespace CropSyst_________________________________________________________/
#endif
