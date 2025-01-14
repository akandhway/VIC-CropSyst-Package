#ifndef CS_databasesH
#define CS_databasesH
#include "CS_suite/file_system/CS_database_directory.h"
#include "corn/seclusion.h"
namespace CS
{
//______________________________________________________________________________
class Databases
: public extends_ CORN::Bidirectional_list // because sorted
{  // Instanciates Database directories found in ancestors of the specified
   // directory (usually scenario)
   nat8 verbose; //usually specified as command line option
 public:
   Databases
      (const CORN::OS::Directory_name &scenario_or_subproject_dir
      ,nat8 verbose);
      // context_dir is usually CWD (scenario directory)
 public:
   CORN::OS::Directory_entry_name  *render_find_in_context_wstring
      (const std::wstring &context_name
      ,const std::wstring &file
      ,const CORN::OS::Extension &extension)                         rendition_;
   CORN::OS::Directory_entry_name *render_find_in_context_string
      (const std::wstring &context_name
      ,const std::string &file
      ,const CORN::OS::Extension &extension)                         rendition_;

   CORN::OS::Directory_entry_name *render_find_in_context_string_any_extension   //161122
      (const std::wstring &context_name
      ,const std::string &file)                                      rendition_;

   CORN::OS::Directory_entry_name *render_find_in_context_int
      (const std::wstring &context_name
      ,int number
      ,const CORN::OS::Extension &extension)                         rendition_;

   CORN::OS::Directory_entry_name *render_find_under_context_wstring
      (const std::wstring &context_name
      ,const std::wstring &undercontext_name
      ,const std::wstring &name
      ,const CORN::OS::Extension &extension)                         rendition_;
   CORN::OS::Directory_entry_name *render_find_under_context_string
      (const std::wstring &context_name
      ,const std::wstring &undercontext_name
      ,const std::string &name
      ,const CORN::OS::Extension &extension)                         rendition_;
   CORN::OS::Directory_entry_name *render_find_under_context_int
      (const std::wstring &context_name
      ,const std::wstring &undercontext_name
      ,int number
      ,const CORN::OS::Extension &extension)                         rendition_;
   // The find under forms
   //  recursively search the context's sub directory structure
   //  for a directory matching undercontext name then
   //  search for the file in that subdirectory.
   // This is used for Database/Initialization context directory
   // that can have any arbitrary subdirectory structure

   CORN::OS::Directory_name *render_closest_context
      (const std::wstring &context_name)                             rendition_;
      /* returns the context directory (I.e. Soil)
         scans the list of Databases and returns the
         Database context directory that exists closest to the
         project/scenario directory.

         May return 0 if no such context found
      */
   CORN::Unidirectional_list *render_specialization_files_qualified_in_context_string
      (const std::string &context_name)                              rendition_;
   CORN::Unidirectional_list *render_specialization_files_qualified_in_context_wstring
      (const std::wstring &context_name)                             rendition_;
      /* returns fully qualified filenames
         for all unique files in each Database directory
         where the most specific file superceeds more generic files.
      */
   nat32 render_subdirectories
      (CORN::Container       &subdirectories
      ,const CORN::Seclusion &seclusion
      ,CORN::OS::File_system::Recursion recursion)                   rendition_;
      // Returns the number of included subdirectories from all databases

   nat32 render_subdirectories_with_name
      (CORN::Container        &subdirectories
      ,const std::string      &name)                                 rendition_;
      // Returns the number of included subdirectories from all databases
};
//_Databases________________________________________________________2016-09-02_/
}//_namespace_CS_______________________________________________________________/
#endif

