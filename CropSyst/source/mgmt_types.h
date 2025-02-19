#ifndef mgmt_typesH
#define mgmt_typesH
#include "options.h"
#include "corn/enum.h"
#include "common/soil/layering.h"
#include <fstream>
#define LABEL_disabled              "disabled"
#define LABEL_optimal               "optimal"
#define LABEL_V3_clipping_fate      "clipping_fate"
#define LABEL_biomass               "biomass"
#define LABEL_periodic              "periodic"
#define LABEL_tillage                "tillage"
#define LABEL_residue                "residue"
#define LABEL_irrigation             "irrigation"
#define LABEL_fertilization          "fertilization"
#define LABEL_chemical               "chemical"
#define LABEL_organic_fertilization  "organic_fertilization"
#define LABEL_clipping               "clipping"
#define LABEL_auto_N_application     "auto_N_application"
#define LABEL_local_calibration      "local_calibration"
#define LABEL_balance_based          "balance_based"
//______________________________________________________________________________
extern const char *irrigation_application_mode_label_table[];
enum Irrigation_application_mode
   {irrigate_fixed_amount
   ,irrigate_soil_condition         // <- probably rename to something like irrigate refill
   };
DECLARE_ENUM_CLAD(Irrigation_application_mode_clad,Irrigation_application_mode,irrigation_application_mode_label_table);
//_Irrigation_application_mode______________________________________2004-02-08_/
extern const char *irrigation_consideration_mode_label_table[];
enum Irrigation_consideration_mode
{  consider_soil_water_depletion
,  consider_leaf_water_potential
};
DECLARE_ENUM_CLAD(Irrigation_consideration_mode_clad,Irrigation_consideration_mode,irrigation_consideration_mode_label_table);
//_Irrigation_consideration_mode____________________________________2009-12-01_/
extern const char *depletion_observation_depth_mode_label_table[];
enum Depletion_observation_depth_mode { constant_fixed_depth_mode, fraction_of_root_depth_mode, soil_profile_depth_mode };
DECLARE_ENUM_CLAD(Depletion_observation_depth_mode_labeled,Depletion_observation_depth_mode,depletion_observation_depth_mode_label_table);
//_Depletion_observation_depth_mode_________________________________2007-06-06_/
extern const char *V3_automatic_clipping_mode_label_table[];
enum V3_Automatic_clipping_mode {AUTO_CLIP_DISABLED,AUTO_CLIP_BIOMASS,AUTO_CLIP_PERIODIC};
DECLARE_ENUM_CLAD(V3_Automatic_clipping_mode_clad,V3_Automatic_clipping_mode,V3_automatic_clipping_mode_label_table)
//_V3_Automatic_clipping_mode____________________________1998-08-26_2002-03-22_/
extern const char *V3_biomass_fate_label_table[];
enum V3_Biomass_fate           {BIOMASS_TO_HARVEST,CLIPPING_REMOVE,CLIPPING_TO_RESIDUE, CLIPPING_GRAZE};
DECLARE_ENUM_CLAD(V3_Biomass_fate_clad,V3_Biomass_fate,V3_biomass_fate_label_table)
//_V3_Biomass_fate_______________________________________1998-08-26_2002-03-22_/
extern const char *irrigation_system_label_table[];
enum Irrigation_system {no_irrigation,center_pivot_irrigation,other_irrigation};
DECLARE_ENUM_CLAD(Irrigation_system_clad,Irrigation_system,irrigation_system_label_table);
//_Irrigation_system________________________________________________2011-07-19_/
extern const char *harvest_amount_mode_label_table[];
enum Harvest_amount_mode
   {HARVEST_CROP_PARAMETER_BASED
   ,HARVEST_REMOVE_FIXED_AMOUNT_BIOMASS
   ,HARVEST_PERCENT_BIOMASS        // Claudio decided this won't be of any use, But then finally decided it was needed Feb 22, 2007
   ,HARVEST_RETAIN_GAI_obsolete           // This mode didn't work, it is now replaced with remove fixed amount but with minimum GAI retained
   ,OTHER_BIOMASS_FATE };
