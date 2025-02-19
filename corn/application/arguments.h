#ifndef argumentsH
#define argumentsH
#include "corn/data_source/datarec.h"
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/application/verbosity.h"
#include "corn/container/SDF_list.h"
//#include "corn/OS/file_system_types.h"
#include "corn/OS/file_system.h"
#include "corn/OS/FS/path_discovery.h"
namespace CORN
{
//______________________________________________________________________________
#define FIND_RESPONSES_FIRST 1
#define FIND_RESPONSES_LAST  3
//______________________________________________________________________________
class Arguments
: public extends_ Data_record
{
 protected:
   cognate_ std::string manual_URI;
 public: // options
   // generally conforms to common/standard command line options (for Linux)
   // http://tldp.org/LDP/abs/html/standard-options.html
   // http://www.catb.org/~esr/writings/taoup/html/ch10s05.html
   CORN::OS::File_name *program_filename; // fully qualified program
   bool verbose_flag;                                                            //191101
   nat8 verbosity;                                                               //191101
// for some reason nat8 isn't working

      // conceptual  actually use have option --log=
      // with SDF list of things to log.

      // The larger the number the greater the verbose level
      // recommended bit patterns are shown above

      /* conceptual
      (There may be multiple --verbose= arguments_

      The value could either be
      log(verbosity) level or a flag or  all
      bits
      0-2  (7 levels)
      3 count     show counts  (I.e. of things process, files openned
      4 progression show step or stage
         (this is separate from --progress)
      5 openning   show when (existing) files/objects openned
      6 creation   show files/object created
      7 discovery/location/origination  show (list) DEN's searched for and discovered
      8 resolution show resolved relative to absolute (or vica versa) DEN's
      9 recursion  show directories entered in recursion
      10 inclusion/selection   show when something deemed relevent/selection.
      11 exclusion/rejection ( elimination) show when something deemed not relevent (also why is known)
      12 Authentication show when handshaking with server or other resource
      13
      14
      15 debug
      Bits in debug mode
      0-2  (7 levels)
      3 enter-program  show methods on entry (debug)
      4 enter-library  show methods on exit  (debug)
      */

   // conceptual std::string explaination;
   // Empty if default explaination filename
   // Otherwise user can superceed with the provided name --explaination=supercede_filename

   bool wizard;   // write wizard (to std::cout)
   bool quiet;       // no prompts
   bool debug;                                                                   //170317
   bool trial;
   bool force;                                                                   //171030
      // I.e. force deletion of dirs even if not empty
      // or force overwrite of existing files
      // (when the program would normally not overwrite files).

   bool replace;                                                                 //190410
   bool update;                                                                  //190410
/*200801 replaced with path_discovery
   nat8 recursion;                                                               //170616
      // bitmask of CORN::OS::File_system_recursion
      // Actually use OS::recursion
   nat8 recursion_depth;
   // NYI conceptual
   bool depth_first;                                                             //191101
      // when When
*/
   bool help;

   nat8 find_responses;
      // if enabled (disabled by default)
      // find and process accessible response files.
      // Bit mask
      // Bit 0 (1=enabled)
      // Bit 1 (0=find first, 1=find last)
   std::wstring response_filename_extension;
   std::string preferred_response_filename; // wstring?                          //191101
      // Conceptual response filename specification
      // could be implemented as new path_discovery  200804
      // with find responses set with recursion option.

   bool absolute_filenames;                                                      //170411
         // When creating link filenames, use absolute paths
         // The default is to use relative filenames.
   CORN::Text_list *collect_remaining_deprecated;
      // This is considered deprecated (it is currently only used by collator),
      // Instead use the form  --XXX=SDF_list
      // Where SDF_list are items in System Data Format instead of
      // space separate list.


      // Derived classes may set collect_remaining
      // When it has been determined that the remaining arguments
      // are to be collected as one set of related elements
      // The convention is that there would typically be an identifying
      // argument of the form   --XXX:
      // where the colon indicates the following arguments are to be collected.
   std::string success_filename;                                                 //180212
      // This is enabled by default, disable with empty string --success=
      // The default is "success".
      // Derived classes (program) may create a file with this name
      // if completed successfully, the file may contain any status information.
      // I am pretty sure this is unqualified name
   CORN::SDF_Clad languages;                                                     //191101_180515
      // preferred language codes ISO 639-1.
      // https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes
      // The first encountered is presumed default
      // This could move moved to CORN::Arguments
      // If none specified, defaults to the system language.
 public:
   contribute_ OS::Directory_name_concrete  start_directory;
      // initially the current working directory
 public:
   contribute_ nat32 encountered_count;                                          //200811
      // This is the number of arguments encountered on the command line (taken)
      // Warning, I need to make sure every occurance is accounted for
      // at the moment, I simply need this to see if any args were encountered
      // (if not trigger response filename prompt)

 private:
   bool prompts_shown;
      // simple recursion, but will setup
   Unidirectional_list required_arguments; // of Item_string                     //180515
      // Simple (unique) text list of argument names/labels
      // Used when derived classes have required arguments.
      // The list is populated using require() method;
      // Note that requires source and/or target filenames
      // are stored in the respective path_discovery if applicable.              //200801
   std::string progress_raw;                                                     //191101
 public:
   bool recurse;                                                                 //180514
   OS::FS::Recursion recursion;                                                  //200818
      // Basic recursion provided for functions/commands such as rsync
      // where recursion is optional
      // (I.e. only sync one directory level or recurse from directory and below)

/* 200801
 // these recursion items are deprecated
         // now using source and target discovery
         // although may make those optational, with fallback to this.
   std::string recursion_raw;                                                    //191101
*/
 public: // but should be private (use accessor)
   CORN::Unidirectional_list unrecognized;                                       //191104
   // options such as directory entry names are added to this list to be
   // consumed (usually during get_end, but may be during processing using the source/target).
   // ideally all args will eventually be consumed

 public:
   CORN::Unidirectional_list path_discoveries;                                   //200808
      // list of OS::FS::Path_discovery
//CONCEPTUAL Actually should have a list of
// path discoveries
//derivec classes could be adding to them as needed
//They are all processed the same way
/*200807 now listed
 protected:
   provided_ OS::FS::Path_discovery *discovery_source;                           //200801
   provided_ OS::FS::Path_discovery *discovery_target;                           //200801
      // May be 0 if not applicable
*/
 public:
   Arguments();
   const CORN::OS::File_name * know_program(const char    *program_path_name_); // ASCIIZ
   const CORN::OS::File_name * know_program(const wchar_t *program_path_name_); // UnicodeZ
   virtual ~Arguments();
 public:
   const CORN::OS::Directory_name *get_program_filename()                 const;
   const CORN::OS::Directory_name *get_program_directory()                const;
   // These get_program_XXXX methods will return 0 if the program is not known.
 public: // Data_record implementations
   virtual bool expect_structure(bool for_write);                                //180514
   virtual bool get_start();
   virtual bool get_end();
   /* conceptual
   virtual bool expect_flag or flags (const char *flag,nat32 bitmask, nat32 mask);                  //101101
      // flags are command line arguments/options
      // that are not assignments and have no further qualification
   */
 public:
   /*conceptual
   virtual bool expect_multiple_filenames (CORN::Container &filenames   ,const char *pattern);
   virtual bool expect_multiple_filenames (CORN::Container &filenames   ,Seclusion *pattern);
   */
 public:
   virtual bool write_help()                                              const;
   // conceptual: add method to render or get command line
   // this would be used to echo back to the user how write
   // where what entered interactively via command line
   virtual bool write_wizard(std::ostream &wizard_stream)            stream_IO_;

   virtual bool interactive_prompt()                              modification_;
      // Prompts user for arguments that have not be
      // provided on the command line (default values not modified).

      // This is now obsolete using wizard
      // or implement using TUI wizard

   nat16 compose_command_line(std::string &command);
      // composes the current command line assignments and setting
      // returns the number of assignments and settings
   inline virtual bool get_author_mailto_URI(std::string &/*email_URI*/)   const
      { return false; }
      // contact email for the author of this program i.e. mailto:rnelson@wsu.edu
      // All program help really should provide a contact email.
   inline virtual bool get_program_version(std::string &version)           const
      { version = "unknown"; return false; }
   virtual bool require(const std::string &argument_key_list);
      // Derived class may implement call this
      // usually in set_start to indicate that the specified arguments (keys)
      // are required.
      // When required arguments are not encountered on the command line
      // get_end() will check and return false if required arguments were
      // not encountered.
      // This is also used in wizard file generation to indicate
   inline virtual bool know_manual_URI(const std::string &manual_URI_)cognition_
      {  manual_URI.assign(manual_URI_);
         return true; }
   inline virtual const std::wstring &get_response_file_extension()        const
      { return response_filename_extension;}
   inline virtual bool is_verbose(nat8 level)                       affirmation_ //170317
      { return IS_VERBOSE(verbosity,level); }
   inline virtual bool is_verbose_in_depth(nat8 level)              affirmation_ //170317
      { return IS_VERBOSE_IN_DEPTH(verbosity,level,global_verbosity_depth); }
   inline virtual bool is_verbose_file_system(nat8 level)           affirmation_ //170317
      { return IS_VERBOSE_FILE_SYSTEM(verbosity,level,global_verbosity_depth); }
 public:
   virtual bool take(Item_string *arg)                              submission_;
      // Novel methods which may be reimplemented by derived class
      // that would likely provide additional arguments.
      // This should intern call these methods to pickup default arguments.
   virtual bool take_unrecognized(Item_string *arg);
   inline virtual bool recognize_option(const std::string &paramstring)  modification_
      {  UNUSED_arg(paramstring);
         return false; }
      // deprecated, eventually recognize_option will be removed from all descendents
   virtual bool take_assignment
      (const std::string &variable
      ,const std::string &value)                                  modification_;
   virtual bool read_response_file_potentially
      (const CORN::OS::Directory_entry_name &param_DEN)           modification_;
      // In most cases this will not need to be overridden
   /* replaced with discover_XXX
   virtual bool s_ubmit_DEN                                                       //180213
      (CORN::OS::Directory_entry_name *param_DEN)                   submission_;
   virtual bool s_ubmit_DEN_qualified                                             //180213
      (CORN::OS::Directory_entry_name *param_DEN_qualified)         submission_;
   virtual bool s_ubmit_DEN_unqualified                                           //180213
      (CORN::OS::Directory_entry_name *param_DEN_unqualified)       submission_;
   */
   void display_interactive_instructions();

   //200804 discover methods are now actually deprecated
   // when using new path discovery for arguments.

   /*200807 obsolete  replaced with Path_discovery
   Item *discover_one
      (const Seclusion                 &seclusion
      ,OS::Directory_entry_type         entry_type = CORN::OS::unknown_entry
      ,CORN::OS::File_system::Recursion recursion
         = CORN::OS::File_system::none_recursion_exclusive)       renunciation_; //191103
      // return a collected item
      // encountered on the command line
      // (usually filename) that matches the pattern,
      // but if multiple discovered options to choose from,
      // it would be the one in the closest context
      // Since these methods removes only one existing not yet recognized argument,
      // subseqent calls to this function or discover_many)
      // can be called will get additional entries.

      // If --quiet option is used
      // will prompt for the one file to be returned
      // (otherwise it returns the first)
      // but all files will be moved to the container list.
      // For single file mode, the one returned file (if any) will be removed from the
      // collected list.

   nat32 discover_many
      (CORN::Container                 &list
      ,const Seclusion                 &seclusion
      ,OS::Directory_entry_type         entry_type = CORN::OS::unknown_entry
      ,CORN::OS::File_system::Recursion recursion
         = CORN::OS::File_system::none_recursion_exclusive
      //abandoned ,nat16 count_max = 0xFFFF
      )                 renunciation_; //191103
      // returns multiple expected items on the command line
      // that match the pattern
      // If list is provided (not null)
      // then matching (DENs) will be listed on std::cout for the user to choose
      // which to be selected/returned.
      // Only one item will be relinquished.

      // If list is provided, all matching times (found on the command line)
      // will be added to the list and 0 is

      // abandoned count_max can be used to limit the number of items to remove from the list
      // (the first ecountered are removed)
      // This option is generally not specified, but it is used by discover_one which call this


      // Need to be consistent as to when items are added to the list.
      // ?are the first or the last items on the list the closest to the WD?


   // discover_XXX general notes:

      // unknown_entry means the expected argument(s) would not necessarily
      // a directory entry name, it can be any arbitrary text.

      // If the entry type is specified (I.e. file or directory),
      // then the first item must also be the specified DEN type and must exist
      // (and be fully qualified or in the current WD).
      //
      // For DEN type, if pattern is not specified on the command line,
      // will search the path for matching filenames.
      // Note that the search only occurs when no matching files are encountered
      // on the command line.

      // The recursion is only applicable for DEN.
      // The recursion implies the the DEN must exist.

      // Since these methods removes existing not yet recognized arguments,
      // subseqent calls to this function can be called will get additional files in the path.

      // These methods will return 0 if nothing relevent is encountered on the command line,
      // or discovered in the file system, or no DEN was was selected by the user.

      // Matching filename(s) encountered on the command line are removed
      // from the encountered/collected unrecognized arguments list.
*/
/*200807
 public:
   virtual OS::FS::Path_discovery *provide_discovery_source() provision_{return 0;} //200801
   virtual OS::FS::Path_discovery *provide_discovery_target() provision_{return 0;} //200801
      // May return 0 if not applicable. Derived classes may implement.
*/
 protected:
   bool read_response_file
      (const CORN::OS::File_name &response_filename)              modification_;
   nat32 find_and_read_response_files()                           modification_;
 };
//______________________________________________________________________________
extern Arguments *global_arguments;
}//_namespace_CORN_____________________________________________________________/
#endif

