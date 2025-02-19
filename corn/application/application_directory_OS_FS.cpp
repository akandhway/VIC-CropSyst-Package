#include "application_directory_OS_FS.h"
#include "suite_directory_OS_FS.h"
namespace CORN {
namespace OS_FS {
//______________________________________________________________________________
Application_directory::Application_directory
(const wchar_t *directory_name)
: OS::File_system_logical::Directory
   (OS::Directory_name_concrete(directory_name,CORN::OS::directory_entry),false) //161029
, suite_directory          (0), suite_directory_owned(false)
, documentation_directory(0)                                                     //141114
, icons_directory          (0)
{}
//______________________________________________________________________________
Application_directory::Application_directory
(const OS::Directory_name &directory_name)
: OS::File_system_logical::Directory(directory_name,false)                       //130116
, suite_directory          (0), suite_directory_owned(false)
, icons_directory          (0)
{}
//______________________________________________________________________________
Application_directory::Application_directory
(const Suite_directory &_suite_directory_reference
,const wchar_t *_directory_name_unqual)
: OS::File_system_logical::Directory
   (_suite_directory_reference,_directory_name_unqual,false)
, suite_directory
   ((Suite_directory *)&_suite_directory_reference),suite_directory_owned(false)
, documentation_directory(0)                                                     //141114
, icons_directory          (0)
{}
//_2014-05-19___________________________________________________________________
Application_directory::~Application_directory()
{  reset();
}
//______________________________________________________________________________
const Suite_directory *Application_directory::provide_suite_directory_const() provision_
{  suite_directory =new Suite_directory(get_parent_directory_name_qualified());
   suite_directory_owned = true;
   return suite_directory;
}
//_2014-11-16___________________________________________________________________
Documentation_directory    &Application_directory
::documentation()                                                     provision_
{  if (!documentation_directory)
   {  OS::Directory_name_concrete dir(*this,"Documentation",CORN::OS::directory_entry);   //161029
      documentation_directory = new Documentation_directory(dir);
   }
   return *documentation_directory;
}
//______________________________________________________________documentation__/
provide_directory_name_implementation(Application_directory,icons,icons_directory,preferred_icons_directory_name)
//______________________________________________________________________________
void Application_directory::reset()                                   provision_
// provides because only modifies provided contributes;
{
   delete documentation_directory;  documentation_directory = 0;                                                 //141114
   delete icons_directory;    icons_directory  =0;
   if (suite_directory_owned)                                                    //140519
   {  delete suite_directory; suite_directory =0;                                //140519
      suite_directory_owned = false;                                             //140519
   }
}
//______________________________________________________________________________
}}//_namespace_CORN::OS_FS_____________________________________________________/