DECLARE_ENUM_COWL(Harvest_amount_mode_cowl,Harvest_amount_mode,harvest_amount_mode_label_table)
//_Harvest_amount_mode______________________________________________2005-08-22_/
extern const char *harvest_mode_and_biomass_fated_cause_label_table[];
enum Harvest_fate_mode
   {HARVEST_ONCE
   ,HARVEST_CLIPPING
   ,HARVEST_LAST_CLIPPING
   ,HARVEST_GRAZE
   ,DEFOLIATION // I.e. potatos
   ,HARVEST_INDEX_obsolete
   ,HARVEST_FATE_COUNT };
DECLARE_ENUM_CLAD(Harvest_fate_mode_clad,Harvest_fate_mode,harvest_mode_and_biomass_fated_cause_label_table)
//_Harvest_fate_mode_____________________________________1998-08-26_2002-03-22_/
extern const char *automatic_N_mode_label_table[];
enum Automatic_N_mode {AUTO_NITRO_DISABLED,
                        AUTO_NITRO_BALANCE_BASED,AUTO_NITRO_LOCAL_CALIBRATION};
DECLARE_ENUM_CLAD
  (Automatic_N_mode_clad,Automatic_N_mode,automatic_N_mode_label_table)          //020322
//_Automatic_N_mode_________________________________________________1999-02-08_/
// The following are fates used by Crop condition events
enum Biomass_fated_cause
{CLIP_NOW_OR_NEVER = HARVEST_FATE_COUNT
,INSUFFICIENT_BIOMASS_FOR_FINAL_CLIPPING
,CLIPPING_DUE_TO_LOW_GROWTH
,CLIPPING_DUE_TO_MAX_BIOMASS                                                     //040830
,CLIPPING_DUE_TO_MAX_LAI                                                         //040830
,CLIPPING_DUE_TO_DAYS_AFTER_FLOWERING                                            //040830
,FREEZING_DAMAGE_CAUSE
,FREEZING_DEATH_CAUSE
,DORMANCY_CAUSE
,NATURAL_EXPIRATION_CAUSE                                                        //040805
,TERMINATION_CAUSE
,MULTIPLE_CAUSES                                                                 //070201
,BIOMASS_FATE_COUNT                                                              //040830
};
//_Biomass_fated_cause______________________________________________2004-06-22_/
struct Biomass_fate_parameters
{
/*
If the crop does not use harvest index
The following percents represent 100 percent of the above ground biomass.
Otherwise, of the above ground biomass excluding yield from harvest index,
these percents add to 100%.
Warning, don't resequence these variables, there are some
autoagregate initialized structions!!!!

These would probably be floats.
*/
                                               // stored as
   int16    remove_for_use;         // use        material other wised harvested for consumption (recorded)
   int16    remove_for_grazing;     // graze
   int16    remain_as_residue;      // flat residue     usually 0 if grazing
   int16    remove_for_disposal;    // dispose    usually 0 if grazing  (I.e. for inefficiency)
      // standing will always be 0 if cut fixed amount or to LAI mode 050822
      // (the amount standing live will be determined based on LAI remaining.
   int16    remain_as_dead;         // standing dead
   int16    remain_as_live;         // standing live        if terminate crop otherwise live matter
//             These root percentages should add to 100.
//             Do not yet show these in the user interface, they are currently
//             only for dormancy biomass fate
   int16    remove_roots_to_dead_residue;                                        //040621
   int16    remain_roots_live;                                                   //040621
   int16    remove_roots_as_loss;   // usually 0.0  may be needed for root crop harvest 040621

//             The following is for the grazing model it is (not)part of the above percentages.
   int16    return_as_manure;  // Fraction of the biomass used for manure production
//             If terminate crop, the live percentages will be set to 0.0
//             (the precentage will be added to the respective dead fate)
};
//_Biomass_fate_parameters__________________________________________2004-06-10_/
extern void clear_biomass_fate_parameters(Biomass_fate_parameters &BMF_param);   //050718

