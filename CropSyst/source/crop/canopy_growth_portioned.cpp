#include "canopy_growth_portioned.h"
//______________________________________________________________________________
#include "corn/math/compare.hpp"
#include "crop/canopy_growth.h"
#include "crop/crop_N_interface.h"
#include "crop/crop_biomass.h"
#include "crop/crop_param.h"
#include "common/residue/residue_decomposition_param.h"
#include "common/residue/residues_interface.h"
namespace CropSyst {
class Crop_biomass;
//______________________________________________________________________________
Canopy_growth_portioned::Canopy_accumulation::Portion::Portion
(float64 _biomass)
: CORN::Item()
, biomass_produced(_biomass)
, biomass_current (_biomass)
, days_dead(0)                                                                   //060824
{}
//______________________________________________________________________________
Canopy_growth_portioned::Canopy_accumulation::Portion::Portion()
: CORN::Item()
,biomass_produced(0)                                                             //060601
,biomass_current(0)                                                              //060601
,days_dead(0)                                                                    //060824
{}
//______________________________________________________________________________
int Canopy_growth_portioned::Canopy_accumulation::Portion::compare(const CORN::Item &other) const
{  return 0; // Comparison is significant in derived class
}
//______________________________________________________________________________
bool Canopy_growth_portioned::Canopy_accumulation::Portion::is_valid()     const
{ return !CORN::is_zero<float64>(biomass_produced);} //200127 ,0.0,0.00000001); }
//060601   Not sure if I need to look at biomass_current. Probably not because I think this was a check when adding new pool

//_is_valid_________________________________________________________2006-05-30_/
CORN::Days Canopy_growth_portioned::Canopy_accumulation::Portion
::inc_days_dead()                                                  modification_
{  days_dead += 1;
   return days_dead;
}
//_inc_days_dead____________________________________________________2006-09-11_/
#ifdef NYN
// currently abstract
float64 Canopy_growth_portioned::Canopy_accumulation::Portion::die()    // replaces clear_GAI()
{  return 0.0;
}
#endif
//_Canopy_accumulation::Portion::die________________________________2005-08-23_/
void Canopy_growth_portioned::Canopy_accumulation::Portion::increment_by
(const Canopy_growth_portioned::Canopy_accumulation::Portion &addend)
{  //Don't add age this method is used to get the current totals, the age has no meaning in this context.
   biomass_produced  += addend.biomass_produced;       // kg/m2 The biomass produced for the day. 060601
   biomass_current  += addend.biomass_current;       // kg/m2 The biomass produced for the day.   060601
   // Don't increment by days_dead  // it is a property not an amount
}
//_Canopy_accumulation::Portion::increment_by__________________________________/
float64 Canopy_growth_portioned::Canopy_accumulation::Portion::dec_biomass_current(float64 subtrahend_biomass)
{  float64 act_subtrahend_biomass =                                              //100517
      (biomass_current > subtrahend_biomass)
      ? subtrahend_biomass
      : biomass_current;
   biomass_current -= act_subtrahend_biomass;                                    //100517
   return act_subtrahend_biomass;
}
//_dec_biomass_current______________________________________________2006-09-11_/
float64  Canopy_growth_portioned::Canopy_accumulation::Portion::slough() renunciation_
{  // This invalidates the canopy portion the biomass amount is relinquished to the caller.
   // Returns the biomass sloughed
   float64 sloughed_biomass = biomass_current;
   biomass_current = 0;    biomass_produced = 0;
   days_dead = 0;
   return sloughed_biomass;
}
//_slough___________________________________________________________2006-08-24_/
bool  Canopy_growth_portioned::Canopy_accumulation::Portion::dormancy()
{  return true;
}
//_dormancy_________________________________________________________2006-08-25_/
Canopy_growth_portioned::Canopy_accumulation::Canopy_accumulation
(Abscission _abscission                                                          //110808
,bool                   _from_previous_season
,Days                   _linger_time)                                            //080910
: CORN::Bidirectional_list()
, abscission(_abscission)                                                        //110808
, from_previous_season(_from_previous_season)
, sloughed_biomass(0)
, linger_time(_linger_time)                                                      //080910
{}
//_Canopy_accumulation:constructor_____________________________________________/
float64 Canopy_growth_portioned::Canopy_accumulation
::relinquish_sloughed_biomass()                                    modification_
{  float64 relinquished_sloughed_biomass = sloughed_biomass;
   sloughed_biomass = 0.0;
   return relinquished_sloughed_biomass;
}
//_relinquish_sloughed_biomass______________________________________2008-08-08_/
bool Canopy_growth_portioned::invalidate()                         modification_
{  return mod_canopy_vital().invalidate()
       && mod_canopy_effete().invalidate();
}
//_invalidate_______________________________________________________2006-09-11_/
float64 Canopy_growth_portioned::Canopy_accumulation::remove_biomass
(float64 living_remaining_available // GAI or fCCg  (was GAI_remaining_available)
,float64 biomass_to_remove
,float64 retain_vital_living        // biomass won't be removed if it takes the GAI below this level  //060323
,bool remove_newest_first
,bool reserve_todays_history)                                      modification_
{
   if (CORN::is_zero<float64>(biomass_to_remove/*200127 ,0.0,0.0000001*/)) return 0.0;
   float64 more_biomass_to_remove = biomass_to_remove;
   // Now loop through the GAI_ages from the tail and remove the newest leaves
   float64 act_removed_biomass = 0.0;
   //We can now either remove the newest biomass first or the oldest biomass first
   Canopy_growth_portioned::Canopy_accumulation::Portion *curr_daily = 0;        //070806
   for (curr_daily = remove_newest_first
         ? dynamic_cast<Canopy_growth_portioned::Canopy_accumulation::Portion *>(pop_tail())
         : dynamic_cast<Canopy_growth_portioned::Canopy_accumulation::Portion *>(pop_head())
      ;(curr_daily && (more_biomass_to_remove > 0.0))
      ;curr_daily = remove_newest_first
         ? dynamic_cast<Canopy_growth_portioned::Canopy_accumulation::Portion *>(pop_tail())
         : dynamic_cast<Canopy_growth_portioned::Canopy_accumulation::Portion *>(pop_head()))
   {  float64 daily_canopy_biomass = curr_daily->get_biomass_current();          //060601
      float64 daily_living = curr_daily->get_living();                           //060323
      bool need_to_retain_living = from_previous_season ? false :                //060911
           ((living_remaining_available - daily_living) < retain_vital_living);  //060911 Don't allow the GAI fCCg to go below the limit
      if (!need_to_retain_living)                                                //060323
      {  if  (more_biomass_to_remove > daily_canopy_biomass)                     //060323
         { //  Remove the whole daily bit
            more_biomass_to_remove -= daily_canopy_biomass;
            act_removed_biomass += daily_canopy_biomass;
            delete curr_daily; curr_daily = 0;                                   //070806
            living_remaining_available -= daily_living;                          //060323
         } else // We just have a little bit left to remove, we just won't bother to do it
         {  float32 living_to_remove = curr_daily->get_living()
               * (more_biomass_to_remove / curr_daily->get_biomass_current());   //060601
            curr_daily->dec_living(living_to_remove);  // was dec_green_area_index 050117
            living_remaining_available -= living_to_remove;                      //060323
            float64 act_mass_dec =                                               //100517
               curr_daily->dec_biomass_current(more_biomass_to_remove);          //060601
            act_removed_biomass += act_mass_dec;                                 //100517
            more_biomass_to_remove -= act_mass_dec;                              //100517
            // We had a tad left so we need to push it back onto the list
            if (remove_newest_first)                                             //100517
                 append (curr_daily);                                            //100517
            else prepend(curr_daily);                                            //100517
         }
      }
   }
   if (curr_daily )                                                              //070806
   {  // We didn't deduct this latest popped portion so put it back on the list
      if  (remove_newest_first)                                                  //070806
             append(curr_daily);                                                 //070806
      else  prepend(curr_daily);                                                 //070806
   }
   invalidate();
   // remove biomass N is now processed with biomatter fate                      //050822
 return act_removed_biomass;
}
//_remove_biomass___________________________________________________2004-06-23_/
bool Canopy_growth_portioned::Canopy_accumulation
::process_day()                                                    modification_ //180111 was process()
{  FOR_EACH(portion,Portion ,each_portion)
   {  if (portion && portion->is_dead())                                         //060824
      {  Days days_dead = portion->inc_days_dead();                              //060824
         #ifdef OLD_SENESCED_BIOMASS_FATE
         switch (senesced_biomass_fate_param)                                    //060824
         {  case senesced_biomass_linger :                                       //060824
            { if (days_dead > linger_time)                                       //060824
              {   sloughed_biomass += portion->slough();                         //060824
                  each_portion->detach_current();                                //070418
                  delete portion; portion = 0;                                   //070328
              }
            } break;
            case senesced_biomass_shed :                                         //060825
            {  if (from_previous_season)                                         //060824
               {  sloughed_biomass += portion->slough();                         //060824
                  each_portion->detach_current();                                //070418
                  delete portion; portion = 0;                                   //070328
               }
            } break;
        } // end switch
         #else
         switch (abscission)                                                     //110808_060824
         {  case semi_deciduous /*was senesced_biomass_linger */:                //110808_060824
            { if (days_dead > linger_time)                                       //060824
              {   sloughed_biomass += portion->slough();                         //060824
                  each_portion->detach_current();                                //070418
                  delete portion; portion = 0;                                   //070328
              }
            } break;
            case marcescent /* was senesced_biomass_shed*/ :                     //110808_60825
            {  if (from_previous_season)                                         //060824
               {  sloughed_biomass += portion->slough();                         //060824
                  each_portion->detach_current();                                //070418
                  delete portion; portion = 0;                                   //070328
               }
            } break;
            default: break;                                                      //170217
        } // end switch
         #endif
      } // if is_dead
   } FOR_EACH_END(each_portion)
   return true;
}
//_Canopy_growth_common::Canopy_accumulation::process_______________2008-09-10_/
bool Canopy_growth_portioned::process_day()                        modification_ //180111 renamed
{
   update();                                                                     //191106
   mod_canopy_vital().process_day();                                             //180111 renamed
   float64 vital_sloughed_biomass = mod_canopy_vital().relinquish_sloughed_biomass(); //060816
   bool lingers =
      #ifdef OLD_SENESCED_BIOMASS_FATE
      crop_parameters.senesced_biomass_fate_labeled_obsolete.get() == senesced_biomass_linger;
      #else
      crop_parameters.abscission_clad.get() == semi_deciduous;                   //110808
      #endif
   const Residue_decomposition_parameters  &residue_decomposition_params
      = crop_parameters.residue_decomposition;                                   //080806_110613
   if (vital_sloughed_biomass > 0.0 && residues)                                 //110613_060824
      residues->add_surface_plant_pool                                           //060824
      (lingers ?  OM_flat_surface_position : OM_stubble_surface_position         //110808_040517
      ,OM_ORIGINAL_CYCLING                                                       //060219
      ,vital_sloughed_biomass
      #ifdef NITROGEN
      ,nitrogen ? nitrogen->get_canopy_concentration(include_vital) : 0.0
      #endif
      ,residue_decomposition_params);                                            //060219_110613
   mod_canopy_effete().process_day();                                            //180111 renamed
   float64 sloughed_biomass_effete = mod_canopy_effete().relinquish_sloughed_biomass(); //060816
   if (sloughed_biomass_effete > 0.0 && residues)                                //060824
      residues->add_surface_plant_pool                                           //060824
      (lingers ?  OM_flat_surface_position : OM_stubble_surface_position         //110808_040517
      ,OM_ORIGINAL_CYCLING                                                       //060219
      ,sloughed_biomass_effete
      #ifdef NITROGEN
      ,nitrogen ? nitrogen->get_canopy_concentration(include_effete) : 0.0
      #endif
      ,residue_decomposition_params                                              //060219
      );                                                                         //060816
   // Do nothing to untouchable portion                                          //071126
   return true;                                                                  //080728
}
//_Canopy_growth_common::process____________________________________2008-09-10_/
bool Canopy_growth_portioned::update()                            rectification_
{  Canopy_growth_portioned &updatable_self = const_cast<Canopy_growth_portioned &>(*this);
   updatable_self.invalidate();
   updatable_self.mod_canopy_vital().update();                                   //060816
   updatable_self.mod_canopy_effete().update();                                  //060816
   return true;                                                                  //080728
}
//_update______________________________________________________________________/
Canopy_growth_portioned::Canopy_growth_portioned
(const CropSyst::Crop_parameters    &crop_parameters_
/*190628
 #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
 // actually doesn't appear to be used
,const Thermal_time_immutable       &thermal_time_                               //151106
#endif
*/
,Residues_interface                 *residues_                                   //060816
,CropSyst::Crop_nitrogen_interface  *nitrogen_)                                  //060816
: Canopy_leaf_growth                                                          ()
, crop_parameters                                             (crop_parameters_)
/*190628
 #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
 // actually doesn't appear to be used
, thermal_time                (thermal_time_)
#endif
*/
, residues                                                           (residues_)
, nitrogen                                                           (nitrogen_)
{}
//_constructor__________________________________________________________________
float64 Canopy_growth_portioned::remove_all_biomass
(Crop_biomass &accumulated_removed_biomass)                        modification_
{  // This removes all canopy biomass from the plant (I.e. in the case of termination)
   // Returns amount of biomass removed
   float64 act_effete_removed_biomass = mod_canopy_effete().remove_biomass(999999.0,999999.0,0,true,  false);   //060911
   float64 act_vital_removed_biomass =  mod_canopy_vital() .remove_biomass(999999.0,999999.0,0,true,  false);   //060911
   // the untouchable portion is not removable                                   //071126
   float64 act_removed_biomass = act_effete_removed_biomass + act_vital_removed_biomass;
   float64 act_removed_N_mass    = 0;                                            //070414
   if (nitrogen)                                                                 //070412
   {  act_removed_N_mass = nitrogen->remove_canopy_nitrogen_for                  //070412
      (999999.0 ,include_vital|include_effete);
   }
   CropSyst::Crop_biomass removed_now(act_removed_biomass,act_removed_N_mass);   //070802
   accumulated_removed_biomass.add_crop_biomass(removed_now);                    //070802
   invalidate();
 return act_removed_biomass;
}
//_remove_all_biomass_______________________________________________2007-06-27_/
float64 Canopy_growth_portioned::remove_biomass
(float64 biomass_to_remove
,float64 retain_vital_living                                               //080806
         // biomass won't be removed if it takes the GAI or fCCg below this level. Use 0.0 for no limitation
,bool remove_newest_first
,bool reserve_todays_portion
,Crop_biomass &accumulated_removed_biomass_returned)               modification_ //070412
{  float64 vital_living_remaining_available = get_living_index(include_vital);   //060911
   float64 effete_living_remaining_available = get_living_index(include_effete); //060911
   float64 act_effete_removed_biomass = mod_canopy_effete()                      //060911
      .remove_biomass(effete_living_remaining_available,biomass_to_remove
         ,retain_vital_living ,remove_newest_first,  reserve_todays_portion);
   biomass_to_remove -= act_effete_removed_biomass;                              //060911
   float64 act_vital_removed_biomass = mod_canopy_vital()                        //060911
      .remove_biomass(vital_living_remaining_available ,biomass_to_remove
         ,retain_vital_living ,remove_newest_first,  reserve_todays_portion);
   biomass_to_remove -= act_vital_removed_biomass;                               //060911
   // At this point biomass_to_remove should be 0.0 if the specified biomass exists
   // otherwise the requested amount was not available.
   float64 act_removed_biomass = act_effete_removed_biomass + act_vital_removed_biomass;
   float64 act_removed_N_mass = 0;                                               //070414
   if (nitrogen)                                                                 //070412
   {  float64 act_vital_removed_N_mass    = nitrogen->remove_canopy_nitrogen_for //070414
         (act_vital_removed_biomass ,include_vital);
      float64 act_effete_removed_N_mass   = nitrogen->remove_canopy_nitrogen_for //070412
         (act_effete_removed_biomass,include_effete);
      act_removed_N_mass = act_vital_removed_N_mass + act_effete_removed_N_mass; //070414
   }
   Crop_biomass removed_now(act_removed_biomass,act_removed_N_mass);             //070802
   accumulated_removed_biomass_returned.add_crop_biomass(removed_now);           //070802
   // remove biomass N is now processed with biomatter fate                      //050822
   // The untouchable_portion is not removable                                   //071126
 return act_removed_biomass;
}
//_remove_biomass______________________________________________________________/
nat32 Canopy_growth_portioned::vital_becomes_effete(bool apply_dormancy) modification_
{
   float64 portion_count = 0;
   while (Canopy_accumulation::Portion *portion
       = dynamic_cast<Canopy_accumulation::Portion *>
          (mod_canopy_vital().pop_head()))                                    //060911
   {
      if (apply_dormancy)                                                        //080401
         portion->dormancy();                                                    //060911
      mod_canopy_effete().append(portion);                                       //060911
      portion_count++;
   }
   return portion_count;
}
//_vital_becomes_effete_____________________________________________2018-07-18_/
bool Canopy_growth_portioned::end_season(bool apply_dormancy)      modification_ //080401
{  // At this point there may be some effete portions from previous years that have been exhausted they can be cleaned now
   vital_becomes_effete(apply_dormancy);                                         //180718
   return true;
}
//_end_season______________________________________________________2006-08-25__/
float64 Canopy_growth_portioned::get_live_green_biomass
(bool include_vital,bool include_effete)                                   const
{  // Note, could make the live_biomass a property of portion
 // and simply add this with the reset or update()
   if (!is_valid()) update();                                                    //060316
   float64 live_biomass = 0.0;  // kg_m2
   if (include_vital)                                                            //060911
   {  FOR_EACH_IN(portion,const Canopy_accumulation::Portion ,ref_canopy_vital() ,each_portion)
      {  if (!portion->is_dead()) // obs caller should make sure senescence does not apply duration already checked to determine if dead(then dont call this function) (parameters->has_leaf_senescence()  && (curr_daily->age > parameters->leaf_duration_deg_day_f32))
         live_biomass += portion->get_biomass_current();  // produced and current should be the same for live 060601
      } FOR_EACH_END(each_portion)
      modifiable_ Canopy_growth_portioned::Canopy_accumulation::Portion          //190816
         *untouchable_portion = mod_untouchable_portion();                       //190816
      live_biomass += untouchable_portion                                        //071126
         ? untouchable_portion->get_biomass_current() : 0.0;
      /*190816
      live_biomass += ref_untouchable_portion()                                  //071126
         ? ref_untouchable_portion()->get_biomass_current() : 0.0;
      */
   }
   if (include_effete)                                                           //060911
   FOR_EACH_IN(portion,Canopy_accumulation::Portion,ref_canopy_effete(),each_portion)
   {  if (!portion->is_dead()) // obs caller should make sure senescence does not apply duration already checked to determine if dead(then dont call this function) (parameters->has_leaf_senescence()  && (curr_daily->age > parameters->leaf_duration_deg_day_f32))
      live_biomass += portion->get_biomass_current();  // produced and current should be the same for live 060601
   } FOR_EACH_END(each_portion)
 return live_biomass;
}
//_get_live_green_biomass___________________________________________2006-03-13_/
bool Canopy_growth_portioned::is_valid()                                   const
{  return ref_canopy_vital() .is_valid()
      ||  ref_canopy_effete().is_valid()                                         //080807
      || (ref_untouchable_portion() != NULL);                                    //080807
}
//_is_valid____________________________________________________________________/
void Canopy_growth_portioned
::die_back(float64 fraction_to_die)                                modification_
{  // This sets the specified fraction of live biomass to effete
   // The biomass remains associated with the plant (it is not sent to residue)
   float64 remaining_to_die = get_living_index(include_vital|include_effete)
      * fraction_to_die;
   // Killing of effete first
   FOR_EACH_IN(effete_portion,Canopy_accumulation::Portion,mod_canopy_effete()
              ,each_effete_portion)
   {  float64 living_this_portion = effete_portion->get_living();
      if ((remaining_to_die > 0.0)  &&
          (living_this_portion > 0.0))                                           //130626
         effete_portion->die();
       remaining_to_die -= living_this_portion;
   } FOR_EACH_END(each_effete_portion)
   FOR_EACH_IN(vital_portion,Canopy_accumulation::Portion
      ,mod_canopy_vital(),each_vital_portion)
   {  float64 living_this_portion = vital_portion->get_living();
      if ((remaining_to_die > 0.0) &&
          (living_this_portion > 0.0))                                           //130626
/*
      if ((remaining_to_die > 0.0) &&
          (living_this_portion > 0.0))                                           //130626
*/
         vital_portion->die();
       remaining_to_die -= living_this_portion;
   } FOR_EACH_END(each_vital_portion)
   // do not die back untouchable_portion                                        071126
   invalidate();
}
//______________________________________________________________________die_back
float64 Canopy_growth_portioned::damage_GAI(float64 GAI_damage)    modification_
{  float64 current_GAI = get_living_index(include_vital|include_effete);
   float64 fract_damage =
      current_GAI > 0.000001
      ? GAI_damage/current_GAI
      : 0.0;
   die_back(fract_damage);
   return current_GAI - GAI_damage;
}
//_damage_GAI_______________________________________________________2015-12-26_/
void Canopy_growth_portioned
::die_back_to_GAI(float64 target_GAI)                              modification_
{  // This sets the specified fraction of live biomass to effete
   // The biomass remains associated with the plant (it is not sent to residue)
   float64 remaining_to_die = get_GAI(include_vital|include_effete)-target_GAI;
   // Killing of effete first
   FOR_EACH_IN(effete_portion,Canopy_accumulation::Portion
              ,mod_canopy_effete(),each_effete_portion)
   {  float64 living_this_portion = effete_portion->get_living();
      if ((remaining_to_die > 0.0)  &&
          (living_this_portion > 0.0))                                           //130626
         effete_portion->die();
       remaining_to_die -= living_this_portion;
   } FOR_EACH_END(each_effete_portion)
   FOR_EACH_IN(vital_portion,Canopy_accumulation::Portion
              ,mod_canopy_vital(),each_vital_portion)
   {  float64 living_this_portion = vital_portion->get_living();
      if (living_this_portion > 0.0)
      {
      if (remaining_to_die > living_this_portion)
      {   vital_portion->die();
         remaining_to_die -= living_this_portion;
      }
      else
      {  float64 new_GAI = living_this_portion - remaining_to_die;
         vital_portion->die_back_to_GAI(new_GAI);
         remaining_to_die = 0.0;
      }
      }
   } FOR_EACH_END(each_vital_portion)

   // Originally we did not die back untouchable_portion                         //071126
   // Claudio says to now die back the untouchable portion                       //131113
   if (remaining_to_die > 0.0)                                                   //131113
   {  Canopy_growth_portioned::Canopy_accumulation::Portion *
         untouchable_portion = mod_untouchable_portion();
      float64 living_untouchable_portion = untouchable_portion->get_living();
      if (remaining_to_die > living_untouchable_portion)
      {  untouchable_portion->die();
         remaining_to_die -= living_untouchable_portion;
      } else
      {  float64 new_GAI = living_untouchable_portion - remaining_to_die;
         untouchable_portion->die_back_to_GAI(new_GAI);
         remaining_to_die = 0.0;
      }
   }
   invalidate();
}
//_die_back_____________________________________________________________________
float64 Canopy_growth_portioned::Canopy_accumulation::get_living()         const
{  if (!is_valid()) update();
   return get_accum_today()->get_living();
}
//_Canopy_accumulation::get_living__________________________________2008-08-08_/
float64 Canopy_growth_portioned::Canopy_accumulation::get_biomass_current()const
{  if (!is_valid())
      update();
   return get_accum_today()->get_biomass_current();
}
//_Canopy_accumulation::get_biomass_current____________________________________/
float64 Canopy_growth_portioned::get_living_index(nat8 include_living)    const
{  float64 living_vital  = (include_living & include_vital)  ? ref_canopy_vital().get_living() : 0.0;   //080808_060921
   float64 living_effete = (include_living & include_effete) ? ref_canopy_effete().get_living(): 0.0;   //080808_060921
   const /*Canopy_growth_portioned::*/
      Canopy_accumulation::Portion *untouchable_portion
      = ref_untouchable_portion();                                               //190816
   float64 living_untouchable
      = (include_living & include_vital)  && untouchable_portion
       //190816 ref_untouchable_portion()
       ? /*190816 get_untouchable_portion()*/
         untouchable_portion->get_living() : 0.0;                                //071126
   return living_vital + living_effete + living_untouchable;                                                                                                                  //071126
}
//_get_living_index_____________________________________________________________
float64 Canopy_growth_portioned::get_biomass_current(nat8 include_biomass) const
{
   //unmodifiable_ Canopy_accumulation &canopy_vital = ref_canopy_vital();
   float64 biomass_current_vital = (include_biomass & include_vital)  ?  ref_canopy_vital().get_biomass_current() : 0.0;        //080808_070321
   float64 biomass_current_effete =(include_biomass & include_effete) ? ref_canopy_effete().get_biomass_current() : 0.0;        //080808_070321
   const Canopy_growth_portioned::Canopy_accumulation::Portion *untouchable_portion =  ref_untouchable_portion();
   float64 biomass_untouchable = (include_biomass & include_vital) && untouchable_portion ? untouchable_portion->get_biomass_current() : 0.0;//071126
   float64 biomass_current
      = biomass_current_vital + biomass_current_effete + biomass_untouchable;    //071126
   return biomass_current;
}
//_get_biomass_current__________________________________________________________
float64 Canopy_growth_portioned::Canopy_accumulation::get_biomass_produced() const
{  if (!is_valid()) update();
   return get_accum_today()->get_biomass_produced();
}
//_Canopy_accumulation::get_biomass_produced________________________2008-08-08_/
float64 Canopy_growth_portioned::get_biomass_produced(nat8 include_biomass) const
{  float64 biomass_produced_vital = (include_biomass & include_vital)
      ? ref_canopy_vital().get_biomass_produced()           : 0.0;               //070321
   float64 biomass_produced_effete =(include_biomass & include_effete)
      ? ref_canopy_effete().get_biomass_produced()          : 0.0;               //070321
   const Canopy_growth_portioned::Canopy_accumulation::Portion *untouchable_portion =  ref_untouchable_portion();
   float64 biomass_untouchable = (include_biomass & include_vital) && untouchable_portion //190816 get_untouchable_portion()
      ? //190816 get_untouchable_portion()
        untouchable_portion->get_biomass_produced()   : 0.0;               //071126
   return biomass_produced_vital + biomass_produced_effete +biomass_untouchable; //071126
}
//_get_biomass_produced_____________________________________________2007-03-21_/
bool Canopy_growth_portioned::take_portion
(Canopy_growth_portioned::Canopy_accumulation::Portion  *todays_portion
,bool initial_restart_vital                                                      //071126
,bool protect_it)                                                  modification_
{  bool portion_valid = todays_portion && todays_portion->is_valid();            //081101
   if (portion_valid)                                                            //081101
   {  if (initial_restart_vital)                                                 //071126
         take_untouchable_portion(todays_portion);                               //080818
      else                                                                       //071126
      {  if (protect_it)                                                         //060317
            mod_canopy_vital().prepend(todays_portion);
         else
         {  const Canopy_growth_portioned::Canopy_accumulation::Portion          //190816
               *untouchable_portion =  ref_untouchable_portion();                //190816
            if (!untouchable_portion) //190816 get_untouchable_portion())                                      //080222
               take_untouchable_portion(todays_portion);                         //080818
            else {  // skim off some GAI or fCCg for regrowth
               float64 regrowth_living = get_regrowth_living_parameter();        //080806
               float64 more_living_needed_for_regrowth
                  = regrowth_living - untouchable_portion
                        //190816 get_untouchable_portion()
                                                         ->get_living();         //080222
               float64 portion_living = todays_portion->get_living();            //080222
               if ((more_living_needed_for_regrowth > 0.0)
                   && (portion_living < more_living_needed_for_regrowth))        //080222
               {  increment_untouchable_portion(*todays_portion);                //080818
                  delete todays_portion; todays_portion = 0;                     //080222
               }
               else
               {  // if started to senesce any GAI (Any GAI has died off)        //080515
                  // This is for the case when we have continuous                //080515
                  // annual grasses (Mike Rivington),                            //080515
                  // the GAI curve was dipping down after peak LAI               //080515
                  // but then recovering                                         //080515
               }
            }
            if (todays_portion) // may be NULL if we added it to the untouchable portion  //080222
               mod_canopy_vital().append(todays_portion);
         }
      }
   } else  delete todays_portion;
   invalidate();
   return portion_valid;                                                         //081101
}
//_take_portion_________________________________________________________________
bool Canopy_growth_portioned::Canopy_accumulation::update()       rectification_
{ return true;}
//_update______________________________________________________________________/
}//_namespace CropSyst_________________________________________________________/

