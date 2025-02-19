#ifndef soil_baseH
#define soil_baseH

#include "options.h"

//#define USE_SOIL_PROPERTIES_SUBLAYERS
#ifdef USE_SOIL_PROPERTIES_SUBLAYERS
#  include "soil/soil_properties_sublayers.h"
#else
#  include "soil/hydraulic_properties.h"
#include "soil/sublayers.h"
#ifdef STATSGO_DATABASE
#include "soil/soil_param_with_STATSGO.h"
#else
// LADSS doesn't need STATSGO
#include "soil/soil_param.h"
#endif
#endif

#include "soil/structure_common.h"
#include "model_options.h"
#include "corn/dynamic_array/dynamic_array_T.h"
#include "common/recorder/recorder.h"
#include "soil/profile_texture.h"
//#if (CROPSYST_VERSION==5)
#include "CS_suite/observation/CS_observation.h"
//#endif
#include "common/physics/water_depth.h"
#include "CS_suite/simulation/CS_simulation_element.h"
#include "soil/soil_I.h"
//______________________________________________________________________________
#define N_coef_variation              0.0
#define MAX_trans_sublayers           5
#define const_record
//Indicates the method would be const, but we record a value for output
//______________________________________________________________________________

using namespace CORN;
namespace CS
{  class Variable_recorders;
}
namespace Soil
{
   class Infiltration_nominal;
   class Hydrology_abstract;
   class Salt_interface;
   class Texture_interface;
} // namespace Soil
class Dynamic_water_entering_soil;

//190118 now using optation_global namespace CS { class Desired_variables; }                                        //170225
namespace CropSyst {
   class Pond;
   class Crop_CropSyst;                                                          //141206
   class Period_accumulators;
}
#if (CS_VERSION==5)
   namespace CropSyst { class CS_daily_data_record; }                            //170225_041022
#else
   class CS_daily_data_record;                                                   //041022
#endif
#if (CS_VERSION==4)
   class CropSyst_DLY_report;   // for XLS output
#endif
//______________________________________________________________________________
//060724  Now tillage can affect texture so sand, clay (bulk density)
//        cannot be constants

