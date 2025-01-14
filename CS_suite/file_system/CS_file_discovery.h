#ifndef CS_file_discoveryH
#define CS_file_discoveryH
#include "CS_suite/file_system/CS_context_discovery_common.h"
namespace CS
{
//______________________________________________________________________________
class File_discovery
: public extends_ Context_discovery_common
{public:
   enum Directory_level {scenario_level,scenarios_level,project_level,file_system_root_level};
 public:
      // The root is a top most directory to search for files.
      // Typically this logical root would be either a project directory (super project)
      // the user's home
      // (For both Linux/Unix and Windows ~ is recognized as home)
      // or less likely to root of the file system,
      // or the same as the leaf to limit discovery to one directory
      // The leaf would typically be the CWD.

   File_discovery();
   File_discovery                                                                //161102
      (const CORN::OS::Directory_name &leaf_
      ,const CORN::OS::Directory_name *root_ = 0);
      // In this case, the leaf is not necessarily the CWD
      // and the root may be known

   CORN::OS::Directory_name *find_level
      (Directory_level level
      ,const CORN::OS::Directory_name *from_directory = 0)           rendition_;
   CORN::OS::Directory_name *                                                    //160301
      set_level_as_root(Directory_level level)                       rendition_; //160301
      // finds the uppermost specified level searching from the leaf.
      // If such a level is found, it is set at the root.
      // Normally this is called once to replaced the root of the file system
      // with a directory that is expected to be the upper most
      // directory identified as the root for the application.
      // Typically this would be the user's home directory.
   bool is_level
      (const CORN::OS::Directory_name &directory
      ,Directory_level level)                                      affirmation_;
      /// \return true if the specified directory is such a level
};
//_2015-12-06___________________________________________________________________
}
#endif
