#ifndef population_insectH
#define population_insectH
#include "pest/pest.h"
#include "common/weather/parameter/WP_air_temperature.h"
#include "common/weather/hour/weather_hours.h"
#include "corn/dynamic_array/dynamic_array_T.h"
#include "corn/math/statistical/sample_dataset_T.h"
#include "corn/data_source/datarec.h"
#include "CS_suite/observation/CS_inspection.h"

//#define USE_DIAPAUSE_STAGE

#define FLOATXX float64

class Test_simulation_with_output;
namespace CropSyst
{
/*
Common name: Cereal Leaf Beetle (abbreviated as CLB)
Scientific name: Oulema melanopus (L.)
Family: Chrysomelidae
Order: Coleoptera
Crop infested: Cereals, Primarily Wheat, Oats and Barley
Population (in general)- numbers per sq. meter of field area
Number of pre-season female adults  (intial input parameter)-numbers per sq. meter of field area
Total feeding" (daily) as mm2 / 1000000 was done to convert the leaf area fed by each instar on daily basis in square millimeter to square meters.
*/
   enum Stage_holometabolic_enum
   {  unused_stage
   ,  ovum_stage
   ,  larva_stage
   ,  pupa_stage
   // imago
   ,  adult_postseason_stage
   #ifdef USE_DIAPAUSE_STAGE
   ,  adult_diapause_stage                                                       //151210
   #endif
   ,  adult_preseason_stage
   ,  STAGE_COUNT };

#define NA 99999.999

#define Oulema_melanopus_instars 4

struct Parameters_insect
{  //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   struct Development
   {
         float32 Tbase;         // Celcius
         float32 Topt;          // Celcius
         float32 Tmax;          // Celcius
         float32 DRmax;         // Warnng need units  Development rate
   };
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   struct Mortality_ova
   {  // some sort of exponential of quadratic regression
      float32 rate_B0;
      float32 rate_B1;
      float32 rate_B2;
   };
   struct Mortality_larva
   {  // quadratic regression
      float32 rate_B0;
      float32 rate_B1;
      float32 rate_B2;
   };
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   struct Mortality_pupa
   {
      float32 theshold_temperature;  // Celcius
      float32 f0;
      float32 f1;
      float32 s0;
      float32 s1;
   };
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   float32 adult_preseason_count_initial;   // count/m^2
   float32 phenology_temperature_base;      // Celcius
   float32 first_egg_deg_days;              // degC/days
   float32 oviposition_rate_coef;
   float32 adult_pre_season_mortality_rate_coef;
   float32 adult_post_season_mortality_rate_coef;
   nat8    instars;                         // count
   struct Development      stage_development[CropSyst::STAGE_COUNT];
   struct Development      larva_instar_development[5]; // index 0 not used
   float32 larva_feeding_rate[5]; //index 0 not used                             //160222
   struct Mortality_ova    ova_morality;
   struct Mortality_larva  larva_instar_mortality[5];   // index 0 not used
   struct Mortality_pupa   pupa_mortality;
};
//_Parameters_insect___________________________________________________________/
extern struct CropSyst::Parameters_insect parameters_Oulema_melanopus;
//______________________________________________________________________________
class Parameters_insect_data_record
: public extends_ CORN::Data_record
{
   Parameters_insect &parameters;
 public:
   inline Parameters_insect_data_record(Parameters_insect &parameters_)
      : CORN::Data_record("insect")
      , parameters(parameters_)
      {}
   virtual bool expect_structure(bool for_write);
};
//_Parameters_insect_data_record_______________________________________________/
class Population_insect
: public extends_ CropSyst::Pest
{
 public:
   // This class currently doesn't recognize hemimetabolism
 public:
   //______________________________________________________________________________
   class Stage
   : public extends_ CS::Simulation_element_abstract
   {
    protected: // convenient references
      const Parameters_insect::Development   &param_development;
      const Population_insect                &population;
      const CS::Temperature                  &air_temperature_mean_daily;
      const CORN::Dynamic_array<float32>     &temperature_by_hour; // air or soil for diapause adults
    protected:
      float64 development_rate_daily;
      float64 mortality_rate_daily;
      CORN::Dynamic_array<FLOATXX> development_cumulative; // 0 to 1
      CORN::Date_clad_32 appearance_date;                                        //180417
      nat16   days_since_appearance;
      CORN::Dynamic_array<FLOATXX> cohort_live;   // count / m2 field area
         // counts are indeed float as they are on a curve
         // indexed by days since stage first appearance day.
    protected:
      virtual float64 calc_development_rate_hourly(float64 Tmean_hour_C)  const;
      virtual float64 calc_mortality_rate_hourly(float64 Tmean_hour_C)  const=0;
         // rename mortality fraction
      inline float64 get_temperature_base           () const { return param_development.Tbase; }
      inline float64 get_temperature_optimum        () const { return param_development.Topt; }
      inline float64 get_temperature_maximum        () const { return param_development.Tmax; }
      inline float64 get_development_rate_maximum   () const { return param_development.DRmax; }
    public:
      Stage
         (const Parameters_insect::Development  &_development
         ,const CORN::date32 &simdate_raw //180417,const CORN::Date                &_today
         ,const Population_insect         &_population
         ,const Air_temperature_average   &_Tmean
         ,const CORN::Dynamic_array<float32>  &_temperature_by_hour);
      virtual bool start_day()                                    modification_;
      virtual bool end_day()                                      modification_;
      virtual bool process_day()                                  modification_;
      virtual bool matriculate(float32 initial_live)              modification_;
      inline virtual float32 get_live()                                   const
         { return cohort_live.get(days_since_appearance); }
      inline virtual float32 total_live()                                 const
         { return cohort_live.sum(); };
      inline virtual nat32 get_key_nat32()                              const=0;
      virtual float64 reduce_to_fraction(float64 fraction)        modification_;
         // 0 essentially clears population
      inline virtual nat16 get_days_since_appearance()                    const
         { return days_since_appearance; }
         ///< Terminates/remove all cohorts.
         ///  \returns number removed.
   protected:
      virtual inline Stage_holometabolic_enum get_stage()                 const
         {  // Warning this is just a quick hack sinc currently
            // the key happens to be the stage.
            // It would be better to reimplement in each derived class
            nat8 stage_num = 0xFF & get_key_nat32();
            return (Stage_holometabolic_enum)stage_num;
         }
      virtual inline nat8 get_instar()                                    const
         {  // Warning this is just a quick hack sinc currently
            // the key happens to be the stage.
            // It would be better to reimplement in each derived class
            return 0;
         }
      virtual inline Stage_holometabolic_enum get_next_stage(nat8 &next_instar) const
         {  // Warning this is just a quick hack sinc currently
            // the key happens to be the stage.
            // It would be better to reimplement in each derived class
            next_instar = 0;
            return (Stage_holometabolic_enum)(0xFF & get_key_nat32()+1);
         }
    public:
      //NYI RENDER_INSPECTORS_DECLARATION;                                       //151229
   };
   //______________________________________________________________________________
   class Ovum
   : public extends_ Stage
   {
      const Parameters_insect::Mortality_ova &param_mortality;
    public:
      Ovum
         (const Parameters_insect::Development     &_development
         ,const Parameters_insect::Mortality_ova   &_param_mortality
         ,const CORN::date32 &simdate_raw //180417,const CORN::Date &_today
         ,const Population_insect            &_population
         ,const Air_temperature_average      &_Tmean
         ,const CORN::Dynamic_array<float32> &_temperature_by_hour);
    protected:
      virtual float64 calc_mortality_rate_hourly(float64 Tmean)           const;
      inline virtual Stage_holometabolic_enum get_next_stage(nat8 &next_instar) const;
      inline virtual nat32 get_key_nat32()   const { return (nat32)ovum_stage; }
   };
   //__________________________________________________________________________/
   class Larva
   : public extends_ Stage
   {
      nat8     instar;     // 0 is unused
      const Parameters_insect::Mortality_larva  &param_mortality;
      float32 param_feeding_rate;  // mm^2 leaf area/larva/day                   //160222
      Metabolism  metabolism;
    protected:
      float64  feeding;    // LAI consumed per day
    protected: // references
      modifiable_ float64 &ref_larvae_feeding_daily; // mm^2 leaf/mm^2 land area
    public:
      Larva(nat8                                       instar
            ,const Parameters_insect::Development     &development
            ,const Parameters_insect::Mortality_larva &param_mortality
            ,float32 feeding_rate                                                //160222
            ,modifiable_ float64                &ref_larvae_feeding_daily
            ,const CORN::date32                 &simdate_raw
            ,const Population_insect            &population
            ,const Air_temperature_average      &air_temperature_mean
            ,const CORN::Dynamic_array<float32> &air_temperature_by_hour);
      virtual bool process_day()                                  modification_;
      virtual bool end_day()                                      modification_; //160115
      bool set_metabolism(Metabolism metabolism)                  modification_; //160115
    public: // output accessor methods
      inline float64 get_feeding()                     const { return feeding; }
         // mm^2/larva
      inline float64 get_feeding_rate()                                   const
         { return param_feeding_rate; }
         // // mm^2 leaf area/larva/day
    protected:
      virtual float64 calc_mortality_rate_hourly(float64 air_temperature_mean_hourly) const;
      inline virtual nat32 get_key_nat32()                                const
         { return (((nat32)instar << 8) | (nat32)larva_stage); }
      virtual Stage_holometabolic_enum get_next_stage(nat8 &next_instar)  const;
      virtual inline nat8 get_instar()                                    const
         {  // Warning this is just a quick hack sinc currently
            // the key happens to be the stage.
            // It would be better to reimplement in each derived class
            return instar;
         }
   };
   //__________________________________________________________________________/
   class Pupa
   : public extends_ Stage
   {
      const Parameters_insect::Mortality_pupa &param_mortality;
    public:
      Pupa
         (const Parameters_insect::Development  &_development
         ,const Parameters_insect::Mortality_pupa &_param_mortality
         ,const CORN::date32 &simdate_raw
         ,const Population_insect            &_population
         ,const Air_temperature_average      &_Tmean
         ,const CORN::Dynamic_array<float32> &_temperature_by_hour);
    protected:
      virtual float64 calc_mortality_rate_hourly(float64 Tmean_hourly_C)  const;
      inline virtual nat32 get_key_nat32()   const { return (nat32)pupa_stage; }
   };
   //__________________________________________________________________________/
   class Adult
   : public extends_ Stage
   {
      float32 param_mortality_rate_coef;
    protected:
      const CS::Temperature                  &air_temperature_min_daily;

    public:
      Adult
         (const Parameters_insect::Development  &_development
         ,float32                                _param_mortality_rate_coef
         ,const CORN::date32 &simdate_raw
         ,const Population_insect            &_population
         ,const Air_temperature_average      &_Tmean
         ,const Air_temperature_minimum      &_air_temperature_min_daily
         ,const CORN::Dynamic_array<float32> &_temperature_by_hour);
    protected:
      virtual float64 calc_mortality_rate_hourly(float64 Tmean)           const;
      inline virtual float64 get_mortality_rate_coef()                    const
         { return param_mortality_rate_coef; }
   };
   //___________________________________________________________________________
   class Adult_post_season
   : public extends_ Adult
   {
    public:
      Adult_post_season
         (const Parameters_insect::Development  &development
         ,float32                                param_mortality_rate_coef
         ,const CORN::date32                    &simdate_raw
         ,const Population_insect               &population
         ,const Air_temperature_average         &Tmean
         ,const Air_temperature_minimum         &air_temperature_min_daily
         ,const CORN::Dynamic_array<float32>    &temperature_by_hour);
    protected:
      #ifdef USE_DIAPAUSE_STAGE
      CORN::statistical::Sample_dataset_clad<float64,float64>diapause_start_window;
      #endif

      inline virtual nat32 get_key_nat32() const{return adult_postseason_stage;}
      virtual bool start_day()                                    modification_; //151210
      virtual bool process_day()                                  modification_;
      virtual float64 calc_development_rate_hourly(float64 Tmean_hour_C)  const
         { return 0.0; }
      //pre and post have same mortality rate virtual float64 calc_mortality_rate_hourly(float64 Tmean)            const;
   };
   //___________________________________________________________________________
   #ifdef USE_DIAPAUSE_STAGE
   class Adult_diapause
   : public extends_ Adult
   {
    public:
      Adult_diapause
         (const Parameters_insect::Development  &development
         ,const CORN::date32                    &simdate_raw
         ,const Population_insect               &population
         ,const Air_temperature_average         &air_temperature_mean_daily
         ,const Air_temperature_minimum         &_air_temperature_min_daily
         ,const CORN::Dynamic_array<float32>    &soil_temperature_by_hour);
    protected:
      CORN::statistical::Sample_dataset_clad<float64,float64>diapause_end_window;
      inline virtual nat32 get_key_nat32() const { return adult_diapause_stage;}
      virtual bool start_day()                                    modification_; //151206
      virtual bool process_day()                                  modification_;
      virtual float64 calc_mortality_fraction_hourly(float64 Tmean)       const;
         // For diapause stage we calculate a cumulative fraction which is not rate used in other stages
   };
   #endif
   //___________________________________________________________________________
   class Adult_pre_season
   : public extends_ Adult
   {
      CORN::DOY            param_mortality_day; // 365 for northern hemisphere
      float32              param_oviposition_rate_coef;
      bool                 laying_eggs;
    protected:
      contribute_ float64 oviposition_rate_daily;
      contribute_ float64 eggs_laid_today;
      float32 live_count;
      const float64 &degree_days_cum;                                            //160222
    public: // for output
      contribute_ float64 eggs_laid_cum_output;
    public:
       Adult_pre_season
         (const Parameters_insect::Development  &development
         ,float32                                param_mortality_rate_coef
         ,float32                                param_oviposition_rate_coef
         ,const CORN::date32                    &simdate_raw
         ,const Population_insect            &cohort
         ,const Air_temperature_average      &air_temperature_mean_daily
         ,const Air_temperature_minimum              &_air_temperature_min_daily
         ,const CORN::Dynamic_array<float32> &air_temperature_by_hour
         ,const float64                      &degree_days_cum                    //160222
         );
      inline bool begin_laying_eggs(bool laying)
         {  laying_eggs = laying; eggs_laid_today = 0.0;
            eggs_laid_cum_output = 0.0; return laying_eggs;}
      inline bool is_laying_eggs()                                 affirmation_
         {return laying_eggs; }
    protected:
      virtual float64 calc_development_rate_hourly
         (float64 Tmean_hour_Celcius) const                      { return 0.0; }

      inline virtual nat32 get_key_nat32()                                const
         { return adult_preseason_stage; }
      virtual bool start_day()                                    modification_;
      virtual bool process_day()                                  modification_;
      float64 calc_oviposition_rate_hourly(float64 Tmean_hour_Celcius)    const;
      #ifdef MATCH_EXCEL_VERSION
      // The Excel (single year) version doesn't use cohorts,
      inline virtual bool matriculate(float32 initial_live)       modification_
         { live_count = initial_live; return true; }
    public:
      inline virtual float32 total_live()           const { return live_count; }
      #endif
   };
   //______________________________________________________________________________
 public:
   const CropSyst::Parameters_insect   &parameters;
   bool                                 northern_hemisphere;                     //160211
 private:
   const Air_temperature_average       &air_temperature_mean_daily;
   const Air_temperature_minimum       &air_temperature_min_daily;
   const CORN::Dynamic_array<float32>  &air_temperature_by_hour;
   const CORN::Dynamic_array<float32>  &soil_temperature_by_hour;
   contribute_ bool first_population;                                            //160118
 protected:
   provided_ CORN::Container &stages;
      // reference to self, not actually needed because this (composite element list) is the population
   contribute_ float64 degree_days_cum;
   mutable float64 larvae_feeding_daily;
      // Sum of all larvae feeding this day (LAI m^2 leaf / m^2 soil)
      // larvae_feeding_rate is mm^2 leaf area/larva/day
      // convert to m2 (divide by 1e6), and multiplied by # larva/m2 ground yields LAI consumed per day
 public:
   Population_insect
      (const CropSyst::Parameters_insect  &parameters
      ,bool                                northern_hemisphere                   //160211
      ,const CORN::date32                 &simdate_raw
      ,const Air_temperature_average      &air_temperature_mean_daily
      ,const Air_temperature_minimum      &air_temperature_min_daily
      ,const CORN::Dynamic_array<float32> &air_temperature_by_hour
      ,const CORN::Dynamic_array<float32> &soil_temperature_by_hour);
   bool restart()                                                 modification_; //160211
   virtual bool start_year()                                      modification_;
   virtual bool start_day()                                       modification_;
   virtual bool end_day()                                         modification_; //160119

   virtual bool restart_thermal_time_accumulation()              rectification_; //151210
   virtual bool set_metabolism_larval(Metabolism metabolism)      modification_; //160115
      // This is used to respond to lack of food
   virtual bool respond_to_harvest()                              modification_; //160119
   #ifdef USE_DIAPAUSE_STAGE
   virtual bool terminate_non_adults_diapause()                   modification_; //160119
   #else
   #endif
      // This is used to respond to harvest event
   Population_insect::Stage *matriculate_to_stage
      (Stage_holometabolic_enum stage
      ,nat8       instar
      ,float64    initial_live)                                      provision_;
   bool has_stage(Stage_holometabolic_enum stage,nat8 instar)      affirmation_;
   const Population_insect::Stage *get_stage(Stage_holometabolic_enum stage,nat8 instar=0) const;
      // Returns 0 if stage doesn't exist.
   virtual float64 get_damaged_GAI()                                      const;
 private:
   Population_insect::Stage *provide_stage
      (Stage_holometabolic_enum stage, nat8 instar)                  provision_;
 protected:
   float64 calc_degree_day()                                       calculation_;
 public: // output accessors
   inline float64 get_degree_days_cum()      const{return degree_days_cum; }
   inline float64 get_larvae_feeding_daily() const{return larvae_feeding_daily;}
      //convert to m2 (divide by 1e6), and multiplied by # larva/m2 ground yields LAI consumed per day
   virtual bool dump
      (std::ostream &population_file
      ,std::ostream &feeding_file)                                        const; //151229
};
//_Population_insect________________________________________________2015-03-15_/
}//_namespace CropSyst_________________________________________________________/
#endif

