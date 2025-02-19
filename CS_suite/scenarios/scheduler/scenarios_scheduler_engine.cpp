
This class is abandoned, instead
under Linux use SLURM TORQUE parallel
under Windows use Powershell https://stackoverflow.com/questions/8781666/run-n-parallel-jobs-in-powershell


#include "scenarios_scheduler_engine.h"
#include "corn/string/strconv.hpp"
#include "corn/seclusion.h"
#include "corn/container/unilist.h"
#include "corn/OS/file_system_engine.h"
#include "corn/datetime/datetime64.h"
#ifdef _Windows
#  include "Windows.h"
   // for SetFileAttributes
#endif
#ifdef __linux__
#  include <unistd.h>
#endif


/* arguments


--sbatch_options=<options file name>
--torque_options=<options file name>

--sbatch_variables=<variables file name>
--torque_variables=<variables file name>

*/

/*
Scenarios are scheduled in groups
the scheduler sleep

*/

namespace CS
{
//______________________________________________________________________________
Scenarios_scheduler_engine::Scenarios_scheduler_engine
(const Arguments &arguments_)
: arguments    (arguments_)
, sleep_seconds_adjusted(3600)
, estimated_runtime_seconds(600) // Assume 10 minutes to run the scenario simulation
, core_count   (1)
, job_script_filename(0)
, verbose      (arguments_.verbose)
{
}
//______________________________________________________________________________
bool Scenarios_scheduler_engine::initialize()
{  bool inited = true;
   /*161116
   if (arguments.scheduler == unspecified_scheduler)
   {
      std::cerr
         << "error: a scheduler must be specified:" << std::endl
         << "scheduler=native  {not for clusters}" << std::endl
         << "scheduler=slurm   {not for clusters}" << std::endl
         << "scheduler=torque  {not for clusters}" << std::endl
      return inited = false;
   }
   */

   if (arguments.queue_or_partition_name.length() == 0)
   {
      switch(arguments.manager)
      {  case TORQUE_manager :{ /* not compiling  arguments.queue_or_partition_name.assign("batch");*/ } break;
         case SLURM_manager :
         {
            std::cerr << "When using SLURM manager,  --partition={name} option is required." << std::endl;
            inited = false;
         } break;
         // native scheduler does not use queue_or_partition_name
      } // switch
   }

   // http://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
   #ifdef _Windows
   SYSTEM_INFO sysinfo;
   GetSystemInfo(&sysinfo);
   core_count = sysinfo.dwNumberOfProcessors;
   #endif
   #ifdef __linux__
   core_count = sysconf(_SC_NPROCESSORS_ONLN);
   #endif
   if (arguments.cores)
         core_count = std::min<nat16>(core_count,arguments.cores);
   nat32 args_sleep_seconds = arguments.get_sleep_seconds();
   if (args_sleep_seconds)
      sleep_seconds_adjusted = args_sleep_seconds;
   if (arguments.manager != native_manager)
   {
      job_script_filename = new CORN::OS::File_name_concrete(arguments.start_directory,"job.sh");
      std::ofstream job_script(job_script_filename->c_str());
      write_script(job_script);
   }
   /* now using submit command
   switch (scheduler)
   {  case TORQUE_scheduler : inited &= write_script_SLURM (job_script); break;
      case SLURM_scheduler  : inited &= write_script_TORQUE(job_script); break;
      // NYN case native_scheduler : inited &= write_script__native(scenario); break;
      //default               : scripts only needed for cluster schedulers
   }
   */
   return inited;
}
//______________________________________________________________________________
#ifdef PROCESS_PENULTIMATES
// obsolete for TORQUE/SLURM now using job arrays.
nat32 Scenarios_scheduler_engine::process_penultimates()
{  nat32 scenarios_encountered = 0;
   CORN::Unidirectional_list ultimate_scenarios;
   if (verbose>1)
      std::clog << "listing ultimates" << std::endl;
   nat32 penultimate_scenarios_count = list_ultimate_scenarios(penultimate_scenarios);
   if (verbose>1)
   {  ultimate_scenarios.set_write_delimiter(',');
      std::clog << "ultimates_identified: { count: " << (int)ultimate_scenarios_count;
      if (ultimate_scenarios_count < 60)
      {  // Only write all the scenarios if less than a dozen of them
         std::clog << ", [";
         ultimate_scenarios.write(std::clog);
         std::clog << " ] ";
      }
      std::clog <<"}" << std::endl;
   }

   nat32 postpone_seconds = arguments.get_postpone_seconds();
   if (postpone_seconds)
   {
      if (verbose) std::clog << "postpone: " << postpone_seconds << "seconds" << std::endl;
      sleep_for(postpone_seconds);
   }
   //nat32 sleep_seconds = arguments.get_sleep_seconds();

//   do
//   {
      FOR_EACH_IN(penultimate,CORN::OS::Directory_name,penultimate_scenarios,each_penultimate)
      {
         CORN::Unidirectional_list &ultimate_scenarios = scenarios_pending;
         if (verbose>3) std::clog << "process penultimate: \"" << penultimate->c_str() << "\"" << std::endl;
         CORN::OS::file_system_engine.list_DENs
            (*penultimate,0,&ultimate_scenarios,CORN::Exclusion("*/Output/",true));
         scenarios_encountered += ultimate_scenarios.count();
/*
         if (actual_seconds_slept <= sleep_seconds_adjusted)
         {  // We didn't need to sleep so long reduce sleep time by 20%
            sleep_seconds_adjusted -= (nat32)(float32)(sleep_seconds_adjusted * 0.2);
         } else
         if (!arguments.get_sleep_seconds())
         {  // sleep not specified, calculate default

            // since we have run scenarios, we could update the estimated_runtime_seconds runtime for a run simulation
            sleep_seconds_adjusted = estimated_runtime_seconds * ultimate_scenarios.count() / core_count;
         }
*/

      } FOR_EACH_END(each_penultimate)
      //sleep(sleep_seconds);
//   } while(!abort && !penultimate_scenarios.is_empty());


   nat32 limit_scenarios
            = arguments.limit_scenarios
            ? arguments.limit_scenarios
            : core_count
            #ifdef _Windows
               * 2 // Generally we can actually run simultaneously 2 times the number of cores
            #endif
            ;
   nat32 scenarios_processed = process_pending
      (limit_scenarios
      //161212      ,sleep_seconds_adjusted
      );
   return scenarios_encountered;
}
//______________________________________________________________________________
#else
nat32 Scenarios_scheduler_engine::process_ultimates()
{  nat32 scenarios_encountered = 0;
   //CORN::Unidirectional_list ultimate_scenarios;
   if (verbose>1)
      std::clog << "listing ultimates" << std::endl;
   nat32 ultimate_scenarios_count = list_ultimate_scenarios(scenarios_pending);
   if (verbose>1)
   {  scenarios_pending.set_write_delimiter(',');
      std::clog << "ultimates_identified: { count: " << (int)ultimate_scenarios_count;
      if (ultimate_scenarios_count < 60)
      {  // Only write all the scenarios if less than a dozen of them
         std::clog << ", [";
         scenarios_pending.write(std::clog);
         std::clog << " ] ";
      }
      std::clog <<"}" << std::endl;
   }
   nat32 postpone_seconds = arguments.get_postpone_seconds();
   if (postpone_seconds)
   {
      if (verbose) std::clog << "postpone: " << postpone_seconds << "seconds" << std::endl;
      sleep_for(postpone_seconds);
   }
   nat32 limit_scenarios
            = arguments.limit_scenarios
            ? arguments.limit_scenarios
            : core_count
            #ifdef _Windows
               * 2 // Generally we can actually run simultaneously 2 times the number of cores
            #endif
            ;
   nat32 scenarios_processed = process_pending
      (limit_scenarios
      //161212      ,sleep_seconds_adjusted
      );
   return scenarios_encountered;
}
//______________________________________________________________________________
#endif
//______________________________________________________________________________
nat32 Scenarios_scheduler_engine::process_pending
(
 nat32 limit_scenarios
//161212,CORN::Seconds sleep_seconds
)
{
   // CORN::Seconds actual_seconds_slept = 0;

   nat32 running_now = 0;  // applies only to native mode
   do
   {
      nat32 schedule_now_count = limit_scenarios - running_now;
      unlist_complete(scenarios_pending);
      unlist_started (scenarios_pending);  // These would be scenarios started by any other instances of the scheduler (so we don't need to start these now)
      unlist_complete(scenarios_started); // These would be scenarios completed by any other instances of the scheduler
      for (CORN::OS::Directory_name *scenario = dynamic_cast<CORN::OS::Directory_name *>(scenarios_pending.pop_first())
          ;scenario && schedule_now_count
          ;scenario = dynamic_cast<CORN::OS::Directory_name *>(scenarios_pending.pop_first()))
      {
         running_now += schedule_scenario(*scenario);
         scenarios_scheduled.append(scenario);
         schedule_now_count --;
      }
      if (running_now)
         sleep_for(arguments.sleep_seconds);
      // else
      // processing while sleeping
      nat32 mine_completed     = unlist_complete(scenarios_scheduled);
      nat32 mine_still_running = unlist_started (scenarios_scheduled);
      nat32 stalled_new_count  = unlist_stalled (scenarios_scheduled);

      running_now -= mine_completed + stalled_new_count;

      //schedule_now_count += mine_completed;
      //schedule_now_count += stalled_new_count;
      // For workload managers
      schedule_now_count = CORN::must_be_less_or_equal_to(schedule_now_count,limit_scenarios);
      if (scenarios_stalled.count() >= limit_scenarios)
      {  // Dont attempt to run anymore scenarios because the stalled scenarios
         // will likely consume system resources.
         scenarios_pending.delete_all();
         std::cerr << "error: stalled jobs exceeds jobs run limit." << std::endl
            << "It is likely necessary to kill the stalled jobs (the scheduler does not do this)."
            << "In Unix/Linux use the kill command. In Windows use the task manager"
            << std::endl;
      }
   } while (!scenarios_pending.is_empty());
   return scenarios_success.count();
}
//______________________________________________________________________________
nat32 Scenarios_scheduler_engine::unlist_started(CORN::Container &scenarios)
{  nat32 unlisted_count = 0;
   FOR_EACH_IN(scenario,CORN::OS::Directory_name,scenarios,each_scenario)
   {  if (is_started(*scenario))
      {  scenarios_started.take(each_scenario->detach_current());
         unlisted_count++;
      }
   } FOR_EACH_END(each_scenario)
   if (verbose) std::clog << "started_elsewhere :" << unlisted_count << std::endl;
   return unlisted_count;
}
//______________________________________________________________________________
nat32 Scenarios_scheduler_engine::unlist_complete(CORN::Container &scenarios)
{
   nat32 unlisted_count = 0;
   FOR_EACH_IN(scenario,CORN::OS::Directory_name,scenarios,each_scenario)
   {  if (is_complete(*scenario))
      {  each_scenario->delete_current();
         unlisted_count++;
      }
   } FOR_EACH_END(each_scenario)
   if (verbose) std::clog << "completed: " << unlisted_count << std::endl;
   return unlisted_count;
}
//______________________________________________________________________________
nat32 Scenarios_scheduler_engine::unlist_stalled (CORN::Container &scenarios)
{
   nat32 unlisted_count = 0;
   FOR_EACH_IN(scenario,CORN::OS::Directory_name,scenarios,each_scenario)
   {  if (is_stalled(*scenario))
      {  each_scenario->delete_current();
         unlisted_count++;
      }
   } FOR_EACH_END(each_scenario)
   if (verbose) std::clog << "stalled: " << unlisted_count << std::endl;
   return unlisted_count;
}
//______________________________________________________________________________

//180808 #define Output_dir_pattern  "*/Output/"

nat32 Scenarios_scheduler_engine::list_ultimate_scenarios
(CORN::Container &ultimate_scenarios)
{
   CORN::Exclusion exclude_output("*/Output/",false);
   CORN::Unidirectional_list all_subscenarios;
   CORN::OS::Directory_name_concrete CWD;
   CORN::OS::file_system_engine.list_DENs
      (CWD
      ,0,&all_subscenarios
      ,exclude_output
      ,CORN::OS::File_system::subdirectory_recursion_inclusive);
   // At this point all_scenarios includes all scenario levels.
//std::cout << "all scenarios: " << std::endl;
//all_subscenarios.write(std::clog); std::cout << std::endl;


   if (verbose>2) std::clog << "consideration: [ ";
   for (CORN::OS::Directory_name *subscenario = dynamic_cast<CORN::OS::Directory_name *>(all_subscenarios.pop_first())
          ;subscenario
          ;subscenario = dynamic_cast<CORN::OS::Directory_name *>(all_subscenarios.pop_first()))
      {  // If there are any subdirectories, this not a preexisting leaf scenario
         CORN::Unidirectional_list any_sub_dirs;
         CORN::OS::file_system_engine.list_DENs
            (*subscenario,0,&any_sub_dirs,CORN::Exclusion("*/Output/",false)
            ,CORN::OS::File_system::none_recursion_inclusive);
         if (verbose>3) std::clog << "{ \"" << subscenario->c_str() << "\": ";
         if (any_sub_dirs.count())
         {
            delete subscenario;
            if (verbose>3) std::clog << "\"not ultimate scenario (checking subdirectories)\" ";
         }
         else
         {
            ultimate_scenarios.append(subscenario); // should be a leaf subscenario
            if (verbose>3) std::clog << "\"ultimate scenario\" " ;
         }
         if (verbose>2) std::clog << "} , ";
      }
   if (verbose>2) std::clog << " ] " << std::endl;
   return ultimate_scenarios.count();
}
//_2016-11-14___________________________________________________________________
bool Scenarios_scheduler_engine::sleep_for(CORN::Seconds seconds)          const
{
   if (verbose>1) { std::clog << "Sleeping (seconds)" << (int)seconds << std::endl; }
   #ifdef __linux__
   sleep(seconds);
   #else
   Sleep(seconds * 1000); // The Sleep function takes milliseconds
   #endif
   return true;
}
//______________________________________________________________________________
Scenarios_scheduler_engine::Arguments::Arguments()
: CORN::Arguments()
, postpone_seconds(0)
, sleep_seconds   (600)
, limit_scenarios (0)
, stale_seconds   (3600)
, cores           (0)
, submit_command  ("")
, manager
   #ifdef __linux__
   (unspecified_manager)
   #else
   (native_manager)
   #endif
, model           ("CropSyst_5") // I don't think Windows needs .exe extension
, recognize_submit_args    (false)
, recognize_model_args     (false)
, queue_or_partition_name  ()
{}
//_2016-10-14___________________________________________________________________
bool Scenarios_scheduler_engine::Arguments::get_author_mailto_URI(std::string &email_URI)  const
{ email_URI.assign("mailto:rnelson@wsu.edu"); return true; }
//_2016-12-12___________________________________________________________________

//200206  redo: recognize_assignment is obsolete, use expect_structure and move these to get end

bool Scenarios_scheduler_engine::Arguments::recognize_assignment
(const std::string &variable
,const std::string &value)                                         modification_
{  bool recognized = false;
          if (  (variable == "--limit"))
   {  limit_scenarios = CORN::cstr_to_nat32(value.c_str(),10);
      recognized = true;
   } else if (  (variable == "--cores"))
   {  cores = CORN::cstr_to_nat16(value.c_str(),10);
      recognized = true;
   } else if (  (variable == "--sleep"))
   {  sleep_seconds = time_seconds(value);
      recognized = true;
   } else if (  (variable == "--postpone"))
   {  postpone_seconds = time_seconds(value);
      recognized = true;
   } else if (  (variable == "--stale"))
   {  stale_seconds = time_seconds(value);
      recognized = true;
   } else if (variable == "--queue")
   {  queue_or_partition_name.assign(value);
      recognized = true;
   } else if (variable == "--partition")
   {  queue_or_partition_name.assign(value);
      recognized = true;
   } else if (variable == "submit")
   {
      submit_command = value;
      if (value == "sbatch")
         { manager = SLURM_manager;  recognized = true; }
      if (value == "qsub")
         { manager = TORQUE_manager; recognized = true; }
      /*
      if ((value == "native")
         { scheduler = native_scheduler; recognized = true; }
     */
      recognize_submit_args = true; recognize_model_args = false;
/*
   } else if (variable == "scheduler")
   {
      if ((value == "slurm")  || (value == "SLURM"))
         { scheduler = SLURM_scheduler;  recognized = true; }
      if ((value == "torque") || (value == "TORQUE"))
         { scheduler = TORQUE_scheduler; recognized = true; }
      if ((value == "native")
         { scheduler = native_scheduler; recognized = true; }
      recognize_scheduler_args = true; recognize_model_args = false;
*/
   } else if (variable == "model")
   {  model = value; recognized = recognize_model_args=true; recognize_submit_args = false;
   }
   return recognized
        ? recognized : CORN::Arguments::recognize_assignment(variable,value);
}
//_2016-10-14___________________________________________________________________
bool Scenarios_scheduler_engine::Arguments::recognize(const std::string &paramstring) modification_
{
   bool recognized = CORN::Arguments::recognize(paramstring);;
   if (!recognized)
   {
      if (recognize_model_args)
      {  // Any unrecognized arguments after model= argument are presumed to be arguments to be given to the model
         //std::string arg=variable + "=" + value;
         model_args.add_string(paramstring);
         recognized = true;
      } else
      if (recognize_submit_args)
      {  // Any unrecognized arguments after scheduler= argument are presumed to be arguments to be given to the scheduler
         //std::string arg=variable + "=" + value;
         model_args.add_string(paramstring);
         recognized = true;
      }
   }
   return recognized;
}
//_2016-11-16___________________________________________________________________
nat32 Scenarios_scheduler_engine::Arguments::time_seconds
(const std::string &value)                                                 const
{
   nat32 seconds = 0;
   if (value.length())
   {  seconds = CORN::cstr_to_nat32(value.c_str(),10);
      char time_units_code = value[value.length() - 1];
      switch (time_units_code)
      {
         case 'm' : seconds *=    60; break;
         case 'h' : seconds *=  3600; break;
         case 'd' : seconds *= 86400; break;
         case 's' : default : break; // default to seconds (no change)
      } // switch
   }
   return seconds;
}
//_2016-10-14___________________________________________________________________
bool Scenarios_scheduler_engine::is_started
(const CORN::OS::Directory_name &scenario)                          affirmation_
{
//161204   CORN::OS::File_name_concrete started_filename(scenario,"started");
//161204   return CORN::OS::file_system_engine.exists(started_filename);
   CORN::OS::Directory_name_concrete scenario_output(scenario,"Output");
   return CORN::OS::file_system_engine.exists(scenario_output);
}
//_2016-10-14___________________________________________________________________
bool Scenarios_scheduler_engine::is_complete
(const CORN::OS::Directory_name &scenario)                          affirmation_
{
   CORN::OS::File_name_concrete success_filename(scenario,"success");
   return CORN::OS::file_system_engine.exists(success_filename);
}
//_2016-10-14___________________________________________________________________
bool Scenarios_scheduler_engine::is_stalled
(const CORN::OS::Directory_name &scenario)                          affirmation_
{
   bool stalled = false;
   if (!is_complete(scenario) && is_started(scenario))
   {
         CORN::OS::Directory_name_concrete output_dir(scenario,"Output");
         CORN::Date_time_64 now;
         CORN::Date_time_64 output_dir_date(CORN::OS::file_system_engine.get_modification_date_time(output_dir));
         CORN::Datetime64 run_time = now.get_datetime64() - output_dir_date.get_datetime64();
         CORN::Date_time_64 run_datetime(run_time);
         if (run_datetime.get_seconds_after_midnight() > arguments.stale_seconds)
            stalled = true;
   }
   return stalled;
}
//_2016-12-12___________________________________________________________________
bool Scenarios_scheduler_engine::schedule_scenario
(const CORN::OS::Directory_name &scenario)                         modification_
{
   bool scheduled = false;
   switch (arguments.manager)
   {  case TORQUE_manager : scheduled = schedule_scenario_workload_manager(scenario); break;
      case SLURM_manager  : scheduled = schedule_scenario_workload_manager(scenario); break;
      case native_manager : scheduled = schedule_scenario_native(scenario); break;
      default               : scheduled = false; break;
   }
   return scheduled;
}
//_2016-10-14___________________________________________________________________
bool Scenarios_scheduler_engine::schedule_scenario_workload_manager(const CORN::OS::Directory_name &scenario) modification_
{
   std::string submit_command_with_args(arguments.submit_command);
   submit_command_with_args.append(" ");
   switch (arguments.manager)
   {  case TORQUE_manager :
      {  // For TORQUE We need the user environment variables to have the path including Simulation/run
         submit_command_with_args.append(" -V ");
         submit_command_with_args.append(" -d \"");
            submit_command_with_args.append(scenario.c_str());
            submit_command_with_args.append("\" ");
      } break;
      case SLURM_manager:
      {
         submit_command_with_args.append(" --partition=\"");
            submit_command_with_args.append(arguments.queue_or_partition_name);
            submit_command_with_args.append("\" ");
         submit_command_with_args.append(" --workdir=\"");
            submit_command_with_args.append(scenario.c_str());
            submit_command_with_args.append("\" ");
      } break;
   } // switch
   arguments.submit_args.string_items(submit_command_with_args,' ');
   submit_command_with_args.append(" \"");
   if (job_script_filename)
      submit_command_with_args.append(job_script_filename->c_str());
   submit_command_with_args.append("\"");
   int result = system(submit_command_with_args.c_str());
   // sleep_for(1); //may wan't to sleep to avoid clogging the submitter
   return result == 0;
}
//_2016-11-16___________________________________________________________________
bool Scenarios_scheduler_engine::schedule_scenario_native
(const CORN::OS::Directory_name &scenario)                         modification_
{
   if (verbose) std::clog << "starting: " << scenario.c_str() << std::endl;
   //161204 std::string model_command(arguments.model); // The arguments model command would be quoted if needed
   std::wstring model_command;
   CORN::append_string_to_wstring(arguments.model,model_command); // The arguments model command would be quoted if needed
   if (verbose) model_command.append(L" --verbose ");
   model_command.append(L" ");
   std::string model_args_str; arguments.model_args.string_items(model_args_str,' ');
   CORN::append_string_to_wstring(model_args_str,model_command);
   //161204 model_command.append(model_args_str);
   CORN::OS::file_system_engine.set_current_working_directory(scenario);
   bool scheduled = true;
   std::string model_command_str;
   CORN::append_wstring_to_string(model_command,model_command_str);
   if (verbose) std::clog << model_command_str << std::endl;
   #ifdef __linux__
   scheduled = system(model_command_str.c_str()) == 0;
   #else
   // Need to use create process because system waits for command to complete
   // but we want to continue immediately
      int nCmdShow = verbose ? SW_SHOWNORMAL : SW_HIDE;
      PROCESS_INFORMATION process_info;
      STARTUPINFOW startup_info;
      GetStartupInfoW(&startup_info);
      startup_info.dwFlags |= STARTF_USESHOWWINDOW;
      startup_info.wShowWindow = nCmdShow;
      scheduled = (CreateProcessW
            (0
            ,(wchar_t *)model_command.c_str()
            ,0 // process attributes
            ,0 // Thread attributes
            ,true
            ,CREATE_SEPARATE_WOW_VDM|NORMAL_PRIORITY_CLASS| CREATE_NEW_CONSOLE
            ,0  // current environment block
            ,0  // pointer to current directory name  (should be current)
            ,&startup_info // pointer to STARTUPINFO
            ,&process_info // pointer to PROCESS_INFORMATION
            ));
   #endif
   return scheduled;
}
//_2016-11-16___________________________________________________________________
bool Scenarios_scheduler_engine::write_script(std::ostream &script)
{
   script << "#!/bin/bash";
   if (arguments.manager == SLURM_manager)
   {
      //no job name  #SBATCH --job-name=REACCH_CT_SF-WW-WP_intermediate_historical_488911943
      script
      << "#SBATCH --requeue"                                      << std::endl
      << "#SBATCH --mem-per-cpu=512"                              << std::endl
      << "#SBATCH --time=40:00"                                   << std::endl
      << "#SBATCH --error=log.error"                              << std::endl
      << "#SBATCH --output=/dev/null"                             << std::endl
      << "#SBATCH --quiet"                                        << std::endl;
      //161212 shouldn't need started file
      //<< "echo \"start\" > \"started\""                           << std::endl
      //<< "date >> \"started\""                                    << std::endl

   } else if (arguments.manager == TORQUE_manager)
   {
      std::string job_name;
      // continue here need to compose job name (from path)
      int wall_hours = 1; // should be specified on the command line?
      script
         << "#PBS -N " << job_name.c_str()                          <<std::endl
         << "#PBS -q " << arguments.queue_or_partition_name                   <<std::endl
         << "#PBS -l mem=512" << "mb,nodes=1:ppn=1"                 <<std::endl
         << "#PBS -l walltime="<< wall_hours << ":00:00"            <<std::endl
         << "#PBS -V"                                               <<std::endl
         << "#PBS -o /dev/null"                                     <<std::endl
         << "#PBS -e error"                                         <<std::endl;
   }
   std::string model_args_str; arguments.model_args.string_items(model_args_str,' ');
   script
      << arguments.model << " " << model_args_str                 << std::endl
      << "exit_code=$?"                                           << std::endl
      << "if [ $exit_code -eq 0 ]"                                << std::endl
      << "then"                                                   << std::endl
      //<< "   rm \"started\" "                                     << std::endl
      /* Now CropSyst outputs success file
      << "   echo \"success\" >> \"started\""                     << std::endl
      << "   echo \"stop\" >> \"started\""                        << std::endl
      << "   date >> \"started\""                                 << std::endl
      << "   echo \"exit:\" >> \"started\""                       << std::endl
      << "   echo $exit_code >> \"started\""                      << std::endl
      << "   mv \"started\" \"success\""                          << std::endl
      */
      << "else"                                                   << std::endl
      << "   echo \"failed\" >> \"failure\""                       << std::endl
      << "   echo \"exit:\" >> \"failure\""                       << std::endl
      << "   echo $exit_code >> \"failure\""                      << std::endl
      << "fi"                                                     << std::endl;
   return true;
}
//_2016-11-16___________________________________________________________________
} // namespace CS


/*

   CORN::OS::Directory_name Linux_all_sh(specific_scenario_path    ,"toto.sh");
   CORN::OS::Directory_name Windows_all.bat(specific_scenario_path ,"toto.bat");
   std::ofstream all_script_Linux(Linux_all_sh.cstr())
   std::ofstream all_script_Windows(Windows_all.cstr())


         all_script_Windows << "PUSHD " << this_specific_scenario_path.c_str() << std::endl;
      all_script_Windows << "CALL toto.sh" << std::endl;
#!/bin/bash
partition_name="kamiak"
if [ ! -f "460211602.started" ] && [ ! -f "460211602.success" ]; then
sbatch  --partition=$partition_name --output=/dev/null "460211602.sl"
fi
....



*/
