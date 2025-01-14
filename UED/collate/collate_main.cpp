#include "collate_engine.h"
#include "corn/OS/file_system_logical.h"
#include "corn/data_source/command_options_datasrc.h"

using namespace UED;

namespace CORN { namespace OS {
File_system &file_system() { return CORN::OS::file_system_logical; }
}}

//______________________________________________________________________________
int main(int argc, const char *argv[])
{
   CORN::Command_options_data_source command_line_options(argc,argv);
   UED::Arguments_collator arguments;
   CORN::global_arguments = &arguments;                                          //200802
   if (argc == 1) arguments.write_help();
   bool processed = true;
   processed &= command_line_options.get(arguments);                             //170328
   Collate_engine   collater(arguments);                                         //170328
   processed &= collater.process();
//200501 I think I fixed this   exit(0); // some cleanup is crashing
   return processed ? EXIT_SUCCESS : EXIT_FAILURE;
}
//_main________________________________________________________________________/
/* Example command line options
--verbose=15 crop@seasonal$d.UED --captions: period CO2 state site cycle crop

--verbose=15 31.4N110.6W.UED   --captions: level1

UED_collate.exe season.UED season.TDF --captions: "CWD"  "Site" "Scenarios_N" > output_season.csv

*/

