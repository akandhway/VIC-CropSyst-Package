#ifndef phenology_AH
#define phenology_AH
#include "crop/phenology_I.h"
#ifndef growth_stagesHPP
#  include "crop/growth_stages.hpp"
#endif
#include "crop/crop_param_struct.h"
#include "CS_suite/observation/CS_observation.h"
#include "corn/chronometry/date_32.h"
#include "CS_suite/simulation/CS_simulation_element.h"
#include "CS_suite/simulation/CS_period.h"

// Replaces Phenology_common
#include "crop/thermal_time_common.h"

namespace CropSyst
{
   class Phenology_adjustor;                                                     //140620
   interface_ Thermal_time;                                                      //190628
//______________________________________________________________________________
class Phenology_abstract // was common
: public implements_ Phenology
{
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   public:
   class Period_thermal
   : public implements_ Phenology::Period_thermal
   , public extends_ CS::Period_clad
   {
    public: // state
      /*now in CS::Period_clad
      CORN::date32 initiated;
      CORN::date32 culminated;
      nat16        day_count; // not sure yet if 0 or 1 based
      */
         // day_count was days_since_start
      nat16 start_day_in_season;
      Thermal_time *thermal_time;
      float64 thermal_time_relative_elapsed;                                     //200304
      float64 thermal_time_relative_remaining;                                   //200304
    public: // parameters
      const Crop_parameters_struct::Phenologic_period *parameters;
      bool parameters_owned;
         // actually should be owned/unowned

    private:
      const CORN::date32 &simdate_raw;
    public: // structors
      Period_thermal
         (const Crop_parameters_struct::Phenologic_period &parameters_
         ,const CORN::date32              &simdate_raw_
         ,const float64                   &stress_adjusted_temperature_C_        //190812
         ,const float64                   &air_temperature_min_C_                //190812
         ,bool                             is_fruit_tree_
         ,Thermal_time_common::Vernalization      *vernalization_optional
         ,Thermal_time_common::Photoperiodization *photoperiodization_optional);
      bool Period_thermal::construction_common                                   //190630
         (const float64                   &stress_adjusted_temperature_C_        //190812
         ,const float64                   &air_temperature_min_C_                //190812
         ,bool                             is_fruit_tree_
         ,Thermal_time_common::Vernalization      *vernalization_optional
         ,Thermal_time_common::Photoperiodization *photoperiodization_optional);
      virtual ~Period_thermal();
      virtual bool start_day()                                    modification_;
      virtual bool end_day()                                      modification_;
      virtual bool has_expired()                                   affirmation_;
      /*200409 replaced with get_lapse
      inline virtual bool get__day_count()             const { return day_count;}
      */
      inline virtual bool is_key_cstr(const char *key)             affirmation_
         {  // parameters will exists
            return (parameters->ontology == key);
         }
      virtual float64 get_thermal_time_accum()                            const; //200210
         //200211 (bool adjusted_for_clipping)
      virtual inline float64 get_thermal_time_relative_elapsed()          const  //200304
         { return thermal_time_relative_elapsed; }
      virtual inline float32 get_thermal_time_relative_remaining()        const  //200304
         { return thermal_time_relative_remaining; }

      virtual float64 calc_thermal_time_relative_elapsed()         calculation_;
      virtual float64 calc_thermal_time_relative_remaining()       calculation_;
/*200304 renamed
      virtual float64 calc_relative_thermal_time()                 calculation_;
         // I think rename this to calc_relative_thermal_time_elapsed.
      virtual float64 calc_relative_thermal_time_remaining()       calculation_;
*/

      inline virtual bool clipping_resets()                        affirmation_
         { return parameters->relevant & RELEVANT_CLIPPING; }
      inline virtual float32 get_param_duration_GDDs()                     const
         { return parameters->duration_GDDs; }
      inline virtual float32 get_param_thermal_time_accum()                const
         { return parameters->initiation.thermal_time_accum; }
      virtual nat8 respond_to_clipping()                                       ; //200210

      void dump(std::ostream &strm);
   };
   //_class Period_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _/
 protected: friend class Phenology_sequencer;                                    //140620
   const Crop_parameters_struct::Phenology &parameters;                          //190701
   bool  is_perennial;
   bool  is_fruit_tree;
 protected: // Periods dont delete because these are owned by subclass
   provided_ Period_thermal *accrescence;                                        //190630
   provided_ Period_thermal *culminescence;                                      //190630
   provided_ Period_thermal *senescence;                                         //190630
   provided_ /*const*/ Period_thermal *root_elongation;                          //191121
 protected: // external references
   const CORN::date32 &simdate_raw;                                              //181113
 protected: // external references for  thermal time limitations
   const float64 &stress_adjusted_temperature_C;                                 //190812
   const float64 &air_temperature_min_C;                                         //190812
   Thermal_time_common::Vernalization        *vernalization;      // owned
   Thermal_time_common::Photoperiodization   *photoperiodization; // owned
 public:
   nat16 season_duration_days;                                                   //181115
      // (the number of full completed days in the season)
   CORN::date32 planting_date;                                                   //181111
   nat16 harvest_DOY;
 public:
   Phenology_abstract // was common
      (const Crop_parameters_struct::Phenology    &parameters_                   //190701
      ,const float64 &stress_adjusted_temperature_C_                             //190812
      ,const float64 &air_temperature_min_C_                                     //190812
      ,Thermal_time_common::Vernalization         *vernalization_given           //190701
      ,Thermal_time_common::Photoperiodization    *photoperiodization_given      //190701
      ,bool  is_perennial_
      ,bool  is_fruit_tree_
      ,const CORN::date32 &simdate_raw_                                          //181108
      );
   virtual ~Phenology_abstract();                                                //191022
   virtual bool end_day()                                         modification_;
   virtual bool start_season()                                    modification_;
   inline virtual nat16 day_of_season()           const {return season_duration_days;}
   inline virtual const nat16 &ref_day_of_season()const {return season_duration_days;;}
   inline virtual nat16 get_season_duration_days()const {return season_duration_days;} //181115
 public: // periods
   virtual bool activate_accrescence()                                         ; //181108
   virtual bool activate_culminescence()                                       ; //181108
   virtual bool activate_senescence()                                          ; //181108
   virtual Phenology::Period_thermal *activate_root_elongation()             =0; //191121_181118
   virtual bool culminate_senescence()                                         ; //200409
   inline virtual const Period_thermal *get_accrescence_period()          const  { return accrescence; }    //181108
   inline virtual const Period_thermal *get_culminescence_period()        const  { return culminescence; }  //181108
   inline virtual const Period_thermal *get_senescence_period()           const  { return senescence; }     //181107
   inline virtual const Period_thermal *get_root_elongation_period()      const  { return root_elongation;} //191121
 public:
   inline virtual const CORN::date32 &ref_planting_date()                 const //181111
      { return planting_date; }
   inline virtual bool activate_harvest()
   {  harvest_DOY = simdate_raw % 100; return true;}                             //181114
   inline virtual bool is_harvested()        affirmation_ { return harvest_DOY;} //181114
 public:
   virtual bool reset
      (Normal_crop_event_sequence from = NGS_RESTART
      ,Normal_crop_event_sequence to   = NGS_TERMINATED)          modification_;
   inline bool initialize()                                       modification_
      { return reset((Normal_crop_event_sequence) NGS_NONE
                    ,(Normal_crop_event_sequence)(NGS_COUNT-1)); }               //141202
   RENDER_INSPECTORS_DECLARATION;
};
//_class_Phenology_abstract_____________________________2018-11-09__2013-09-02_/
}//_namespace_CropSyst_________________________________________________________/
#endif

