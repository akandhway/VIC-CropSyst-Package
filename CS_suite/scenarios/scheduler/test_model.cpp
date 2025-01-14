#include "corn/OS/file_system_engine.h"
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/data_source/command_options_datasrc.h"
#include "corn/application/arguments.h"
#include "corn/math/random.h"

#include <process.h>
#ifdef _Windows
#  include "Windows.h"
   // for SetFileAttributes
#endif
#ifdef __linux__
#  include <unistd.h>
#endif
//#include <process.h>
//______________________________________________________________________________
class Test_model_engine
{
 public:
   //______________________________________________________________________________
   class Arguments
   : public extends_ CORN::Arguments
   {
    public:
      bool  build_mode;
      nat32 runtime; // maximum amount of time to simulate (seconds)
    public:
      inline Arguments()
         : CORN::Arguments()
         , build_mode(false)
         , runtime(5)
         {}
      virtual bool recognize_option(const std::string &paramstring) modification_;
      virtual bool recognize_assignment
         (const std::string &variable
         ,const std::string &value)                                   modification_;


   };
   //______________________________________________________________________________
 public:
   Arguments arguments;
 public:
   inline Test_model_engine()
      {}
   bool simulate_run();
   bool build_test_directory_structure();
};
//______________________________________________________________________________
// Note that recognized_count may be obsolete
bool Test_model_engine::Arguments::recognize_option(const std::string &paramstring) modification_
{  bool recognized = false;
   if (  (paramstring == "build")
       ||(paramstring == "b"))
   { build_mode = true; recognized = true; recognized_count++; }
   return recognized;
}
//______________________________________________________________________________

//200206  redo: recognize_assignment is obsolete, use expect_structure and move these to get end

bool Test_model_engine::Arguments::recognize_assignment
(const std::string &variable
,const std::string &value)                                         modification_
{  bool recognized = false;
   if (variable == "--runtime")
   {
      runtime = CORN::cstr_to_nat32(value.c_str(),10);
      recognized = true; recognized_count++;
   }
   return recognized;

}
//______________________________________________________________________________
char *dir_struct[] =
{ "A/1/X" , "B/1/X" , "C/1/X"
, "A/1/Y" , "B/1/Y" , "C/1/Y"
, "A/1/Z" , "B/1/Z" , "C/1/Z"
, "A/2/X" , "B/2/X" , "C/2/X"
, "A/2/Y" , "B/2/Y" , "C/2/Y"
, "A/2/Z" , "B/2/Z" , "C/2/Z"
, "A/3/X" , "B/3/X" , "C/3/X"
, "A/3/Y" , "B/3/Y" , "C/3/Y"
, "A/3/Z" , "B/3/Z" , "C/3/Z"
, 0
};
bool Test_model_engine::build_test_directory_structure()
{
   for (int f = 0; dir_struct[f]; f++)
   {
      char subdir_name[100];
      for (nat8 n = 0; n <= strlen(dir_struct[f]); n++)
      {
         subdir_name[n] =
            (dir_struct[f][n] == '/')
            ? '\\'
            : dir_struct[f][n];
      }

      CORN::OS::Directory_name_concrete scenario_dir(arguments.start_directory/*CWD*/,subdir_name);
      std::clog << subdir_name << ':' << scenario_dir.c_str() << std::endl;
      CORN::OS::Directory_name_concrete scenario_dirA(scenario_dir.c_str());
      CORN::OS::file_system_engine.provide_directory/*180321 create_directory*/(scenario_dirA);
   }
   return EXIT_SUCCESS;
}
//______________________________________________________________________________
bool Test_model_engine::simulate_run()
{
   // Scenario directory should exist
   // Calling program (scheduler) will set CWD which would be a scenario directory
   std::cout << "running: " << arguments.start_directory.c_str() << std::dendl;
   CORN::OS::Directory_name_concrete output_dir(arguments.start_directory,"Output");

   CORN::OS::file_system_engine.provide_directory/*180321 create_directory*/(output_dir);

   //randomize();
   srand(getpid());

   float32 seconds = CORN_uniform_random_between(1,arguments.runtime);
   //if (arguments.verbose)
   { std::clog << "Sleeping (seconds)" << (int)seconds << std::endl; }
   #ifdef _Windows
   Sleep(seconds * 1000);
   #endif
   #ifdef __linux__
   sleep(seconds);
   #endif

   float32 success_frequency = CORN_uniform_random_between(0,100);
   bool simulate_success = success_frequency < 90.0;
   if (simulate_success)
   {
      CORN::OS::File_name_concrete success_filename(arguments.start_directory,"success");
      std::ofstream success_file(success_filename.c_str());
      success_file << "status:success" << std::endl;
      success_file << "runtime:" << seconds <<std::endl;
      // NYI should record the simulated runtime.
   } else
   {
      CORN::OS::File_name_concrete failure_filename(arguments.start_directory,"failure");
      std::ofstream failure_file(failure_filename.c_str());
      failure_file << "status:failure" << std::endl;
      failure_file << "runtime:" << seconds <<std::endl;
      // NYI should record the simulated runtime.
   }
   return true;
}
//______________________________________________________________________________
int main(int argc,const char *argv[])
{
   CORN::Command_options_data_source command_line_options((int32)argc,argv);
   Test_model_engine test_model;
   command_line_options.get(test_model.arguments);
   int exit_status = EXIT_SUCCESS;
   //if ((argc ==2 ) && strcmp(argv[1],"--build") == 0)
   if (test_model.arguments.build_mode)
      exit_status = test_model.build_test_directory_structure();
   else
      exit_status = test_model.simulate_run();
   return exit_status;
}
//______________________________________________________________________________
