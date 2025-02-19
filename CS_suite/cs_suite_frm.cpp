//______________________________________________________________________________

#include <vcl.h>
#pragma hdrstop
SHSTDAPI_(void) SHAddToRecentDocs(UINT uFlags, LPCVOID pv);

#include "cs_suite_frm.h"
#include <CS_suite/application/CS_suite_directory.h>
//#include <cropsyst/cpp/cs_paths.h>
#include <common/simulation/paths.h>
#include <corn/file_sys/smart_dirname.h>
#include <corn/data_source/vv_file.h>
#include <corn/application/user/appdata_directory.h>
#include <corn/application/user/settings.h>
#include <corn/OS/Windows/VCL/registry.h>
//______________________________________________________________________________
#pragma package(smart_init)
#pragma link "SHDocVw_OCX"
#pragma link "AutoRadioGroup"
#pragma link "FlCtrlEx"
#pragma link "SsShlDlg"
#pragma link "StShlCtl"
#pragma link "SsBase"
#pragma link "StShrtCt"
#pragma link "AdvDirectoryEdit"
#pragma link "AdvEdBtn"
#pragma link "AdvEdit"
#pragma link "AutoViewFileButton"
#pragma link "AutoUnitsButton"
#pragma resource "*.dfm"
TCS_suite_form *CS_suite_form;
extern bool reg_file_types_mode;

