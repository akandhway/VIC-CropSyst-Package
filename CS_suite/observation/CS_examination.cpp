#include "CS_suite/observation/CS_examination.h"
#include "CS_suite/observation/CS_emanator.h"
#include "CS_suite/association/CS_adjustment.h"
#include "corn/data_source/vv_file.h"
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/OS/file_system_engine.h"
namespace CS
{
//______________________________________________________________________________
Participation::Participation(const CORN::date32 &processing_date_ref)
: Simulation_element_abstract(processing_date_ref)
, associates(true)
{}
//_Participation:constructor________________________________________2018-10-15_/
bool Participation::start()                                        modification_
{
   start_date.set_date32(simdate.get_date32());
   return Simulation_element_abstract::start();
}
//_Participation:start______________________________________________2018-10=16_/
Examination::Examination
(const CORN::date32              &processing_date_ref_                           //171114
,const CORN::OS::Directory_name  &output_directory_ref)                          //190124
: Participation(processing_date_ref_)                                            //181015
, seclusion                      ()
, desired_stats                  (0xFFFF)
, inspectors                     (associates)                                    //181015
, inspections                    ()
, inspectors_completed           ()
, inspections_completed_periodic ()
, inspections_completed_annual   ()
, deletable                      ()
, inspection_tallies             (true) // unique
, periods                        ()
, output_directory               (output_directory_ref)                          //190124
{
#ifdef DEBUG_INSPECTORS
   debug_unique_inspector_instances.set_unique_keys(true);
   debug_unique_inspector_ontologies.set_unique_keys(true);
#endif
}
//_Examination:constructor__________________________________________2016-06-06_/
/*nyn
Examination::Examination(const Examination &copy_from)
//: emanator(copy_from.emanator)
{
}
//_2016-06-06_____________________________________________________constructor__/
*/
Examination::~Examination()
{
   // It is not necessary to clear the lists because corn container
   // destructors clear themselves.
   #ifdef DEBUG_INSPECTORS
   debug_unique_inspector_instances.sort();
   debug_unique_inspector_ontologies.sort();
   std::ofstream instances("instances.dat");
   std::ofstream ontologies("ontologies.dat");
   debug_unique_inspector_instances.write(instances);
   debug_unique_inspector_ontologies.write(ontologies);
   #endif
}
//_Examination:destructor___________________________________________2016-06-06_/
bool Examination::start_year()                                     modification_
{  bool started = true;
   FOR_EACH_IN(inspection,Inspection,inspections,each_inspection)
      if (inspection->period.level_or_period == annual_inspection)               //171115
         started &= inspection->reset();
   FOR_EACH_END(each_inspection)
   return Simulation_element_abstract::start_year() && started;                  //170528
}
//_start_year_______________________________________________________2016-06-13_/
bool Examination::end_year()                                       modification_
{  deletable.delete_all();
   return Simulation_element_abstract::end_year();
}
//_end_year_________________________________________________________2016-06-16_/
bool Examination::end_day()                                        modification_
{  bool ended = true;
   // Note that we must process the inpections before the inspectors.
   FOR_EACH_IN(inspection,Inspection,inspections,each_inspection)
   {
      ended &= inspection->commit();
      if (inspection->inspector.context.get_status() == Emanator::deleted_status)
      {  // The inspection's inspector's emanator has been marked for deletion
         // so the inspection is complete.
         // This shouldn't be needed because emanating objects should call
         // complete_period_inspection, this is just in case they dont.
         CORN::Unidirectional_list &detach_from_inspections                      //170528
            = (inspection->period.level_or_period == annual_inspection)          //171115
            ? inspections_completed_annual
            : inspections_completed_periodic;
         detach_from_inspections.append(each_inspection->detach_current());      //170528
      }
   } FOR_EACH_END(each_inspection)
   FOR_EACH_IN(inspector,Inspector_abstract,inspectors,each_inspector)
   {  if (inspector->context.get_status() == Emanator::deleted_status)
      {  // Transfer inspector whose emanating object has been deleted so the inspector is complete
         inspectors_completed.append(each_inspector->detach_current());
      }
   } FOR_EACH_END(each_inspector)
   return Simulation_element_abstract::end_day() && ended;                       //170528
}
//_end_day__________________________________________________________2016-06-13_/
bool Examination::stop()                                           modification_
{  bool stopped = true;
   FOR_EACH_IN(period,CS::Inspection::Period,periods,period_iter)
   {
     period->complete(simdate.get_datetime64());                                 //190405
     if (period->name.length()) // <- shouldnt be needed  but there is an unnamed period
     {
      std::wstring instance_path_as_wstring;
      std::wstring composed_output_instance_filename;
      if (period->context)
      {
         CORN::Text_list instance_path;
         period->context->list_instance_path(instance_path);
         instance_path.string_items
            (instance_path_as_wstring,DEFAULT_DIRECTORY_SEPARATORw_chr);
         output_directory.append_components_to(composed_output_instance_filename);
         composed_output_instance_filename += DEFAULT_DIRECTORY_SEPARATORw_chr;
         composed_output_instance_filename += instance_path_as_wstring;
         composed_output_instance_filename += DEFAULT_DIRECTORY_SEPARATORw_chr;
         composed_output_instance_filename += L"periods.yaml";
      } else
      {
         output_directory.append_components_to(composed_output_instance_filename);
         composed_output_instance_filename += DEFAULT_DIRECTORY_SEPARATORw_chr;
         //composed_output_instance_filename = L"simulation";
         composed_output_instance_filename += L"periods.yaml";
      }


      /*
      CORN::OS::File_name_concrete output_instance_filename
         (output_directory,instance_path_as_wstring);
      // Currently each instance (each crop) gets a periods file
      CORN::OS::File_name_concrete periods_filename
         (output_instance_filename,CORN::OS::Extension(L"periods.yaml"));
      */
      CORN::OS::File_name_concrete periods_filename
         (composed_output_instance_filename);


      // There may be multiple periods in each object instance.
      // so we append to the file.
      // NYI should sort or list the relevent periods so we just
      // open the file once.

      const CORN::OS::Directory_name &periods_dirname
         = periods_filename.get_parent_directory_name_qualified();
      CORN::OS::file_system_engine.create_directory(periods_dirname);
      std::ofstream periods_stream(periods_filename.c_str(),std::ios::app);
      period->write_YAML(periods_stream,0);
     }
   } FOR_EACH_END(period_iter)
   return stopped && Participation::stop();
}
//_stop_____________________________________________________________2019-01-24_/
bool Examination::add_period_inspection
(const CS::Inspection::Period       &period
,CORN::Units_code    timestep_code                                               //170319
,const Emanator     &context
,nat32               applicable_descriptive_summary_elements)
{  bool added = false;
   FOR_EACH_IN(inspector,Inspector_abstract,inspectors,each_inspector)
   {  if (inspector->is_scalar() &&                                              //170330
           (context.is_ephemeral() == inspector->context.is_ephemeral()))
      //was test if (context.instance_name == inspector->context.instance_name)  //170321
      {  // We are not saying that both the context and the inspectors context
         // needs to be ephemeral.
         // It is simply that both the context and the inspectors context
         // should have the same ephemorality (both always present, or both ephemeral).
         nat32 desired_applicable_descriptive_summary_elements                   //170318
            = desired_stats & applicable_descriptive_summary_elements;
         Inspection *inspection = new Inspection
            (dynamic_cast<CS::Inspector_scalar &>(*inspector)
            ,period
            ,timestep_code                                                       //170319
            /*190119 now using optation global
            ,desired_variables_daily                                             //170319
            */
            ,desired_applicable_descriptive_summary_elements);                   //170318
         inspections.append(inspection); //uniquely
         added = true;
      } // else we don't tally vectors
      #ifdef DEBUG_INSPECTORS
      debug_unique_inspector_instances .add_string(inspector->provide_instance_name_qualified());
      debug_unique_inspector_ontologies.add_string(inspector->provide_ontology_qualified());
      #endif
   } FOR_EACH_END(each_inspector)
   return added;
}
//_add_period_inspection____________________________________________2016-06-13_/
bool Examination::complete_period_inspection
(nat32 level_period
,const Emanator &context
,bool before_end_day)                                                            //171222

{  bool completed = true;
   // Currently always true because not having any inspections is not an error

/* for debug
CORN::Text_list context_qualified_instance; // for debug output
context.list_instance_path(context_qualified_instance);
*/
   FOR_EACH_IN(period,CS::Inspection::Period,periods,each_period)                //200409
   {
      if (&period->context == &context)
          period->complete(simdate_raw);
   } FOR_EACH_END(each_period)
   FOR_EACH_IN(inspection,Inspection,inspections,each_inspection)
   {
/*
std::clog << "i_CD:" << inspection->inspector.context.ontology_domain
<< "\tCD:" << context.ontology_domain << std::endl;
*/

      if (&inspection->inspector.context == &context)
      {
      if (   (inspection->period.level_or_period == level_period)
          || (inspection->period.level_or_period == ANY_INSPECTION))             //170521

      {


         // Complete period inspection probably occurs before
         // the emanating object gets to end_day() so the daily
         // values would not otherwise have been commited
         if (before_end_day)
            inspection->commit();



         inspection_tally_commit(*inspection);
         Inspection *detached_inspection
            = dynamic_cast<Inspection *>(each_inspection->detach_current());     //170521
         switch (inspection->period.level_or_period  == annual_inspection)       //170521
         {  case annual_inspection :
               inspections_completed_annual  .append(detached_inspection);       //170521
            break;
            case continuous_inspection  :
               inspections_completed_periodic.append(detached_inspection);       //170521
            break;
            default:
               std::clog << "warning: examination inspection period case not handled (programming error?)" << std::endl;
            break;
         } // switch
         completed = true;
      }
      }
   } FOR_EACH_END(each_inspection)
   return completed ;
}
//_complete_period_inspection_______________________________________2016-06-13_/
bool Examination::filter()                                         modification_
{
   // First remove unwanted inspections because inspections reference
   // inspectors, deleting an inspector will leave dangling reference.
   FOR_EACH_IN(inspection,Inspection,inspections,each_inspection)
   {
      // Currently I have a single seculsion filter for both
      // instance names and ontology
      const CORN::Text_list &inspector_instance_name_path
         = inspection->inspector.provide_instance_qualification_list();
      const CORN::Text_list &inspector_context_path_path
         = inspection->inspector.provide_ontology_qualification_list();
      if     (seclusion.is_excluded_text_list(inspector_instance_name_path))
         each_inspection->delete_current();
      else if(seclusion.is_excluded_text_list(inspector_context_path_path))
         each_inspection->delete_current();
   } FOR_EACH_END(each_inspection)
   FOR_EACH_IN(inspector,Inspector_abstract,inspectors,each_inspector)
   {
      // Currently I have a single seculsion filter for both
      // instance names and ontology
      const CORN::Text_list &inspector_instance_name_path
         = inspector->provide_instance_qualification_list();
      const CORN::Text_list &inspector_context_path_path
         = inspector->provide_ontology_qualification_list();
      if     (seclusion.is_excluded_text_list(inspector_instance_name_path))
         each_inspector->delete_current();
      else if(seclusion.is_excluded_text_list(inspector_context_path_path))
         each_inspector->delete_current();
   } FOR_EACH_END(each_inspector)
   return true;
}
//_filter___________________________________________________________2015-10-13_/
Examination::Inspection_tally_store &Examination::provide_inspection_tally_store
(const Inspection &inspection
,CORN::Units_code  timestep_)                                         provision_
// I think the timestep of the tally store is/could be different from the inspection 170319
{  Examination::Inspection_tally_store *new_inpection_tally
      = new Examination::Inspection_tally_store
         (dynamic_cast<const Inspector_scalar &>(inspection.inspector)
         ,inspection.period                                                      //171115
         ,timestep_);
   Examination::Inspection_tally_store *listed_inpection_tally
      = dynamic_cast<Examination::Inspection_tally_store *>
      (inspection_tallies.append(new_inpection_tally));
   return *listed_inpection_tally;
}
//_provide_inspection_tally_store___________________________________2016-06-17_/
bool Examination::inspection_tally_commit
(const Inspection   &inspection)                                   modification_ //171114
{  Inspection_tally_store &inspection_tally
   = provide_inspection_tally_store(inspection,inspection.timestep.get());
      // Not sure what the timestep would be (so far here I only see inspection.timestep
   return inspection_tally.commit(inspection);
}
//_inspection_tally_commit__________________________________________2016-06-17_/
Examination::Inspection_tally_store::Inspection_tally_store
(const Inspector_scalar    &inspector_
,const CS::Inspection::Period              &period_                                              //171115
,CORN::Units_code           timestep_code)                                       //170319
: CORN::Item()
,inspector        (inspector_)
,timestep         (timestep_code)                                                //170319
,period           (period_)                                                      //171115
{  for (nat8 s = 0; s < CORN::statistic_COUNT; s++)
      stat_tally[s] = 0;
}
//_Inspection_tally_store:constructor_______________________________2016-06-17_/
bool Examination::Inspection_tally_store::commit
(const Inspection &inspection)                                     modification_
{
   for (nat8 s = 0; s < CORN::statistic_COUNT; s++)
   {  CORN::Statistic stat = (CORN::Statistic)s;                                 //160812
      bool is_desired_stat = inspection.is_tally_statistic_applicable(stat);
      if (is_desired_stat)
      {  if (!stat_tally[s])
            stat_tally[s]
           = new CORN::statistical::Sample_dataset_clad<float64,float32,float32>(STAT_ALL_bits);
         if (stat_tally[s])
            stat_tally[s]->append(inspection.get_tally_statistic(stat));
      }
   }
   return true;
}
//_commit___________________________________________________________2016-06-17_/
int Examination::Inspection_tally_store::compare
(const CORN::Item &other_item)                                             const
{  int comparison = -1;
   const Examination::Inspection_tally_store &other =
      dynamic_cast<const Examination::Inspection_tally_store &>(other_item);
   if (&(this->inspector) == &(other.inspector))
   {
      if (this->period.compare(other.period) == 0) comparison = 0;               //171115
   }
   return comparison;
}
//_Inspection_tally_store::compare__________________________________2016-06-19_/
bool Examination::Inspection_tally_store::is_key_string
(const std::string &key_)                                           affirmation_
{  return inspector.Associate::is_key_string(key_); }
//_is_key_string____________________________________________________2018-08-23_/

Examination *examination_global = 0;                                             //161202
//_is_key_string____________________________________________________2016-06-14_/
Examination &instantiate_examination_global
(const CORN::date32              &processing_date
,const CORN::OS::Directory_name  &output_dir)                                    //190124

{  if (!examination_global)
        examination_global = new Examination(processing_date,output_dir);        //190124
   return *examination_global;
}
//_instantiate_examination_global___________________________________2017-11-15_/
Examination &get_examination_global()
{  if (!examination_global)
   {  std::cerr << "error(programming): examination have not been instanciated." << std::endl;
      exit(1); // should use a specific standard error code
   }
   return *examination_global;
}
//_get_examination_global___________________________________________2017-11-15_/
//______________________________________________________________________________
Alteration::Alteration(const CORN::date32 &processing_date_ref)
: Participation(processing_date_ref)
, modifiers (associates)
{}
//_Alteration:constructor___________________________________________2018-10-15_/
nat32 Alteration::take_adjustment_files(modifiable_ CORN::Container *adjustment_filenames) appropriation_
{  nat32 took_count = 0;
   if (adjustment_filenames)
   {
      FOR_EACH_IN(adjustment_fname,CORN::OS::File_name,*adjustment_filenames,each_adjust_file)
      {
//std::clog << "adjfile:" << adjustment_fname->c_str() << std::endl;
         took_count += take_adjustment_file(adjustment_fname);
      } FOR_EACH_END(each_adjust_file)
      delete adjustment_filenames;
   }
   return took_count;
}
//_Alteration::take_adjustment_files________________________________2018-10-15_/
bool Alteration::take_adjustment_file
(CORN::OS::File_name *adjustment_filename_given)
{
   CORN::VV_File adjustment_file(adjustment_filename_given->c_str());
   Adjustment_properties *adjustment_props = new Adjustment_properties;
   adjustment_file.get(*adjustment_props);

   Adjustment *adjustment = 0;
   if (adjustment_props->alteration == "assignment")
   {
      Assignment *assignment = new Assignment(adjustment_props);
      adjustment = assignment;
   }
   switch (adjustment_props->domain.units.get())
   {
         case UT_date :
            adjustments_daily.take(adjustment);
         break;
         /*
         case UT_day :  conceptual
         */
         case UT_year :
            adjustments_annual.take(adjustment);
         break;
         default:
            std::clog << "Warning select adjustment domain units not currently implemented" << std::endl;
            // At the time I added this adjustments were dated, but
            // any domain could be specified.
         break;


   } // switch

   delete adjustment_filename_given; // currently don't need to keep the filename
   return modifiers.count();
}
//_take_adjustment_file_____________________________________________2018-10-15_/
bool Alteration::start_day()                                       modification_
{  bool started = true;
   FOR_EACH_IN(adj_daily,Adjustment,adjustments_daily,each_adj)
   {
      // conceptual: this processing could actually be handled with Event_scheduler
      if (adj_daily->date_matches(simdate.get_date32(),start_date.get_date32()))
         started &= adj_daily->commit();
   }FOR_EACH_END(each_adj)
   return started;
}
//_start_day________________________________________________________2018-10-15_/
bool Alteration::start_year()                                      modification_
{
   bool started = true;
   FOR_EACH_IN(adj_annual, Adjustment, adjustments_annual,each_adj)
   {
      // conceptual: this processing could actually be handled with Event_scheduler
      if (adj_annual->year_matches(simdate.get_year(),start_date.get_year()))
         started &= adj_annual->commit();
   }FOR_EACH_END(each_adj)
   return started;

}
//_start_year_______________________________________________________2018-10-15_/

/* currently not global because each simulation element can maintain
their own alteration, but may want to be global to allow
simulation elements to share their alterable elements


Alteration *alteration_global = 0;                                             //181015
//_2016-06-14___________________________________________________________________
Alteration &instantiate_alteration_global
(const CORN::date32 &processing_date)
{  if (!alteration_global)
        alteration_global = new Alteration(processing_date);
   return *alteration_global;
}
//_instantiate_alteration_global____________________________________2017-11-15_/
Alteration &get_alteration_global()
{  if (!alteration_global)
   {  std::cerr << "error(programming): alteration have not been instanciated." << std::endl;
      exit(1); // should use a specific standard error code
   }
   return *alteration_global;
}
//_get_alteration_global____________________________________________2017-11-15_/
*/

} // namespace CS