// The following are above ground mass fate indexes
// These are used to index the Crop_mass_fate above ground value array
// These are atomic
#define INDEX_YIELD            0
#define INDEX_USED             1
#define INDEX_DISPOSAL         2
#define INDEX_CHAFF            3
#define INDEX_STUBBLE          4
#define INDEX_GRAZED_MANURE    5
#define INDEX_GRAZED_DISPOSAL  6
#define INDEX_UNCUT_CANOPY     7
// WARNING DO NOT ADD ANY MORE indexes or CropSyst variable codes will have to be readjusted
// Because we are limited to 8 bits in the variable code.
// If something needs to be added then move the GRAZED variable codes out to a separate variable set
// and a separate array in the Crop_mass_fate
#define FATE_INDEX_COUNT 8

#define BITMASK_YIELD            (1 << INDEX_YIELD)
#define BITMASK_USED             (1 << INDEX_USED)
#define BITMASK_DISPOSAL         (1 << INDEX_DISPOSAL)
#define BITMASK_CHAFF            (1 << INDEX_CHAFF)
#define BITMASK_STUBBLE          (1 << INDEX_STUBBLE)
#define BITMASK_GRAZED_MANURE    (1 << INDEX_GRAZED_MANURE)
#define BITMASK_GRAZED_DISPOSAL  (1 << INDEX_GRAZED_DISPOSAL)
#define BITMASK_UNCUT_CANOPY     (1 << INDEX_UNCUT_CANOPY)

#define BITMASK_REMOVED_ABOVE_GROUND   (BITMASK_YIELD | BITMASK_USED | BITMASK_DISPOSAL | BITMASK_CHAFF | INDEX_STUBBLE | BITMASK_GRAZED_MANURE | BITMASK_GRAZED_DISPOSAL)
#define BITMASK_REMOVED_NON_YIELD      (                BITMASK_USED | BITMASK_DISPOSAL | BITMASK_CHAFF | INDEX_STUBBLE | BITMASK_GRAZED_MANURE | BITMASK_GRAZED_DISPOSAL)
#define BITMASK_REMOVED_RESIDUE        (                                                  BITMASK_CHAFF | INDEX_STUBBLE)
#define BITMASK_REMOVED_GRAZING        (                                                                                  BITMASK_GRAZED_MANURE | BITMASK_GRAZED_DISPOSAL)
#define BITMASK_TOTAL_ABOVE_GROUND     (BITMASK_YIELD | BITMASK_USED | BITMASK_DISPOSAL | BITMASK_CHAFF | INDEX_STUBBLE | BITMASK_GRAZED_MANURE | BITMASK_GRAZED_DISPOSAL | BITMASK_UNCUT_CANOPY)

extern const char *mass_fate_label_table[];
// The above ground mass fate bitmasks are used to select above ground value
// These are also used to compose CropSyst variable codes for above ground biomass and nitrogen fate

//______________________________________________________________________________
class Crop_mass_fate
{
   // This is used to store the actual values of biomass fate from the crop (I.e. kg biomass/ha)
   // Also use to store plant N mass fate (I.e. kg N/ha)
 public: // The following are the result of management operations // the sum of these should be the above ground biomass
   Biomass_fated_cause  cause;
   float64  above_ground[FATE_INDEX_COUNT];
   float64  &yield;                    // grain or fruit harvest_index_yield
   float64  &useful;                       // secondary_yield additional harvested material that is not grain or fruit
   float64  &disposal;                  // (I.e. from harvest or post harvest processing where material is not applied to the soil)
   float64  &chaff_or_leaf_litter;              // cut material laying flat on soil surface  rename this to chaff
   float64  &stubble_dead;     // standing material   rename this to stubble_dead
 public: // The following are used by the grazing pasture submodel
   float64  &grazing_to_manure;         // grazed biomatter that turns to manure applied to field
                                                   // This is actually only used for the N fate
                                                   // It is temporarily computed for the biomatter fate only to
                                                   // determine the value for the N fate (from the plant concentration).
                                                   // After that, removed_for_grazing_to_manure has no meaning
                                                   // in the plant biomass fate!
                                                   // rename this to grazing_recycled
   float64  &grazing_to_disposal;       // grazed biomatter that turns to manure removed from simulation (not recycled)
                                                   // rename this to grazing_loss
   float64  &uncut_canopy;                      // standing material Not removed from the plant
 public: // used for mass balance
   float64  canopy_before;  // This records the total canopy biomass before distributions  //131004 canopy_biomass_before
   float64  root_before;    // This records the total root biomass before distribution to layers //131004 was root_biomass_before
   float64  canopy_after;  // This records the total canopy biomass before distributions    //131004 was canopy_biomass_after
   float64  root_after;    // This records the total root biomass before distribution to layers   //131004 was root_biomass_after
 public: // roots
   // Note, we don't currently have a root biomass balance, it is computed from
   // the fraction or roots and the estimated act_root_biomass.
   //  remained_roots_as_dead_residue represents the amount of roots
   //  moved to residues for dormancy consideration.
   //  Roots remaining alive in the case of dormancy become dormant and are handled differently. 070201
   soil_sublayer_array_64(roots_live);
   soil_sublayer_array_64(roots_dead);
 public: // constructors
   Crop_mass_fate(Biomass_fated_cause cause
                 ,float64 canopy_before,float64 root_biomass_before);
      // This constructor is used for Crop biomass fate
   Crop_mass_fate(const Crop_mass_fate &biomass_fate
                 ,float64 top_concentration,float64 root_concentration);         //070131
      // This constructor is used for Plant N from plant biomass and concentration
   Crop_mass_fate(const Crop_mass_fate &copy_from_biomass_fate);                 //070130
      // Copy constructor

#define CANOPY_ONLY false
#define ENTIRE_PLANT_INCLUDING_ROOTS true