//______________________________________________________________________________
__fastcall TCS_suite_form::TCS_suite_form(TComponent* Owner)
   : TForm(Owner)
   , show_master_sim_note_on_create(false)
   ,target_project_directory("")
{
   load_settings();
   master_sim_diredit->Text =
      simulation_paths->master_simulation_parameters
      .master_simulation_dir.c_str();

   CORN::Ustring version_button_caption = "Version: ";
   version_button_caption.append(CS_VERSION_STRING);
   button_version->Caption = version_button_caption.c_str();
//   version_number_label->Caption = VERSION;

   // We set the current working directory to the Simulation\database directory
   // so the database editor will find the Simulation/Database directory.

#if (CS_VERSION_CODE >=  0x040D00)
   simulation_paths->provide_database_directory(false).set_as_current_working_directory();
#else
   Smart_directory_name sim_database_dir(simulation_paths->get_database_dir());
   sim_database_dir.set_as_current_working_directory();
#endif
   project_type_radiobuttons->bind_to(&curr_project_mode_labeled,0); // NO_HELP

   CORN::Smart_directory_name &simulation_projects_dir =  const_cast<CORN::Smart_directory_name &>(simulation_paths->get_projects_dir());
   simulation_projects_dir.set_as_current_working_directory();

   CORN::Smart_file_name cs_suite_about_htm;          CS::Suite_directory->find_file_name("about.htm",cs_suite_about_htm); //101124
   CORN::Smart_file_name cs_suite_instructions_htm;   CS::Suite_directory->find_file_name("instructions.htm",cs_suite_instructions_htm);
   CORN::Smart_file_name cs_suite_manual_htm;   CS::Suite_directory->find_file_name("manual.htm",cs_suite_manual_htm);
   about_webbrowser        ->Navigate(WideString(cs_suite_about_htm.c_str()       /*101124 cs_suite_paths->get_cs_suite_about_htm()*/));
   instructions_webbrowser ->Navigate(WideString(cs_suite_instructions_htm.c_str()/*cs_suite_paths->get_cs_suite_instructions_htm()*/));
   utilities_webbrowser    ->Navigate(WideString(cs_suite_manual_htm.c_str()      /*101124 cs_suite_paths->get_cs_suite_manual_utilities_htm()*/));

   //050308 master_sim_diredit_onchange should already handle this
   // it gets called when we set master_sim_diredit
   // But keep this incase that function is removed in the future.
   // simulation_project_shelllistview->RootFolder = simulation_paths->get_simulation_dir().c_str();

   //Note I need to set these to the respective instructions
   change_display(0);
#ifdef NYI
Currently master simulation folder is stored in the INI file
but it may need to go in the registry
   TRegIniFile *reginifile = new TRegIniFile("CropSyst_Suite_4");
   try
   {
      // reginifile->RootKey = HKEY_CURRENT_USER   // Default
      master_simulation_SScombobox->SelectedFolder->Path = reginifile->ReadString("master", "simulation","C:\Simulation");
  }
  __finally
  {   delete reginifile; }
#endif
   recent_project_shelllistview->SpecialRootFolder = sfRecentFiles;  //051230
   simulation_projects_shelllistview->SpecialRootFolder = sfNone;
   simulation_projects_shelllistview->RootFolder = simulation_paths->get_projects_dir().c_str();
   show_hide_register_file_types_button();
   if (reg_file_types_mode)
   {
      register_file_types(0);
      exit(EXIT_SUCCESS);
   }
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::project_open_button_onclick(TObject *Sender)
{
   if (target_project_directory.Length() == 0)
      // The user has not selected a different folder to create/open projects in.
      // so by default is will be to root folder for the default simulation projects
      project_open_dialog->InitialDir = simulation_projects_shelllistview->RootFolder;
   else
      project_open_dialog->InitialDir = target_project_directory;
   if(project_open_dialog->Execute())
   {
      Ustring command(CS::Suite_directory->CropSyst().project_editor_executable()/*101124cs_suite_paths->get_project_editor_exe().c_str()*/);
      command.append(" \"");
      command.append(project_open_dialog->FileName.c_str());
      command.append("\" ");
      command.append(curr_project_mode_labeled.get_label());
      WinExec(command.c_str(),SW_NORMAL);
      Close();
   };
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::cropsyst_image_onclick(TObject *Sender)
{  setup_mode(CROPSYST_PROJECT);
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::arccs_image_onclick(TObject *Sender)
{  setup_mode(GIS_PROJECT);
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::watershed_image_onclick(TObject *Sender)
{  setup_mode(WATERSHED_PROJECT);
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::climgen_image_onclick(TObject *Sender)
{  Ustring command(CS::Suite_directory->ClimGen_executable()/*101124cs_suite_paths->get_climgen_exe().c_str()*/);
   WinExec(command.c_str(),SW_NORMAL);
}
//______________________________________________________________________________
void TCS_suite_form::setup_mode(CropSyst_project_type mode)
{  curr_project_mode_labeled.set(mode);
   change_display(0);
};
//______________________________________________________________________________
void __fastcall TCS_suite_form::change_display(TObject *Sender)
{  project_type_radiobuttons->ItemIndex   = curr_project_mode_labeled.get_int32();
   cropsyst_button_panel->BevelOuter      = bvRaised;
   arccs_button_panel->BevelOuter         = bvRaised;
   watershed_button_panel->BevelOuter     = bvRaised;

   switch (curr_project_mode_labeled.get())
   {
      case CROPSYST_PROJECT :
         cropsyst_button_panel->BevelOuter      = bvLowered;
      break;
      case GIS_PROJECT :
         arccs_button_panel->BevelOuter         = bvLowered;
//NYI         project_webbrowser->Navigate (WideString(cs_suite_paths->get_arccs_instructions_htm()));
      break;
      case WATERSHED_PROJECT :
         watershed_button_panel->BevelOuter     = bvLowered;
//NYI         project_webbrowser->Navigate (WideString(cs_suite_paths->get_watershed_instructions_htm()));
      break;
   };
};
//______________________________________________________________________________
void __fastcall TCS_suite_form::cropsyst_image_ondblclick(TObject *Sender)
{
   setup_mode(CROPSYST_PROJECT);
   project_open_button_onclick(Sender);
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::arccs_image_ondblclick(TObject *Sender)
{
   setup_mode(GIS_PROJECT);
   project_open_button_onclick(Sender);
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::watershed_image_ondblclick(TObject *Sender)
{
   setup_mode(WATERSHED_PROJECT);
   project_open_button_onclick(Sender);
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::database_button_panel_onclick(TObject *Sender)
{
   CORN::Smart_directory_name sim_dir(simulation_paths->get_simulation_dir().c_str());
   Ustring command;
         CORN::Smart_file_name CS_explorer_exe(ParamStr(0).c_str());
         CS_explorer_exe.set_name("CS_explorer");
         command.append("\"");
         command.append(CS_explorer_exe.c_str() );
         command.append("\" \"");
         command.append(sim_dir.c_str());
         command.append("\" ");
   WinExec(command.c_str(),SW_NORMAL);

/*101113   //Launch master database obsolete
Now use only CS Explorer

      Ustring command(cs_suite_paths->get_database_editor_exe().c_str());
      command.append(" \"");
      command.append(sim_dir.c_str());
      command.append("\" /master");
      WinExec(command.c_str(),SW_NORMAL);
      return;
*/
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::help_button_onclick(TObject *Sender)
{  Smart_file_name cropsyst_web_page("http://www.bsyse.wsu.edu/cropsyst");
   cropsyst_web_page.invoke_viewer();
}
//______________________________________________________________________________
void TCS_suite_form::show_hide_register_file_types_button()
{
   // currently always show
};
//______________________________________________________________________________
//041130  Eventually replace register extension with
// dev/corn/OS/Windows/Registry  Registry class
void TCS_suite_form::register_extension
(TRegIniFile *reginifile
,const char *extension
,const char *description
,const char *program
,const char *icon
)
{
   char dot_ext[100];
   dot_ext[0] = '.';
   dot_ext[1] = 0;
   strcat(dot_ext,extension);
   reginifile->WriteString(dot_ext, "", extension);
   reginifile->WriteString(extension, "", description);

   char key[256];
   strcpy(key,extension);
   strcat(key,"\\DefaultIcon");
   reginifile->WriteString(key, "", icon);

   strcpy(key,extension);
   strcat(key,"\\Shell");
    char ext_default_action[256];
   strcpy(ext_default_action,extension);
   strcat(ext_default_action,"_default_action");
   reginifile->WriteString(key, "", ext_default_action);

   strcpy(key,extension);
   strcat(key,"\\Shell\\First_Action");
   char open_app[256];
   strcpy(open_app,"Open ");
   strcat(open_app,description);
   reginifile->WriteString(key, "",open_app);

   strcpy(key,extension);
   strcat(key,"\\Shell\\First_Action\\command");
   char prog_arg[256];
   strcpy(prog_arg,program);
   strcat(prog_arg," \"%1\"");
   reginifile->WriteString(key, "", prog_arg);

   strcpy(key,extension);
   strcat(key,"\\Shell\\");
   strcat(key,ext_default_action);
   reginifile->WriteString(key, "", open_app);

   strcat(key,"\\command");
   reginifile->WriteString(key, "", prog_arg);
};

void __fastcall TCS_suite_form::register_file_types(TObject *Sender)
{
   //101124 This should now work for XP, Vista and Windows7
   AnsiString registry_subkey ("CropSyst_Suite_");
//NYN   bool setup_for_version_4 = radio_group_version->ItemIndex == 0;
//NYN   if (setup_for_version_4)
      registry_subkey += "4";
//NYN   else                       registry_subkey += "5";

   CORN::Registry *registry_CropSyst_Suite_X = new CORN::Registry(registry_subkey.c_str());
   try
   {
      CORN::File_name project_ico_file;    CS::Suite_directory->CropSyst().icons().find_file_name("project.ico"    ,project_ico_file);
      CORN::File_name location_ico_file;   CS::Suite_directory->CropSyst().icons().find_file_name("location.ico"   ,location_ico_file);
      CORN::File_name format_ico_file;     CS::Suite_directory->CropSyst().icons().find_file_name("report.ico"     ,format_ico_file);
      CORN::File_name scenario_ico_file;   CS::Suite_directory->CropSyst().icons().find_file_name("scenario.ico"   ,scenario_ico_file);
      CORN::File_name soil_ico_file;       CS::Suite_directory->CropSyst().icons().find_file_name("soil.ico"       ,soil_ico_file);
      CORN::File_name rotation_ico_file;   CS::Suite_directory->CropSyst().icons().find_file_name("rotation.ico"   ,rotation_ico_file);
      CORN::File_name crop_ico_file;       CS::Suite_directory->CropSyst().icons().find_file_name("crop.ico"       ,crop_ico_file);
      CORN::File_name management_ico_file; CS::Suite_directory->CropSyst().icons().find_file_name("management.ico" ,management_ico_file);
      CORN::File_name schedule_ico_file;   CS::Suite_directory->CropSyst().icons().find_file_name("schedule.ico"   ,schedule_ico_file);
      CORN::File_name UED_ico_file;        CS::Suite_directory->icons().find_file_name("UED.ico"        ,UED_ico_file);

//NYN      if (setup_for_version_4)
      {
         registry_CropSyst_Suite_X->register_extension("csp","CropSyst Suite project" ,CS::Suite_directory->CropSyst().project_editor_executable().c_str()      ,project_ico_file.c_str());
         registry_CropSyst_Suite_X->register_extension("loc","CropSyst location"      ,CS::Suite_directory->CropSyst().location_editor_executable().c_str()       ,location_ico_file.c_str());
         registry_CropSyst_Suite_X->register_extension("fmt","CropSyst format"        ,CS::Suite_directory->CropSyst().format_editor_executable().c_str()         ,format_ico_file.c_str());

         registry_CropSyst_Suite_X->register_extension("csn","CropSyst scenario"      ,CS::Suite_directory->CropSyst().scenario_editor_executable().c_str()   ,scenario_ico_file.c_str());
         registry_CropSyst_Suite_X->register_extension("sil","CropSyst soil"          ,CS::Suite_directory->CropSyst().soil_editor_executable().c_str()       ,soil_ico_file.c_str());
         registry_CropSyst_Suite_X->register_extension("crp","CropSyst crop"          ,CS::Suite_directory->CropSyst().crop_editor_executable().c_str()       ,crop_ico_file.c_str());
         registry_CropSyst_Suite_X->register_extension("mgt","CropSyst management"    ,CS::Suite_directory->CropSyst().management_editor_executable().c_str() ,management_ico_file.c_str());
         registry_CropSyst_Suite_X->register_extension("rot","CropSyst Cropping system/rotation",CS::Suite_directory->CropSyst().rotation_editor_executable().c_str()  ,rotation_ico_file.c_str());
      }
/*NYN
      else  // setup for V5
      {
         registry_CropSyst_Suite_X->register_extension("CropSyst_scenario"      ,"CropSyst scenario"    ,scenario_editor_target_file.c_str()   ,scenario_ico_target_file.c_str());
         registry_CropSyst_Suite_X->register_extension("CS_soil"                ,"CropSyst soil"        ,soil_editor_target_file.c_str()       ,soil_ico_target_file.c_str());
         registry_CropSyst_Suite_X->register_extension("CS_crop"                ,"CropSyst crop"        ,crop_editor_target_file.c_str()       ,crop_ico_target_file.c_str());
         registry_CropSyst_Suite_X->register_extension("CS_management"          ,"CropSyst management"  ,management_editor_target_file.c_str() ,management_ico_target_file.c_str());
         registry_CropSyst_Suite_X->register_extension("CS_rotation"            ,"CropSyst Cropping system/rotation",rotation_editor_target_file.c_str()  ,rotation_ico_target_file.c_str());
      // Currently schedule files are text files, but I may want a specified file extension for version 5
      // registry_subkey.register_extension(????,"Schedule",schedule_viewer_target_file.c_str(),schedule_ico_target_file.c_str());
      };
*/      
      // Setup all versions:
      registry_CropSyst_Suite_X->register_extension("loc"     ,"ClimGen site"         ,CS::Suite_directory->ClimGen_executable().c_str()           ,location_ico_file.c_str());
      registry_CropSyst_Suite_X->register_extension("UED","Universal Environment Database",CS::Suite_directory->weather_editor_executable().c_str(),UED_ico_file.c_str());

      Application->MessageBox("File type registration complete","File type registration", MB_OK	);
  }
  __finally
  {   delete registry_CropSyst_Suite_X; }


/*101124 obsolete  (not working for Windows 7
//041130 eventually replace this:

   TRegIniFile *reginifile = new TRegIniFile("CropSyst_Suite_4");
   try
   {  reginifile->RootKey = HKEY_CLASSES_ROOT;
      register_extension(reginifile,"csp","CropSyst Suite project","C:\\CS_suite_4\\CropSyst\\project_editor.exe","C:\\CS_suite_4\\icons\\cs_project.ico");
      register_extension(reginifile,"csn","CropSyst scenario","C:\\CS_suite_4\\CropSyst\\scenario_editor.exe","C:\\CS_suite_4\\CropSyst\\icons\\scenario.ico");

      register_extension(reginifile,"loc","CropSyst location"                 ,"C:\\CS_suite_4\\CropSyst\\location_editor.exe"   ,"C:\\CS_suite_4\\CropSyst\\icons\\location.ico");
      register_extension(reginifile,"sil","CropSyst soil"                     ,"C:\\CS_suite_4\\CropSyst\\soil_editor.exe"       ,"C:\\CS_suite_4\\CropSyst\\icons\\soil.ico");
      register_extension(reginifile,"crp","CropSyst crop"                     ,"C:\\CS_suite_4\\CropSyst\\crop_editor.exe"       ,"C:\\CS_suite_4\\CropSyst\\icons\\crop.ico");
      register_extension(reginifile,"rot","CropSyst Cropping System/rotation" ,"C:\\CS_suite_4\\CropSyst\\rotation_editor.exe"   ,"C:\\CS_suite_4\\CropSyst\\icons\\rotation.ico");
      register_extension(reginifile,"mgt","CropSyst rotation management"      ,"C:\\CS_suite_4\\CropSyst\\management_editor.exe" ,"C:\\CS_suite_4\\CropSyst\\icons\\crpmgmt.ico");
      register_extension(reginifile,"mgs","CropSyst fixed management"         ,"C:\\CS_suite_4\\CropSyst\\management_editor.exe" ,"C:\\CS_suite_4\\CropSyst\\icons\\spcmgmt.ico");
      register_extension(reginifile,"fmt","CropSyst format"                   ,"C:\\CS_suite_4\\CropSyst\\format_editor.exe"     ,"C:\\CS_suite_4\\CropSyst\\icons\\report.ico");
      register_extension(reginifile,"evs","CropSyst schedule"                 ,"C:\\CS_suite_4\\CropSyst\\schedule_viewer.exe"   ,"C:\\CS_suite_4\\icons\\schedule.ico");

      register_extension(reginifile,"UED","Universal Environment Database"    ,"C:\\CS_suite_4\\common\\weather\\weather_editor.exe"    ,"C:\\CS_suite_4\\icons\\UED.ico");
//NYI      register_extension(reginifile,"ued","CropSyst scenario","C:\\CS_suite_4\\CropSyst\\scenario_editor.exe","C:\\CS_suite_4\\CropSyst\\icons\\scenario.ico");
      Application->MessageBox("File type registration complete","File type registration", MB_OK	);
  }
  __finally
  {   delete reginifile; }

*/

};
//______________________________________________________________________________
void __fastcall TCS_suite_form::crop_calib_image_onclick(TObject *Sender)
{  Ustring command(CS::Suite_directory->CropSyst().crop_calibrator_executable()/*101124 cs_suite_paths->get_crop_calibrator_exe().c_str()*/);
   WinExec(command.c_str(),SW_NORMAL);
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::FormClose(TObject *Sender,TCloseAction &Action)
{  save_settings(); }
//______________________________________________________________________________
void  TCS_suite_form::load_settings()
{  Application_data_directory appdata_dir(CORN::Smart_file_name(ParamStr(0).c_str())); // 080514
   User_settings user_settings;
   appdata_dir.load_user_settings(user_settings,"CS_suite");
   if ((user_settings.GUI_top || user_settings.GUI_left)
       && user_settings.GUI_width && user_settings.GUI_height)
   {  Top   = user_settings.GUI_top;
      Left  = user_settings.GUI_left;
      Width = user_settings.GUI_width;
      Height= user_settings.GUI_height;
      Position = poDesigned;
   };
   cs_suite_pagecontrol->ActivePageIndex = (reg_file_types_mode)
   ? 0 // about page
   : user_settings.GUI_activepageindex;
};
//______________________________________________________________________________
void  TCS_suite_form::save_settings()
{  simulation_paths->set_simulation_root( master_sim_diredit->Text.c_str());
   User_settings user_settings;
   user_settings.GUI_top = Top;
   user_settings.GUI_left = Left;
   user_settings.GUI_width= Width;
   user_settings.GUI_height= Height;
   user_settings.GUI_activepageindex = (reg_file_types_mode)
   ? 0 // about_tabsheet
   : cs_suite_pagecontrol->ActivePageIndex;
   Application_data_directory appdata_dir(CORN::Smart_file_name(ParamStr(0).c_str())); // 080514
   appdata_dir.save_user_settings(user_settings,"CS_suite");
#ifdef NYI
Currently master simulation folder is stored in the INI file
but it may need to go in the registry

   TRegIniFile *reginifile = new TRegIniFile("CropSyst_Suite_4");
   try
   {
      // reginifile->RootKey = HKEY_CURRENT_USER   // Default
      reginifile->WriteString("master", "simulation", master_simulation_SScombobox->SelectedFolder->c_str());
  }
  __finally
  {   delete reginifile; }
#endif
};
//______________________________________________________________________________
void __fastcall TCS_suite_form::project_help_button_onclick(TObject *Sender)
{
   CORN::Smart_file_name manual_projects_htm;
   CS::Suite_directory->find_file_name("projects.htm",manual_projects_htm);
   manual_projects_htm/*101124 cs_suite_paths->cs_suite_manual_projects_htm*/.invoke_viewer();
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::recent_project_shelllistviewItemDblClick(
      TObject *Sender, TStShellItem *Item, bool &DefaultAction)
{
   CORN::Smart_file_name project_filename( Item->Path.c_str());
   // Shellshock selects both directories and filenames
   // Here we allow the project directory to be selected
   // we append the csp extension to open the project
   // if either a directory or a project is made.
   if (project_filename.ext_is("lnk"))
   {  // If this is a link (I.e. from recent directory) then get the actual filename.

      shortcutter->ShortcutFileName = Item->Path;
      if (shortcutter->ResolveShortcut())
         project_filename.set( shortcutter->FileName.c_str());
      else return; // shortcut is no longer linked.
   }
   else  // This may be a project folder see if there is a .csp project file associated with this directory.
      project_filename.set_ext("csp");

   if (project_filename.exists())
   {
      Ustring command(CS::Suite_directory->CropSyst().project_editor_executable()/*101124cs_suite_paths->get_project_editor_exe().c_str()*/ );
      command.append(" \"");
      command.append(project_filename.c_str());
      command.append("\" ");
      // actually, in the project file the project type
      //190109command.append(curr_project_mode_labeled.get_label());
      curr_project_mode_labeled.append_label(command);  ///190109
      WinExec(command.c_str(),SW_NORMAL);
      DefaultAction=false; // We have already openned the project,
      // don't let the shell try to open it also.
      SHAddToRecentDocs(2 /*SHARD_PATH*/,project_filename.c_str());
      Close();

   };
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::simulation_projects_shelllistviewItemDblClick(
      TObject *Sender, TStShellItem *Item, bool &DefaultAction)
{  CORN::Smart_file_name project_filename( Item->Path.c_str());
   // Shellshock selects both directories and filenames
   // Here we allow the project directory to be selected
   // we append the csp extension to open the project
   // if either a directory or a project is made.
   project_filename.set_ext("csp");
   Ustring command;
   if (project_filename.exists())
   {  // Original version 4 project file and project directory so open project with
      // project editor.
      command.append("\"");
      command.append(CS::Suite_directory->CropSyst().project_editor_executable() /*101124cs_suite_paths->get_project_editor_exe().c_str() */);
      command.append("\" \"");
      command.append(project_filename.c_str());
      command.append("\" ");
      // actually, in the project file the project type
      command.append(curr_project_mode_labeled.get_label());
      SHAddToRecentDocs(2 /*SHARD_PATH*/,project_filename.c_str());
   } else
   {  // 090701  Perhaps it is a new project directory context
      CORN::Smart_directory_name project_directory(Item->Path.c_str());
      project_filename.set_name_ext(".CS_project");
      project_filename.set_path(project_directory);
      if (project_filename.exists())
      {  // It is a new project context so open with CS_explorer.
         CORN::Smart_file_name CS_explorer_exe(ParamStr(0).c_str());
         CS_explorer_exe.set_name("CS_explorer");
         command.append("\"");
         command.append(CS_explorer_exe.c_str() );
         command.append("\" \"");
         command.append(project_directory.c_str());
         command.append("\" ");
      };
   };
   if (command.length())
   {
      WinExec(command.c_str(),SW_NORMAL);
      DefaultAction=false; // We have already openned the project,
      // don't let the shell try to open it also.
      Close();
   };
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::master_sim_diredit_onchange(TObject *Sender)
{  save_settings();
   if (show_master_sim_note_on_create)
      Application->MessageBox("You will need to close and reopen any project and scenario files to put the new master simulation folder selection into effect in those editors","Reopen parameter files",MB_OK);
   simulation_projects_shelllistview->RootFolder = simulation_paths->get_projects_dir() .c_str();
   show_master_sim_note_on_create = true;
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::location_edit_button_onclick(TObject *Sender)
{  Ustring command;
   command.append("\"");
   command.append(CS::Suite_directory->CropSyst().location_editor_executable() /*101124 cs_suite_paths->get_location_editor_exe().c_str()*/);
   command.append("\"");
   WinExec(command.c_str(),SW_NORMAL);
}
//______________________________________________________________________________
void __fastcall TCS_suite_form::pagecontrol_projects_listChange(TObject *Sender)
{  bool on_simulation_projects_page = pagecontrol_projects_list->ActivePage == simulation_projects_tabsheet;
   simulation_projects_shelllistview->Enabled = on_simulation_projects_page;
   recent_project_shelllistview     ->Enabled = !on_simulation_projects_page;
}
//______________________________________________________________________________
// 080722 456 lines

void __fastcall TCS_suite_form::simulation_project_shelllistnagFolderSelected(
      TObject *Sender, TStShellFolder *Folder)
{
   target_project_directory = (Folder->Path);
}
//---------------------------------------------------------------------------

void __fastcall TCS_suite_form::button_versionClick(TObject *Sender)
{
/*101124
   Directory_name &cropsyst_dir = cs_suite_paths->get_cropsyst_dir();
   CORN::Ustring cropsyst_documentation_history_index_htm(cropsyst_dir.c_str());
   cropsyst_documentation_history_index_htm.append("\\documentation\\history\\index.htm");
   CORN::Smart_file_name  cropsyst_documentation_history_index_htm_filename(cropsyst_documentation_history_index_htm.c_str());
*/
   CORN::Smart_file_name  cropsyst_documentation_history_index_htm_filename("history.htm");  //101124
   cropsyst_documentation_history_index_htm_filename.set_path                                //101124
      (CS::Suite_directory->documentation());                                                //101124
   cropsyst_documentation_history_index_htm_filename.invoke_viewer();
}
//---------------------------------------------------------------------------
void __fastcall TCS_suite_form::panel_lauch_CS_explorerClick(TObject *Sender)
{
      Ustring command(CS::Suite_directory->CS_explorer_executable()/*101124 cs_suite_paths->get_CS_explorer_exe().c_str()*/);
      command.append(" \"");
      command.append(simulation_paths->master_simulation_parameters.master_simulation_dir.c_str());
      command.append("\" ");
      WinExec(command.c_str(),SW_NORMAL);
      Close();
}
//---------------------------------------------------------------------------

