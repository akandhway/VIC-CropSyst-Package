#ifndef cs_ETH
#define cs_ETH
#include "options.h"

// This options.h needs to be for the current program being compiled
// is should not necessarily be /cropsyst/cpp/options.h
#if (CROPSYST_VERSION == 3)
#  define CO2
#  include "options.h"
#  include "common/weather/ET_reference.h"
#  include "UED_out.h"
   class Recorder_data_record;
#endif
#  include "common/biometeorology/ET_reference_FAO.h"
/* NYI
#if (CROPSYST_VERSION == 5)
#  include "common/biometeorology/ET_reference_best.h"
#endif
*/
#include "common/weather/loc_param.h"
#include "weather/parameter/WP_vapor_pressure_deficit.h"

// (For ET model types)
namespace CS {
   class Priestley_Taylor_Parameters;                                            //170808
}
//______________________________________________________________________________
class CropSyst_ET
: public ET_Reference_FAO                                                        //011018
{
   const Weather_provider  &weather_provider;                                    //150804
   float64                  screening_height; //m                                //151022
   const Wind_speed        &wind_speed;                                          //151022
   const Vapor_pressure_deficit_fullday &vapor_pressure_deficit_fullday_avg;     //151022
 public :
   const CS::Priestley_Taylor_Parameters &P_T_parameters;                        //170808
      // actually could be optional
   float64 daily_ratio_elevated_to_baseline_ppm_CO2_ET;                          //090316
      // Used when atmospheric CO2 concentration change
   contribute_ ET_Model ET_model_selected;                                       //180822_040607
      // The ET model selected today with the best quality
 public: // output control                                                       //030801
   bool structure_defined;                                                       //030801
 public: // constructor
   CropSyst_ET
      (
       const Weather_provider &_weather_provider                                 //150804
      ,const Geocoordinate &geocoordinate                                        //151118
      ,float64 screening_height                                                  //151022
      ,const float64       &day_length_hours_today                               //150627
      //NYN,Aerodynamics &_aerodynamics                                          //150601
      ,ET_Model            preferred_ET_model                                    //011109
      ,const CS::Priestley_Taylor_Parameters &P_T_parameters);                   //170808_051231
         // actually could be optional
 public: // Daily transition;
   virtual const CORN::Quality &update()                         rectification_; //180822
 public: // Dependent functions
   float64 other_atmospheric_transmissivity(float64 pot_solar_rad);
   float64 aero_dynamic_resistance_vapor_exchange
      (float64 plant_height_m) const;    // day/m                                //180822
 public: // ET calculation
   float64 calc_Priestley_Taylor_pot_ref_et_mm
      (float64 slope_of_sat_vap_press                                            //051231
      ,float64 psychrometric  // Units: kPa/C                                    //980311
      ,float64 latent_heat                                                       //980310
      ,float64 net_rad          // day time                                      //980310
      ,float64 soil_heat_flux  //{ MJ/m2/day }                                   //980310
      ,float64 fullday_VPD) const;                                               //180822_020212
   // This returns the daytime potET!!
   float64 calc_pot_ref_evapotranspiration_mm()                    calculation_;
   // This returns the daytime potET!!
 public: // Accessors
//190708 now always available#ifdef CO2_CHANGE
   inline float64 get_daily_ratio_elevated_to_baseline_ppm_CO2_ET()        const
      { return daily_ratio_elevated_to_baseline_ppm_CO2_ET; };
//190708 now always available#endif
 public: // Recording object overrides
   inline virtual const char *get_record_section_label()   const { return "et";}
   bool setup_structure(CORN::Data_record &data_rec,bool for_write);             //030801
   inline const char *get_selected_ET_model_label()                        const
      { return CS_ET_model_label_table[ET_model_selected].label;}                //090802
};
//_class CropSyst_ET___________________________________________________________/
#endif
//cs_ET.h

