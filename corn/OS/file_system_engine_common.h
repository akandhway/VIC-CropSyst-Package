#ifndef File_system_engine_commonH
#define File_system_engine_commonH
#include "corn/const.h"
#include "corn/primitive.h"
#include "corn/OS/file_system_engine.h"

namespace CORN { namespace OS {
//_______________________________________________________forward declarations__/
class File_system_engine_common
: public implements_ File_system_engine
// The file system engine must implement all the methods of the file system
// even though file systems specific for OS's are not themselves 'engines'
{
 public: // virtual methods declared in File_system (See file_system.h) for documentation.
   virtual File_system::Directory::Type identify_type
      (const Directory_entry_name &entry_name)                            const;
   virtual Directory_name *render_current_working_directory()             const; //091208
   virtual Directory_name *render_current_working_directory_on_drive
      (const Drive_designator &logical_drive_name)                        const; //120430
      // Instanciates a directory name corresponding to the
      // the current working directory on the specified drive.
      // The directory name object is relinquished to the caller.
   virtual Directory_entry_name *render_resolved_absolute_DEN                    //120430
      (const Directory_entry_name &relative_or_absolute_actual_DEN_or_shortcut_LNK
      ,const Directory_name &with_respect_to_directory_name)         rendition_;
   virtual Directory_entry_name *render_relative_DEN                             //170411
      (const Directory_entry_name &DEN_absolute
      ,const Directory_name       &with_respect_to_dir_absolute)     rendition_;
   virtual Provision provide_directory                                           //140825
      (const Directory_name &directory_name)                              const;
   virtual bool rename
      (const Directory_entry_name &old_name
      ,const Directory_entry_name &new_name)                              const; //121209
   virtual Directory_entry_name *find_qualified_name_in_path                     //131111
      (const std::wstring &unqualified_name
      ,const CORN::OS::Directory_name &from_directory
      ,Path_inclusion include_directories_in_search
         = Directory_entry_name::include_all)                        rendition_; //140210
   virtual CORN::OS::Directory_entry_name *find_DEN                              //171227
      (const std::wstring             &target_name_unqual
      ,CORN::OS::Directory_entry_type  type
      ,const CORN::OS::Directory_name &from_directory
      ,File_system::Recursion          recursion
      ,nat8                            recursions=255                            //200426
      ,const Directory_name           *logical_root = 0
      ,bool case_insensitive = FILENAME_CASE_INSENSITIVITY)          rendition_; //171227
   virtual CORN::OS::File_name *find_file_name                                   //150412
      (const std::wstring             &target_filename//160922 can now have wild card
      ,const CORN::OS::Directory_name &from_directory
      ,File_system::Recursion          recursion                                 //150908
      ,nat8                            recursions=255                            //200426
      ,const Directory_name *logical_root = 0
      ,bool                            case_sensitive=true)          rendition_; //160924
/*abandoned, just use find_DEN
   virtual CORN::OS::File_name *find_directory_name                              //191223
      (const std::wstring             &target_filename// can have wild card
      ,const CORN::OS::Directory_name &from_directory
      ,File_system::Recursion          recursion
      ,const Directory_name *logical_root = 0
      ,bool                            case_sensitive=true)          rendition_;
   virtual CORN::OS::File_name *find_path
      (const std::wstring             &target_path //can have wild card
      ,const CORN::OS::Directory_name &from_directory
      ,File_system::Recursion          recursion                                 //150908
      ,const Directory_name *logical_root = 0
      ,bool                            case_sensitive=true)          rendition_; //160924
*/
   virtual bool delete_directory                                                 //150320
      (const Directory_name & directory_name_to_delete
      ,bool recurse_subdirectories  // Note: must not recurse superdirectories so
      ,CORN::Container *remember_deleted_file_names         //optional
      ,CORN::Container *remember_not_deleted_file_names     //optional
      ,CORN::Container *remember_deleted_directory_names    //optional
      ,CORN::Container *remember_not_deleted_directory_name //optional
      ,CORN::Text_list *error_log                  //optional
      ,bool delete_only_file_and_preserve_folder_and_subfolders )         const;
   virtual nat32 list_DENs
      (const Directory_name & directory_name
      ,CORN::Container *file_name_list       //= 0 // Optional, pass null if file list not wanted
      ,CORN::Container *directory_name_list  //= 0 // optional, pass null if file list not wanted
      ,const Seclusion &seclusion
      ,File_system::Recursion recursion = File_system::none_recursion_inclusive
      ,nat8                            recursions=255                            //200426
      ,const Directory_name *logical_root = 0
      /*180808 obsolete never really used this,
         In any case use qualification methods to transform
      ,Directory_entry_name::Qualification qualification
         = Directory_entry_name::full_qualification
      */
         ) const;
   virtual bool matches_any_in                                                   //151212
      (const std::wstring &name_with_ext
      ,const CORN::Text_list &wild_card_patterns_to_compare
      ,bool case_insensitive = false)                              affirmation_;
   virtual bool copy_directory_contents                                          //161105
      (const Directory_name &source, const Directory_name &target
      ,const Seclusion *seclusion_optional = 0
      ,bool use_links = false
      ,bool fail_if_exists = false)                                       const;
 protected:
   bool log_any_delete_error
      ( int unlink_or_rm_dir_error
      ,CORN::Text_list &error_log)                                        const;
   bool resolve_designator
      (const Item_wstring *designator
      ,const Item_wstring *next_path_item
      ,Text_list        &resolved)                                   rendition_; //171204
      // Derived classes may override this to define specialized designators.
      // designator will generally not be 0
      // next_path_item may be 0,
      // designator and next_path_item may be consumed by this (if returns true)
   CORN::OS::Directory_name *find_from_environment_variable                      //180906
     (const std::wstring &envvar,const std::wstring &subitem_to_find)rendition_;
/*
   virtual bool get_environment_variable_value                                   //180906
      (const std::wstring &envvar,std::wstring &envvar_paths)           const=0;
      // Must be implemented for specific OS in derived file_system_engine class.
      // This is currently implemented in the file_system_engine
      // because it is currently only used by this class, but
      // eventually these would be an an operating system engine class.
*/
   bool system_command_flags_source_destination
      (const std::string &command
      ,const std::string &flags
      ,const Directory_entry_name &source_DEN
      ,const Directory_entry_name &destination_DEN)                       const;

};
//_File_system_engine_common___________________________________________________/
}}//_namespace_CORN:OS_________________________________________________________/
#endif

