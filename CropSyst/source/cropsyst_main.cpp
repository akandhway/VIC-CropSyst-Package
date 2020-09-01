#define VERBOSE
#include "corn/data_source/command_options_datasrc.h"
//200524 #include "arguments_CropSyst.h"
#include "CS_suite/application/CS_arguments.h"

#if (CROPSYST_VERSION>=1 && CROPSYST_VERSION <=4)
#include "land_unit_output_V4.h"
#else
#include "land_unit_output.h"
#endif
#include "cs_mod.h"
#include "corn/data_source/vv_file.h"
#include "fmt_param.h"
#include "cs_scenario.h"
#ifdef LADSS_MODE
#   include "LADSS.h"
#   include "LADSS_CropSyst.h"
#endif
#include "cs_mod.h"

#include "static_phrases.h"
#include "common/simulation/log.h"
#include "common/residue/residues_common.h"
extern Common_simulation_log CS_event_log;                                       //030308

#include <options.h>
#include "cs_mod.h"
#ifdef STATSGO_DATABASE
#  ifdef LADSS_UNIX
      namespace STATSGO { class STATSGO_Database;};
#  else
#     include "STATSGO/library/STATSGO_database.h"
#  endif
#endif

#define LABEL_score "score"
#define LABEL_graph "graph"
#include "project/GIS/GIS_table.h"
#include "common/simulation/exit_codes.h"
using namespace CS;                                                              //200524

unsigned _stklen = 16384;
using namespace CORN;
using namespace std;
bool quick_and_dirty_disable_chem_balance_when_watertable = false;               //150424
bool prompt_denitrification_params = false;                                      //151110
   // This is temporary until I put these parameters in the scenario editor.
#ifdef _Windows
#include <conio.h>
//______________________________________________________________________________
void click_close_message(bool also_abort)
{
   cout <<endl <<  endl << endl << "Press any key or click [X] close button." << endl;
   getch();
   if (also_abort)
      abort();
}
//______________________________________________________________________________
void pause_for_error()
{
   cout << "The simulation encountered an unexpected error:" <<endl;
   cout << "Usually this problem is do to an invalid input parameter." << endl;
   cout << endl;
   cout << ("Windows identifies the error as ");
   cout<< endl;
   cout << endl << "A list of error messages and likely causes are given in the manual." << endl;
   cout << endl << "See the file  \\cs_suite\\cropsyst\\manual\\runtime\\errors.htm" << endl;
   cout <<         "See the file \\CS_suite\\cropsyst\\manual\\parameters\\troubleshooting.htm" << endl;
   cout <<         "Go to our WWW page http://www.bsyse.wsu.edu/cropsyst under the links 'Assistance: Quesions and answers'" << endl;
   click_close_message(true);
}
//_pause_for_error__________________________________________________2000-04-02_/
#endif
#ifdef _Windows
#ifdef DUMP_CURVE_NUMBERS
std::ofstream *SCS_runoff_dat_file;                                              //131216
#endif

#endif
namespace CORN { namespace OS {
File_system &file_system() { return CORN::OS::file_system_logical; }
}}
//______________________________________________________________________________
int main(int argc, const char *argv[])
{
   CORN::Command_options_data_source command_line_options(argc,argv);            //160829
   CS::Identification ID_unused(0);                                              //200815
   CS::Arguments arguments(ID_unused,L".CropSyst_scenario");                     //200815
   CORN::global_arguments = &arguments;                                          //200815
   command_line_options.get(arguments);                                          //200815


   #if (CS_VERSION == 4)
   // In V4 the scenario was specified on the command line, subsequently
   // the CWD is the scenario directory
   if (argc > 1)                                                                 //160407
   {
      CORN::OS::Directory_name_concrete scenario_filename(argv[1]);              //160312
      if (scenario_filename.has_extension_ASCII("CropSyst_scenario",false))      //170224_161201
      {  // As of 16-03-11 the scenario editor set the current working directory
         // to the scenario's parent directory (scenarios)
         // I dont know why
         // Now the simulation engine expects the CWD to be the scenario directory
         CORN::OS::file_system_engine.set_current_working_directory
            (scenario_filename.get_parent_directory_name_qualified());
      } else                                                                     //160622
      {
         CORN::OS::Directory_name_concrete scenario_directory_name(argv[1]);     //160316
         if (CORN::OS::file_system_engine.exists(scenario_directory_name))       //160316
            CORN::OS::file_system_engine.set_current_working_directory
               (scenario_directory_name);
      }
   }
   #endif
   CS::Simulation_run_result run_result = RUN_SUCCESS;
   #ifdef _Windows
   set_terminate(pause_for_error);
   #endif

   //  desired variables are now discovered 181030
   #if (CS_VERSION == 4)
   #ifdef LADSS_MODE
   char scenario_ID_cstr[10];CORN_nat32_to_str(scenario_ID,scenario_ID_cstr,10); //030215
   #endif
   #ifdef LADSS_MODE
   CropSyst_Model_options LADSS_model_options;                                   //051231
   if (!LADSS_mode)                                                              //060908
   #endif
   {}
   #endif
   CropSyst_V4::CropSyst_Model *simulation =                                     //020504
      #ifdef LADSS_UNIX
      LADSS_mode ? new LADSS_CropSyst_Model(arguments,desired_variables) :
      #endif
      new CropSyst_V4::CropSyst_Model                                            //041030
      (arguments);                                                               //200815
      //200815 (command_line_options);                                                    //160829

   #ifdef LADSS_MODE
   // We must set LADSS_CropSyst_Model for LADSS mode so, operations can be loaded from data sources
   if (LADSS_mode) LADSS_CropSyst_model = (LADSS_CropSyst_Model *)simulation;    //031123
   #endif
   run_result =  simulation->run();                                              //040806
   #  ifdef LADSS_ORACLE
   // In LADSS, parameters are stored in a fixed Oracle database structure.
   // To use Oracle we first need to open a server session.
      if (oracle_session) delete oracle_session;                                 //020611
   #  endif
   #ifdef __linux
   // something is not deleting correctly
   // when simulation is deleted, so we simply abort the program
   exit(run_result);                                                             //151014_030415
   #endif
   delete simulation;       // also deletes LADSS_CropSyst_Model                 //021214
   return run_result;                                                            //040806
}
//_main________________________________________________________________________/

