#ifndef soil_param_classH
#define soil_param_classH
//______________________________________________________________________________
#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include "common/soil/layering.h"
#include "common/soil/SCS/SCS.h"
#include "corn/parameters/parameters_datarec.h"
#ifndef MICROBASIN_VERSION
#  include "corn/OS/directory_entry_name_concrete.h"
#endif
#include "corn/measure/measures.h"
#include "corn/dynamic_array/dynamic_array_T.h"
//______________________________________________________________________________
#define LABEL_soil                     "soil"
#define LABEL_bypass_coef              "bypass_coef"
#define LABEL_bulk_density             "bulk_density"
#define LABEL_cation_exchange_capacity "cation_exchange_capacity"
#define LABEL_field_capacity           "field_capacity"
#define LABEL_override_curve_number    "override_curve_number"
#define LABEL_fallow_curve_number      "fallow_curve_number"
#define LABEL_surface_storage          "surface_storage"
#define LABEL_compute_surface_storage  "compute_surface_storage"
#define LABEL_hydrologic_condition     "hydrologic_condition"
#define LABEL_air_entry_pot            "air_entry_pot"
#define LABEL_b                        "b"
#define LABEL_vol_WC_at_1500           "vol_WC_at_1500"
#define LABEL_vol_WC_at_33             "vol_WC_at_33"
#define LABEL_override_leaching_depth  "override_leaching_depth"
#define LABEL_leaching_depth           "leaching_depth"
#define LABEL_fixed_evaporative_layer  "fixed_evaporative_layer"
#define LABEL_hydrologic_group         "hydrologic_group"
#define LABEL_number_layers            "number_layers"
#define LABEL_sand      "sand"
#define LABEL_clay      "clay"
#define LABEL_silt      "silt"
#define LABEL_perm_wilt_point   "perm_wilt_point"
#define LABEL_pH                "pH"
#define LABEL_profile           "profile"
#define LABEL_slope_length      "slope_length"
#define LABEL_steepness         "steepness"
#define LABEL_thickness          "thickness"
#define LABEL_user_value        "user_value"
#define LABEL_sat_hydraul_cond  "sat_hydraul_cond"
#define LABEL_organic_matter     "organic_matter"
#define DEF_Campbell_b 6.54
#define ENTIRE_PROFILE_DEPTH 0.0
//______________________________________________________________________________
class SoilDialog; // Forward for friend class  // Version 3
class Tsoil_editor_form;                 // Version 4
class LADSS_soil_data_source; // forward for friend
class Tsoil_convertor_form;
class Soil_texture_hydraulics;                                                   //150411

