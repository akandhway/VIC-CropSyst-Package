#ifndef canopy_growth_cover_basedH
#define canopy_growth_cover_basedH
#include "options.h"
#include "crop/crop_param.h"
#include "crop/canopy_growth_portioned.h"
#include "crop/crop_types.h"
#ifdef CO2_CHANGE
# if (CO2_RESPONSE_VERSION==4)
#  include "crop/crop_CO2.h"
# else
#  include "crop/CO2_response.h"
# endif
#endif
#include "crop/phenology_I.h"
#include "crop/canopy_cover_continuum.h"

//______________________________________________________________________________
namespace CropSyst
{
class Crop_parameters;
typedef struct Crop_parameters_struct::Canopy_growth_cover_based Canopy_parameters;
typedef struct Crop_parameters_struct::Morphology                Canopy_growth_common_parameters;
typedef struct Crop_parameters_struct::Thermal_time              Thermal_time_parameters;
}
namespace CropSyst {
//______________________________________________________________________________
class Canopy_growth_cover_based
: public Canopy_growth_portioned
{  //190612 This should be renamed Canopy_leaf_growth_cover_based
protected: // Classes
   //_________________________________________________________________
   class Canopy_accumulation
   : public Canopy_growth_portioned::Canopy_accumulation
   {public:
      class Portion
      : public Canopy_growth_portioned::Canopy_accumulation::Portion
      {public:
         float64 canopy_fraction_green;   // this is  global_solar_rad_intercepted  (delta of the day)
         float64 canopy_fraction_dead;
         float64 thermal_time;            // daily amount                        //130510
      public:
         Portion
            (float64 _biomass
            ,float64 _canopy_fraction_green
            ,float64 _thermal_time)
            : Canopy_growth_portioned::Canopy_accumulation::Portion(_biomass)
            , canopy_fraction_green(_canopy_fraction_green)
            , canopy_fraction_dead(0)
            , thermal_time(_thermal_time)                                        //130510
            {}
         Portion()
            : Canopy_growth_portioned::Canopy_accumulation::Portion()
            , canopy_fraction_green(0)
            , canopy_fraction_dead(0)
            , thermal_time(0)                                                    //130510
            {}
            inline virtual ~Portion() {}                                         //120217
      public: // overrides
         virtual int compare(const CORN::Item &other)                     const; //970701
         inline virtual const char *label_string(std::string &buffer)     const  //030123
                                       { buffer.clear(); return buffer.c_str();}
      public: // Accessors
                virtual bool    is_valid()                                const; //050823
         inline virtual bool is_dead()                                    const
                                      { return canopy_fraction_dead > 0.000001;} //050823
         inline virtual float64 get_GAI()                                 const;
         virtual float64 get_living()     const { return canopy_fraction_green;} //080806
            // Gets the living green portion GAI or fract_canopy_cover_green
         inline virtual float64 get_biomass_current()                      const
                                                     { return biomass_current; } //070801
            // Originally we did not have any loss (I.e. from decomposition)
         inline virtual float64 get_thermal_time() const { return thermal_time;} //130513
      public: // Processing
         virtual void die();                                                     //050823
         virtual void die_back_to_GAI(float64 target_GAI);                       //130624
         CORN::Days inc_days_dead()                               modification_; //060911
         // returns true if the portion reached senescence today and just died.  //060316
      public:
         void increment_by(const Portion &addend)                 modification_;
      float64 dec_living(float64 subtrahend_living)               modification_; //060530
            // Decrements the area_index by the subtrahend value.
            // Returns the resulting area index.
         virtual float64  slough()                                renunciation_;                                              //060824
            // This invalidates the canopy portion.
            // the biomass amount is relinquished to the caller.                 //060824
            // Returns the biomass sloughed                                      //060824
         virtual bool  dormancy();                                               //060825
            // In senesced_biomass_shed mode,                                    //060825
            // at dormancy GAI_vital is transferred to GAI_effete                //060825
            // This is to prevent any GAI from the previous season to influence new growth //060825
      public:
         float64 reduce_green_fraction_canopy_cover
            (float64 canopy_cover_to_reduce_fraction)             modification_;
      };
      //_Portion_______________________________________________________________/
   private : mutable Portion  *accum_today;     // Accumulated values todate.
   public: // Canopy_accumulation sturctors
      inline Canopy_accumulation
         (
          #ifdef OLD_SENESCED_BIOMASS_FATE
          #error reached
          Senesced_biomass_fate_obsolete  _senesced_biomass_fate_param
          #else
          Abscission _abscission                                                 //110808
          #endif
         ,bool                   _from_previous_season
         ,CORN::Days             _linger_time                                    //080910
         )
         : Canopy_growth_portioned::Canopy_accumulation
            (
            #ifdef OLD_SENESCED_BIOMASS_FATE
            _senesced_biomass_fate_param
            #else
            _abscission
            #endif
            ,_from_previous_season,_linger_time)
         , accum_today(0)
         {};
      inline ~Canopy_accumulation(){
          if (accum_today) delete accum_today;                                      //201128LML
      }
   public: // Canopy_accumulation members
      float64 reduce_green_fraction_canopy_cover(float64 canopy_cover_to_reduce_fraction);
         // returns the remainder of canopy_cover_to_reduce_fraction
         // Normally there should be no remainder, except for any canopy_effete.
      virtual Portion *provide_accum_today()                         provision_;
      virtual Portion *provide_accum_calculated_today()              provision_; //130520
      inline virtual bool is_valid()           const { return accum_today != 0;}
      virtual bool invalidate()                                   modification_;
      inline virtual Canopy_growth_portioned::Canopy_accumulation::Portion
         *get_accum_today()                   provision_ { return  accum_today;}
      virtual bool update()                                      rectification_;
      float64 get_thermal_time_for_reset()                               const;  //130512
   };
 protected:
   mutable Canopy_accumulation canopy_vital;                                     //060911
   mutable Canopy_accumulation canopy_effete;                                    //060911
   Canopy_accumulation::Portion *untouchable_portion;                            //080818
   //_Biomass_accumulation_____________________________________________________/
 public:
   const Crop_parameters_struct::Canopy_growth_cover_based &parameters;          //140318
   virtual float64 get_regrowth_living_parameter()                         const //080807
      { return parameters.cover_initial;};                                       //110208
        // In the case of canopy_growth_LAI_based() this is the regrowth_GAI
        // in the case of canopy_growth_cover_base() this is currently simply
        // the initial fraction of canopy cover.                                 //080807
 protected: //
   const float64 &water_canopy_expansion_factor; // reference to Crop
 protected: // contributes
   mutable  float64 fract_canopy_cover_green_expansion;
   //______________________________________________________________
   const Phenology &phenology;                                                   //110901
   contribute_ float64 LWP_yesterday;                                            //110901
 public:
   mutable Canopy_cover_reference  cover_reference;                              //110901
   mutable Canopy_cover_actual     cover_actual;                                 //110901
 public: // constructor and initialization
   Canopy_growth_cover_based
      (const Crop_parameters &crop_parameters_                                   //140318
      ,const Crop_parameters_struct::Canopy_growth_cover_based &parameters_
      ,Residues_interface                   *residues_                           //060816
      ,modifiable_ Crop_nitrogen_interface  *nitrogen_                           //151006_060816
      ,const Phenology                      &phenology_
      ,const float64                        &ref_water_stress_index_yesterday_
      ,const float64                        &water_canopy_expansion_factor_
      #ifdef CO2_CHANGE
      ,Crop_CO2_response                    *CO2_response_
      #endif
      );
   inline virtual ~Canopy_growth_cover_based()                                {} //151204
   virtual bool initialize()                                    initialization_;
 public: // Canopy_growth_portioned actualized methods
   inline virtual   modifiable_ Canopy_growth_portioned::Canopy_accumulation &
      mod_canopy_vital()                                 { return canopy_vital;} //060911
   inline virtual   modifiable_ Canopy_growth_portioned::Canopy_accumulation &
      mod_canopy_effete()                               { return canopy_effete;} //060911
   inline virtual unmodifiable_ Canopy_growth_portioned::Canopy_accumulation &
      ref_canopy_vital()                           const { return canopy_vital;} //090312
   inline virtual unmodifiable_ Canopy_growth_portioned::Canopy_accumulation &
      ref_canopy_effete()                         const { return canopy_effete;} //090312
 public:
   virtual float64 get_senescence_reduction_factor()                      const; //080807
   virtual float64 calc_adjusted_ET_crop_coefficient()                    const; //200127_080811
 public:
   //NYN   virtual bool start()                                   modification_; //110901
   virtual bool update()                                         rectification_;
   virtual bool process_day()                                     modification_; //180111_110901
   virtual bool emerge_cotyledon
      (float64 new_cotalydon_GAI
      ,float64 new_cotalydon_BM)                                  modification_; //080728
   virtual float64 get_influence_on_root_activity_factor
      (bool after_end_of_vegetative_growth)                               const; //08080
   virtual bool restart_with
      (float64 restart_biomass
      ,float64 restart_GAI,bool clumping)                         modification_;
   virtual bool respond_to_clipping()                             modification_; //080515
   virtual float64 get_GAI(nat8 include_GAI)                              const;
   virtual float64 get_LAI()                                              const; //060824
   virtual float64 get_LAI(bool from_canopy)                              const; //080728
   virtual float64 get_peak_LAI()                                         const; //080728
   inline virtual float64 get_reported_peak_LAI()                         const  //141203
      { return get_peak_LAI(); }
   virtual float64 calc_plant_development                                        //990215
      (float64 length_max
      ,float64 last_length
      ,float64 overall_growth_stress)                                     const; //080805
   virtual float64 calc_cover_fract                                              //000623
      (float64 xAI
      , bool photosynthetic_active_rad_based
      ,bool use_clumping_factor = true                                           //080801
      ,bool record_shading_porosity_interception = false)                 const; //081104
      // record_shading_porosity_interception only applies to fruit tree
   virtual float64 get_interception_PAR_green()                           const;      //181107
   virtual float64 get_interception_global_green()                        const;
   //200226 virtual float64 calc_interception_global_green()             const;
   virtual modifiable_ float64 &reference_fract_canopy_cover()    modification_;
   virtual bool end_day()                                         modification_; //190821
   virtual bool start()                                           modification_;
   bool end_season(bool apply_dormancy)                           modification_; //110901

   virtual bool know_accrescence  (const Phenology::Period_thermal *accrescence)   modification_; //181230
   virtual bool know_senescence   (const Phenology::Period_thermal *senescence)    modification_; //181109
   virtual bool know_culminescence(const Phenology::Period_thermal *culminescence_)modification_; //181109
   virtual bool know_maturity_initiation()                        modification_; //200409
   virtual bool know_N_leaf_stress_factor(float64 N_leaf_stress_factor_);        //200528

 public:
   virtual float64 get_interception_global_total()                        const; //200228
   //200228 virtual float64 get_fract_canopy_cover()                      const;
   // virtual float64 get_interception_global_green()                     const;
   //200228 virtual float64 interception_insolation_global_green()        const;
      //this was previously called and is the same as fraction cover_green_fract

   virtual float64 increment_untouchable_portion
      (Canopy_growth_portioned::Canopy_accumulation::Portion &by_portion) modification_;  //080818
   virtual bool take_untouchable_portion
      (Canopy_growth_portioned::Canopy_accumulation::Portion *untouchable_portion) modification_; //080818
   virtual modifiable_ Canopy_growth_portioned::Canopy_accumulation::Portion *
      mod_untouchable_portion()                                           const; //190816
   virtual unmodifiable_ Canopy_growth_portioned::Canopy_accumulation::Portion *
      ref_untouchable_portion()                                           const;
   virtual void terminate()                                       modification_; //080728
 public:
   virtual float64 remove_biomass
            (float64 biomass_to_remove
            ,float64 retain_GAI        // biomass won't be removed if it takes the GAI below this level. Use 0.0 for no limitation
            ,bool remove_newest_first
            ,bool reserve_todays_portion
            ,Crop_biomass &removed_biomass_returned)              modification_; //190503
 public: // Not applicable to this model
   inline virtual bool age_leaf                                                  //070328
      (float64  thermal_time_GDD
      ,float64 water_stress_index)
      {  UNUSED_arg(thermal_time_GDD); UNUSED_arg(water_stress_index);
         invalidate(); return true; }
   virtual bool develop_leaf //_or_age_leaf
      (bool continue_leaf_growth,
       float64 canopy_growth
      ,float64 LAI_related_growth  // Today's LAI related growth   (Not used in this model)
      ,float64 thermal_time_growing_degree_day);     // (Not used in this model) //080728
   virtual float64 update_LAI_for_self_shading_response
      (bool after_accescence
      ,bool before_maturity)                                      modification_;
   inline float64 know_LWP_yesterday(float64 _LWP_yesterday)         cognition_
                                      { return LWP_yesterday = _LWP_yesterday; }
   float64 get_thermal_time_for_reset()                                   const; //130512
private: // Specific to this model
   float64 calc_potential_fraction_soil_interception
      (float64 CurrentCC
      ,float64 relative_thermal_time
      ,float64 cumulative_thermal_time)                                   const;
   #ifdef CO2_CHANGE
   Crop_CO2_response *CO2_response; // will be 0 if not simulating CO2           //020409
   #endif
   float64 reduce_green_fraction_canopy_cover(float64 cover_to_reduce_fraction);
};
//_Canopy_growth_cover_based___________________________________________________/
//NYI will need canopy model for orchard crop,
// don't forget  get_solid_canopy_interception()
}//_namespace CropSyst_________________________________________________________/
#endif

