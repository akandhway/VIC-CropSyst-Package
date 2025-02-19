#include "corn/OS/shell_graphical.h"

#include <shlobj.h>

# ifdef WINAPI
#  define USE_SYSTEM_REGISTRY
#endif

namespace CORN {
namespace OS {
//______________________________________________________________________________
// Currently all OS specializations are in this unit, but they will be moved to separate units eventually
namespace MS_Windows
{
//______________________________________________________________________________
bool Shell_graphical::browse_directory
(const Directory_name &dir_name
,bool open_new_window
,bool show_directory_tree_view
,bool show_as_apparent_root
,bool select)                                                              const
{  bool browsed = false;
   std::wstring command(L"explorer ");                                           //080929
   if (open_new_window)          command.append(L"/n,");                         //091215
   if (show_directory_tree_view) command.append(L"/e,");                         //091215
   if (select)                   command.append(L"/select,");                    //080929
   if (show_as_apparent_root)    command.append(L"/root,");                      //080929
   command.append(L"\"");                                                        //080929
   // With Windows explorer in a new window with explore mode
   // (double pane with directory tree list file list)
   // I Prefer /n which will open a new window rather than using
   // an arbitrary currently open Explorer session (which I consider bad form).
   // If the item is a directory the file list shows the directory contents.
   // If the item is a registered document type, Windows may
   // open the document using an embedded editor in an Internet Explorer like window/session.
   // This tends to have limited functionality and applicability,
   // and Windows is inconsistent in how it handles other types of files.
   // (With the /e option one would think the left pane directory tree view
   //  would focus the folder the file/document is in, but instead it always focuses on Desktop).
   // With the select option (recommended mode), the left pane directory tree view focus's on the folder the entry resides.
   // right pane file list view selects the file.

   dir_name.get_Windows_representation(command); // the name will be appended to command.  //091215
   command.append(L"\"");

   PROCESS_INFORMATION process_info;
   STARTUPINFOW  startup_info;
   GetStartupInfoW(&startup_info);
   startup_info.dwFlags |= STARTF_USESHOWWINDOW;
   startup_info.wShowWindow = SW_MAXIMIZE;

   browsed = CreateProcessW
         (0
         ,(wchar_t *)command.c_str()
         ,0 // default security
         ,0 // default security
         ,true
         ,CREATE_SEPARATE_WOW_VDM|NORMAL_PRIORITY_CLASS
         ,0  // current environment block
         ,0  // pointer to current directory name  (should be current)
         ,&startup_info
         ,&process_info);
   CloseHandle(process_info.hProcess);                                           //180116
   CloseHandle(process_info.hThread);                                            //180116

#ifdef NYN
This is another way to explore a directory with Windows (maybe 2000 and greater)
This was used in the CropSyst output manager.
We may need to use this some time in the future if WinExec becomes Obsolete
(May be with Vista)
      Variant  ShellOLE;
      ShellOLE=Variant::CreateObject("Shell.Application");
      Function Explore("Explore");
      AnsiString directory(scenario_directory->output_dir.c_str());
      ShellOLE.Exec(Explore << Variant(directory));
#endif
   return browsed;
}
//_browse_directory_________________________________________________2008-09-29_
bool Shell_graphical::edit_file
(const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)             const
{  ///< Edit the file with a suitable  associated program.
   handle_or_error = ShellExecuteW                                               //181030
      (0,L"edit",file_name.w_str(),NULL,NULL,SW_SHOW);                           //090615
   return (int) handle_or_error>32;                                              //181030
      // If ShellExectute return value > 32 it succeeded
}
//_edit_file___________________________________________________________________/
bool Shell_graphical::view_file
(const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)             const
{  return invoke(file_name,"open");
}
//_view_file___________________________________________________________________/
bool Shell_graphical::print_file
(const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)             const
{  handle_or_error = ShellExecuteW
      (0,L"print",file_name.w_str(),NULL,NULL,SW_SHOWNORMAL);                    //001109
  return ((int)handle_or_error > 32);
}
//_print_file__________________________________________________________________/
bool Shell_graphical::open_file
(const File_name &file_name, HANDLE_OR_ERROR &handle_or_error)             const
{  handle_or_error= ShellExecuteW                                                //181030
      (0,L"open",file_name.w_str(),NULL,NULL,SW_SHOW);                           //090615
   return (int)handle_or_error > 32;                                             //181030
}
//_open_file___________________________________________________________________/
int Shell_graphical::invoke(const File_name &file_name,const char *shell_open_mode) const
{  int exit_status = EXIT_FAILURE;                                               //090624
   #ifndef __GNUG__

    wchar_t shell_open_mode_W[50];                                                //121024
   for (int i = 0; i < 50 && shell_open_mode[i] != 0; i++)
   {  shell_open_mode_W[i] = shell_open_mode[i];
      shell_open_mode_W[i+1] = 0;
   }
#ifdef USE_SYSTEM_REGISTRY
   // Find out what the file extension is
   exit_status = EXIT_SUCCESS;                                                   //090624
   wchar_t ext[20];                                                              //120209
   wcscpy(ext,file_name.get_extension().c_str());                                //120209_091219
   for (int i = 0; i < 20; i++)                                                  //971124
      if (ext[i] == '#') ext[i] = 0;  // Strip off any #xxx from a URL           //971124
   // Find out if this file extension is in the windows 3.1 registry.
   HKEY RegKey;
   if (RegOpenKeyW(HKEY_CLASSES_ROOT, ext, &RegKey) == ERROR_SUCCESS)
   {
      #ifdef _MSC_VER
      wchar_t
      #else
      char
      #endif
      ClassName[80];
      LONG cb;
      cb = sizeof(ClassName);
      if (RegQueryValue(RegKey, NULL, ClassName, &cb) == ERROR_SUCCESS)
      {  RegCloseKey(RegKey); // CLose the key for the next key
         // The registry will contain the filename of the viewe exe
         // in the \shell\open\command key.
         #ifdef _MSC_VER
         wcscat(ClassName,L"\\shell\\");                                         //121024_060907
         wcscat(ClassName,shell_open_mode_W);                                    //060907
         wcscat(ClassName,L"\\command");                                         //060907
         #else
         #ifdef __BCPLUSPLUS__
         strcat(ClassName,"\\shell\\");                                          //121024_060907
         strcat(ClassName,shell_open_mode);                                      //060907
         strcat(ClassName,"\\command");                                          //060907
         #endif
         #endif
         if (RegOpenKey(HKEY_CLASSES_ROOT, ClassName, &RegKey) == ERROR_SUCCESS)
         {  wchar_t ExeFilename[512];
            cb = sizeof(ExeFilename);
            if (RegQueryValueW(RegKey, NULL, ExeFilename, &cb) == ERROR_SUCCESS)
            {  // Make the assumption that it wil only have one
               // ExeFilename as a parameter as specified with the %1 value.
              if (wcsstr(ExeFilename,L"firefox") != 0)                            //090922
                 exit_status = 1;   // We cannot use Firefox to open local files (We default to Windows explorer below) //090922
              else                                                               //090922
              {wchar_t *Pos_ = wcsstr(ExeFilename, L"%1");
               if (Pos_ != 0)
                   wcscpy(Pos_,file_name.w_str());                               //001109
               else                                                              //980210
                {  // Windows NT or MS I.e. not giving %1 in registry.
                  wcscat(ExeFilename,L" \""); //Make sure the filename is quoted //020215
                  wcscat(ExeFilename,file_name.w_str());                         //001109
                  wcscat(ExeFilename,L"\"");                                     //020215
                }
               PROCESS_INFORMATION process_info;
               STARTUPINFOW  startup_info;
               GetStartupInfoW(&startup_info);
               startup_info.dwFlags |= STARTF_USESHOWWINDOW;
               startup_info.wShowWindow = SW_MAXIMIZE;
               exit_status = CreateProcessW
                  (0
                  ,ExeFilename
                  ,0 // default security
                  ,0 // default security
                  ,true
                  ,CREATE_SEPARATE_WOW_VDM|NORMAL_PRIORITY_CLASS
                  ,0  // current environment block
                  ,0  // pointer to current directory name  (should be current)
                  ,&startup_info
                  ,&process_info);

               CloseHandle(process_info.hProcess);                               //180116
               CloseHandle(process_info.hThread);                                //180116
               }
            }
            RegCloseKey(RegKey);
   }  }  }
   if (!exit_status)  // See if windows explorer can open it                     //030905
   {  wchar_t command[1024];
      wcscpy(command,L"explorer \"");
      wcscat(command,file_name.w_str());
      wcscat(command,L"\"");
      PROCESS_INFORMATION process_info;
            STARTUPINFOW  startup_info;
            GetStartupInfoW(&startup_info);
            startup_info.dwFlags |= STARTF_USESHOWWINDOW;
            startup_info.wShowWindow = SW_MAXIMIZE;
            exit_status = CreateProcessW
               (0
               ,command
               ,0 // default security
               ,0 // default security
               ,true
               ,CREATE_SEPARATE_WOW_VDM|NORMAL_PRIORITY_CLASS
               ,0  // current environment block
               ,0  // pointer to current directory name  (should be current)
               ,&startup_info // pointer to STARTUPINFO
               ,&process_info // pointer to PROCESS_INFORMATION
               );
      CloseHandle(process_info.hProcess);                                        //180116
      CloseHandle(process_info.hThread);                                         //180116
   }
#else
181030
This appears to be long obsolete

   // The filename may be a URL of the form: file:///E|/ued/docs/ued.htm  or  file:///E|\ued\docs\ued.htm
   char file_or_url[255];                                                        //971124
   std::string params("");                                                       //181030
   if ((find(".htm") != std::string::npos)                                       //181030
     ||(find(".HTM") != std::string::npos)                                       //181030
     ||(find(".html") != std::string::npos)                                      //181030
     ||(find(".HTML") != std::string::npos))                                     //181030_ 971124
   {  strcpy(file_or_url,"file:///");
      int j = strlen(file_or_url);
      for (uint16 i = 0; i < length(); i++)
      {   char help_char = get()[i];
          help_char = help_char == ':' ? '|' : help_char;
          help_char = help_char == '#' ? (char)0 : file_or_url[j];
          file_or_url[j] = help_char;
          file_or_url[j+1] = 0;
      }
      if (contains("#"))
          params.assign(get());
   }
   else
      strcpy(file_or_url,get());
   HINSTANCE hInst = ShellExecute
      (0//this->HWindow
      ,NULL  //"open"
      ,file_or_url
      ,params.c_str()
      ,NULL,SW_SHOWNORMAL);
   exit_status = (int)hInst;                                                     //090624
#endif
   if (exit_status >= 32) exit_status = EXIT_SUCCESS;                            //090624
#endif
   return exit_status;
}
//_invoke___________________________________________________________2006-09-07_/
Shell_graphical shell_graphical;
//_____________________________________________________________________________/
}//_namespace MS_Windows_______________________________________________________/
Shell_graphical &shell_graphical = MS_Windows::shell_graphical;
}}//_namespace CORN OS_________________________________________________________/