   void add(const Crop_mass_fate &augend);
   float64 get_total_removed_from_plant(bool include_root = CANOPY_ONLY)  const;
   inline float64 get_total_harvested()         const { return yield + useful; }
   inline float64 get_total_sent_to_residue()     const { return stubble_dead; } //050120
   inline float64 get_removed_for_grazing()                               const
                            { return grazing_to_manure +  grazing_to_disposal; }
   inline float64 get_total_any_harvested()                               const
                                 { return yield +  useful + grazing_to_manure; } //060323
   inline float64 get_unclipped_biomass()         const { return uncut_canopy; }
   float64 get_dead_roots_residue()                                       const;
   bool has_any_mass()                                                    const; //070625
 public: // output writing
   void log(std::ostream &event_log)                                      const;
   void dump_headers(std::ostream &dump_log,bool include_live
      ,nat8 _root_soil_layers,const char *caption_suffix)                 const; //070201
   void dump(std::ostream &dump_log,bool include_live)                    const; //070201
   // dump_headers() prints a tab delimited row of labels matching
   // the variables printed by dump()
   // include_live outputs live standing canopy and live roots
};
//_Crop_mass_fate___________________________________________________2004-06-10_/
class Crop_mass_fate_biomass_and_nitrogen
{  // This structure a simple tuple to group both mass object for a function return
 public:
   Crop_mass_fate *biomass;   // owned by this
   Crop_mass_fate *nitrogen;  // owned by this
 public: // structors
   Crop_mass_fate_biomass_and_nitrogen
      (Crop_mass_fate *_biomass
      ,Crop_mass_fate *_nitrogen);
   virtual ~Crop_mass_fate_biomass_and_nitrogen();
 public:
   void accumulate(const Crop_mass_fate_biomass_and_nitrogen &augend);           //070627
};
//_Crop_mass_fate_biomass_and_nitrogen______________________________2007-06-25_/
#if ((DIRECTED_IRRIGATION >= 2014) && (DIRECTED_IRRIGATION <= 2016))
// abandoned
struct Directed_irrigation_soil_wetting
{  float32 surface_evaporation_area_fraction;
   float32 profile_volume_fraction;
   nat16   days_effective;          // Not currently a user modifiable parameter
 public:
   inline Directed_irrigation_soil_wetting()
      :surface_evaporation_area_fraction(1.0)
      ,profile_volume_fraction(1.0)
      ,days_effective(14)
      {}
   inline Directed_irrigation_soil_wetting(const Directed_irrigation_soil_wetting &copy_from)
      :surface_evaporation_area_fraction(copy_from.surface_evaporation_area_fraction)
      ,profile_volume_fraction(copy_from.profile_volume_fraction)
      ,days_effective(14)
      {}
} ;
//_Directed_irrigation_soil_wetting_________________________________2013-03-13_/
#endif
#endif

