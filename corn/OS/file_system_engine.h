#ifndef file_system_engineH
#define file_system_engineH
#include "corn/OS/file_system.h"
//#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/OS/directory_entry_name.h"

#define NAME_WILDCARD_NONE 0
#define QUALIFIED true
#define UNQUALIFIED false
#define LINK_SYMBOLIC true
#define LINK_HARD     false

namespace CORN {
class Seclusion;                                                                 //160814
namespace OS {

enum Provision
{fail
,preexist
,create
};
//_______________________________________________________forward declarations__/
interface_ File_system_engine
{
 public: // Methods that change the file system structure
   virtual bool create_directory                                                 //121222
      (const Directory_name &directory_name)                            const=0;
      /**< Creates a directory with the specified name in the file system.
       \return true if the directory could be created
       or null if the operating system was unable to create the directory.
       This is persnickity so it
       fails if the directory already exists or directorys
       in the path do not exists.
       It does not try to create parent directories as needed (use provide directory for this)
       Similar to Windows API CreateDirectory
       https://msdn.microsoft.com/en-us/library/windows/desktop/aa363855(v=vs.85).aspx
      **/
   virtual Provision provide_directory                                           //140825
      (const Directory_name &directory_name)                            const=0;
      /** Similar to create directory, but does not fail if paths or the
        ultimate directory already exists.
       Also creates the interrum directories in the path as necessary.
       * \return true if the directory was created or already exists.
       **/
   virtual
      bool create_directory_link
      (const Directory_name &actual_target_directory_name
      ,const Directory_name &link_directory_name
      ,bool symbolic = LINK_SYMBOLIC)                                   const=0;
      /**< Create a hard link to a directory
         Note that this is dangerous, and some operating systems do not
         support this.
         It is recommended to use only symbolic links to directories.
         \return true if successfully created
      **/
   virtual bool create_file_link
      (const File_name &actual_file_name
      ,const File_name &link_file_name
      ,bool symbolic = LINK_SYMBOLIC)                                   const=0;
      /**< Create a link to a file
      \return the created directory element or null if the OS was unable to create the directory.
      **/

   // Conceptual  have  provide_xxxx_link which return true if the links already exists
   // where the create_XXXX_link methods return false if they already exist

   virtual bool recycle
      (const Directory_entry_name  &entry_name  ///< Name of the directory or file to be recycled
      , bool delete_forever                     ///< Permenantly and irrevokably delete it
      )                                                                 const=0;
      ///< Moves the directory item to the recycle bin.
      ///< For operating systems/shells that don't have recycle bin,
      ///< the file is not deleted unless delete_forever is true;
      ///<\return true if the file was successfully recycled, false if it failed (nothing happened).
   virtual bool delete_directory                                                 //000211
      (const Directory_name &directory_name_to_delete
      ,bool recurse_subdirectories
      ,CORN::Container *remember_deleted_file_names_optional            = 0
      ,CORN::Container *remember_not_deleted_file_names_optional        = 0
      ,CORN::Container *remember_deleted_subdirectory_names_optional    = 0
      ,CORN::Container *remember_not_deleted_subdirectory_name_optional = 0
      ,CORN::Text_list *error_log_optional                              = 0
      ,bool delete_only_file_and_preserve_folder_and_subfolders=false)  const=0;
      ///< Delete the directory, all files and all subdirectories.
      ///< File folders that were deleted are listed in deleted_items.
      ///< File and folders that were deleted are listed in not_deleted_items
      ///< If any files could not be deleted error log reports why not_deleted_items could not be deleted.
      ///< \returns true if the directory itself was deleted
 public:
   // In the following methods
   // DEN denotes a qualified names either file, directory or other directory entry type.
   // name denotes unqualified names including only the name and extension.

   // The deprecated methods are obsolete and should be replaced
   // with the new seclusion methods

   virtual nat32 list_DENs                                                       //161013_160814
      (const Directory_name & directory_name
      ,CORN::Container *file_names       //= 0 // Optional, pass null if file list not wanted
      ,CORN::Container *directory_names  //= 0 // optional, pass null if file list not wanted
      ,const Seclusion &seclusion
      ,File_system::Recursion recursion = File_system::none_recursion_inclusive
      ,nat8                            recursions=255                            //200426
      ,const Directory_name *logical_root = 0
         // Optional directory at which super directory recursion stops
         // By default recursion stops at the users (home) directory
         // This option will override to stop before or continue up
         // above the home directory.
         // May be omitted if not using superdirectory recursion
      /*180808 obsolete never really used this,
         In any case use qualification methods to transform
      ,Directory_entry_name::Qualification qualification = Directory_entry_name::full_qualification
      */
      ) const = 0;
      ///< seclusion specifies the rules for including/excluding entries in the list
      /// It directory patterns with trailin / will also restrict the recursion,
      /// that is recursions will not go down into excluded subdirectories
      /// nor up into superdirectories were the name matches an exclude pattern.
   virtual nat32 list_names_in                                                   //161029
      (const Directory_name & directory_name
      ,CORN::Text_list *file_name_list_unqual
      ,CORN::Text_list *directory_name_list_unqual)                     const=0;
 public:
   virtual bool exists
      (const Directory_entry_name &directory_entry_name)                const=0;
      ///< \return true if the named file or directory exists
   virtual datetime64 get_modification_date_time
      (const Directory_entry_name &directory_entry_name)                const=0;
   virtual File_system::Directory::Type identify_type
      (const Directory_entry_name &entry_name)                          const=0;
      // This queries the operating system to identify the actual
      // type of this named directory entry in the file system.
      // (Will return unknown_entry if this file name does not exist).
   virtual bool is_symbolic_link(const Directory_entry_name &file_name) const=0; //121101
      /** Queries the operating system and returns true if the specified file
         is a symbolic link.
      **/
   virtual bool is_shortcut(const Directory_entry_name &file_name)      const=0; //151207
      /** Queries the operating system and returns true if the specified file
       *  is a Windows style shortcut.
      **/
   virtual Directory_name *render_current_working_directory()           const=0; //091208
      // Instanciates a directory name corresponding to the
      // the current working directory.
      // The directory name object is relinquished to the caller.
   virtual Directory_name *render_current_working_directory_on_drive
      (const Drive_designator &logical_drive_name)                      const=0; //120430
      // Instanciates a directory name corresponding to the
      // the current working directory on the specified drive.
      // Logical drive name is
      // The directory name object is relinquished to the caller.

      // Note this is applicable only to Windows AmigaDOS
      // and other OS with drive designator.
   virtual Directory_name *render_CSIDL_directory_name(nat16 CSIDL)     const=0; //100101
      ///< Instanciates a directory name for the specified CSIDL directory code
      ///< This is primarily intended for Windows but could be generally applicable
      ///< \return 0 if the CSIDL code is unknown.
      // under UNIX, the conventional application data local settings folder is typically $HOME/.local
      // For other operating systems
      // there may or may not be special folders matching the Windows CSIDL codes.
   virtual bool copy_file
      (const File_name &source, const File_name &destination
      ,bool fail_if_exists)                                             const=0; //120209
      /**< Currently this copies one file
         Source and destination file can be either qualified, fully qualified
         or unqualified for the current working directory.
         (Should be able to handle relatively qualified file name paths,
          but this has not yet been tested)
      **/
   virtual bool copy_directory                                                   //130215
      (const Directory_name &source, const Directory_name &target
      // conceptual add seclusion idiom const Seclusion &seclusion
      ,bool fail_if_exists)                                             const=0;
   virtual bool copy_directory_contents                                          //161105
      (const Directory_name &source, const Directory_name &target
      ,const Seclusion *seclusion_optional = 0
      // conceptual add seclusion idiom const Seclusion &seclusion
      ,bool use_links = false
      ,bool fail_if_exists = false)                                     const=0;

/*conceptual
   virtual bool copy_directory_with_linking
      (const Directory_name &source, const Directory_name &target
      ,const Seclusion &seclusion);

      )                                                const = 0; //130215
*/
   /**< Create a (target) directory structure that matches the source

      In the target links will be made to actual files in the source.
      (So the target directory, subdirectories will be actual directories
       and files will be links to the actual files in the source
       (if the source directory item is itself a link, the link is copied in the target).

      // probably should always fail if exists because if the
      // target is inadvertently the same as the source
      // or there already exists actual files in the target with the same names as in the source
      // real files would be replaced with less real links
   **/

   // Conceptual: eventually add regular expression for the file names.
   virtual bool rename
      (const Directory_entry_name &old_name
      ,const Directory_entry_name &new_name)                            const=0;

   /*Conceptual:
   virtual bool move_file_into_directory                                           //171215
      (const Directory_entry_name &DEN
      ,const Directory_name       &destination_directory
      ,bool  fail_if_exists)                                            const=0;
      // Moves the specified file to the
      // specified destination directory.
   */
   virtual bool move                                                             //171215
      (const Directory_entry_name &source_DEN
      ,const Directory_entry_name &destination_DEN
      ,bool force
         // overwrite destination file without prompt
      ,bool interactive
         // prompt before overwrite
      ,bool update
      ,bool verbose)                                                    const=0;
      // Moves the specified DEN  to the
      // specified DEN.

      // Note linux also has options:
      // --backup
      // --no-clober
      // --strip-trailing-slashes
      // -S, --suffix=suffix
      // -T, --no-target-directory
      // --version

   /* conceptual  Not sure if Window
   virtual bool move_directory_into                                                             //171215
      (const Directory_entry_name &source_DEN
      ,const Directory_entry_name &destination_DEN
      ,bool force
      ,bool interactive
      ,bool update
      ,bool verbose)                                                    const=0;
      // Moves the specified directory into the
      // specified destination directory.
   */

   /* conceptual
   virtual bool are_in_same_file_system
      (const Directory_entry_name &DEN1
      ,const Directory_entry_name &DEN2)                          affirmation=0;
      // Returns true if both Directory entries are on the same file system
   */

   virtual Directory_entry_name *render_resolved_shortcut_or_assume_actual
      (const Directory_entry_name &actual_directory_entry_or_shortcut_LNK) rendition_ = 0;
   virtual Directory_entry_name *render_resolved_absolute_DEN
      (const Directory_entry_name &relative_or_absolute_actual_directory_entry_or_shortcut_LNK
      ,const Directory_name &with_respect_to_directory_name)       rendition_=0; //120430
      /**<
         Does whatever is necessary to resolve the actual file name.
         Always returns a rendered fully qualified filename.
         \returns 0 if cannot resolve.

         Special designators

         These a special designators the serve a kadigins.
         These are replaced with fully qualified predefined paths.
         The defined paths will not have a terminating directory separator.

         ~ users home directory
         ^ searches the context Search stops at users home directory
            (if encountered otherwise continues to root)
         <999 searchs the specified number of superdirectories including CWD
            (the number may be omitted which means no limit
         >999 searches the specified number of subdirectories including CWD
            (the number may be omitted which means no limit
         $variable  specified environment variable. The function will return 0 if not defined.
         //obsolete $variable: (conceptual) specified environment variable. The function will return 0 if not defined.
         //obsolete ~CONTEXT: Search stops at users home directory (if encountered otherwise continues to root)
         //obsolete -CONTEXT: (conceptual probably not needed) Search continues to root (does not stop at users home directory).

            Search the path of the with_respect_to_directory_name
            - search for the first DEN following the extent specifier
               I.e.  ^/a/b/c/f.ext will search for directory 'a'
               and replace ^ with the parent path of a.
            - The directory a is expected (the function returns 0 if not found).
            - The sub path (I.e. b and c and file f.exe)
               need not nececessary exists, and the subdirectories
               (I.e. b and c) need not necessarily be in the path
               of with_respect_to_directory_name
            - search is superdirectory_recursion_inclusive
         ~landmark (conceptual) stop at users home
         -landmark (conceptual probably not needed) stops only at root
            Derived classes reimplementing this method may
            have special landmark directory names.
            I.e. CS suite landmarks are
               $Scenarios:
               $project:
               $project_ultimate:
               $project_penultimate:
               $Database: (search all the project's databases

      **/
   virtual Directory_entry_name *render_resolved_symbolic_link_or_shortcut
      (const Directory_entry_name &director_entry_name,bool relative) rendition_=0;//121101
      /**
         \return
            If the specified file is a symbolic link
            (or a shortcut in Windows or a Junction point in Windows
            or a ??? in MacOS)
            returns a rendered directory entry name that the link referes.
            or returns 0 if name is not any type of link.
      **/
   virtual Directory_entry_name *render_relative_DEN                             //170411
      (const Directory_entry_name &DEN_absolute
      ,const Directory_name       &with_respect_to_dir_absolute)   rendition_=0;
      /** Performs opposite of render_resolved_absolute_DEN.
         where \returns version of the DEN_absolute name
         relatively qualified with respect to the specified directory.
         Both DEN_absolute and with_respect_to_dir_absolute
         are expected to be fully absolute qualified names.
         \returns 0 if unable to find a relative path (shouldn't occur).
         render a relative path.
      */
   virtual nat32 get_file_size_bytes(const File_name &file_name)        const=0; //121101

   virtual Directory_entry_name *find_qualified_name_in_path
      (const std::wstring &unqualified_name
      ,const CORN::OS::Directory_name &from_directory
      ,Path_inclusion include_directories_in_search
         = Directory_entry_name::include_all)                      rendition_=0; //131111

 // I probably should rename the to find_name_in_path and


      /* Search the from_directory for the specified filename
         If include_qualification_directories_in_search
         Recursively searches up the specified directory tree
         until the specified file/directory name is found in an
         the from_directory, its parent or an ancestor directory.
         \return qualified rendered directory entry name if it exists
            or 0 if not found.
      */
   /* not implemented because this can  be done with list_entry_names
   virtual nat16 find_names_in_path_with_substring
      (const std::wstring              *name_substring
      ,modified_ CORN::Container       &found_entries
      ,const CORN::OS::Directory_name  &from_directory
      ,bool                             relative
      ,Path_inclusion include_directories_in_search = Directory_entry_name::include_all) rendition_;
   */
   virtual CORN::OS::Directory_entry_name *find_DEN                              //171227
      (const std::wstring             &target_name_unqual // can be a pattern
      ,CORN::OS::Directory_entry_type  type
      ,const CORN::OS::Directory_name &from_directory
      ,File_system::Recursion          recursion
      ,nat8                            recursions=255                            //200426
      ,const Directory_name           *logical_root = 0
      ,bool case_insensitive = FILENAME_CASE_INSENSITIVITY)        rendition_=0;
   virtual CORN::OS::File_name        *find_file_name                            //150412
      (const std::wstring             &target_filename // can be a pattern
      ,const CORN::OS::Directory_name &from_directory
      ,File_system::Recursion          recursion                                 //150908
      ,nat8                            recursions=255                            //200426
      ,const Directory_name           *logical_root = 0
      ,bool case_insensitive = FILENAME_CASE_INSENSITIVITY)        rendition_=0; //160924
/*just use find_DEN
   virtual CORN::OS::Directory_name        *find_directory_name                            //150412
      (const std::wstring             &target_dirname // can be a pattern
      ,const CORN::OS::Directory_name &from_directory
      ,File_system::Recursion          recursion                                 //150908
      ,const Directory_name           *logical_root = 0
      ,bool case_insensitive = FILENAME_CASE_INSENSITIVITY)        rendition_=0; //160924
*/
   virtual CORN::OS::Directory_name   *render_home()               rendition_=0; //161025
      // Renders directory name which is the user's home directory
   virtual std::wstring *render_environment_variable_value                       //180906
      (const std::wstring &envvar)                                 rendition_=0;
      // Must be implemented for specific OS in derived file_system_engine class.
      // This is currently implemented in the file_system_engine
      // because it is currently only used by this class, but
      // eventually these would be an an operating system engine class.
   virtual char get_envvar_list_separator()                             const=0; //200611
 protected:
   virtual bool matches_any_in                                                   //151212
      (const std::wstring &name_with_ext
      ,const CORN::Text_list &wild_card_patterns_to_compare
      ,bool case_insensitive = false)                            affirmation_=0;
      // Eventually make this public, put in interface, and/or implement as function in CORN::Text_list
 public:
   virtual bool set_current_working_directory(const Directory_name &dir_name) const=0;
};
//_File_system_engine__________________________________________________________/
extern File_system_engine &file_system_engine;
}}//_namespace_CORN_OS_________________________________________________________/
#endif

