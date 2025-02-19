#ifndef shell_graphicalH
#define shell_graphicalH
#include "corn/const.h"
#include "corn/OS/directory_entry_name.h"

#define HANDLE_OR_ERROR void *
// Windows API functions such as ShellExecuteW return handle, but
// if the value is <= 32 it is considered an error (0 is error also)
// For Linux there isn't necessarily a handle,
//    the return value may simply be an error code and nothing else.

namespace CORN {
namespace OS {
//______________________________________________________________________________
interface_ Shell_graphical
{
 public:
   virtual bool browse_directory                                                 //080929
      (const Directory_name &dir_name
      ,bool open_new_window         = true
      ,bool show_directory_tree_view= true
      ,bool show_as_apparent_root   = false
      ,bool select                  = false)                            const=0;
      /**< Opens the directory entry with a File manager/browser (Window Explorer) window.
         \param open_new_window Opens a new window otherwise opens and brings to front an existing window that is already open to the directory. //091215_
         \param show_directory_tree_view Show directory tree view panel as well as the file list view panel //091215
         \param show_as_apparent_root show_as_apparent_root This directory will appear as root (so you can't navigate to parent directories). //091215
         \param select Selects/Highlights this directory in the file list view.
      **/
   virtual bool edit_file
      (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)    const=0; //181030_091215
      ///< Edit the file with a suitable  associated editor program.
      ///< return true if the file could be edited and editor was invoked.
   virtual bool view_file
      (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)    const=0; //181030_091215
      ///< View the file with a suitable  associated viewer program.
      ///< The OS may elect to open the file with an editor (hopefully in read_only mode)
      ///< if there is no specific viewer.
      ///< return true if the file could be viewed and viewer was invoked.
   virtual bool print_file
      (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)    const=0; //181030_091215
      ///< Print the file with a suitable printing program.
      ///< The OS may have a specific printing facility for the file's type
      ///< or it may invoke and editor or viewer and initiate the print command.
      ///< return true if the file could be printed and printer was invoked.
   virtual bool open_file
      (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)    const=0; //181030_091215
      ///< Opens the file with the default open method. This could be an editor or a viewer or other default action.
      ///< return true if the file was openned.


};
//_Shell_graphical________________________________________________________2018_/


// Currently all OS specializations are in this unit,
// but they could be moved to separate units eventually
namespace MS_Windows
{
//______________________________________________________________________________
class Shell_graphical
: public implements_ CORN::OS::Shell_graphical
{
 public:
   virtual bool browse_directory
      (const Directory_name &dir_name
      ,bool open_new_window
      ,bool show_directory_tree_view
      ,bool show_as_apparent_root
      ,bool select)                                                       const;
      ///< Declared in and documented File_system
   virtual bool edit_file
      (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)      const; //180130
      ///< Edit the file with a suitable  associated program using ShellExecute
   virtual bool view_file
      (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)      const; //181030
   virtual bool print_file
      (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)      const; //181030
   virtual bool open_file
      (const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)      const; //181030
 protected:
   int invoke(const File_name &file_name,const char *shell_open_mode)     const;
};
//_Shell_graphical________________________________________________________2018_/
extern Shell_graphical shell_graphical;
} // namespace MS_Windows
extern Shell_graphical &shell_graphical;
}}//_namespace CORN OS_________________________________________________________/
#endif