using namespace CORN;                                                            //061005
//______________________________________________________________________________
struct Langmuir
{
   CORN::Dynamic_array<float32> K; // kg water/kg solute
   CORN::Dynamic_array<float32> Q; // kg solute/kg soil
 public:
   inline Langmuir
      (float32 default_K
      ,float32 default_Q)
      : K(MAX_soil_horizons,5,default_K)
      , Q(MAX_soil_horizons,5,default_Q)
      {}
   inline bool clear()
   {  return K.clear()
          && Q.clear();
   }
   inline bool remove_at(nat32 index)
   {  K.remove_at(index);
      Q.remove_at(index);
      return true;
   }
   inline bool insert_at(nat32 index)
   {
      // I dont currently have delete_at method in dynamic array
      // but this currently isnt used
      //USED_arg(index);
      assert(false);
      return false;
   }
   inline bool copy(nat32 from_index,nat32 to_index)
   {  K.set(to_index,K.get(from_index));
      Q.set(to_index,Q.get(from_index));
      return true;
   }
};
//_Langmuir_________________________________________________________2019-04-04_/
class Soil_parameters_class_common //was Soil_parameters_class                   //181206
: public Common_parameters_data_record
{
   friend class SoilDialog;         // Version 3                                 //981030
   friend class Tsoil_editor_form;  // Version 4                                 //020508
   friend class LADSS_soil_data_source;                                          //020613
   friend class Tsoil_convertor_form;                                            //060110
 public: // These may now either be user specified values or computed by the smart_soil_file class
   enum user_specified_fields { all_computer_generated      =   0                //981030
                              , specified_texture           =   1
                              , specified_FC                =   2
                              , specified_PWP               =   4
                              , specified_bulk_density      =   8
                              , specified_water_pot_at_FC   =  16                //130326
                         //???    , specified_water_pot_at_PWP  =  32                //130326
                              , specified_sat_hydraul_cond  =  64
                              , specified_air_entry_pot     = 128 // currently either present or not (always calculated) 990218X
                              , specified_soil_B            = 256 // currently either present or not  990218X
                              , specified_sand              = 512
                              , specified_clay              =1024
                              , specified_silt              =2048
                              , specified_saturation        =4096                //080313
                              };
 public:  // so STATSGO_Database_interpretor::Map_unit_interpretor can use it (not sure how to forward declare nested classes for frienship
   nat8        horizon_count;                                                    //080207
   float32     evaporative_layer_thickness_cascade;                              //120724
   float32     evaporative_layer_thickness_finite_difference;                    //120724
      //meter
 public:
   contribute_ int16 user[MAX_soil_horizons_alloc]; // user specified fields marker (used for user interface to lock user entered values) //981030
 public:
//110217 already in Common_parameters_data_record /*_______*/    CORN::Ustring description;
   USDA_NRCS::Hydrologic_group_clad     hydrologic_group_clad;
   Hydrologic_condition_clad hydrologic_condition_clad;  // moved from soil   //971011
   int16    override_curve_number_16;                                            // runoff curve number override (also for models without hydro cond/group) _990213
   int16    user_fallow_curve_number_16;                                         //990213
   bool     compute_surface_storage;                                             //990422
   float32  surface_storage_mm_32;                                               //990213
   float32  steepness_percent; // percent 0-100 (also slope)  //071117 was steepness_32
   float32  slope_length;  // meters  if simulating a field, you can use the average distance from any point in the field to a channel
   float32  albedo_dry;    // albedo when dry  (fraction 0-1)                    //041203
   float32  albedo_wet;    // albedo when wet  (fraction 0-1)                    //041203
   float32  water_vapor_conductance_atmosphere_adj;         // 080213  This may be obsolete
   float32  SLPF_CropGro;  //   Photosynthesis factor (0-1)  1.0 = no limitation. used only for CropGro 071016
//NYN     soil_horizon_array32(water_holding_capacity_m);   // added by RLN for AgroClimatic project but probably of general use
 public: // These two parameters were special for the Agroclimate simulations    //080728RLN
   float32 capability_class_irrigated;                                           //080728
   float32 capability_class_dryland;                                             //080728
   bool    agricultural_dryland;                                                 //080728
   bool    agricultural_irrigated;                                               //080728
   float32 water_holding_capacity_to_1m;    // (value meter depth) to 1.5meter   //080728
   float32 aggregated_water_holding_capacity_to_1m;                              //080728
 public: // The following are optional for recording a selected STATSGO soil (This is not used at run time)
   struct STATSGO_support
   {
      #ifndef MICROBASIN_VERSION
      CORN::OS::Directory_name_concrete path;                                    //050921
         // I.e. C:\Simulation\Database\STATSGO
      #endif
      std::string              mukey;                                            //110216
      std::string              cokey;                                            //110216
      int16                    COMPPCT;                                          //080303
   } STATSGO;
   bool bound_by_bedrock;    //160923 rename to aquiclude
   bool                       save_only_specified_layers;                        //060129
   // Normally all possible layers are saved to the file                         //060129
   // (so that the user can temporarily change the soil layers)                  //060129
   // Soil generator programs dont need to save all the layers.                 //060129
   Langmuir NH4_Langmuir;                                                        //190402
 protected: // Derived file classes can use these methods to setup for prior version compatibility
   void  set_compatible();                                                       //010109
   void  get_compatible();                                                       //010109
   virtual void copy_horizon(nat8 from_horizon,nat8 to_horizon)              =0; //160915
 public: // Constructors
   Soil_parameters_class_common();
 public:
   virtual bool expect_structure(bool for_write = false);                        //161025_981103
   inline virtual const char *get_primary_section_name()                  const  { return LABEL_soil;}   //020911
   inline virtual const char *get_type_label()                            const  { return "CropSyst soil";} //051011_
 public: // Accessors
   inline virtual float64 get_steepness_percent()                         const  { return (float64)steepness_percent ;}   //981231
   inline virtual float64 get_slope_length()                              const  { return (float64)slope_length;}         //981231
   inline void set_steepness_percent(float64 new_steepness_percent)              { steepness_percent = (float32)new_steepness_percent; }   //990721

   virtual bool delete_horizon(nat8 horizon); // This is intended for the parameter editor  //041214_
   virtual bool insert_horizon(nat8 horizon); // This is intended for the parameter editor  //041214_
   virtual nat8  get_horizon_at
      (float64 a_given_depth_m = ENTIRE_PROFILE_DEPTH)                    const; //060127
   virtual float64 get_horizon_center_depth(nat8 horizon)                 const; //130207
   virtual float64 get_horizon_depth(nat8 horizon)                        const; //130207
   inline virtual float64 get_surface_storage_mm()                        const  { return (float64)surface_storage_mm_32;} //990213X

   nat8 limit_profile_depth(float32 soil_profile_depth_limit)     modification_;
      // This limits the soil profile to the specified depth.
      // [CropSyst provides a scenario model option to override
      //  the soil profile depth specified in the soil file).
      // The (bottom) soil layer thicknesses are set to zero
      // until the last layer would contain the specified depth.
      // the last layer thickness is adjusted accordingly.
      // Does nothing if soil_profile_depth_limit is approximately 0.
      // Returns the new number of layers.
   virtual float32 set_horizon_thickness(nat8 horizon,float32 horizon_thickness_new) = 0;//170609
   virtual void set_horizons_thickness(soil_horizon_array32(horizon_thickness_32)) = 0 ; // Used for GIS projects //990929
   // horizon accessor abstract

   virtual float64 get_cation_exchange_capacity          (nat8 horizon) const=0; //160915
   virtual float64 get_pH                                (nat8 horizon) const=0; //160915

   //  The following are not frequently accessed, so I would create 32bit versions now
   virtual float32 get_horizon_sand                      (nat8 horizon) const=0; //160915
   virtual float32 get_horizon_clay                      (nat8 horizon) const=0; //160915
   virtual float32 get_horizon_silt                      (nat8 horizon) const=0; //160915
   virtual float32 get_horizon_organic_matter            (nat8 horizon) const=0; //160915

   virtual float64 get_horizon_thickness                 (nat8 horizon) const=0; //160915
   virtual float64 get_horizon_bulk_density_g_cm3        (nat8 horizon) const=0; //160915
   virtual float64 get_horizon_bulk_density_kg_m3        (nat8 horizon) const=0; //160915
   virtual float64 get_horizon_permanent_wilt_point_volumetric(nat8 horizon)  const=0; //160915
   virtual float64 get_horizon_field_capacity_volumetric (nat8 horizon) const=0; //160915
   virtual float64 get_horizon_water_pot_at_FC           (nat8 horizon) const=0; //160915
   virtual float64 get_horizon_bypass_coef               (nat8 horizon) const=0; //160915
   virtual float64 get_horizon_sat_hydraul_cond_m_d      (nat8 horizon) const=0; //160915
   //160921 NYN virtual float64 get_horizon_sat_hydraul_cond_lateral_flow_m_d  (nat8 horizon) const = 0; //160921
   virtual float64 get_horizon_saturation_WC             (nat8 horizon) const=0; //160915
   virtual float64 get_horizon_air_entry_pot             (nat8 horizon) const=0; //160915
   virtual float64 get_horizon_Campbell_b                (nat8 horizon) const=0; //160915
 public:
   modifiable_ Soil_texture_hydraulics  *mod_texture_hydraulics(nat8 layer)      //150411
      { UNUSED_arg(layer); return 0;}
      // This was added for Microbasin model which doesn't need texture hydraulics.
};
//_class Soil_parameters_class_common__________________________________________/
#define DEF_pH  7.0
#define DEF_cation_exchange_capacity  10.0

