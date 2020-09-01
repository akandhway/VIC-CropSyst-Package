#include "corn/std/std_fstream.h"
#include "project/scenario_generator.h"
#include "corn/OS/os.h"
#include "CS_suite/application/CS_suite_directory.h"
//______________________________________________________________________________
Scenario_generator::Scenario_generator(const char *i_generator_directory_qual)
: generation_directory_qual(i_generator_directory_qual)
{
}
//______________________________________________________________________________
const char *Scenario_generator::generate_scenario
(const char *scenario_name // unqualified
,const char *template_scenario_name)  // fully qualified or relative to
{  scenario_filename_qual.set_cstr(scenario_name);
   scenario_filename_qual.set_ext("csn");
   scenario_filename_qual.set_path(generation_directory_qual);
   OS_copy_file(template_scenario_name,scenario_filename_qual.c_str());
   // This copies the template_scenario to the new generated scenario_name
   // in the generation directory.
   // Returns the fully qualified scenario name.
   //
   // In most cases the model override this method and will open the generated file,
   // make any necessary changes and save the generated file.
   // Changes parameter file names so they are fully qualified.
   return scenario_filename_qual.c_str();
}
//______________________________________________________________________________
Simulation_run_result Scenario_generator::run_scenario
(const char *command_line_parameters
,bool minimize_window)
{  std::string command("\"");
   command.append(CS::Suite_directory->CropSyst().cropsyst_model_executable());
   command.append("\" ");
   command.append(command_line_parameters);

//std::ofstream dbg_scenario_gen("scenario_run_commands.txt",std::ios::app);
//dbg_scenario_gen << command << std::endl;

   DWORD dwExitCode=0;  // will be set to 0 if fails                             //040810
   #ifdef __WIN32__
      PROCESS_INFORMATION process_info;
      STARTUPINFO  startup_info;
      GetStartupInfo(&startup_info);
      startup_info.dwFlags |= STARTF_USESHOWWINDOW;
      startup_info.wShowWindow = (minimize_window) ? SW_SHOWMINNOACTIVE /*SW_MINIMIZE*/ : SW_MAXIMIZE;

      if (CreateProcess
            (0
            ,(char *)command.c_str()
            ,0 // default security
            ,0 // default security
            ,true
            ,CREATE_SEPARATE_WOW_VDM|NORMAL_PRIORITY_CLASS
            ,0  // current environment block
            ,0  // pointer to current directory name  (should be current)
            ,&startup_info // pointer to STARTUPINFO
            ,&process_info // pointer to PROCESS_INFORMATION
            ))
         WaitForSingleObject(process_info.hProcess,INFINITE);
   // Must close process and thread handles or you get a memory leak.            //021108
      GetExitCodeProcess(process_info.hProcess, &dwExitCode);                    //040810
      CloseHandle(process_info.hProcess);                                        //021108
      CloseHandle(process_info.hThread);                                         //021108
   #else
      UINT winexec_result = WinExec(sim_command.c_str(),SW_SHOW);
      dwExitCode= (winexec_result < 32) /*error indicator*/  ? 0 : winexec_result;  // In this case we don't have a process to get the
      // Not sure if process exit code is the same as the result of winexec. I am assuming it is, it isn't critical at thispoint
   #endif
   return (Simulation_run_result)dwExitCode;
}
//______________________________________________________________________________

