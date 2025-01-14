#include "context_directory_OS_FS.h"
#include "corn/OS/directory_entry_name_concrete.h"
namespace CORN
{
namespace OS_FS
{
//______________________________________________________________________________
Context_directory::Context_directory()
:CORN::OS::Directory_name_concrete()
,project_directory(0)
{}
//______________________________________________________________________________
Context_directory::Context_directory
(const CORN::OS::Directory_entry_name &DEN
,CORN::OS::Directory_entry_type DEN_type)
:CORN::OS::Directory_name_concrete
   ((DEN_type == CORN::OS::directory_entry)
   ?DEN
   :*(DEN.get_path_directory_name()),CORN::OS::directory_entry)                  //161029
,project_directory(0)
{}
//_2014-01-25___________________________________________________________________
bool Context_directory::is_project_directory(const CORN::OS::Directory_name &dir_name) affirmation_
{  const wchar_t *canonical_project_file_name =  get_canonical_project_file_name();
   // A directory containing a canonical project filename (I.e. for CropSyst .CS_project file)
   bool is_project = (canonical_project_file_name != 0);
   if (is_project)
   {
      CORN::OS::Directory_name_concrete potential_project_dir
         (dir_name,canonical_project_file_name,CORN::OS::directory_entry);       //161205
      CORN::OS::file_system_engine.exists(potential_project_dir);                //11029
   }
   return is_project;
}
//_2014-01-25___________________________________________________________________
Project_directory  *Context_directory::provide_project_directory()    provision_
{  if (!project_directory) project_directory = find_project_directory(this);
   return project_directory;
}
//_2014-01-25___________________________________________________________________
Project_directory  *Context_directory::find_project_directory(const CORN::OS::Directory_name *current_dir)       rendition_
{  return current_dir
      ?  (is_project_directory(*current_dir))
         ? render_project_directory(*current_dir,false)
         : find_project_directory(current_dir->get_path_directory_name())
      : 0; // we are at the root dir, didn't find a project
}
//_2014-01-25___________________________________________________________________
}}//_namespace_CORN::OS_FS_____________________________________________________/