//______________________________________________________________________________
namespace version4
{
//______________________________________________________________________________
class Soil_parameters_class
: public extends_ ::Soil_parameters_class_common //181206 Soil_parameters_class
{
 public:
   soil_horizon_array32(horizon_bulk_density_g_cm3);      // tonne/m3 = g/cm3
   soil_horizon_array32(horizon_permanent_wilt_point);   // m3/m3 = cm3/cm3
   soil_horizon_array32(horizon_field_capacity);    // m3/m3 = cm3/cm3
   soil_horizon_array32(horizon_water_pot_at_FC);                                //130127
   soil_horizon_array32(horizon_sand);              // %  // 060724 was sand_32  //970505
   soil_horizon_array32(horizon_clay);              // %  // 060724 was clay_32  //970505
   soil_horizon_array32(horizon_silt);              // %  // 060724 was silt_32  //970505
   soil_horizon_array32(horizon_sat_hydraul_cond_m_d); // m/day                  //980528
   //NYN soil_horizon_array32(horizon_sat_hydraul_lateral_flow_cond_m_d); // m/day     //160921
   soil_horizon_array32(horizon_saturation_WC);         // m3/m3 saturation water content (This used to always be computed during the run, but many of the V.B. models have this as an input).  //080313
   soil_horizon_array32(horizon_air_entry_pot);// J/kg                           //980528
   soil_horizon_array32(horizon_Campbell_b);            // -  The Campbell b is not absolutely required, if 0.0, it will be calculated from PWP and FC   //980528
   soil_horizon_array32(horizon_pH);                  // Soil pH in water  (not buffer)  //011130
   soil_horizon_array32(horizon_cation_exchange_capacity);   // (cMol_c/kg soil) centimole of positive char/ kilogram soil (equivelent to meq/100g) milliequivelents of cations per 100 grams soil  For ammonium volitalization 011130
   soil_horizon_array32(horizon_organic_matter);        // percent. 0 if unknown.        //060130
   soil_horizon_array32(horizon_organic_matter_low);    // percent. 0 if unknown. This is usually from STATSGO (it specifies a range of values for the component (series))   //060130
   soil_horizon_array32(horizon_organic_matter_high);   // percent. 0 if unknown. This is usually from STATSGO (it specifies a range of values for the component (series))   //060130
   soil_horizon_array32(horizon_thickness); // m
   soil_horizon_array32(horizon_bypass_coef);     // Not entered as a parameter, copied from init H2O,eventauly delete
 public: // structors
   Soil_parameters_class();
 public: // accessors
   virtual float32 set_horizon_thickness(nat8 horizon,float32 horizon_thickness_new);//170609
   virtual void set_horizons_thickness(soil_horizon_array32(horizon_thickness_32)); // Used for GIS projects //990929
   inline virtual float64 get_cation_exchange_capacity(nat8 horizon)       const { return (float64)horizon_cation_exchange_capacity[horizon];}  //011120_
   inline virtual float64 get_pH(nat8 horizon)                             const { return (float64)horizon_pH[horizon]; }         //011130_

   //  The following are not frequently accessed, so I would create 32bit versions now
   inline virtual float32 get_horizon_sand                  (nat8 horizon) const { return horizon_sand[horizon]; }
   inline virtual float32 get_horizon_clay                  (nat8 horizon) const { return horizon_clay[horizon]; }
   inline virtual float32 get_horizon_silt                  (nat8 horizon) const { return horizon_silt[horizon]; }
   inline virtual float32 get_horizon_organic_matter        (nat8 horizon) const { return horizon_organic_matter[horizon]; } //160915

   inline virtual float64 get_horizon_thickness             (nat8 horizon) const { return (float64)horizon_thickness[horizon];}  // m //981119
   inline virtual float64 get_horizon_bulk_density_g_cm3    (nat8 horizon) const { return (float64)horizon_bulk_density_g_cm3[horizon];}     // tonne/m3 = g/cm3
   inline virtual float64 get_horizon_bulk_density_kg_m3    (nat8 horizon) const { return (float64)g_cm3_to_kg_m3(horizon_bulk_density_g_cm3[horizon]);}     // tonne/m3 = g/cm3
   inline virtual float64 get_horizon_permanent_wilt_point_volumetric(nat8 horizon)  const { return (float64)horizon_permanent_wilt_point[horizon];}  // m3/m3
   inline virtual float64 get_horizon_field_capacity_volumetric(nat8 horizon)        const { return (float64)horizon_field_capacity[horizon];}   // m3/m3
   inline virtual float64 get_horizon_water_pot_at_FC(nat8 horizon)        const { return (float64)horizon_water_pot_at_FC[horizon]; }
   inline virtual float64 get_horizon_bypass_coef           (nat8 horizon) const { return (float64)horizon_bypass_coef[horizon];}      // m3/m3 //981119
   inline virtual float64 get_horizon_sat_hydraul_cond_m_d  (nat8 horizon) const { return (float64)horizon_sat_hydraul_cond_m_d[horizon];}   // m/day //980528p
   //NYN inline virtual float64 get_horizon_sat_hydraul_cond_lateral_flow_m_d  (nat8 horizon) const { return (float64)horizon_sat_hydraul_cond_lateral_flow_m_d[horizon];}   // m/day //160921
   inline virtual float64 get_horizon_saturation_WC         (nat8 horizon) const { return (float64)horizon_saturation_WC[horizon];}   //080313
   inline virtual float64 get_horizon_air_entry_pot         (nat8 horizon) const { return (float64)horizon_air_entry_pot[horizon];}        // J/kg   //980528p
   inline virtual float64 get_horizon_Campbell_b            (nat8 horizon) const { return (float64)horizon_Campbell_b[horizon];}               // - //980528p
 public:
   virtual void clear();                                                         //070516
   virtual bool expect_structure(bool for_write = false);                        //161025_981103
   virtual bool delete_horizon(nat8 horizon); // This is intended for the parameter editor  //041214
   virtual bool insert_horizon(nat8 horizon); // This is intended for the parameter editor  //041214
   virtual void copy_horizon(nat8 from_horizon,nat8 to_horizon);
};
//_Soil_parameters_class_______________________________________________________/
}//_namespace version__________________________________________________________/

