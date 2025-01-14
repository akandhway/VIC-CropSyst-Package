#include "corn/OS/file_system_engine.h"
#include "CS_suite/application/CS_suite_directory.h"
#include "CropSyst/source/cs_filenames.h"
#include "corn/application/program_directory_OS_FS.h"

namespace CS {
//______________________________________________________________________________
bool provide_Suite_directory(const CORN::OS::File_name &program_file_name)
{
   CORN::OS_FS::program_directory = new CORN::OS_FS::Program_directory
      (program_file_name.get_parent_directory_name_qualified(),true);
   CS::Suite_directory = new CS::Suite_Directory
      (*CORN::OS_FS::program_directory);                                         //141119
   if (!(CS::Suite_directory->is_valid()))
   {  delete CS::Suite_directory;
      if (program_file_name.get_path_directory_name())
      {
         CS::Suite_directory = new CS::Suite_Directory
            (*(program_file_name.get_path_directory_name()
            ->get_path_directory_name()));
         if (!(CS::Suite_directory->is_valid()))
         {  delete CS::Suite_directory;
            if (program_file_name.get_path_directory_name())
               CS::Suite_directory = new CS::Suite_Directory
                  (*(program_file_name.get_path_directory_name()
                     ->get_path_directory_name()->get_path_directory_name()));
         }
      }
   }
   return CS::Suite_directory->is_valid();
}
//_provide_Suite_directory__________________________________________2014-11-11_/
}//_namespace_CS_______________________________________________________________/

#define INSTANCIATE_CS_SUITE_DIRECTORY(qualified_program_name)                   \
   CORN::OS::File_name_concrete  program_file_name                               \
      (qualified_program_name ,CORN::OS::file_entry);                            \
   CS::provide_Suite_directory(program_file_name);
//______________________________________________________________________________
//110810 program_directory is now a pointer stored in the CORN namespace
// it is used by the Explainations engine
//CORN::Program_directory program_directory(program_file_name);

