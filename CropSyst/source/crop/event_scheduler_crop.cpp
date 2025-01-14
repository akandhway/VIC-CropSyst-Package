#include "crop/event_scheduler_crop.h"
#include "crop/yield.h"
#include "crop/crop_cropsyst.h"
namespace CropSyst {
//______________________________________________________________________________
nat16  Event_scheduler_crop::synchronize_thermal_time
(float32 thermal_time_today)
// we can optionally synchronize only if the associated object matches
{
   // This function scans the scenario events list for thermal time
   // matching synchronizations.
   // It creates new actual date events based on the date to syncronize to.
   // The actual events operation pointer currently is normally
   // set to the potential event, because syncronization may
   // produce multiple actual events based on the potential
   // event definition, we dont delete or convert potential
   // events to actual events, we do move the potential events
   // to the synced_pot_events so we can optimize the matching
   // process not having to scan for events that have already
   // been syncronized and probably already processed.
   // Returns the number of events added to the act_events
   // list for the syncronization.
   nat16  sync_count = 0;
   Common_event_list &queue = *active_queue;
   FOR_EACH_IN(event,Simple_event,queue,each_thermal_time_sync)
   {
      bool event_needs_synchronization = !event->is_synchronized();              //060412
      if (event_needs_synchronization)                                           //150930_060412
      {  // Check the start date
         int32 synced_date = event->begin_sync.determine_from_thermal_time
            (thermal_time_today,today_scheduled);
         if (synced_date)                                                        //060412
         {  sync_count ++;
            event->date.set_date32(synced_date);                                 //170525_120314
         }
         // Check the end date
         Common_event *operation_event = dynamic_cast<Common_event *>(event);
         if (operation_event && operation_event->period)                         //120725
         {  int32 synced_date = operation_event->period->end_sync
               .determine_from_thermal_time(thermal_time_today,today_scheduled);
            sync_count += synced_date != 0;
            operation_event->period->end_date.set_date32(synced_date);           //170525
         }
   }  } FOR_EACH_END(each_thermal_time_sync)
   return sync_count;   // 021103 not sure how sync_count is used, it may be obsolete now
}
//_synchronize_thermal_time_____________________________2015-09-30__2003-06-09_/
bool Event_scheduler_crop::end_season()                            modification_ //150930
{
   // The object is usually a crop
   // When a crop restarts after dormancy, call this function specifying the related crop.
   // The synchronizations will be clear
   Common_event_list unscheduled_events;                                         //030614
   nat16  rescheduled_count = 0;
   while (!events_for_next_season.is_empty())                                    //030614
   {
      Simple_event *event = dynamic_cast<Simple_event *>
         (events_for_next_season.pop_first());                                   //011207
         active_queue->append(event);
         rescheduled_count += 1;
   }
   return true;
}
//______________________________________________________________________________
/* In V5 this shouldn't be needed we simply let the queues delete themselves
when the crop and this event schedule goes out of scope.
nat16  Event_scheduler_crop::clear_growing_season_events(void *object)
{  // The object is usually a crop
   // When a crop is terminated, call this function specifying the related crop.
   // This calls remove_events_associated_with
   // and also clears events_for_next_season
   // Note we dont simply clear the list, because their may be
   // multiple event (I.e. CANMS or intercropping etc.)
   nat16  clear_count = 0;
   FOR_EACH_IN(event,Simple_event,events_for_next_season,event_iter)
   {  if (event->is_associated_with(object))
      {  event_iter->detach_current();                                           //030709
         completed_events.append(event);
         clear_count++;
      }
   } FOR_EACH_END(event_iter)                                                    //030709
   // Note we must not remove synced pot event since they may be
   // sharing operations.
   return clear_count + remove_events_associated_with(object);
}
*/
//______________________________________________________________________________
Event_status_indicator Event_scheduler_crop_complete::process_model_specific_event
(const Simple_event *event)
{  Event_status_indicator event_status = ES_FAIL;
   #ifdef YAML_PARAM_VERSION
      assert(false); // NYI  I intend to handle parameter adjustments differently with YAML version
   #else
      const CropSyst::Crop_parameters::Adjustment_event *as_adjustment_event
         = dynamic_cast<const CropSyst::Crop_parameters::Adjustment_event *>(event); //120719
      if (as_adjustment_event)                                                   //120725
      {  event_status = perform_crop_adjustment
            (dynamic_cast<const CropSyst::Crop_parameters::Adjustment&>(*as_adjustment_event)); //120719
      } else
      {  const CropSyst::Crop_parameters_class::Yield::Grain_stress_sensitivity_period *stress_period               //130428
            = dynamic_cast<const CropSyst::Crop_parameters_class::Yield::Grain_stress_sensitivity_period *>(event); //130428
         bool iniciate_period = stress_period != NULL;                           //130428
         if (iniciate_period )                                                   //150825
         {
            Yield_grain *grain_yield = dynamic_cast<Yield_grain *>(crop.yield);  //150825
            if (grain_yield && grain_yield->temperature_extreme_reponse)         //150825
            {  grain_yield->temperature_extreme_reponse->add_period(*stress_period); //150825_130428
               event_status = ES_COMPLETE;                                       //130428
            }
         }
      }
   #endif
   return event_status;
}
//_process_model_specific_event_____________________________________2013-04-28_/
#ifdef YAML_PARAM_VERSION
// YAML files will have parameter adjustment mechanism
#else

Event_status_indicator Event_scheduler_crop_complete::perform_crop_adjustment
(const CropSyst::Crop_parameters::Adjustment &adjustment_event)
{  Event_status_indicator status = ES_FAIL;
   CropSyst::Crop_parameters *associated_crop_param = crop.parameters;
   if (associated_crop_param)
   {
      std::string parameter(adjustment_event.parameter);
      size_t left_param_pos = parameter.find("[");
      size_t right_param_pos = parameter.find("]");
      if (left_param_pos != std::string::npos)
      {  size_t stripped_length = (right_param_pos - left_param_pos) - 1;
         std::string stripped_parameter(parameter,left_param_pos+1 ,stripped_length);
         parameter.assign(stripped_parameter);
         // I had incorrectly named these parameters in the adjustment list      //151207
         if (parameter == "opt_temp") parameter = "optimal_temp";
         if (parameter == "max_temp") parameter = "maximum_temp";
      }
      FOR_EACH_IN(section,VV_Section,associated_crop_param->sections,each_section)
      {  //for each entry in current section:
         VV_abstract_entry *entry = dynamic_cast<VV_abstract_entry *>
            (section->entries.find_string(parameter));                           //180820
            if (entry)                                                           //180910
            {  entry->set_str(adjustment_event.value.c_str());
               status = ES_COMPLETE;
            }
      } FOR_EACH_END(each_section)
   }
   return status;
}
#endif
//_perform_crop_adjustment__________________________________________2012-07-19_/
}//_namespace CropSyst_________________________________________________________/

