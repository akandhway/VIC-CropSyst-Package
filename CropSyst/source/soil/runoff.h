#ifndef runoffH
#define runoffH

// This options.h needs to be for the current program being compiled
// is should not necessarily be /cropsyst/cpp/options.h
#include "options.h"
#include "common/soil/SCS/SCS.h"
#include "USDA/NRCS/soil/survey/domains.h"
#include "corn/dynamic_array/dynamic_array_T.h"
#define Dynamic_float_array Dynamic_array<float32>
#include "cs_UED_harvest.h"
#include "model_options.h"
#include "cs_vars.h"
#include "soil/soil_base.h"

class Water_balance_accumulators_class;
#ifdef XLS_OUTPUT
class CropSyst_DLY_report_class;
#endif

class Soil_cropsyst_specific;                                                    //060504
namespace CropSyst {
//______________________________________________________________________________
interface_ Soil_runoff
{
   //200316 inline virtual bool using_numerical_runoff_model() affirmation_{return true;}
          virtual void add_runoff_from_infiltration                              //990214
             (float64 additional_runoff)                        modification_=0;
   virtual Land_use know_crop_land_use(Land_use crop_land_use)  recognition_=0;  //190617
   virtual bool clear()                                         modification_=0; //200316
   virtual const float64 &ref_estimated_runoff()                        const=0; //200316
   virtual float64 calculate_crop_storage                                        //200316
      (Land_treatment land_treatment
      , float64 fract_canopy_cover)=0;

   virtual float64 calculate_residue_storage(float64 fract_residue_cover)    =0; //200316_990528
      ///< These are called every day.
   virtual float64 set_random_roughness_m(float64 _random_roughness_m)       =0; //990422
      ///< This is called when management event is processed
      /// and the operation disturbs the soil.
      /// Returns the same value set.
   virtual void adjust_random_roughness                                          //990422
      (float64 effective_precipitation
      ,float64 fract_canopy_cover
      ,float64 fract_residue_cover)                                          =0;
      ///< This is called every day.
      /// If there is precipitation, it smooths out the soil that may be
      /// roughened by previous tillage operations.
   virtual float64 maximum_depression_storage()                              =0; //990422
      ///< This is used to calculate the surface water storage (Numerical runoff model only)
};
//_interface_Soil_runoff____________________________________________2014-12-01_/
class Soil_runoff_common
: public implements_ Soil_runoff
{
 protected:
   Soil_base &soil;  // reference to the soil status                             //020527
 protected:
   Land_use crop_land_use; // SCS
   float64        act_crop_storage;      // m                                    //990528
   float64        act_residue_storage;   // m                                    //990528
   float64        random_roughness_m;                                            //990422
      ///< This is used to compute maximum depression storage.
      ///  It is reset with tillage events, and changes with each precipitation event.

 public: // <- temporarily public so we can access these variables for output
   float64        estimated_runoff;
 public:
   Soil_runoff_common(Soil_base &soil_);                                         //200316
   inline virtual ~Soil_runoff_common()                                       {} //170217
   inline virtual void add_runoff_from_infiltration(float64 additional_runoff)   //990214
      { estimated_runoff += additional_runoff; }
      ///< Each iteration in the infiltration model may produce a little bit of
      /// runoff this is accumulated in estimated runoff (stored by this object).
   #if (CROPSYST_VERSION==4)
   // May want this in V5
   bool setup_structure(CORN::Data_record &data_rec,bool for_write);             //170225_030801
   #endif
   inline virtual Land_use know_crop_land_use
      (Land_use crop_land_use_)                                     recognition_ //190617
      {  return crop_land_use = crop_land_use_; }
   inline virtual bool clear()    modification_{estimated_runoff=0;return true;} //200316
   virtual const float64 &ref_estimated_runoff() const{return estimated_runoff;} //200316
   virtual float64 calculate_crop_storage
      (Land_treatment land_treatment
      , float64 fract_canopy_cover);                                             //200316
   virtual float64 calculate_residue_storage(float64 fract_residue_cover);       //200316
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
   virtual float64 maximum_depression_storage();                                 //990422
      ///< This is used to calculate the surface water storage (Numerical runoff model only)

};
//_Soil_runoff_common_______________________________________________2014-12-01_/
}//_namespace CropSyst_________________________________________________________/
#endif
//soilroff.h