class Soil_base
: public Soil::Soil_interface                                                    //181206_051118
, public extends_ CS::Simulation_element_abstract                                //150723
#ifdef USE_SOIL_PROPERTIES_SUBLAYERS
, public extends_  Soil_properties_sublayers                                      //101104
#else
#endif
{public:
#ifdef USE_SOIL_PROPERTIES_SUBLAYERS
#else
   const Smart_soil_parameters   *parameters_ownable;                            //140609_060206
   bool                           parameters_owned;                              //140609
   Soil::Profile_texture          texture;                                       //060724
#endif
 friend class Soil_infiltration_cascade_daily;
 friend class Soil_infiltration_cascade_hourly;                                  //070107
 friend class Soil_infiltration_finite_difference;
 friend class Water_balance_accumulators;

 friend class CropSyst::Period_accumulators;

 friend class Crop_common;                                                       //031130
 friend class CropSyst::Crop_CropSyst;                                           //141206_031130
 friend class Management_event;
#ifdef XLS_OUTPUT
 friend class CropSyst_DLY_report;
#endif
 friend class Soil_chemicals_profile;                                            //030710
//           Enabling variables for output;
 protected: // These variables are computed only for output.
   mutable soil_sublayer_array_64(record_all_water_content);                     //040208
 protected: Soil::Evaporator *evaporator; bool evaporator_owned;                  //160606
   // owned by this
 protected: // Daily soil variables:
   // In cascade model this is set to the water_entering the soil.
   Infiltration_model      infiltration_model;                                   //130301
   soil_sublayer_array_64(required_freezing_index);  // layer 0 is the snow layer, now global to soil //980618
   // From global space
   int16      drainage_layer;  //The layer below the current root depth.
   float64    initial_drainage;
#ifdef USE_SOIL_PROPERTIES_SUBLAYERS
#else
 public:
   Soil_sublayers             *layers; // Must come before hydraulic_properties  //061003
 public: // Temporarily public (so I can alloc Soil_tillage_effect in Land_unit_sim)
      //Should add an accessor to get this  //061108
   Soil::Hydraulic_properties  *hydraulic_properties;  // The current soil hydraulic properties (these may be redone when there is erosion, or dust mulching ). //051204
 public:
   Langmuir               NH4_Langmuir;                                          //190404
#endif
 public:
   Soil::Structure_common      *structure;                                        //070725
   Soil::Hydrology_interface   *hydrology;  // The current soil hydrology  (must come after hydraulic_properties declaration) (owned by this)       //060504
   Seconds                     hydrology_timestep_resolution_today;  // The number of seconds in the hydrology time step use today  (daily = 86400)//600508
//NYI  continue here, need hydrology for each each time step (I.e. in F.D. or Cascade by Hour which is not yet implemented).
 public:

   //modifiable_ Physical::Water_depth &evaporation_pot_remaining;                 //160411
      // potential evaporation is calculated at the beginning of the day
      // The store is then consumed/deducted by elements (I.e. pond residue plant
      // that have demand through out the day and is ultimately exhausted
      // by the soil.
      // Currently ET is partitioned into transpiration and evaporation.
      // but I think that this store could start with ET.
   //#define pot_evaporation evaporation_pot_effective
      // This is temporary until I work out what should be effective and what should be the dynamic remaining
 public: // Required submodel
   Soil::Infiltration_nominal *infiltration_submodel;                            //071012
 public: // output control
   bool structure_defined;                                                       //030801
 public:  // setup and initialization
   Soil_base                                                                     //061005
      (const Smart_soil_parameters  *parameters                                  //160916
      ,bool parameters_given                                                     //160916
      ,const CORN::date32                 &today_);                              //170524
   Soil_base                                                                     //140609
      (const Soil_base &copy_from
      ,float32 fraction);
      // Copy constructure used in directed irrigation.
   virtual ~Soil_base();                                                         //030710
   #if (CROPSYST_VERSION==4)
   // V5 will use inspection
   bool setup_structure
      (CORN::Data_record &data_rec,bool for_write);
   #endif

   virtual const char *get_record_section_label()  const { return "soil_base"; }
   virtual void expand_last_layer_for_boundary_conditions                        //080130
      (nat8  number_layers,nat8  number_expanded_layers)          modification_; //080211
 public:
   virtual void subdivide_horizon
      (nat8  horizon
      ,nat8  sublayer
      ,float64 sublayer_thickness
      ,float64 sublayer_depth
      #ifdef USE_SOIL_PROPERTIES_SUBLAYERS
      #else
      ,Moisture_characterization moisture                                        //170320
      ,float64 H2O_l                                                             //990305
      ,bool beginning_of_simulation
      #endif
      );
   virtual bool reinitialize
      (Moisture_characterization moisture                                        //170320
      ,soil_horizon_array32(H2O) // not const because initialized to avgWC when 0 values 140311
      ,bool beginning_of_simulation
      ,bool subdivide_horizons);                                                 //060229
   virtual bool reinitialize_for_crop(float64 wilt_leaf_water_pot);
 public: // initialization
   virtual bool set_infiltration_hydrology //161005 initialize                                                       //980801
      (Infiltration_model          _infiltration_model // default CASCADE_MODEL  //041116
      ,Soil::Infiltration_nominal  *_infiltration_submodel                       //071012
      ,Soil::Hydrology_abstract    *_hydrology);                                 //060512
         // Optional if hourly resolution is required
 public: // status accessors
   virtual bool using_numerical_runoff_model()            const { return false;} //041117
 public: // Write accessor functions
 public: // Read accessor functions
   inline virtual float64 get_rooting_depth() const { return layers->get_depth_m(2);}  //981210
   virtual float64 get_reported_drainage()                                const; //990407
   virtual float64 get_infiltration()                                     const; //070118
 public: // Darcian public methods                                               //990214
   #ifdef SOIL_INFILTRATION
   virtual bool infiltrate
      (Infiltration_model            infiltration_model
      ,float64                       water_entering_soil_not_irrigation          //070107
      ,float64                       management_irrigation
      ,float64                       management_auto_irrigation_concentration
      ,Dynamic_water_entering_soil  *non_runoff_water_entering_in_intervals_mm   //070119
      ,Dynamic_water_entering_soil  *irrigation_water_entering_in_intervals_mm   // optional (may be 0) //070119
      ,const Dynamic_array<float32> &runon_in_intervals                          //990409
      ,Dynamic_array<float32>       &runoff_in_intervals);                       //990409
   #endif
   // Water tables currently only recognized in finite difference model.
 public: // External influences on soil water
   inline virtual modifiable_ Soil::Evaporator *mod_evaporator()   modification_ //160606
      { return evaporator; }
   inline virtual const Soil::Evaporator *ref_evaporator()                 const //160606
      { return evaporator; }
 public: // variables for lateral flow
   float64 lateral_flow_in;  // sum for all layers                               //990525
   float64 lateral_flow_out; // sum for all layers                               //990525
 public: // methods for lateral flow.
   void subsurface_flow_in(soil_sublayer_array_64(lateral_flow_in));
   bool subsurface_flow_out(float64 cell_width, soil_sublayer_array_64(lateral_flow_depth_m));
   // returns true if there was flow in any sublayer.
 public: // methods for working with sublayers

#define copy_sublayer_array(t,s) copy_array64(t,s)

   virtual void clear_sublayer_array(soil_sublayer_array_64(target))      const;//161008
   // This is the number of sublayers identified in the soil profile.
   nat8  horizon_at(float64 a_given_depth )                               const;
   /*190707 unused/obsolete
   virtual float64 tally_to_depth
      (soil_sublayer_array_64(values), float64 a_given_depth, bool get_average)  const; //990208
   */
   float64 current_plant_available_water(const soil_sublayer_array_64(fract_root_length)) modification_;
      //There return value was never used
public:  // chemcical methods to be overridden
   inline virtual void chemical_transport_and_exchange                           //980724
      (nat8                   num_sublayers_eff
      ,uint32                 time_step                                          //990320
      ,soil_sublayer_array_64(old_water_content)
      ,soil_sublayer_array_64(new_water_content)
      ,float64                water_entering  // total water entering (irrig + precip)
      ,float64                irrigation
      ,bool                   override_leaching_depth                           //981230
      ,nat8                   leaching_sublayer)                                 //981230
      {
         UNUSED_arg(num_sublayers_eff); UNUSED_arg(time_step);
         UNUSED_arg(old_water_content); UNUSED_arg(new_water_content);
         UNUSED_arg(water_entering);    UNUSED_arg(irrigation);
         UNUSED_arg(override_leaching_depth); UNUSED_arg(leaching_sublayer);
      } // This will be derived in chemical model
   inline virtual   modifiable_ Soil::Nitrogen *mod_nitrogen()     modification_ { return 0; } //160412_060206
   inline virtual unmodifiable_ Soil::Nitrogen *ref_nitrogen()             const { return 0; } //160412_060206
   inline virtual unmodifiable_ Soil::Salt_interface *ref_salinity()       const { return 0; } //160412
   inline virtual   modifiable_ Soil::Salt_interface *mod_salinity() modification_{ return 0; } //160412_060206
   inline virtual bool set_salinity(Soil::Salt_interface *salinity,bool keep_it = true)
      {  UNUSED_arg(salinity); UNUSED_arg(keep_it); return false; }                      //051120
   inline virtual float64 get_temperature_C(nat8  sublayer)                const
      { UNUSED_arg(sublayer); return 0.0;}
   virtual bool start_day();                                                     //051206
   virtual bool end_day();                                                       //160719
   inline virtual bool process_day()                modification_{ return true;} //150611_051206
   inline virtual float64 get_salt_stress_function
      (nat8  sublayer,float64 osmotic_pot_50,float64 salt_tolerance_P )    const
      {  UNUSED_arg(sublayer); UNUSED_arg(osmotic_pot_50); UNUSED_arg(salt_tolerance_P);
         return 1.0; }
      // This will be overloaded by the soil chemical profile class
      // When not simulating chemicals or salinity salt stress has no effect and this function is 1.0
   inline virtual float64 get_osmotic_pot_solution(nat8 sublayer)          const
      { UNUSED_arg(sublayer); return 0.0; }
     // This will be 0 when not running salinity
   float64 recalibrate_water_content                                             //011119
      (const Dynamic_array<float32>  &WC_values_by_layer);
      /**< values are by layer!
       \return the change in soil profile water depth to be fed to water balance.
       \return the amount of change in water content
      */
   float64 recalibrate_plant_available_water                                     //000202
      (return_ Dynamic_array<float32> &PAW_values_by_layer);                     //011119
      /**< values are by layer!
       \return the change in soil profile water depth to be fed to water balance.
       \return the amount of change in water content
      */
   bool initialize_water_content_from_PAW(float32 soil_profile_plant_available_water);                    //010520
      // This is used by GIS mode, but may be useful for other simulation scenarios
      // when detailed initial water content is not available.
   inline virtual void add_runoff_from_infiltration(float64 additional_runoff)
      { UNUSED_arg(additional_runoff); } //990214
   void output_layering(UED::Database_file_indexed  &output_database)     const;
   void output_hydraulic_properties(UED::Database_file_indexed &output_database) const; //070209

      // 170315 I think the output_database could be Database_file_indexed

public:  // These accessors are currently duplicated in the Soil_cropsyst_specific because the compiler is complaining
   virtual float64 get_water_content_at(nat8  sublayer,float64 potential) const; //991119
   inline virtual float64 get_pH(nat8  sublayer)                          const  { return parameters_ownable->get_pH(layers->in_horizon_of[sublayer]); }  //011130
   inline virtual float64 get_cation_exchange_capacity(nat8 sublayer)     const  { return parameters_ownable->get_cation_exchange_capacity(sublayer);}    //150820LML talk with Roger
   inline virtual float64 get_plant_avail_water(nat8  sublayer)           const  { return ref_hydrology()->get_plant_avail_water(sublayer); }
   virtual USDA_NRCS::Hydrologic_group get_hydrologic_group()             const;
   inline virtual int16 get_override_curve_number()                       const  { return parameters_ownable->override_curve_number_16;}            //99021
   inline virtual int16 get_user_fallow_curve_number()                    const  { return parameters_ownable->user_fallow_curve_number_16;}         //99021
   inline virtual bool get_run_salinity()                                 const  { return false; }                                          //99022
   virtual float64 get_albedo()                                           const; //041203
public: // Soil parameter accessors added for Compatibility with CropGro    //031122
   virtual float64 get_SLPF()                                             const  { return 0.92; }
   // Currently this is a constant we have not decided where to make this parameter from
public: // Soil profile parameter accessors (sublayer number is 1 based)
   virtual float64 get_root_hospitality_factor(nat8  sublayer)             const
      {  UNUSED_arg(sublayer); return 1.0; }
   // Currently this is a constant we have not decided where to make this parameter from
   // Note in the following cm3/cm3 is the same as m3/m3
 public: // Soil_interface overrides
   inline virtual float64 get_clay_percent          (nat8  sublayer)       const { return parameters_ownable->get_horizon_clay(layers->in_horizon_of[sublayer]);}              //060206
   inline virtual float64 get_sand_percent          (nat8  sublayer)       const { return parameters_ownable->get_horizon_sand(layers->in_horizon_of[sublayer]);}              //060206
   inline virtual float64 get_silt_percent          (nat8  sublayer)       const { return parameters_ownable->get_horizon_silt(layers->in_horizon_of[sublayer]);}              //060206
   inline virtual const         Soil::Layers_interface    *ref_layers()    const { return layers;}         //051120
   inline virtual modification_ Soil::Layers_interface    *mod_layers() modification_ { return layers;}         //160412_051120
   inline virtual const         Soil::Hydrology_interface *ref_hydrology() const { return hydrology; }         //160412_060504
   inline virtual               Soil::Hydrology_interface *mod_hydrology() modification_{ return hydrology; }   //060504
   virtual const                Soil::Hydrology_interface *ref_hydrology_at(nat16 seconds) const;               //060504
   virtual Seconds get_hydrology_timestep_resolution_today()              const;                              //060510
   inline virtual const         Soil::Texture_interface *ref_texture()    const { return &texture; }          //160412_051201
   inline virtual modification_ Soil::Texture_interface *mod_texture() modification_{ return &texture; }      //160412_051201
      // Returns the soil texture parameters object.
   inline virtual unmodifiable_ Soil::Structure *ref_structure()           const { return structure; }   //160412_070725
   inline virtual   modifiable_ Soil::Structure *mod_structure()   modification_ { return structure; }   //160412_070725
   inline virtual unmodifiable_ Soil::Hydraulic_properties_interface *ref_hydraulic_properties() const { return hydraulic_properties; }          //160412_051201
   inline virtual   modifiable_ Soil::Hydraulic_properties_interface *mod_hydraulic_properties() modification_ { return hydraulic_properties; }  //160412_051201
      // Returns the soil properties that are commonly derived from soil survey data
      // and/or pedo transfer functions.

   virtual float64 get_act_water_entering_soil()                          const; //150925
   virtual modifiable_ float64 &mod_act_water_entering_soil();                   //160412_150930
 public:
   inline const Soil_sublayers *ref_sublayers()                           const  { return layers; }   //160412_070607
      // In CropSyst soil layer inputs are horizon values
      // these need to be converted to the sublayering system.

   //Note that the following two methods are deprecated, moved to Soil_sublayers //070607

   virtual float64 distribute_amount
      (const soil_horizon_array32(from_horizon_amount)
      ,soil_sublayer_array_64(to_sublayer_amount))                        const; //051120
      // This is used for amounts which are divided into the respetive sublayers for each horizon
      // Returns the total amount for the profile.
   virtual float64 distribute_property
      (const soil_horizon_array32(from_horizon_property)
      ,soil_sublayer_array_64(to_sublayer_property))                      const; //051120
      // This is used for percentage and concentrations where the percentage or  concentration for the sublayer is the same as the horizon.
      // Currently returns 0.0 (not sure what useful information could be return (maybe mean concentration?)
   inline virtual bool submit_evaporator(Soil::Evaporator *evaporator_)
   {  // NYN delete evaporator
      evaporator = evaporator_;
      evaporator_owned = true;
      return true;
   }
   RENDER_INSPECTORS_DECLARATION;                                                //150916
   contribute_ CS::Emanator *emanator_known;                                     //160616
   inline virtual const CS::Emanator *get_emanator()                       const { return emanator_known; }  //170413
   inline virtual const CS::Emanator *get_emanator_annual()                const { return emanator_known; }  //170413
   inline const char *get_ontology()                                       const { return "soil"; } //160606
};
//______________________________________________________________________________
#endif
//soilbase.h
//060727 588 lines
//070607 490 lines

