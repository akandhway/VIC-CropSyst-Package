#ifndef infiltration_finite_diffH
#define infiltration_finite_diffH

// This soil superclasss add finite difference water infiltration
// This options.h needs to be for the current program being compiled
// is should not necessarily be /cropsyst/cpp/options.h
#include "options.h"
#include "soil/soil_base.h"

#include "soil/infiltration_A.h"
#include "soil/evaporation_without_vapor_path.h"
#include "soil/texture_I.h"
#include "soil/soil_evaporator.h"
#ifdef CROPSYST
#include <fstream>
#endif

class Water_table_curve;
class Relative_humidity;                                                         //151215
namespace CropSyst {
class Soil_runoff;                                                               //130308
class Pond;
};

namespace Soil {                                                                 //181206
class Hydrology_finite_difference;
//______________________________________________________________________________
class Infiltration_finite_difference
: public Infiltration_abstract
{public:
   enum FD_status {START,ITERATE,FLUX_BOUNDARY,SATURATED_BOUNDARY,CASCADE_REDISTRIBUTION,MAXED_OUT_ITERATIONS,TURNING_INTO_SATURATED_BOUNDARY,FAILED_ALL_CONDITIONS,CONVERGED};  //981012
 private: // references
   #ifndef MICROBASIN_VERSION
   Soil_base                             &soil_base;    // Eventually will simply use the soil interface  in Soil_infiltation, but there are still a number of soil_base variables accessed //070119
   #endif
   /*modifiable_*/ Soil::Layers_interface           &soil_layers;
      // Not const because we adjust the number of effective layers              //061004
   const Soil::Texture_interface                    &soil_texture;               //140509
   modifiable_ Soil::Hydrology_finite_difference    &hydrology;
      // Owned by the soil class                                                 //060509
             Water_table_curve                     *water_table_depth_curve;                                                                                                                          //000504
   Evaporation_without_vapor_path &evaporator;  //NYI probably should be Evaporator_soil_FD     //160606
#ifdef CROPSYST
   std::ofstream status_report;                                                  //000307
#endif
   const Relative_humidity &relative_humidity_min;                               //151216
   mutable float64 atmospheric_water_potential;                                  //051130
   nat8  number_of_real_plus_extended_layers;                                    //080211
   nat8  number_of_real_plus_extended_nodes;                                     //080211
   nat8  number_FD_solution_nodes; nat8  number_FD_solution_layers;              //080212
 protected: // parameters                                                        //071010
   bool numerical_runoff_mode;                                                   //071010
   Seconds     time_step;  // Second in the timestep  I.e. 3600 is hour (86400 is day but currently always running hour)
 protected: // parameters
   bool water_table_boundary_condition;    //  True when we have a water table today
   float64 water_table_depth;                                                    //070309
 protected: // setup for each timetep                                            //071010
   Hour     hour;                                                                //071010
   Seconds  time_elapsed;
   float64 runoff_this_interval;

 protected: // carried throughout the day                                        //071010
   float64 deferred_water_to_enter_soil_mm;   // This is water that could not enter and is deferred until next timestep (even to the next day) //071008
   float64 cascade_drainage;
 private:  // attributes results from mass balance iteration
   mutable soil_layer_array64(water_potential);               // J/kg ?  This may need to be extended, but water_pot[0] doesn't appear to be used;
           soil_node_array64(water_flow_nodal);              // m (I believe + values are flow down)  //080208
   soil_node_array64(node_depth);                                                //080215
 private:
   contribute_ std::string convergence_info;                                     //140509_071010
      // Now a member to reduce passing
 public:
   float64 daily_error;  // So water balance has access to it.                   //990221
   #ifdef MICROBASIN_VERSION
   float64 current_error_dt;                                                     //LML 141028
   #endif
   float64 water_table_input;
 public: // constructor
   // Water table form:
   Infiltration_finite_difference /*181206 Soil_infiltration_finite_difference*/                                           //980730
      (
      #ifndef MICROBASIN_VERSION
      Soil_base                           &soil_,                                //001116
      #endif
       /*const*/ Soil::Layers_interface    &soil_layers_                          //140509
      , const Soil::Texture_interface      &soil_texture_                         //140509
      , const Soil::Hydraulic_properties_interface &soil_hydraulic_properties_    //140423
      , modifiable_ Soil::Evaporation_without_vapor_path &evaporator_  //NYI probably should be Evaporator_soil_FD     //160606
      , CropSyst::Soil_runoff              *runoff_                              //130308
      , CropSyst::Pond                     &pond_                                //991102
      , Soil::Hydrology_finite_difference   &hydrology_                           //060509
      , const Relative_humidity            &relative_humidity_min_               //151216
      , bool                                numerical_runoff_mode_);             //140509
   #if (DIRECTED_IRRIGATION==2)
   virtual Soil_infiltration_nominal *copy(float64 fraction)               const;//140609
   #endif
   bool initialize();
   virtual bool reinitialize(const soil_horizon_array32(H2O));
 public:  // process
   virtual bool                      infiltrate
      (float64                       water_entering_soil_not_irrigation
      ,float64                       management_irrigation
      ,float64                       management_auto_irrigation_concentration
      ,Dynamic_water_entering_soil *non_runoff_water_entering_in_intervals_mm    //990204
      ,Dynamic_water_entering_soil *irrigation_water_entering_in_intervals_mm // optional (may be 0)     //070119
      ,const Dynamic_array<float32>    &runon_in_intervals                       //990409
      ,Dynamic_array<float32>          &runoff_in_intervals);                    //990409

#ifdef WATERTABLE
   inline virtual float64 get_water_table_input()                          const { return water_table_input;};
   inline virtual float64 get_water_table_depth()                          const { return water_table_depth ; };     //070309
   inline         bool    water_table_applies()                            const { return water_table_depth_curve;}; //050428
#else
   inline virtual float64 get_water_table_input()                          const { return 0;};
   inline virtual float64 get_water_table_depth()                          const { return 99999.9;};     //070309
   inline         bool    water_table_applies()                            const { return false;};       //050428
#endif
   virtual inline float64 get_deferred()                                   const { return deferred_water_to_enter_soil_mm; }; //071024
 private:
   //_______________________________________________________________
   class Mass_balance_iteration
   {
   private:  //
      mutable Infiltration_finite_difference /*181206 Soil_infiltration_finite_difference*/::FD_status status;
      const Soil::Layers_interface               &soil_layers;  // Not const because we adjust the number of effective layers
      const Soil::Hydrology_finite_difference    &hydrology;    // Owned by the soil class
      const Soil::Hydraulic_properties_interface &soil_hydraulic_properties;
      // water potential is considered to be adjuncts becuase
      // although we keep the last value between invocations,
      // it is essentially recomputed and converges to the 'right'
      // value as part of the mass balance iteration.
      mutable soil_layer_array64(water_pot);               // J/kg ?  This may need to be extended, but water_pot[0] doesn't appear to be used;
      mutable float64                  *water_pot_committed;
      float64                          *node_depth;
      Soil::Evaporation_without_vapor_path &evaporation;                 // reference to Infiltration_finite_difference /*181206 Soil_infiltration_finite_difference*/
      float64                          typical_water_potential_below_rootzone;   //090119
   private:  // parameters
      float64 time_step;
   public:  // attributes results from mass balance iteration
      mutable float64 finite_diff_error;
      mutable float64 actually_infiltrated_m_ts;
    private:  // adjuncts
      mutable float64 atmospheric_water_potential;
      mutable soil_layer_array64(water_flux_mm);                                 //070117
      mutable soil_sublayer_array_64(new_up_WC);
      mutable soil_sublayer_array_64(new_low_WC);
   public: // Constructor
      Mass_balance_iteration                                                     //071010
         (Infiltration_finite_difference /*181206 Soil_infiltration_finite_difference*/::FD_status  _status
         ,float64                                         _time_step                  //   number of seconds in this timestep
         ,const Soil::Layers_interface                    &_soil_layers
         ,const Soil::Hydrology_finite_difference         &_hydrology
         ,const Soil::Hydraulic_properties_interface      &_hydraulic_properties
         ,float64                                         _atmospheric_water_potential
         ,Soil::Evaporation_without_vapor_path            &_evaporation                 // reference to Infiltration_finite_difference /*181206 Soil_infiltration_finite_difference*/
         ,float64                                        *_water_pot_committed
         ,float64                                        *_node_depth
         ,float64                                         _typical_water_potential_below_rootzone);
   public:
      FD_status resolve
         (Hour hour
         , nat16 max_iterations
         , float64 water_entering_soil_m_ts   // Note these value are all per time step
         , bool water_table_boundary_condition                                   //071010
         , nat8  number_FD_solution_layers                                       //080211
         , nat8  number_FD_solution_nodes                                        //080131
         , float64 &flux_term_driving_drainage_m_ts)                      const; //080227
      bool commit                                                                //071010
         (modifiable_ Soil::Hydrology_finite_difference &hydrology               //071010
         , soil_layer_array64                         (water_flow_nodal)         //080202
         , nat8                                        num_layers_eff)    const; //080202
   private:                                                                      //071010
      float64 calc_water_content
         (nat8  i , float64 p, float64 &dwdp )                            const;
      float64 element_flux
         (nat8     i
         ,float64 &djdpu
         ,float64 &djdpl
         ,float64 &ku)                                                    const; //990220
   private: // evaporation
      float64 evaporate                                                          //080812
         (float64 &flux_kg_m2_s  // Flux will be reduced by evaporation
         ,float64 max_act_evaporation_mm_ts
         ,Seconds time_step)                                      modification_;
   };
   //_Mass_balance_iteration________________________________________2007-10-10_/
 private:
#ifdef WATERTABLE
   virtual void set_water_table                                                  //020620
      (const char       *water_table_filename
      ,const CORN::date32 &today                                                 //170525
      ,const CORN::Date_const &first_date
      ,const CORN::Date_const &last_date);
  virtual void water_table_limitations() modification_;
#endif
 private:  // The following methods are called by the finite_difference infiltration routine
   FD_status flux_boundary_infiltration
      (float64 water_entering_this_time_step_m                                   //071010
      ,float64 &actually_infiltrated_time_step
      ,float64 &flux_term_driving_drainage                                       //080227
      ,float64 &finite_diff_error);
   FD_status saturation_boundary_infiltration
      (float64 water_entering_this_time_step_m
      ,return_ float64 &actually_infiltrated_time_step                           //071010
      ,return_ float64 &flux_term_driving_drainage                               //080227
      ,return_ float64 &finite_diff_error);
   FD_status cascade_redistribution                                              //071010
      (return_ float64 &actually_infiltrated_time_step                           //071010
      ,return_ float64 &finite_diff_error);
   float64 calc_atmospheric_water_potential                                      //071010
      (float64 relative_humidity_TS_or_daily_min_percent)                 const;
 public:
   FD_status infiltrate_and_transport_timestep                                   //140509
      (Hour hour
      ,float64 eff_precip_interval
      ,float64 irrigation_entering_this_time_step
      ,float64 runon_interval
      // return values
      ,float64 &actually_infiltrated_time_step
      ,float64 &actually_evaporated_time_step_m_ts
      //,float64 &actually_reported_drainage_m_ts                                //141022LML
      #ifdef MICROBASIN_VERSION
      ,float64 &actually_balance_drainage_m_ts                                   //141022LML
      ,float64 &surface_runoff_m_ts                                              //141031LML
      #endif
      )                                      modification_;
 private: // The following a daily temporary variables common to the F.D. subroutines
   bool infiltration_finite_difference
      (float64 irrigation
      ,Dynamic_water_entering_soil *non_runoff_water_entering_in_intervals_mm    //990204
      ,Dynamic_water_entering_soil *irrigation_water_entering_in_intervals_mm // optional (may be 0)
      ,const Dynamic_array<float32> &runon_in_intervals                          //990409
      ,Dynamic_array<float32> &runoff_in_intervals);                             //990409
   FD_status mass_balance_time_step_control                                      //990215
      (float64 pot_water_entering_soil                                           //070319
      ,uint16  max_intervals_per_day
      ,Dynamic_water_entering_soil *non_runoff_water_entering_in_intervals_mm    //990204
      ,Dynamic_water_entering_soil *irrigation_water_entering_in_intervals_mm    // optional (may be 0)
      ,float64 &runoff
      ,const Dynamic_array<float32> &runon_in_intervals                          //990409
      ,Dynamic_array<float32> &runoff_in_intervals);                             //990409
   float64 get_field_capacity_ice_corrected(Layer layer)                  const; //070125
   float64 typical_water_potential_below_rootzone()                       const; //080325

   virtual float64 calc_evaporation_potential()                    calculation_  //160603
      { assert(false); return 0; }
};
//_Infiltration_finite_difference___________________________________1998-07-29_/
}//_namespace_Soil_____________________________________________________________/
#endif

