#ifndef runoff_SCSH
#define runoff_SCSH
#include "soil/runoff.h"
namespace CropSyst {
#ifdef DUMP_CURVE_NUMBERS
extern std::ofstream *SCS_runoff_dat_file; // special outout for Rolf Sommer
#endif
//______________________________________________________________________________
class Soil_runoff_SCS
: public extends_ Soil_runoff_common
{
//200316    Runoff_model   runoff_model;                                                  //990220
   friend class Soil_cropsyst_specific;
   #ifdef XLS_OUTPUT
   friend class CropSyst_DLY_report_class;
   #endif
public: // <- temporarily public so we can access these variables for output
   /*200316 moved to common
   float64        random_roughness_m;                                            //990422
      ///< This is used to compute maximum depression storage.
      ///  It is reset with tillage events, and changes with each precipitation event.
   float64        act_crop_storage;      // m                                    //990528
   float64        act_residue_storage;   // m                                    //990528
   */
public:
   Soil_runoff_SCS
      (
      //200316 Runoff_model _runoff_model,
       Soil_base &_soil );                                                       //020527
   /*200316 moved to common
   inline float64 set_random_roughness_m(float64 _random_roughness_m)            //990422
      { return random_roughness_m =  _random_roughness_m; }
      ///< This is called when management event is processed
      /// and the operation disturbs the soil.
      /// Returns the same value set.
   virtual void adjust_random_roughness                                          //990422
      (float64 effective_precipitation
      ,float64 fract_canopy_cover
      ,float64 fract_residue_cover);
      ///< This is called every day.
      /// If there is precipitation, it smooths out the soil that may be
      /// roughened by previous tillage operations.
   float64 maximum_depression_storage();                                         //990422
      ///< This is used to calculate the surface water storage (Numerical runoff model only)
   float64 calculate_crop_storage
      (Land_treatment land_treatment
      //190616 now member ,Land_use land_use
      , float64 fract_canopy_cover);  //020322
   float64 calculate_residue_storage(float64 fract_residue_cover);               //990528
      ///< These are called every day.
   */
 protected: // CSC curve number calculations                                     //031107
   float64 surface_storage_n
      (float64 steepness  // % 0-100
      ,float64 condition );
   float64 lookup_condition
      (Land_use             land_use   // <- not a var will be modified only locally
      ,Land_treatment       treatment
      ,Hydrologic_condition hydrocondition
      ,USDA_NRCS::Hydrologic_group     hydrogroup
      ,int16                 hydrologic_curve_number // will override curve number selection
      ,int16                 user_fallow_curve_number                            //990216
      ,CNs                   antecedent_moisture_content
      ,bool                  soil_frozen_1
      ,float64               soil_bulk_density_1
      ,float64               crop_fract_canopy_cover
      ,bool                  has_crop_cover);                                    //181108

 public:  // CSC curve number calculations                                       //031107
   float64 get_surface_storage
      (Hydrologic_condition hydrologic_condition                                 //971011
      ,Land_treatment treatment
      ,bool                   has_crop_cover                                     //181108
      ,float64 crop_fract_canopy_cover);
      ///< Calculates and returns the surface storage (m) when using the SCS curve number method.
      /// Note that the numerical solution, will use either a user specified value (soil paramter)
      /// Or will calculate it based on depressional storage.
   float64 runoff
      (Hydrologic_condition   crop_hydrologic_condition                          //971011
      ,Land_treatment         management_land_treatment
      ,bool                   has_crop_cover                                     //181108
      ,float64                crop_fract_canopy_cover
      ,float64                non_intercept_precip);
      ///< Calculate and return the runoff estimated by the SCS curve number method.
   /*200316
   inline virtual bool using_numerical_runoff_model()                      const //990302
      {  ///< This method is overridden by Soil_runoff_SCS class which does the actual determination
         return runoff_model == NUMERICAL_RUNOFF_MODEL;
      }
   */
 public:  // The following are used only in the watershed model
   void distribute_runoff_in_intervals                                           //990722
      (Dynamic_float_array &runoff_in_intervals
      ,const Dynamic_float_array &precip_pattern);
      ///< In the watershed model, runoff is communicated to adjacent cells
      /// in (30 minute) intervals.
      /// This method takes the daily runoff and outputs it
      /// in a hydrocurve matching the precipitation pattern (intervals).
      /// This is used only in the watershed model.
   inline virtual void add_runoff_from_runon_curve_number(float64 additional)    //990214
      {  estimated_runoff += additional; }
      ///< Runon from surrounding cells may be added to this cell's runoff.
      /// This is used only in the watershed model.
};
//_Soil_runoff_SCS__________________________________________________1998-07-29_/
}//_namespace CropSyst_________________________________________________________/
#endif