namespace version5
{
//______________________________________________________________________________
class Soil_parameters_class
: public ::Soil_parameters_class_common //181206 Soil_parameters_class ::Soil_parameters_class
{
 public:  // so STATSGO_Database_interpretor::Map_unit_interpretor can use it (not sure how to forward declare nested classes for frienship
   struct Horizon_properties
   {
      CORN::Dynamic_array<float32> thickness; // m
      CORN::Dynamic_array<float32> bulk_density_g_cm3;      // tonne/m3 = g/cm3
      CORN::Dynamic_array<float32> permanent_wilt_point;   // m3/m3 = cm3/cm3
      CORN::Dynamic_array<float32> field_capacity;    // m3/m3 = cm3/cm3
      CORN::Dynamic_array<float32> water_pot_at_FC;                                 //130127
      CORN::Dynamic_array<float32> sand;              // %  // 060724 was sand_32   //970505
      CORN::Dynamic_array<float32> clay;              // %  // 060724 was clay_32   //970505
      CORN::Dynamic_array<float32> silt;              // %  // 060724 was silt_32   //970505
      CORN::Dynamic_array<float32> sat_hydraul_cond_m_d; // m/day                   //980528
      //NYN 160921 CORN::Dynamic_array<float32> sat_hydraul_cond_lateral_flow_m_d; // m/day      //160921
      CORN::Dynamic_array<float32> saturation_WC;         // m3/m3 saturation water content (This used to always be computed during the run, but many of the V.B. models have this as an input).  //080313
      CORN::Dynamic_array<float32> air_entry_pot;// J/kg                           //980528
      CORN::Dynamic_array<float32> Campbell_b;            // -  The Campbell b is not absolutely required, if 0.0, it will be calculated from PWP and FC   //980528
      CORN::Dynamic_array<float32> pH;                  // Soil pH in water  (not buffer)  //011130
      CORN::Dynamic_array<float32> cation_exchange_capacity;   // (cMol_c/kg soil) centimole of positive char/ kilogram soil (equivelent to meq/100g) milliequivelents of cations per 100 grams soil  For ammonium volitalization 011130
      CORN::Dynamic_array<float32> organic_matter;        // percent. 0 if unknown.        //060130
      CORN::Dynamic_array<float32> organic_matter_low;    // percent. 0 if unknown. This is usually from STATSGO (it specifies a range of values for the component (series))   //060130
      CORN::Dynamic_array<float32> organic_matter_high;   // percent. 0 if unknown. This is usually from STATSGO (it specifies a range of values for the component (series))   //060130
      CORN::Dynamic_array<float32> bypass_coef;     // Not entered as a parameter, copied from init H2O,eventauly delete
    public:
      Horizon_properties();
   } properties;
 public: // Constructors
   Soil_parameters_class();
 public:
   virtual bool expect_structure(bool for_write = false);                        //161025_981103
 public: // Accessors
   virtual float32 set_horizon_thickness (nat8 horizon,float32 horizon_thickness_new);//170609
   virtual void set_horizons_thickness(soil_horizon_array32(horizon_thickness_32)); // Used for GIS projects //990929

//   inline virtual float64 get_steepness_percent()                          const { return (float64)steepness_percent ;}   //981231
//   inline virtual float64 get_slope_length()                               const { return (float64)slope_length;}         //981231
   inline virtual float64 get_cation_exchange_capacity(nat8 horizon)       const { return (float64)properties.cation_exchange_capacity.get(horizon);}  //011120_
   inline virtual float64 get_pH(nat8 horizon)                             const { return (float64)properties.pH.get(horizon); }         //011130_

   //  The following are not frequently accessed, so I would create 32bit versions now
   inline virtual float32 get_horizon_sand                  (nat8 horizon) const { return properties.sand.get(horizon); }
   inline virtual float32 get_horizon_clay                  (nat8 horizon) const { return properties.clay.get(horizon); }
   inline virtual float32 get_horizon_silt                  (nat8 horizon) const { return properties.silt.get(horizon); }
   inline virtual float32 get_horizon_organic_matter        (nat8 horizon) const { return properties.organic_matter.get(horizon); } //160915

   inline virtual float64 get_horizon_thickness             (nat8 horizon) const { return (float64)properties.thickness.get(horizon);}  // m //981119
   inline virtual float64 get_horizon_bulk_density_g_cm3    (nat8 horizon) const { return (float64)properties.bulk_density_g_cm3.get(horizon);}     // tonne/m3 = g/cm3
   inline virtual float64 get_horizon_bulk_density_kg_m3    (nat8 horizon) const { return (float64)g_cm3_to_kg_m3(properties.bulk_density_g_cm3.get(horizon));}     // tonne/m3 = g/cm3
   inline virtual float64 get_horizon_permanent_wilt_point_volumetric(nat8 horizon)  const { return (float64)properties.permanent_wilt_point.get(horizon);}  // m3/m3
   inline virtual float64 get_horizon_field_capacity_volumetric(nat8 horizon)        const { return (float64)properties.field_capacity.get(horizon);}   // m3/m3
   inline virtual float64 get_horizon_water_pot_at_FC       (nat8 horizon) const { return (float64)properties.water_pot_at_FC.get(horizon); }
   inline virtual float64 get_horizon_bypass_coef           (nat8 horizon) const { return (float64)properties.bypass_coef.get(horizon);}      // m3/m3 //981119
   inline virtual float64 get_horizon_sat_hydraul_cond_m_d  (nat8 horizon) const { return (float64)properties.sat_hydraul_cond_m_d.get(horizon);}   // m/day //980528p
   //NYN 160921 inline virtual float64 get_horizon_sat_hydraul_cond_lateral_flow_m_d  (nat8 horizon) const { return (float64)properties.sat_hydraul_cond_m_d.get(horizon);}   // m/day //160921
   inline virtual float64 get_horizon_saturation_WC         (nat8 horizon) const { return (float64)properties.saturation_WC.get(horizon);}   //080313
   inline virtual float64 get_horizon_air_entry_pot         (nat8 horizon) const { return (float64)properties.air_entry_pot.get(horizon);}   // J/kg   //980528p
   inline virtual float64 get_horizon_Campbell_b            (nat8 horizon) const { return (float64)properties.Campbell_b.get(horizon);}      // - //980528p
   virtual bool delete_horizon(nat8 horizon); // This is intended for the parameter editor  //041214_
   virtual bool insert_horizon(nat8 horizon); // This is intended for the parameter editor  //041214_

   inline virtual void copy_horizon(nat8 from_horizon,nat8 to_horizon)
      { UNUSED_arg(from_horizon); UNUSED_arg(to_horizon); return ; }
      //160915 shouldn't be needed
};
//_Soil_parameters_class(V5)___________________________________________________/
class Soil_parameters_dynamic
{  // These are soil p
};
//_Soil_parameters_dynamic_____________________________________________________/
#if (CROPSYST_VERSION==4)
#define Soil_parameters_class_parent version4::Soil_parameters_class
#else
#define Soil_parameters_class_parent version5::Soil_parameters_class
#endif
}//_namespace version_5________________________________________________________/

#endif

