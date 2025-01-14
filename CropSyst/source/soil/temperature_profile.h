#ifndef temperature_profileH
#define temperature_profileH

// This options.h needs to be for the current program being compiled
// is should not necessarily be /cropsyst/cpp/options.h

#include "corn/const.h"
#include "soil/temperature_I.h"
#include "soil/surface_temperature.h"
#include "common/soil/layering.h"
#include "CS_suite/observation/CS_observation.h"

//------------------------------------------------------------------------------
class Soil_parameters;
class Residues_interface;                                                        //050726
class Air_temperature_maximum;                                                   //151022
class Air_temperature_minimum;                                                   //151022
class Air_temperature_average;                                                   //151022
class Solar_radiation;
namespace CORN { class Data_record; }
namespace CS   { class Land_unit_meteorological; }                               //151022
namespace Soil {                                                                 //181206
class Layers_interface;
class Structure;                                                                 //070725
class Hydrology_interface;
//------------------------------------------------------- Forward declaration__/
class Temperature_profile
: public Soil::Temperature_interface                                              //080123
, public Soil_surface_temperature                                                //080123
{
   float64  surface_temperature;  // Was a local variable
   float64  residue_lagging_factor ;
   soil_layer_array64(temperatures);
   soil_layer_array64(new_temperatures); // 0 INDEXED!!!
   soil_layer_array64(previous_temperatures);
   float64 new_temperature_0;
 public:
   const Soil_parameters                     &soil_parameters;                   //061004
   const Structure                           &soil_structure;                    //070725
   const Layers_interface                    &soil_layers;                       //061004
   const Hydrology_interface                 &soil_hydrology;                    //061004
   const CS::Land_unit_meteorological        &meteorological;                    //151022
   float64   annual_temperature_avg;                                             //060502
 protected: // meteorogical weather element references
   const Air_temperature_maximum             &air_temperature_max;               //151022
   const Air_temperature_minimum             &air_temperature_min;               //151022
   const Air_temperature_average             &air_temperature_avg;               //151022
   const Solar_radiation                     &solar_radiation;                   //151022
 public: // 'structors and initialization
   Temperature_profile                                                           //010207
      (const Soil_parameters              &soil_parameters                       //061004
      ,const Layers_interface        &soil_layers                                //061004
      ,const Hydrology_interface     &soil_hydrology                             //061004
      ,const Structure               &soil_structure                             //070725
      ,const CS::Land_unit_meteorological &meteorological);                      //151022
   bool initialize(float64 initial_soil_temp,float64 _annual_temperature_avg);   //010207
   bool setup_structure                                                          //030801
      (CORN::Data_record &data_rec
      ,bool for_write)                                            modification_;
 public:
   void long_term_initialization                                                 //030508
      (float64 annual_temperature_avg // (deep_constant_temperature)             //990213
      ,float64 temperature_influence_from_above)                   modification_;
   void find_soil_temp                                                           //990202
      (float64   leaf_area_index                                                 //980601
      ,bool      water_was_applied
      ,const  Residues_interface *residue                                        //050726
         //  may be 0 if no residue or no residue simulation
      ,float64   fraction_canopy_interception
      ,float64   plant_height);
 public: // virtual accessor overrides
   inline virtual float64 get_temperature(nat8 layer)                      const
      { return temperatures[layer]; }
   inline virtual float64 get_surface_temperature()                        const
      { return surface_temperature; }
 private:
   float64 profile_water_amount();
   float64 damping_depth();    // m
   void get_temperatures                                                         //990201
      (float64 deep_constant_temperature     //C
      ,const  Residues_interface *residue                                        //060628
      ,float64 fract_crop_interception
      ,float64 plant_height);                // m
   float64 radiation_load  // J/(m2 sec)
      (float64 fract_crop_interception
      ,float64 fract_residue_interception);
   float64 albedo_for_soil_temp();
   float64 find_heat_source_sink  // J/m2/s
      (float64 thickness_sl       // m
      ,float64 new_ice_content_sl
      ,float64 old_ice_content_sl);
   float64 base_line_temperature                                                 //990120
      (nat8      layer
      ,float64    deep_constant_temperature
      ,float64    temperature_influence_from_above);
   void SolveTemp
      (float64 plant_height
      ,float64 fract_crop_interception
      ,const  Residues_interface *residue                                        //060628
#ifdef HEAT_SINK_FREEZING
//check obsolete
      ,soil_layer_array64(new_ice_content)
      ,soil_layer_array64(heat_source_sink)
#endif
      );
   RENDER_INSPECTORS_DECLARATION;                                                //150916
};
//_Temperature_profile______________________________________________1998-07-28_/
}//_namespace_Soil_____________________________________________________________/
#endif
//soiltemp.h

