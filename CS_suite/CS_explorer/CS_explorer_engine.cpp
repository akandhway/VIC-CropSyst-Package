//______________________________________________________________________________
#include <vcl.h>
#pragma hdrstop
#include "CS_explorer_engine.h"

#include "CropSyst/source/options.h"
#include "CS_suite/CropSyst/file_system/project_directory_CropSyst.h"
#include "CS_suite/application/CS_suite_directory.h"
#include "corn/container/text_list.h"
#include "corn/data_source/vv_file.h"
#include "corn/OS/OS.h"
#include "corn/OS/directory_entry_name.h"
#include "corn/OS/file_system_engine.h"
#include "corn/OS/shell_graphical.h"
#include "corn/string/strconv.hpp"
#include "UED/convert/convert_response.h"
#include "UED/library/UED_fname.h"
#include "CropSyst/source/cs_filenames.h"
#include "CropSyst/source/cs_scenario.h"
#include "CropSyst/source/output.h"
#include <assert.h>
#include <fstream>

using namespace std;
using namespace CS;
#include "corn/Internet/HTTP.h"
#include "corn/Internet/HTTP/request.hpp"
//______________________________________________________________________________
CS_Explorer_engine::CS_Explorer_engine()
: Explorer_engine_abstract
   (&user_settings
   ,&user_application_directory
   ,&CS::Suite_directory->CS_Explorer()
   ,(CS::Suite_directory))
, CS_contextualization                                                       (0) //180723
, CS_contextualization_owned                                             (false) //180723
, user_application_directory     (CS::Suite_directory->CS_explorer_executable())
, output_options                                                             (0)
, identified_context                                           (UNKNOWN_context)
, UED_tools_mode                                                         (false)
, preferred_model_executable_qual                                            (0) //171121
{
   preferred_model_executable_qual = new CORN::OS::File_name_concrete            //171121
      (CS::Suite_directory->CropSyst().cropsyst_model_executable());             //171121
}
//_constructor_________________________________________________________________/
CS_Explorer_engine::~CS_Explorer_engine()
{
   delete output_options;
   delete preferred_model_executable_qual;                                       //171121
   if (CS_contextualization_owned) delete CS_contextualization;                  //180723
}
//_destructor__________________________________________________________________/
CS::Contextualization &CS_Explorer_engine
::provide_contextualization()                                         provision_
{  if (!CS_contextualization)
   {  CORN_contextualization = CS_contextualization = new CS::Contextualization;
      CS_contextualization_owned = true;
   }
   return *CS_contextualization;
}
//_provide_contextualization________________________________________2018-07-23_/
Directory_context CS_Explorer_engine::identify_context_of
(const std::string &dir_name_unqual)                                      const
{
   {  // check if database
      if (dir_name_unqual == "Database")
          return identified_context = database_context;
   }
   {  // check if scenarios
      if (dir_name_unqual == "Scenarios"  )
          return identified_context = scenarios_context;
   }
   {  // check if enterprises
      if (dir_name_unqual == "Enterprises"  )
          return identified_context = enterprises_context;
   }
   {  // check if scenario output
      //180406 if (dir_name_unqual == "Output"  )
      if (dir_name_unqual.find("Output") != std::string::npos  )                 //180506
          return identified_context = scenario_output_context;
      // NYI Should also check if the parent folder is a scenario context
   }
   {  // check if database weather
      if (dir_name_unqual == "Weather"  )
          return identified_context = weather_context;
      // NYI Should also check if the parent folder is a database context
   }
   {  // check if database rotation
      if (dir_name_unqual == "Rotation"  )
          return identified_context = rotation_context;
      // NYI Should also check if the parent folder is a database context
   }
   {  // check if database crop
      if (dir_name_unqual == "Crop"  )
          return identified_context = crop_context;
      // NYI Should also check if the parent folder is a database context
   }
   {  // check if database management
      if (dir_name_unqual == "Management"  )
          return identified_context = management_context;
      // NYI Should also check if the parent folder is a database context
   }
   {  // check if database Biomatter
      if (dir_name_unqual == "Biomatter"  )
          return identified_context = biomatter_context;
      // NYI Should also check if the parent folder is a database context
   }
   return 0;
}
//_identify_context_of_________________________________________________________/
Directory_context CS_Explorer_engine::identify_context_of
(const CORN::OS::Directory_name &dir_name_qualified)                       const
{
   identified_context =
      UED_tools_mode ? weather_context : arbitrary_context ;
   std::string directory_name;                                                   //120614
   dir_name_qualified.append_components_to_string
      (directory_name,CORN::OS::Directory_entry_name::include_name_extension);
      // Identifies the current directory context by either:
      // - The name of the directory.
      // - The presence of key files.
      // - The key extension of some files.
      // - The desktop.ini file has a CS context description InfoTip
      //      or one of the CS icons.
   {  // Check for CS version 5 project file
      CORN::OS::File_name_concrete CS_project_filename
         (dir_name_qualified,L"","CS_project");                                  //120515
      if (CORN::OS::file_system_engine.exists(CS_project_filename))
         return  identified_context = project_context;
   }
   {  // Check for CS_version 4 project file
      CORN::OS::File_name_concrete CS_project_filename
       (dir_name_qualified,dir_name_qualified.get_name(),OS::Extension("csp"));
      if (CORN::OS::file_system_engine.exists(CS_project_filename))
         return  identified_context = project_context;
   }
   identified_context = identify_context_of(directory_name);
   if (identified_context)
      return identified_context;
   {  // check if CropSyst scenario context
      CORN::OS::File_name_concrete CS_scenario_filename
         (dir_name_qualified,L"",L"CropSyst_scenario");
      if (CORN::OS::file_system_engine.exists(CS_scenario_filename))
         return  identified_context = scenario_context;
   }
   {  // check if CANMS scenario context
      CORN::OS::File_name_concrete CANMS_scenario_filename
         (dir_name_qualified,L"",L"CANMS_scenario");
      if (CORN::OS::file_system_engine.exists(CANMS_scenario_filename))
         return  identified_context = scenario_context;
   }
   {  // check if CAFE dairy scenario context
      CORN::OS::File_name_concrete CAFE_dairy_scenario_filename
         (dir_name_qualified,L"",L"CAFE_dairy_scenario");
      if (CORN::OS::file_system_engine.exists(CAFE_dairy_scenario_filename))
         return  identified_context = scenario_context;
   }
   {  // check if NIFA dairy scenario context                                    //141111
      CORN::OS::File_name_concrete NIFA_dairy_scenario_filename
         (dir_name_qualified,L"",L"NIFA_dairy_scenario");
      if (CORN::OS::file_system_engine.exists(NIFA_dairy_scenario_filename))
         return  identified_context = scenario_context;
   }
   {  // check if CS project context                                             //141025
      CORN::OS::Directory_name_concrete project_scenarios_dirname
         (dir_name_qualified,L"Scenarios",CORN::OS::directory_entry);            //121029
      if (CORN::OS::file_system_engine.exists(project_scenarios_dirname))
         return  identified_context = project_context;
   }
   {  // Check for CS_version 4 project file
      CORN::OS::File_name_concrete CS_scenario_filename
        (dir_name_qualified,dir_name_qualified.get_name(),OS::Extension("csn"));
      if (CORN::OS::file_system_engine.exists(CS_scenario_filename))
         return identified_context = scenario_context;
   }
   {  // check if database soil
      if (directory_name.compare("Soil"  ) == 0)
      {  // We need to distinguish between database soil directory and output soil directory   (090803)
         return identified_context
            = (dir_name_qualified.get_parent_directory_name_qualified().get_name().find(L"Output") != std::string::npos)
            ? scenario_output_context : soil_context;
      }
      // NYI Should also check if the parent folder is a database context
   }
   {  // check if database Biomatter
      if (directory_name.compare("Initial"  ) == 0)
          return identified_context = initial_context;
      // NYI Should also check if the parent folder is a database context
   }
   #if (CS_VERSION==4)
   {  // check if database format
      if (directory_name.compare("Format"  ) == 0)
          return identified_context = format_context;
      // NYI Should also check if the parent folder is a database context
   }
   #endif
   //171231 warning, the following Output and Scenarios are probably obsolete for V5
   // because in CS_exporer(5) uses beneath checks, but need to check how this case is handled
   if (dir_name_qualified.has_in_path_cstr("Output",CORN::OS::Directory_entry_name::include_all)) //180407_171124
      // Output can now be in the extension i.e. backup                          //180407
      return identified_context = scenario_output_context;                       //171124

   if (dir_name_qualified.has_in_path_cstr("Scenarios",CORN::OS::Directory_entry_name::include_all)) //180407_171121
      return identified_context = scenario_context;                              //171121

/* NYI need to implement provide_monuments  171231

   std::string monumement_name;                                                  //171231
   if (provide_monuments().identify(directory_name,monument_name))               //171231
   {  identified_context = identify_context_of(monument_name);                   //171231
      if (identified_context)                                                    //171231
         return identified_context;                                              //171231
   }
   // NYI check .directory file
*/

   {  // This checks the desktop.ini                                             //101124
      CORN::OS::File_name_concrete desktop_ini_filename("desktop.ini");
      char infotip[512];
      GetPrivateProfileString(".ShellClassInfo","InfoTip","",infotip,sizeof(infotip),desktop_ini_filename.c_str());
      char iconfile[512];
      GetPrivateProfileString(".ShellClassInfo","IconFile","",iconfile,sizeof(iconfile),desktop_ini_filename.c_str());
      for (nat8 context_index = 0;  context_index < CONTEXT_COUNT; context_index++)
      {
         if (strcmp(infotip,context_strings[context_index].description) == 0)
            identified_context = context_index;
         if (strcmp(iconfile,context_strings[context_index].icon) == 0)
            identified_context = context_index;
      }
   }
   if (identified_context == arbitrary_context)
   {  // If we still haven't found the context it is possible that we are in a subdirectory
      // of a context (For example we often of subdirectories of the weather database)
      if (directory_name.find("\\Weather\\")    != std::string::npos) identified_context = weather_context;
      if (directory_name.find("\\Soil\\")       != std::string::npos) identified_context = soil_context;
      if (directory_name.find("\\Crop\\")       != std::string::npos) identified_context = crop_context;
      if (directory_name.find("\\Output\\")     != std::string::npos) identified_context = scenario_output_context;
      if (directory_name.find("\\Management\\") != std::string::npos) identified_context = management_context;
      if (directory_name.find("\\Rotation\\")   != std::string::npos) identified_context = rotation_context;
      if (directory_name.find("\\Biomatter\\")  != std::string::npos) identified_context = biomatter_context;
      if (directory_name.find("\\Scenarios\\")  != std::string::npos) identified_context = scenarios_context;
      if (directory_name.find("\\Enterprises\\")!= std::string::npos) identified_context = enterprises_context;
      // Don't check here for the following contexts:
      // arbitrary_context            //  0
      // project_context              //  1
      // database_context             //  2
      // scenario_context             //  5
      // generated_scenarios_context  //  6
   }
   if (UED_tools_mode)
      identified_context = weather_context;
   return identified_context;
}
//_identify_context_of_________________________________________________________/
CS::Directory_context CS_Explorer_engine::identify_context()               const
{  return identify_context_of(get_active_directory()); }
//_identify_context____________________________________________________________/
bool CS_Explorer_engine::project_create_scenario(const std::wstring &new_scenario_name)
{  // in this case we are in the project directory
   // Setup the scenarios directory (if it doesn't aready exist)
   CORN::OS::Directory_name_concrete scenarios_dir
      (get_active_directory(),L"Scenarios",L"",CORN::OS::directory_entry);       //161029
   if (CORN::OS::file_system_engine.exists(scenarios_dir))
   {  CORN::OS::file_system_engine.provide_directory(scenarios_dir); // if the Scenarios directory doesn't already exist.
      provide_contextualization().change_active_directory                        //180723
         (scenarios_dir.c_str());                                                //180723
      setup_context_CWD();
   }
   // Now set up the scenario directory (probably will be creating but this doesn't hurt if it already exists).
   // 090624 Although the version 4 scenario editor will create the scenario directory
   // it doesn't currently setup the context.
   CORN::OS::Directory_name_concrete scenario_directory
      (scenarios_dir,new_scenario_name,L"",CORN::OS::directory_entry);           //161029
   bool scenario_already_exists = CORN::OS::file_system_engine
      .exists(scenario_directory);                                               //140213
   CORN::OS::file_system_engine.provide_directory(scenario_directory);
   provide_contextualization().change_active_directory(scenario_directory.c_str());      //180723
   CORN::OS::File_name_concrete  created_scenario_filename(scenarios_dir,L"",OS::Extension(CS_scenario_EXT));
   // In version 4 I don't want to create a context because
   CORN::OS::File_name_concrete  scenario_editor
      (CS::Suite_directory->CropSyst().scenario_editor_executable()
         /*I.e. "C:\\CS_suite_4\\CropSyst\\scenario_editor.exe"*/);
   #ifdef _Windows
   WritePrivateProfileString("scenario","","",created_scenario_filename.c_str()); // This is to ensure that a scenario file exists so we can recognize the context
   #endif
   setup_context_CWD(); // Setup the scenario directory context
   // May need to be setup_context_in  180625
   return run_program_with_primary_DEN
      ( scenario_editor
      , created_scenario_filename
      , L""
      , false // dont wait_for_termination     // True if this editor is held until the other program exits
      , SW_SHOWNORMAL);
}
//_project_create_scenario_____________________________________________________/
bool CS_Explorer_engine::project_scenarios_create_scenario  // rename, nolonger necessarily the Scenario directory
(const STRING_WITH_FILENAME & new_scenario_name
,const STRING_WITH_FILENAME & scenario_file_name_or_extension
,const CORN::OS::File_name  & scenario_editor_exe)
// In version 4 we use extension, in version 5 it is just the name
{  // In this case we are already in the scenarios directory
   if (new_scenario_name.empty()) return false; // we need a name to create       //180330
   setup_context_CWD();
   // In most cases the scenarios directory will already have been setup but this could be the first scenario.
   CORN::OS::Directory_name_concrete scenario_dir
      (get_active_directory(),new_scenario_name,CORN::OS::directory_entry);     //161029
   CORN::OS::file_system_engine.provide_directory(scenario_dir);
   provide_contextualization().change_active_directory(scenario_dir);            //180723
//   NYI the scenario will be a scenario directory with an associated directory file: .CropSyst_scenario .
   CORN::OS::file_system_engine.set_current_working_directory(scenario_dir);
   CORN::OS::File_name_concrete created_scenario_filename
      (scenario_dir,scenario_file_name_or_extension); // the filename is simply .CropSyst_scenario
   // Use the last selected scenario file as the template to copy to this new scenario
   if (CORN::OS::file_system_engine.exists
       (recently_selected_DEN[scenarios_context]))
      CORN::OS::file_system_engine.copy_file
         (recently_selected_DEN[scenarios_context],created_scenario_filename,true);
#ifdef _Windows
   WritePrivateProfileString("scenario","","",created_scenario_filename.c_str()); // This is to ensure that a scenario file exists so we can recognize the context
#else
   NYI , open the file name and write [scenario]
#endif
   setup_context_CWD(); // Although the version 4 scenario editor will create the scenario directory as needed it might not setup the directory context
   return run_program_with_primary_DEN
      ( scenario_editor_exe
      , created_scenario_filename
      , L""
      , false // not wait_for_termination     // True if this editor is held until the other program exits
      , SW_SHOWNORMAL);
}
//_project_scenarios_create_scenario___________________________________________/
/*NYN
const std::string &CS_Explorer_engine::project_scenarios_create_scenario
(const wchar_t *new_scenario_name
,const char *scenario_file_name_or_extension
,const CORN::Smart_file_name &scenario_editor_exe)
{
   std::wstring new_scenario_name_wstr(new_scenario_name);
   std::string  new_scenario_name_str;
   wstring_to_string(new_scenario_name_wstr,new_scenario_name_str);
   return project_scenarios_create_scenario(new_scenario_name_str.c_str(),scenario_file_name_or_extension,scenario_editor_exe);
}
*/
//______________________________________________________________________________
bool CS_Explorer_engine::project_enterprises_create_enterprise
(const std::wstring &new_enterprise_name)
{  // In this case we are already in the scenarios directory
   CORN::OS::Directory_name_concrete enterprises_dir(get_active_directory(),CORN::OS::directory_entry); //161029
   change_active_directory(enterprises_dir);                                     //180723
   setup_context_CWD(); // In most cases the scenarios directory will already have been setup but this could be the first scenario.
   CORN::OS::Directory_name_concrete enterprise_dir(enterprises_dir,new_enterprise_name,CORN::OS::directory_entry); //161029
   change_active_directory(enterprise_dir);                                      //180723
   CORN::OS::File_name_concrete created_enterprise_filename(enterprise_dir,".CANMS_enterprise");
#ifdef _Windows
   WritePrivateProfileString("enterprise","","",created_enterprise_filename.c_str()); // This is to ensure that a scenario file exists so we can recognize the context
#else
   NYI file the files doesn't exists, just create a file with the [enterprise] section
#endif
   setup_context_CWD(); // Although the version 4 scenario editor will create the scenario directory as needed it might not setup the directory context
   return run_program_with_primary_DEN
      ( CS::Suite_directory->CANMS().scenario_editor_exe // the scenario editor also edits enterprises
      , created_enterprise_filename
      , L""
      , false // dont wait_for_termination     // True if this editor is held until the other program exits
      , SW_SHOWNORMAL);
}
//_project_enterprises_create_enterprise_______________________________________/
bool CS_Explorer_engine::project_GIS_simulation()
{  // In this case I may still need to update the desktop file icon
   return edit_parameter_file(CS_PROJECT_FILENAME,"","GIS_project_editor",L"");  //120515
}
//_project_GIS_simulation______________________________________________________/
bool CS_Explorer_engine::project_watershed_simulation()
{
/*190418 obsolete
#if (CROPSYST_VERSION == 4)
   CORN::OS::File_name_concrete
      project_filename(*directory_name_qualified);
#endif
*/
// In this case I may still need to update the desktop file  icon
   return edit_parameter_file
      (CS_PROJECT_FILENAME,"","project_editor",L"Watershed");                    //120515
}
//_project_watershed_simulation________________________________________________/
bool CS_Explorer_engine::project_carbon_sequestration_for_grazing_regimen()
{  return edit_parameter_file(CS_PROJECT_FILENAME,"","project_editor",L"Grazing");
}
//_project_carbon_sequestration_for_grazing_regimen____________________________/
bool CS_Explorer_engine::setup_context_in
(const CORN::OS::Directory_name &context_dir
,nat16 context_type_index)             const
{
   if (context_type_index == 1)
   {
      CORN::OS::File_name_concrete project_filename(context_dir,".CS_project");
      std::ofstream project_file(project_filename.c_str());
      project_file << "[project]" << std::endl;
   }
   return  Explorer_engine_abstract::setup_context_in(context_dir,context_type_index);
}
//_setup_context_in_________________________________________________2019-08-18_/
const CORN::OS::Directory_name *CS_Explorer_engine::create_simulation_directory
(const STRING_WITH_FILENAME &simulation_folder_name
,const STRING_WITH_FILENAME &projects_folder_name)
{  CORN::OS::Directory_name_concrete simulation_dir
      (get_active_directory()
      ,simulation_folder_name,CORN::OS::directory_entry);                        //161029
   change_active_directory(simulation_dir);                                      //180723
   setup_context_in(simulation_dir,arbitrary_context);                           //180625
   CORN::OS::Directory_name_concrete projects_dir
      (simulation_dir,projects_folder_name,CORN::OS::directory_entry);           //161029
   CORN::OS::file_system_engine.provide_directory(projects_dir);
   CORN::OS::Directory_name_concrete database_dir
      (simulation_dir,"Database",CORN::OS::directory_entry);                     //161029
   change_active_directory(database_dir);                                        //180723
   setup_context_in(database_dir,database_context);                              //180625
   CORN::OS::File_name_concrete database_desktop_ini(database_dir,"desktop.ini");
   std::ofstream desktop_ini(database_desktop_ini.c_str());
   const CORN::OS::File_name *database_ico_fname_ptr
      = CS::Suite_directory->find_file_name //161103 this has been updated, need to check if this is still working
         (L"database.ico",CORN::OS::File_system::subdirectory_recursion_inclusive);
   if (database_ico_fname_ptr)
      desktop_ini << "[.ShellClassInfo]" << endl
               << "IconFile=" << database_ico_fname_ptr->c_str() << endl // NYI warning, need to get the icon file from CS_suite icons path
               << "IconIndex=0" << endl
               << "InfoTip=CropSyst Suite project" << endl;
   // Go into the projects directory because the next likely thing to
   // do is create a project.
   change_active_directory(projects_dir);                                        //180723
   setup_context_in(projects_dir,arbitrary_context);                             //180625
   return &get_active_directory();
}
//_create_simulation_directory_________________________________________________/
const CORN::OS::Directory_name *CS_Explorer_engine::add_database_context
(const STR_WITH_FILENAME parameter_file_directory_name_unqual
,bool set_parameter_directory_as_current)                                        //110423
{  CORN::OS::Directory_name_concrete database_dir(get_active_directory());       //161029_110423
   CORN::OS::Directory_name_concrete database_param_dir
      (database_dir,parameter_file_directory_name_unqual,CORN::OS::directory_entry); //161029
   change_active_directory(database_param_dir);                                  //180723
   setup_context_CWD();                                                          //180624
   if (!set_parameter_directory_as_current)                                      //180723
      change_active_directory(database_dir);                                     //180723
   return &get_active_directory();
}
//_add_database_context________________________________________________________/
bool CS_Explorer_engine::database_has_directory(const char *subdirectory_name) const
{  CORN::OS::Directory_name_concrete subdirectory(get_active_directory()         //180723
      ,subdirectory_name,CORN::OS::directory_entry);                             //161029
   return CORN::OS::file_system_engine.exists(subdirectory);
}
//_database_has_directory______________________________________________________/
bool CS_Explorer_engine::edit_parameter_file
(const STRING_WITH_FILENAME & name
,const_ASCIIZ extension
,const_ASCIIZ editor_name  /* without .exe extension */
,const_UnicodeZ option)                                                    const
{  bool result = false;
      STRING_WITH_FILENAME unqualified_editor_name;
      CORN::ASCIIZ_to_wstring(editor_name,unqualified_editor_name);
      #ifdef _WIN32
      append_ASCIIZ_to_wstring(".exe",unqualified_editor_name);  // Windows executables have .exe extension
      #endif
   std::wstring options(option? option:L"");
   CORN::OS::File_name_concrete *parameter_file_name = new CORN::OS::File_name_concrete (name);   // First assume name is fully qualified
   if (!CORN::OS::file_system_engine.exists(*parameter_file_name))
   {  // Probably then the name is unqualified name with no extension.
      delete parameter_file_name;
      parameter_file_name = new CORN::OS::File_name_concrete
         (get_active_directory(),name,extension);                                //180723

   }
   CORN::OS::File_name *parameter_editor = CS::Suite_directory->find_file_name
      (unqualified_editor_name
      ,CORN::OS::File_system::subdirectory_recursion_inclusive);                 //161103
   if (parameter_editor)
      result = run_program_with_primary_DEN
      ( *parameter_editor
      , *parameter_file_name
      , options
      , false // wait_for_termination     // True if this editor is held until the other program exits
      , SW_SHOWNORMAL);
   //else  should output status message that the parameter editor could not be found
   delete parameter_editor;
   delete parameter_file_name;
   return result;
}
//______________________________________________________________________________
bool CS_Explorer_engine::edit_context_file
(const STRING_WITH_FILENAME &name,const_UnicodeZ option)                   const
{
   const char *parameter_file_ext_or_name = context_strings[identified_context].
         parameter_file_ext_V4;
         /*200604 abandoned
            #if (CS_VERSION == 4)
            parameter_file_ext_V4;
            #else
            parameter_file_ext_or_name;
            #endif
         */
   CORN::OS::File_name_concrete *parameter_file_name =
      (context_strings[identified_context].parameter_directory)
      ? new CORN::OS::File_name_concrete
         (get_active_directory()
         ,name,parameter_file_ext_or_name)
      : new CORN::OS::File_name_concrete
         (get_active_directory()
         ,context_strings[identified_context].parameter_directory);
   CORN::OS::File_name_concrete parameter_editor(*CS::Suite_directory,context_strings[identified_context].editor);
   bool result =
      run_program_with_primary_DEN
      ( parameter_editor
      ,*parameter_file_name
      , option ? option : L"" // options
      , false // wait_for_termination     // True if this editor is held until the other program exits
      , SW_SHOWNORMAL);
   delete parameter_file_name;                                                   //141122
   return result;
}
//______________________________________________________________________________
bool CS_Explorer_engine::edit_database_parameter_file
(const CORN::OS::Directory_name  &database_dirname // fully qualified
,CS::Directory_context           parameter_context
,const STRING_WITH_FILENAME     &name)                                     const
{  // This is used by CropSyst Wizard
   CORN::OS::Directory_name_concrete parameter_directory
      (database_dirname,context_strings[parameter_context].context
      ,CORN::OS::directory_entry);                                               //161029
   const char *parameter_file_ext_or_name = context_strings[parameter_context].
         parameter_file_ext_V4;
         /*abandoned
            #if (CS_VERSION == 4)
            parameter_file_ext;
            #else
            parameter_file_ext_or_name;
            #endif
         */
   CORN::OS::File_name_concrete parameter_file_name
      (parameter_directory,name,parameter_file_ext_or_name);
   CORN::OS::File_name_concrete parameter_editor(*CS::Suite_directory,context_strings[parameter_context].editor);
   return run_program_with_primary_DEN
      ( parameter_editor
      ,parameter_file_name
      ,  L"" // options
      , false // wait_for_termination     // True if this editor is held until the other program exits
      , SW_SHOWNORMAL);
}
//_2014-11-22__________________________________________________________________/
bool CS_Explorer_engine::edit_CWD_parameter_file
(CS::Directory_context           parameter_context
,const STRING_WITH_FILENAME     &name)                                     const
{  // This is used by CS_explorer parameter panel
   const char *parameter_file_ext = context_strings[parameter_context]
      .parameter_file_ext_V4;
   CORN::OS::File_name_concrete parameter_file_name
      (get_active_directory(),name,parameter_file_ext);
   CORN::OS::File_name_concrete parameter_editor(*CS::Suite_directory,context_strings[parameter_context].editor);
   return run_program_with_primary_DEN
      ( parameter_editor,parameter_file_name,  L"", false , SW_SHOWNORMAL);
      // do not wait for termination and
      // Use true if this editor is held until the other program exits
}
//_2017-01-02_________________________________________________________________/
bool CS_Explorer_engine::launch_UED_import()                               const
{  return run_program                                                            //120411
         (CS::Suite_directory->UED().import_wizard_executable()
         ,get_active_directory().w_str(), false, true );
         // do not wait for termination and visible
}
//______________________________________________________________________________
bool CS_Explorer_engine::launch_UED_export()                               const
{  return run_program_with_primary_DEN
         (CS::Suite_directory->UED().export_wizard_executable()
         ,recently_selected_DEN[identify_context()], L"", false, true );
         // do not wait for termination and visible
}
//______________________________________________________________________________
bool CS_Explorer_engine::launch_UED_spatial_interpolator()                 const
{  return run_program                                                            //120411
         (CS::Suite_directory->UED().spatial_interpolator_executable()
         , L"", false, SW_SHOWNORMAL);
         // do not wait for termination and visible
}
//______________________________________________________________________________
bool CS_Explorer_engine::launch_ClimGen()                                  const
{  // If the current selected file in the file list is a UED file.
   // setproviteprofile string the associated location file with the database filename
   // to the ued file
   bool parameterization_mode = false;
   if (!CORN::OS::file_system_engine.exists(recently_selected_DEN[identify_context()]))
       recently_selected_DEN[identify_context()].set_DEN
         (CORN::OS::File_name_concrete(get_active_directory() //180723 directory_name_qualified
            ,"unnamed","UED"));
   CORN::OS::File_name_concrete location_filename(recently_selected_DEN[identify_context()] ,Extension("loc"));
   if (recently_selected_DEN[identify_context()].is_extension_ASCII("UED"))
   {  parameterization_mode = CORN::OS::file_system_engine.exists(location_filename);
      WritePrivateProfileString("[location]","database", recently_selected_DEN[identify_context()].c_str() ,location_filename.c_str());
   }
   return run_program_with_primary_DEN
         (CS::Suite_directory->ClimGen().ClimGen_exe()
         ,location_filename
         ,L"" // location_filename.exists() ? (parameterization_mode ? "PARAMETERIZE" : "GENERATE") : "" // no options
         , false // do not wait for termination
         , SW_SHOWNORMAL);
         // do not wait for termination and visible
}
//______________________________________________________________________________
bool CS_Explorer_engine::launch_soil_convertor()    const
{
   bool result = false;
   if (CORN::OS::file_system_engine.exists(recently_selected_DEN[identify_context()])) //140311
      result = run_program_with_primary_DEN
         (CS::Suite_directory->CropSyst().soil_convertor_executable()
         ,recently_selected_DEN[identify_context()],L"",false,SW_SHOWNORMAL);
         // do not wait for termination and visible
   return result;
}
//______________________________________________________________________________
nat32 CS_Explorer_engine::run_potential_scenarios                                //180110
(
 const CORN::Unidirectional_list &pot_scenarios_qual
,bool                    runtime_graph
,bool                    parallel)
{  nat32 actual_run_count = 0;
   if (!preferred_model_executable_qual) return 0;
   CORN::Unidirectional_list ultimate_scenarios;
   FOR_EACH_IN(pot_scenario_qual,CORN::OS::Directory_name, pot_scenarios_qual,each_pot_scenario)
   {
      provide_contextualization().list_ultimate_scenarios
         (*pot_scenario_qual,ultimate_scenarios,CPF_percents_water);
   } FOR_EACH_END(each_pot_scenario)
   if (CPF_percents_water) // NYI  && preferred model is CropSyst_X
   {
      FOR_EACH_IN(ultimate_scenario,CORN::OS::Directory_name,ultimate_scenarios,each_ultimate)
      {
         actual_run_count += run_ultimate_scenario_with_CPF(*ultimate_scenario,parallel);
      } FOR_EACH_END(each_ultimate)
   } else
   {
         actual_run_count = ultimate_scenarios.count();
         nat32 queued_count = run_program_multiple_DENS
               (*preferred_model_executable_qual
               ,ultimate_scenarios
               , L"" // currently no options but eventually will have
               , parallel);
      // WARNING  should return the number of DENs to run
      // and the script filename for the Explorer to confirm
      // from the user to run all those
   }
   return actual_run_count;
}
//_2018-01-10___________________________________________________________________run_potential_scenarios/
bool CS_Explorer_engine::run_ultimate_scenario_with_CPF
(const CORN::OS::Directory_name &ultimate_scenario
,bool parallel)                                                            const
{
//   if (is_scenario_ultimate(pot_ultimate_scenario_dirname)) // actually I think caller has already checked
   {

//NYI     171122
// run the general scenario
// generate crop production function subscenarios each will have a fixed
//  management file created to
// the run each of the subscenarios.
// Perhaps run the collator
//    (which will use TDF file(s) defined specifically for CPF output analysis)


 /*180110
  This needs to be redone

  Run the primary simulation
  generate the CPF percents subscenarios
  find the percent scenario dirs and run (in parallel)

 */
   CORN::OS::File_name_concrete &scenario_run_executable                         //171121
      = *preferred_model_executable_qual;                                        //171121
   std::wstring scenario_run_options;                                            //150421_110106

      CORN::OS::File_name_concrete crop_production_function_irrigation_adjustments_filename
         (get_directory_name_qualified(),"irrigation_adjustments.dat");
      if (CORN::OS::file_system_engine.exists(crop_production_function_irrigation_adjustments_filename))             //150418
      {
         //171121 scenario_run_executable.set_DEN(CS::Suite_directory->CropSyst().CPF_runner_executable());


         // I Dont think I need runner any more,
         // just run the primary ultimate scenarios in sequence
         // need to wait for all of these to finish,
         // then can generate the CPF percent scenarios
         // which can be run in parallel.


         scenario_run_executable = CS::Suite_directory->CropSyst().CPF_runner_executable(); //171121
         crop_production_function_irrigation_adjustments_filename.append_components_to(scenario_run_options);
      } else if (CPF_percents_water)                                             //150418
      {
         //scenario_run_options.append(L"irrigation_adjustments_percent=");
         for (nat8 percent_index = 0; percent_index < 16; percent_index++)
         {
            //171121 scenario_run_executable.set_DEN(CS::Suite_directory->CropSyst().CPF_runner_executable());
            scenario_run_executable = CS::Suite_directory->CropSyst().CPF_runner_executable(); //171121
            if ((1 << percent_index) & CPF_percents_water)
            {
               nat8 percent = percent_index * 10;
               CORN::append_nat32_to_wstring(percent,scenario_run_options);
               scenario_run_options.append(L" ");
            }
         }
      }

/*180110 This needs to be redone

      command_history.
         add_wstring
         ((report_filename.length()) //171227 report_format_filename_optional                                        //131006
            ?run_program_with_primary_and_secondary_DEN
               (scenario_run_executable                                          //171121
               ,ultimate_scenario
               ,report_filename//171227 *report_format_filename_optional                                 //131006_130610
               ,scenario_run_options
               , !run_simulateously // don't wait for termination if simultaneous run
               , SW_SHOWNORMAL)
            : run_program_with_primary_DEN                                       //131006
               (scenario_run_executable                                          //171121
               ,ultimate_scenario
               ,scenario_run_options
               , !run_simulateously // don't wait for termination if simultaneous run
               , SW_SHOWNORMAL)
         );
*/
   }
   return true;
}
//_2018-01-10___________________________________________________________________
/*181215 obsolete just create a normal directory
CropSyst::Project_directory
*CS_Explorer_engine::provide_project_directory()
{  if (identify_context() == project_context)
   {  if (project_directory)
      {  // If we have already setup a project directory, check that it matches the current project directory
         // If it doesn't then we have entered a different project context,
         // and we will and a new project directory
         if (get_active_directory(). //180723 directory_name_qualified->
             compare_including(*project_directory)!= 0)
         {  delete project_directory;
            project_directory = 0;
         }
      }
      if (!project_directory)
      {  project_directory = new
            CropSyst::Project_directory(get_active_directory()
               ,CROPSYST_PROJECT) ;
      }
   }
   return project_directory;
}
*/
//______________________________________________________________________________
bool CS_Explorer_engine::launch_output_fast_graph()                        const
{
   CORN::OS::File_name_concrete daily_UED_filename(get_active_directory()
      ,"daily","UED"); //130123
   return run_program_with_primary_DEN
      (CS::Suite_directory->CropSyst().fastgraph_executable()
      ,daily_UED_filename
      , L""
      , false // don't wait_for_termination
      , SW_SHOWNORMAL);
}
//______________________________________________________________________________
bool CS_Explorer_engine::launch_output_schedule_viewer()                   const
{
   CORN::OS::File_name_concrete schedule_filename(get_active_directory(),"schedule","txt"); //130123
   return run_program_with_primary_DEN
      (CS::Suite_directory->CropSyst().schedule_viewer_executable()
      ,schedule_filename
      , L""
      , false // don't wait_for_termination
      , SW_SHOWNORMAL);
}
//______________________________________________________________________________
bool CS_Explorer_engine::launch_output_schedule_text_viewer()              const
{  CORN::OS::File_name_concrete schedule_filename(get_active_directory(),"schedule","txt");  //130123
   HANDLE_OR_ERROR handle_or_error;                                              //181030
   return CORN::OS::shell_graphical //181030 file_system_engine
      .view_file(schedule_filename,handle_or_error);                             //181030
}
//______________________________________________________________________________
bool CS_Explorer_engine::launch_output_harvest_report_viewer()             const
{  CORN::OS::File_name_concrete harvest_filename(get_active_directory(),"harvest","htm");  //130123
   HANDLE_OR_ERROR handle_or_error;                                              //181030
   return CORN::OS::shell_graphical// 181030 file_system_engine
      .view_file(harvest_filename,handle_or_error);                              //181030
}
//______________________________________________________________________________
bool CS_Explorer_engine::scenario_output_has_file(const char *filename_unqual)    const
{  CORN::OS::File_name_concrete output_filename(get_active_directory(),filename_unqual); //120518
   return CORN::OS::file_system_engine.exists(output_filename);
}
//____________________________________________________scenario_output_has_file_/
class Simulation_run_period_data_record
: public CORN::Data_record
{  // NOTE: This class is duplicated in CS_explorer_engine,  I only really need one

 public:  // "simulation" section
   CORN::Date_clad_32 starting_date;
   CORN::Date_clad_32 ending_date;
   CORN::OS::File_name_concrete output_options_filename_rel;
 public:  //
   inline Simulation_run_period_data_record()
      : CORN::Data_record("simulation")
      , starting_date((CORN::Year)2000,(CORN::DOY)1) //170826 ,D_YMD,D_YYYY|D_Mmm|D_lead_zero,'/')
      , ending_date  ((CORN::Year)2001,(CORN::DOY)1) //170826 ,D_YMD,D_YYYY|D_Mmm|D_lead_zero,'/')
   {}
public:  //
   inline virtual bool expect_structure(bool for_write)
   {  bool expected = CORN::Data_record::expect_structure(for_write);            //120314
      set_current_section(LABEL_simulation);
      expect_int32(LABEL_starting_date,starting_date.mod_date32());              //171117_170532
      expect_int32(LABEL_ending_date,ending_date.mod_date32());                  //171117_170532
      set_current_section("output");
      expect_file_name("options",output_options_filename_rel);
      structure_defined = true;                                                  //130123
      return expected;
   }
};
//______________________________________________________________________________
bool CS_Explorer_engine::output_export_UED_to_XLS_file_command_line
(const CORN::OS::File_name &XXXXX_ued
,const CORN::OS::File_name &XXXXX_xxx
,const CORN::OS::File_name &XXXXX_tdf
,bool include_full_year)                                                   const
{
   Simulation_run_period_data_record sum_run_period;
   CORN::OS::Directory_name_concrete scenario_dir
      (get_active_directory()
      ,CORN::OS::Directory_entry_name::include_ancestors_path_designated);       //161029
   CORN::OS::File_name_concrete scenario_filename(scenario_dir,".CropSyst_scenario");
   #if (CS_VERSION == 4)
   // We don't have a new scenario file, it may be and old version 4 scenario
   // so just use the backup file to get the path
   if (!CORN::OS::file_system_engine.exists(scenario_filename))
      scenario_filename.set_cstr("backup.csn");  // This is sort of a hack: Version 4 makes this backup copy of the scenario file, I am using this as a convenient way to find the parameter file
   #endif
   VV_File scenario_file(scenario_filename.c_str());
   scenario_file.get(sum_run_period);
   CORN::Date_clad_32 include_starting_date(sum_run_period.starting_date);
   if (include_full_year) include_starting_date.set_DOY(1);
   CORN::Date_clad_32 include_endinging_date(sum_run_period.ending_date);
   if (include_full_year) include_endinging_date.set_DOY(sum_run_period.ending_date.days_in_this_year());
   CORN::OS::File_name_concrete ued_filename(get_active_directory(),XXXXX_ued.w_str()); //130123
   CORN::OS::File_name_concrete xxx_filename(get_active_directory(),XXXXX_xxx.w_str()); //130123
   CORN::OS::File_name_concrete tdf_filename(get_active_directory(),XXXXX_tdf.w_str()); //130123
   if (xxx_filename.has_in_path_cstr("soil",CORN::OS::Directory_entry_name::include_qualification,false))
   {
      CORN::OS::Directory_name_concrete output_soil_dir(get_active_directory(),"soil",CORN::OS::directory_entry); //161029
      CORN::OS::file_system_engine.provide_directory(output_soil_dir);

      // NYI at this point we are in the output directory
      // CropSyst run does not create a context for the scenario output
      // but we could do it now if it doesn't already exist

   }
   #define USE_convert_text_dBase_Excel_with_TDF_executable
   #ifdef USE_convert_text_dBase_Excel_with_TDF_executable
// Eventually I will implement the Excel tabular file in the UED convertor
   UED::Convertor_arguments response;
   response.operation         ="export";
   response.UED_filename      .set_DEN(ued_filename);                            //130123
   response.station_IDs_filter="";
   response.use_station_name  = false; // default UED will match the location
   response.year_earliest     = include_starting_date.get_year();
   response.year_latest       = include_endinging_date.get_year();
   response.date_time_earliest= include_starting_date.get_datetime64();
   response.date_time_latest  = include_endinging_date.get_datetime64();
   // N/A   response.prefix            = cropsyst_prefix_edit->Text.c_str();
   //response.target_filename   .set_DEN(xxx_filename);                            //130123

   //CORN::wstring_to_string(xxx_filename,response.target_filename);               //190330

   response.target_filename.set_DEN(xxx_filename);                               //190806

   response.format_filename   .set_DEN(tdf_filename);                            //130123
   CORN::OS::File_name_concrete convert_rsp(Directory_name_concrete(),"convert.rsp");
   CORN::VV_File response_file(convert_rsp.c_str());
   response_file.set(response);
   return run_program_with_primary_DEN                                           //120412
      (CS::Suite_directory->UED().convert_text_dBase_Excel_with_TDF_executable() //120313
      ,convert_rsp
      ,L"" // options
      ,true // wait for termination
      ,SW_MINIMIZE);
   #else
   // 070621  This will eventually be replaced with UED with TDF convertor
   // but I haven't implemented the Excel tabular file
   if (ued_filename.exists()      && tdf_filename.exists())
   {  char date_num_buf[10];
      Ustring options("\"");
      options.append(xxx_filename);  // File name is quoted (I.e. xls file)
      options.append("\" ");
      if (xxx_filename.find("nnual") != SUBSTRING_NOT_FOUND)
      {  // if the output is annual and the starting data is jan 1
         // then hack the start date to be dec31 of previous year to get the effective initial condintions
         // in the annual reprochange this to
         if (include_starting_date.get_DOY() == 1)
            include_starting_date.dec();
      }
      options.append(ltoa(include_starting_date.get_date32(),date_num_buf,10));
      options.append(" ");
      options.append(ltoa(include_endinging_date.get_date32(),date_num_buf,10));
      return run_program_with_file_with_primary_and_secondary_file
      (CS::Suite_directory->UED().convert_export_executable()
      ,ued_filename
      ,tdf_filename
      ,options
      ,true // wait for termination
      ,SW_MINIMIZE);
   }
   return true;
   #endif
}
//______________________________________________________________________________
const CropSyst::Scenario_output_options* CS_Explorer_engine::provide_output_options() provision_
{  if (output_options) delete output_options;
   if (identify_context() == scenario_output_context)
   {  Simulation_run_period_data_record sum_run_period;
      CORN::OS::File_name_concrete scenario_filename(get_active_directory(),".CropSyst_scenario");
      VV_File   scenario_file(scenario_filename.c_str());
      scenario_file.get(sum_run_period);
      if (CORN::OS::file_system_engine.exists(sum_run_period.output_options_filename_rel))

         //130123 WARNING, may need to resolve output_options_filename_rel if it is relative

      {  output_options = new CropSyst::Scenario_output_options ;
         VV_File output_options_file(sum_run_period.output_options_filename_rel.c_str());
         output_options_file.get(*output_options);
      }
   }
   return output_options;
}
//______________________________________________________________________________
bool CS_Explorer_engine
::edit_file_if_known_type(const STRING_WITH_FILENAME &filename)            const
{  bool result = false;
   CORN::OS::File_name_concrete  file_name_to_edit(filename);
   command.clear();
   if      (file_name_to_edit.has_extension_ASCII(CS_scenario_EXT,true))       result = edit_parameter_file(filename,CS_scenario_EXT  ,context_strings[scenario_context]  .editor /*"scenario_editor"   */,0);
   else if (file_name_to_edit.has_extension_ASCII(CS_crop_EXT_V4,true))           result = edit_parameter_file(filename,CS_crop_EXT_V4      ,context_strings[crop_context]      .editor /*"crop_editor"       */,0);
   else if (file_name_to_edit.has_extension_ASCII(CS_soil_EXT_V4,true))           result = edit_parameter_file(filename,CS_soil_EXT_V4      ,context_strings[soil_context]      .editor /*"soil_editor"       */,0);
   else if (file_name_to_edit.has_extension_ASCII(CS_rotation_EXT_V4,true))       result = edit_parameter_file(filename,CS_rotation_EXT_V4  ,context_strings[rotation_context]  .editor /*"rotation_editor"   */,0);
   else if (file_name_to_edit.has_extension_ASCII(CS_management_EXT_V4,true))     result = edit_parameter_file(filename,CS_management_EXT_V4  ,context_strings[management_context].editor /*"management_editor" */,0);
   else if (file_name_to_edit.has_extension_ASCII("UED",false))                result = edit_parameter_file(filename,"UED"            ,context_strings[weather_context]   .editor /*"weather_editor"    */,0);
   else if (file_name_to_edit.has_extension_ASCII(CS_biomatter_EXT,true))      result = edit_parameter_file(filename,CS_biomatter_EXT ,context_strings[biomatter_context] .editor /*"biomatter_editor"  */,0);
#if (CS_VERSION==4)
   else if (file_name_to_edit.has_extension_ASCII("LOC",true))                 result = edit_parameter_file(filename,"LOC"            ,"CropSyst\\location_editor"   ,0);
   else if (file_name_to_edit.has_extension_ASCII("FMT",true))                 result = edit_parameter_file(filename,"FMT"            ,context_strings[format_context]    .editor /*"format_editor"     */,0);
   // In version 4, the following file types are text files so should be picked up as simply open
//NYI      :   (file_name_to_edit.has_extension_ASCII(CS_water_table_EXT))    ? edit_parameter_file(filename,CS_water_table_EXT,"xxxxx",0)
//NYI      :   (file_name_to_edit.has_extension_ASCII(CS_recalibration_EXT))     ? edit_parameter_file(filename,CS_recalibration_EXT,"xxxxx",0)
#endif
   else
   {
      std::string file_name; file_name_to_edit.append_components_to_string(file_name,CORN::OS::Directory_entry_name::include_name);     //130123
      std::string file_ext;
      CORN::wstring_to_string(file_name_to_edit.get_extension(),file_ext);       //140111
      bool has_filename = file_name.length();
      bool has_ext      = file_ext .length();
      for (int context = 0; context < CONTEXT_COUNT; context++)
      {  // For each context type, check if the filename matches the name or extension for the context
         bool valid_context_file_ext_or_name = context_strings[context].parameter_file_ext_or_name_V5;
         bool valid_context_ext              = context_strings[context].parameter_file_ext_V4 ;
         if (  (has_filename && valid_context_file_ext_or_name && (file_name == context_strings[context].parameter_file_ext_or_name_V5))
             ||(has_filename && valid_context_ext              && (file_name == context_strings[context].parameter_file_ext_V4        ))
             ||(has_ext      && valid_context_file_ext_or_name && (file_ext  == context_strings[context].parameter_file_ext_or_name_V5))
             ||(has_ext      && valid_context_ext              && (file_ext  == context_strings[context].parameter_file_ext_V4        )))
         {  return edit_parameter_file
               (filename
               ,context_strings[context].parameter_file_ext_V4
               ,context_strings[context].editor ,0);
         }
      }
   }
   if (!command.length()) // Not a CS_suite file
      result = Explorer_engine_abstract::edit_file_if_known_type(filename);
   return result;
}
//_edit_file_if_known_type_____________________________________________________/
bool CS_Explorer_engine::scenario_edit()                                   const
{
   CORN::OS::File_name_concrete scenario_file_name;                              //171228
   CORN::OS::File_name_concrete scenario_editor(CS::Suite_directory->CropSyst().scenario_editor_executable());
   bool is_version_5_CropSyst_scenario = true;
   {  // Support for version 5
      CORN::OS::File_name_concrete curr_scenario_filename(get_active_directory(),L".CropSyst_scenario"); //171228
      is_version_5_CropSyst_scenario = CORN::OS::file_system_engine.exists(curr_scenario_filename);
      if (is_version_5_CropSyst_scenario)
         scenario_file_name.set_DEN(curr_scenario_filename);
      if (!CORN::OS::file_system_engine.exists(scenario_editor))
           scenario_editor.set_DEN(CS::Suite_directory->CAFE().Dairy().field_editor_executable()); //140111
   }
   if (!is_version_5_CropSyst_scenario)
   {  // Support for version 4:
      CORN::OS::File_name_concrete scenario_file_name(L".CropSyst_scenario");
      CORN::OS::File_name_concrete csn_scenario_filename(get_active_directory(),L"csn"); //180723
      if (CORN::OS::file_system_engine.exists(csn_scenario_filename))
         scenario_file_name.set_DEN(csn_scenario_filename);
      else                                                                       //110106
      {  CORN::OS::File_name_concrete CAFE_dairy_scenario_filename(get_directory_name_qualified(),L".CAFE_dairy_scenario");
         if (CORN::OS::file_system_engine.exists(CAFE_dairy_scenario_filename))
         {
            scenario_file_name.set_DEN(CAFE_dairy_scenario_filename);
            scenario_editor.set_DEN(CS::Suite_directory->CAFE().Dairy().scenario_editor_executable());
   }  }  }
   return run_program_with_primary_DEN                                           //180128
      ( scenario_editor , scenario_file_name , L""  , false , SW_SHOWNORMAL);
}
//______________________________________________________________________________
bool CS_Explorer_engine::scenario_edit_model(nat8 model_index) const
{  bool result = false;                                                          //180128
   CORN::OS::File_name_concrete *single_scenario_file_name = 0;
   CORN::OS::File_name_concrete *scenario_editor = 0;
   switch (model_index)
   {
      case 0 : // CropSyst
         single_scenario_file_name = new CORN::OS::File_name_concrete(get_directory_name_qualified(),".CropSyst_scenario");
         scenario_editor = new CORN::OS::File_name_concrete(CS::Suite_directory->CropSyst().scenario_editor_executable());

         // When only NIFA (or CAFE or CANMS) is installed,
         // then we are probably in a field directory
         // use the simple field editor
         if (!CORN::OS::file_system_engine.exists(*scenario_editor))
         {  delete scenario_editor;
            scenario_editor = new CORN::OS::File_name_concrete
               (CS::Suite_directory->CAFE().Dairy().field_editor_executable());
         }
      break;
      case 1 : // NIFA dairy
         single_scenario_file_name = new CORN::OS::File_name_concrete
            (get_directory_name_qualified(),L".NIFA_dairy_scenario");
         scenario_editor = new CORN::OS::File_name_concrete(CS::Suite_directory->CAFE().Dairy().scenario_editor_executable());
         // CAFE and NIFA dairy editor are the same
         //NYI CS::Suite_directory->XXX.scenario_editor_executable());
      break;
      case 2 : // CAFE dairy
         single_scenario_file_name = new CORN::OS::File_name_concrete
            (get_directory_name_qualified(),L".CAFE_dairy_scenario");
         // CAFE and NIFA dairy editor are the same
         scenario_editor = new CORN::OS::File_name_concrete(CS::Suite_directory->CAFE().Dairy().scenario_editor_executable());
      break;
      case 3 : // CANMS
         single_scenario_file_name = new CORN::OS::File_name_concrete
            (get_directory_name_qualified(),L".CAFE_dairy_scenario");
         //NYI scenario_editor = new CORN::OS::File_name_concrete(CS::Suite_directory->CANMS .scenario_editor_executable());
      break;
   }
   if (   single_scenario_file_name
       && CORN::OS::file_system_engine.exists(*single_scenario_file_name))
   {  result =
      (run_program_with_primary_DEN
      ( *scenario_editor
      , *single_scenario_file_name
      , L""
      , false // wait_for_termination     // True if this editor is held until the other program exits
      , SW_SHOWNORMAL
      ));                                                                        //130123
   } // else caller setups for editing compositional scenario in nativation composition tab
   return result;
}
//_scenario_edit_model______________________________________________2017-12-28_/
bool CS_Explorer_engine::scenario_run_GIS()                                const
{  std::wstring no_options;
   run_program_with_primary_DEN                                                  //120411
         (CS::Suite_directory->CropSyst().GIS_runner_executable()
         ,get_active_directory() ,no_options , false , true );
      // do not wait for termination
   return true;
}
//_scenario_run_GIS_________________________________________________2014-02-14_/
bool CS_Explorer_engine
::get_context_icon_filename(CORN::OS::File_name &icon_filename,int &icon_index) const
{  bool icon_filename_available = true;
   icon_index = 0;
   switch (context_strings[identify_context()].mode)
   {  case 0 : icon_filename_available = false;  break; // no icon for arbitrary context
      case 1 : icon_filename_available =  false; break; // don't yet have UED icons directory
      case 2 :
      {  CORN::OS::File_name_concrete icon_fname(CS::Suite_directory->icons(),context_strings[identify_context()].icon,"ico");
         icon_filename.set_DEN(icon_fname);
      } break;
      case 3 :
      {  CORN::OS::File_name_concrete icon_fname(CS::Suite_directory->CropSyst(),context_strings[identify_context()].icon,"ico");
         icon_filename.set_DEN(icon_fname);
      } break;
   }
   if (icon_filename_available) // so far so good
       icon_filename_available = context_strings[identify_context()].icon != 0;
   return icon_filename_available;
}
//_get_context_icon_filename___________________________________________________/
bool CS_Explorer_engine
::get_context_description(std::string &description)                        const
{  description.assign(context_strings[identify_context()].description);
      ///< The derived class should override to set the brief one line description. (Currently this codes in the desktop infotip).
      ///  By default no description is assigned.
      ///\return false if this folder does not need or should not have a description.
   return true;
}
//_get_context_description_____________________________________________________/
/*190809 obsolete now using context definition
bool CS_Explorer_engine::get_specified_context
(modifiable_ std::string &context) const
{  nat16 context_index = identify_context();
   bool got = (bool)context_index;
   if (got)
      context.assign(context_strings[identify_context()].context);
   else
      got = provide_contextualization().get_specified_context(context);
   return got;
}
//_get_specified_context____________________________________________2017-12-31_/
*/
bool CS_Explorer_engine
::navigator_double_clicked_file(const STRING_WITH_FILENAME &item_path)     const
{  bool did_something = false;
   CORN::OS::File_name_concrete file_name_to_process(item_path);                 //130418
   if (edit_file_if_known_type(item_path)) did_something = true;
   else
   {
      HANDLE_OR_ERROR handle_or_error;                                           //181030
      int open_status = CORN::OS::shell_graphical //81030 file_system_engine
         .open_file(file_name_to_process,handle_or_error);                       //181030
#ifdef NYI
         switch (open_status < 32)
         {
/* NYI
ERROR_FILE_NOT_FOUND The specified file was not found.
ERROR_PATH_NOT_FOUND The specified path was not found.
ERROR_BAD_FORMAT The .exe file is invalid (non-Microsoft Win32 .exe or error in .exe image).
SE_ERR_ACCESSDENIED The operating system denied access to the specified file.
SE_ERR_ASSOCINCOMPLETE The file name association is incomplete or invalid.
SE_ERR_DDEBUSY The Dynamic Data Exchange (DDE) transaction could not be completed because other DDE transactions were being processed.
SE_ERR_DDEFAIL The DDE transaction failed.
SE_ERR_DDETIMEOUT The DDE transaction could not be completed because the request timed out.
SE_ERR_DLLNOTFOUND The specified DLL was not found.
SE_ERR_FNF The specified file was not found.
SE_ERR_NOASSOC There is no application associated with the given file name extension. This error will also be returned if you attempt to print a file that is not printable.
SE_ERR_OOM There was not enough memory to complete the operation.
SE_ERR_PNF The specified path was not found.
SE_ERR_SHARE A sharing violation occurred.
*/
            case 0: case default:
            {  // Invoke open failed
               // NYI
            }
            break;
         }
#endif
         did_something = open_status > 32;                                       //130416
         command.clear(); // invoke_open uses Windows API ShellExecute so the command is not available
         // Set explorer_engine recently selected file because it may be used
         // in the next invoked function.
         recently_selected_DEN[identify_context()].set_wstr(item_path.c_str());
   }
   return did_something;
}
//_navigator_double_clicked_file_______________________________________________/
bool CS_Explorer_engine
::navigator_single_clicked_file(const STRING_WITH_FILENAME &item_path)     const
{
   // Set explorer_engine recently selected file because it may be used
   // in the next invoked function.
   nat8 identified_context = identify_context();
   if (identified_context < CONTEXT_COUNT)
      recently_selected_DEN[identified_context].set_wstr(item_path.c_str());
   return true;
}
//_navigator_single_clicked_file_______________________________________________/
bool CS_Explorer_engine::CANMS_scenario_edit()      const
{
   CORN::OS::File_name_concrete *scenario_file_name_ptr= new CORN::OS::File_name_concrete(get_directory_name_qualified(),L".CANMS_scenario");
   if (!CORN::OS::file_system_engine.exists(*scenario_file_name_ptr))
   {  // this is a version 4 scenario
      delete scenario_file_name_ptr;
      scenario_file_name_ptr = new  CORN::OS::File_name_concrete(L".CANMS_scenario");
   } //else  this is a version 5 scenario
   CORN::OS::File_name_concrete &scenario_file_name = *scenario_file_name_ptr;
   CORN::OS::File_name_concrete scenario_editor(CS::Suite_directory->CropSyst().scenario_editor_executable());
   return run_program_with_primary_DEN
      ( scenario_editor ,  scenario_file_name , L"" , false , SW_SHOWNORMAL );
}
//_CANMS_scenario_edit_________________________________________________________/
Directory_context CS_Explorer_engine
::assume_context(Directory_context assumed_context,bool permanently) modification_
{  identified_context = assumed_context;
   return identified_context;
}
//_assume_context______________________________________________________________/
nat8 CS_Explorer_engine::get_model_version()                               const
{  return (preferred_model_executable_qual
      && (preferred_model_executable_qual->get_name() == L"CropSyst_4"))
      ? 4: 5;
}
//_get_model_version________________________________________________2018-03-26_/
bool CS_Explorer_engine::map_UED_locations()                               const
{  bool result = run_program_with_primary_DEN                                    //180128
      ( CS::Suite_directory->UED().map_google_executable()
      , get_active_directory() , L"" , false , SW_SHOWNORMAL);
   CORN::OS::File_name_concrete location_map(get_active_directory(),L"locations_map.htm"); // 180723
   HANDLE_OR_ERROR handle_or_error;                                              //181030
   result &= CORN::OS::shell_graphical //181030file_system_engine
      .view_file(location_map,handle_or_error);                                  //181030_180128
   return result;
}
//_map_UED_locations___________________________________________________________/
bool CS_Explorer_engine::launch_notepad()                                  const
{  return
      CORN::OS::file_system_engine.exists(recently_selected_DEN[identified_context])
      ?  run_program_with_primary_DEN
         (CORN::OS::File_name_concrete("notepad.exe")
         , recently_selected_DEN[identified_context] , L"" , false , true )
      : Explorer_engine_abstract::launch_notepad();
}
//_launch_notepad______________________________________________________________/
bool CS_Explorer_engine::launch_UED_condense()                             const
{  return
      (CORN::OS::file_system_engine.exists(recently_selected_DEN[identified_context])
       && recently_selected_DEN[identified_context].has_extension_ASCII("UED",false)) // Only condense UED filenames
      ?  run_program_with_primary_DEN                                            //120516
         (CS::Suite_directory->UED().condense_executable()
         , recently_selected_DEN[identified_context] , L"" , false , true )
      : false;
}
//_UED_condense________________________________________________________________/
bool CS_Explorer_engine::launch_CropSyst_wizard()                          const
{  run_program(CS::Suite_directory->CropSyst().CropSyst_wizard_executable(),L"",false,true);
   return true;
}
//_launch_CropSyst_wizard___________________________________________2014-10-22_/
bool CS_Explorer_engine::update_application_suite()                        const
{  return (int)ShellExecuteW(0,L"runas",CS::Suite_directory->install_or_update().w_str(),NULL,NULL,SW_SHOW)>32;
}
//_update_application_suite_________________________________________2012-01-12_/
bool CS_Explorer_engine::translate_AgMIP_ACE     // actually this could be renamed to indicate any translator program
(const CORN::OS::File_name &translator_exe
,const CORN::OS::File_name &JSON_filename)                                 const
{
   return run_program_with_primary_DEN
      ( translator_exe, JSON_filename
      , L"" , true  , SW_SHOWNORMAL);
}
//_translate_AgMIP_ACE______________________________________________2013-04-18_/
nat16 CS_Explorer_engine::translate_AgMIP_ACE_JSON_files
(const CORN::Text_list &selected_JSON_filenames_presumed)                  const
{  nat16 translated = 0;
   bool ACE_to_CropSyst_translator_installed = true;
   CORN::OS::File_name *ACE_to_CropSyst_translator =
      (CS::Suite_directory->find_file_name
         (L"ACE_to_CropSyst.exe" ,CORN::OS::File_system::subdirectory_recursion_exclusive));  //161003
   if (ACE_to_CropSyst_translator_installed)
   FOR_EACH_IN(selected_filename,Item_wstring ,selected_JSON_filenames_presumed,each_JSON_file)
   {  CORN::OS::File_name_concrete selected_filename_instance(get_directory_name_qualified(),*selected_filename); //130513
      if (selected_filename_instance.has_extension_ASCII("json",false))
         translated += translate_AgMIP_ACE(*ACE_to_CropSyst_translator,selected_filename_instance);
   } FOR_EACH_END(each_JSON_file)
   return translated;
}
//_translate_AgMIP_ACE_JSON_files___________________________________2013-04-18_/
int CS_Explorer_engine::export_scenarios
(const CropSyst::Scenario_export_engine::Arguments &export_args
,nat8 format_index)
{  CORN::OS::File_name *exporter_exe =
      (CS::Suite_directory->find_file_name
         ((format_index == 1) ? L"CropSyst_to_ACMO.exe" : L"CropSyst_to_ACE.exe"
         ,true));
   // NYI should append command to command history
   if (exporter_exe)                                                             //150503
      run_program                                                                //120411
      ( *exporter_exe , export_args.compose() , false , true);
   delete exporter_exe;
   return 1;
}
//_export_scenarios_________________________________________________2013-01-11_/
nat32 CS_Explorer_engine::UED_file_quality_import_or_export
(const std::string &command
,const CORN::Text_list  &selected_UED_filenames_qual_or_unqual)
{  nat32 files_processed=0;
   FOR_EACH_IN(UED_qual_or_unqual,Item_wstring,selected_UED_filenames_qual_or_unqual,each_UED) //180115
   {
      // Note that selected_UED_filenames_qual_or_unqual could be any file
      // that happens to be in the weather folder.
      // In most situations these will be UED files
      // but the user may have put arbitrary files in this directory.
      // so we filter only UED files
      CORN::OS::File_name_concrete *UED_qual = new CORN::OS::File_name_concrete(*UED_qual_or_unqual);
      if (!UED_qual->is_qualified(false))                                        //120212
      {  delete UED_qual;
         UED_qual = new CORN::OS::File_name_concrete(get_active_directory(),*UED_qual_or_unqual); //180723
      }
      // UED filename  should now be qualified.
      if (UED_qual->has_extension_ASCII("UED",false))
      {  CORN::OS::File_name_concrete UED_quality_filename(*UED_qual,CORN::OS::Extension("UED_quality"));
         std::wstring args = L"operation=";                                      //171120
         CORN::append_string_to_wstring(command,args);
         CORN::append_string_to_wstring(" UED_filename=\"",args);
         UED_qual->append_to_wstring(args);
         CORN::append_ASCIIZ_to_wstring("\" target_filename=\"",args);
         UED_quality_filename.append_to_wstring(args);
         CORN::append_ASCIIZ_to_wstring("\"",args);
         files_processed += run_program
            (CS::Suite_directory->UED().quality_import_export_executable()
            ,args, true,SW_SHOWNORMAL);
      }
      delete UED_qual;
   } FOR_EACH_END(each_UED)
   return files_processed;
}
//_UED_file_quality_import_or_export________________________________2014-06-29_/
bool CS_Explorer_engine::generate_scenarios
(const CORN::Text_list &selected_scenario_filenames_unqual)                const
{  bool generated = true;
   std::wstring args;
   FOR_EACH_IN(dir_name,CORN::Item_wstring,selected_scenario_filenames_unqual,each_scenario)
   {  args.append(L" --template=");
      args.append((*dir_name));
   } FOR_EACH_END(each_scenario)
   CORN::OS::File_name *generator_exe =
      (CS::Suite_directory->find_file_name
         (L"generator.exe"
         ,CORN::OS::File_system::subdirectory_recursion_exclusive));
   // NYI should append command to command history
   if (generator_exe)                                                            //150503
   {  generated =
      run_program
      ( *generator_exe, args, true, true);                                       //120411
   }
   delete generator_exe;
   return generated;
}
//_generate_scenarios_______________________________________________2016-11-03_/
bool CS_Explorer_engine::netCDF_to_UED
(const std::wstring &geocoordinates_filename
,const std::wstring &bounding_box_coord1
,const std::wstring &bounding_box_coord2
,const std::wstring &cell_size_and_units
,const CORN::Date &date_from
,const CORN::Date &date_to)
{
   std::wstring args;
   args + geocoordinates_filename;                                args += L" ";
   args += bounding_box_coord1;                                   args += L" ";
   args += bounding_box_coord2;                                   args += L" ";
   args += cell_size_and_units;                                   args += L" ";
   CORN::append_int32_to_wstring(date_from.get_date32(),args);    args += L" ";
   CORN::append_int32_to_wstring(date_to  .get_date32(),args);    args += L" ";
   CORN::OS::File_name *netCDF_to_UED_exe = CS::Suite_directory->find_file_name
      (L"netCDF_to_UED"
      ,CORN::OS::File_system::subdirectory_recursion_exclusive);
   return (netCDF_to_UED_exe)
      ? run_program(*netCDF_to_UED_exe,args,false,true)
      : false;
}
//_netCDF_to_UED____________________________________________________2016-11-13_/
bool CS_Explorer_engine::prefer_model_executable
(const wchar_t *model_executable_qual)                            rectification_
{  delete preferred_model_executable_qual;
   preferred_model_executable_qual
      = new CORN::OS::File_name_concrete(model_executable_qual);
   return true;
}
//_preferred_model_executable_qual__________________________________2017-11-21_/
nat16 CS_Explorer_engine::compare_directory
(const CORN::OS::Directory_name &source_dir
,const CORN::OS::Directory_name &other_dir
,const CORN::OS::Directory_name &compare_dir)                              const
{  nat16 compared_UED_file_count = 0;
   CORN::Text_list source_filenames_unqual;
   CORN::Text_list source_dirnames_unqual;
   nat32 DEN_count = file_system_engine.list_names_in
      ( source_dir
      ,&source_filenames_unqual
      ,&source_dirnames_unqual);
   if (!DEN_count) return 0;
   CORN::OS::file_system_engine.provide_directory(compare_dir);
   FOR_EACH_IN(filename_unqual,std::wstring,source_filenames_unqual,each_file)
   {  CORN::OS::File_name *other_filename = CORN::OS::file_system_engine.find_DEN
         (*filename_unqual,CORN::OS::file_entry,other_dir
         ,CORN::OS::File_system::none_recursion_inclusive);
      if (other_filename && (other_filename->has_extension_ASCII("UED",true))) //case insensitive
      {  CORN::OS::File_name_concrete source_filename (source_dir ,(*filename_unqual));
         CORN::OS::File_name_concrete compare_filename(compare_dir,(*filename_unqual));
         const CORN::OS::File_name &UED_comparer = CS::Suite_directory->UED()./*UED_*/compare_executable();
         std::wstring options(L"\"");
         source_filename.append_components_to(options);
         options.append(L"\" \"");
         other_filename->append_components_to(options);
         options.append(L"\" \"");
         compare_filename.append_components_to(options);
         options.append(L"\"");
         run_program(UED_comparer,options,false,true); // dont wait but show window
         compared_UED_file_count += 1; // eventually increment if run_program success
      }
      delete other_filename;
   } FOR_EACH_END(each_file)
   FOR_EACH_IN(dirname_unqual,std::wstring,source_dirnames_unqual,each_dir)
   {
      CORN::OS::Directory_name_concrete source_sub (source_dir ,*dirname_unqual);
      CORN::OS::Directory_name_concrete other_sub  (other_dir  ,*dirname_unqual);
      if (CORN::OS::file_system_engine.exists(other_sub))
      {
         CORN::OS::Directory_name_concrete compare_sub(compare_dir,*dirname_unqual);
         CORN::OS::file_system_engine.provide_directory(compare_sub);            //180321
         compared_UED_file_count += compare_directory(source_sub,other_sub,compare_sub);
      }
   } FOR_EACH_END(each_dir)
   return compared_UED_file_count;
}
//_compare_directory________________________________________________2018-01-27_/
nat16 CS_Explorer_engine::compare_multiple_directories
(const CORN::OS::Directory_name &primary_dir
,const CORN::Container &other_dirnames)                                    const
{  nat16 compared_dir_count = 0;
   // Put the comparison results directory in
   CORN::OS::Directory_name_concrete comparison_dir
      (get_directory_name_qualified(),"Comparison");
   FOR_EACH_IN(other_dir,CORN::OS::Directory_name,other_dirnames,each_other)
   {  std::wstring diff_dir_unqual;
      primary_dir.append_name_with_extension_to(diff_dir_unqual);
      diff_dir_unqual.append(L"-");
      std::wstring other_dir_unqual;
      other_dir->append_name_with_extension_to(other_dir_unqual);
      diff_dir_unqual.append(other_dir_unqual);
      CORN::OS::Directory_name_concrete diff_dir(comparison_dir,diff_dir_unqual);
      // Check if compare_directory creates diff_dir
      compared_dir_count += compare_directory
         (primary_dir,(*other_dir),diff_dir);
   } FOR_EACH_END(each_other)
   return compared_dir_count;
}
//_compare_multiple_directories_____________________________________2018-01-28_/
nat16 CS_Explorer_engine::instantiate_daily_output_formats()               const
{  nat16 instantiate_count = 0;
   CORN::Unidirectional_list format_files;
   CORN::OS::Directory_name_concrete CS_Database_Format
      (CS::Suite_directory->Database(),"Format");
   CORN::Inclusion daily_TDFs("daily.xls.TDF",true);                             //180117
   //NYI daily_TDFs.include_pattern("*daily*.TDF")
   CORN::OS::file_system_engine.list_DENs
      ( CS_Database_Format // may want to put this under Database
      ,&format_files,0,daily_TDFs
      ,CORN::OS::File_system::subdirectory_recursion_inclusive);
   FOR_EACH_IN(format_filename,CORN::OS::File_name,format_files,each_format)
   {  std::wstring format_name_with_ext;;                                        //180125
      CORN::OS::File_name_concrete target
         (get_directory_name_qualified(),format_filename->
            get_name_with_extension(format_name_with_ext));                      //180125
      instantiate_count += instantiate_DEN(*format_filename,target);
      // instantiate_count += instanciate_wstr(format_filename->w_str(),target.w_str());
   } FOR_EACH_END(each_format)
   return instantiate_count;
}
//_instantiate_daily_output_formats_________________________________2017-11-24_/
bool CS_Explorer_engine::is_installed_CANMS()                       affirmation_
{
   return CORN::OS::file_system_engine.exists(CS::Suite_directory->CANMS());
}
//_is_installed_CANMS_______________________________________________2017-12-06_/
bool CS_Explorer_engine::BlackBoard_promotion()                    modification_
{
   CORN::OS::Directory_name_concrete blackboard_dir
      (get_directory_name_qualified(),"BlackBoard");
   CORN::OS::file_system_engine.provide_directory(blackboard_dir);               //180321
   return true;
}
//_BlackBoard_promotion_____________________________________________2018-01-01_/
bool CS_Explorer_engine::output_generate_report
(const char *format_TDF_raw    // I.e. "annual.xls.TDF"
,const char *pattern_UED   // I.e. "*@annual$d.UED" "*@seasonal$d.UED"
,const char *report_xls)   // I.e. "annual.xls"
const
{
   //CORN::OS::File_name_concrete format_TDF(
   //operation=export format_filename="C:\CS_Suite_5\Database\Format\annual_Excel.TDF" UED_filenames="G:\FVSC\potato\Simulation\Scenarios\test_roger\Historical\CO2_360\CA\35.06N118.90W\Output\0\land_unit@annual$d.UED","G:\FVSC\potato\Simulation\Scenarios\test_roger\Historical\CO2_360\CA\35.06N118.90W\Output\0\soil\soil@annual$d.UED","G:\FVSC\potato\Simulation\Scenarios\test_roger\Historical\CO2_360\CA\35.06N118.90W\Output\0\SOM\organic_matter@annual$d.UED","G:\FVSC\potato\Simulation\Scenarios\test_roger\Historical\CO2_360\CA\35.06N118.90W\Output\meteorology\meteorology@annual$d.UED","G:\FVSC\potato\Simulation\Scenarios\test_roger\Historical\CO2_360\CA\35.06N118.90W\Output\0\annual.UED"  target_filename="annual_test.xls"
   std::wstring gen_options;
   gen_options.append(L"operation=export format_filename=\"");                   //171120
   // For V5 CS_Suite_5/Database/

   std::wstring format_TDF_wide;                                                 //181029
   CORN::append_ASCIIZ_to_wstring(format_TDF_raw,format_TDF_wide);               //181029
   // Searching for really needed now because the user can put custom report formats in the context.
   const CORN::OS::File_name *XXX_Excel_TDF_filename = 0;                        //180303
   XXX_Excel_TDF_filename
      =CORN::OS::file_system_engine.find_file_name
      (format_TDF_wide //181029 // format_TDF L"daily.xls.TDF" //181025 L"daily_Excel.TDF"
      ,/*explorer_engine->*/get_directory_name_qualified()
      ,CORN::OS::File_system::superdirectory_recursion_inclusive);
   if (!XXX_Excel_TDF_filename)                                                  //180303

   {
      CORN::Unidirectional_list format_files;
      CORN::OS::Directory_name_concrete CS_Database_Format
         (CS::Suite_directory->Database(),"Format");

      /*CORN::OS::File_name **/XXX_Excel_TDF_filename
      =CORN::OS::file_system_engine.find_file_name
      (format_TDF_wide //181025 L"daily_Excel.TDF"
      ,CS_Database_Format
      ,CORN::OS::File_system::subdirectory_recursion_inclusive);
   }
   CS::Suite_directory->Database().append_components_to(gen_options);
   gen_options += DEFAULT_DIRECTORY_SEPARATORw_chr;
   gen_options.append(L"Format");
   gen_options += DEFAULT_DIRECTORY_SEPARATORw_chr;
   CORN::append_ASCIIZ_to_wstring(format_TDF_raw, gen_options);
   gen_options += L'"';

   /*This was for V4
   gen_command.append("C:\\Simulation\\Database\\Format\\all\\annual.tdf");
   gen_command.append("\" \"UED_filename=");
   CORN::OS::File_name_concrete annual_ued(explorer_engine->get_directory_name_qualified(),"annual.ued");
   gen_command.append(annual_ued.c_str());
   gen_command.append("\" ");  */
   // Eventually search for the .TDF file
   CORN::Unidirectional_list UED_filenames;
   CORN::OS::file_system_engine.list_DENs
      (get_directory_name_qualified(),&UED_filenames,0
      ,CORN::Inclusion(pattern_UED,true)
      ,CORN::OS::File_system::subdirectory_recursion_inclusive);
   gen_options.append( L" UED_filenames=");
   bool first= true;
   FOR_EACH_IN(XXX_UED,CORN::OS::File_name,UED_filenames,each_UED)
   {  if (!first) gen_options += L',';
      gen_options += L'"';
      XXX_UED->append_components_to(gen_options);
      gen_options += L'"';
      first = false;
   } FOR_EACH_END(each_UED)
   gen_options.append(L" target_filename=\"");
   CORN::OS::File_name_concrete xxxx_xls(get_directory_name_qualified(),report_xls);
   gen_options.append(xxxx_xls.w_str());
   gen_options += L'"';
   //NYI command_history.add_string
   run_program
      ( CS::Suite_directory->UED().convert_text_dBase_Excel_with_TDF_executable()
      , gen_options, false, true);
   // use run_program instead of WinExec
   // WinExec(gen_command.c_str(),SW_NORMAL);
   return true;
}
//_output_generate_report___________________________________________2018-01-07_/
bool CS_Explorer_engine::output_generate_annual_report()                  const
{  return output_generate_report
      ("annual.xls.TDF"
      ,"*annual*.UED" // ,"*@annual$d.UED"
      ,"annual.xls");
}
//_output_generate_annual_report__2018-01-07____________________________________/
bool CS_Explorer_engine::output_generate_season_report()                   const
{  return output_generate_report
      ("season.xls.TDF"/*"season_Excel.TDF"*/,"*season*.UED","season.xls");
      //"*@seasonal$d.UED"
}
//_output_generate_season_report____________________________________2018-01-07_/
bool CS_Explorer_engine::output_generate_daily_report()                    const
{
   return output_generate_report("daily.xls.TDF","*daily*.UED","daily.xls");

/*
   std::string gen_command("\"");
   gen_command.append
      (CS::Suite_directory->UED().convert_text_dBase_Excel_with_TDF_executable().c_str()); //131006
   gen_command.append("\" \"operation=export\" \"format_filename=");             //171120


   //NYN CORN::Inclusion daily_Excel_TDF_pattern("daily_Excel.TDF",true);              //180304
   const CORN::OS::File_name *daily_Excel_TDF_filename                           //180303
      =CORN::OS::file_system_engine.find_file_name
      (L"daily.xls.TDF" //181025 L"daily_Excel.TDF"
      ,explorer_engine->get_directory_name_qualified()
      ,CORN::OS::File_system::superdirectory_recursion_inclusive);
*/
/*
   if (!daily_Excel_TDF_filename)                                                //180303
   {
      CORN::Unidirectional_list format_files;
      CORN::OS::Directory_name_concrete CS_Database_Format
         (CS::Suite_directory->Database(),"Format");

      CORN::OS::File_name *daily_Excel_TDF_filename
      =CORN::OS::file_system_engine.find_file_name
      (L"daily.xls.TDF" //181025 L"daily_Excel.TDF"
      ,CS_Database_Format
      ,CORN::OS::File_system::subdirectory_recursion_inclusive);
   }
*/
/*

   if (!daily_Excel_TDF_filename) return;                                        //180303
   daily_Excel_TDF_filename->append_to_string(gen_command);                      //180303

   // Eventually search for the .TDF file
   gen_command.append("\" \"UED_filename=");
   CORN::OS::File_name_concrete daily_ued(explorer_engine->get_directory_name_qualified(),"daily.ued");
   if (CORN::OS::file_system_engine.exists(daily_ued))
   {
      CORN::OS::File_name_concrete daily_xls(explorer_engine->get_directory_name_qualified(),"daily.xls");
      gen_command.append(daily_ued.c_str());
      gen_command.append("\" \"target_filename=");
      gen_command.append(daily_xls.c_str());
      gen_command.append("\"");
   } else
   {
      Application->MessageBox
      (_TEXT("Either CropSyst_4 was run with no daily output, or CropSyst_5 currently has no daily output formats.")
      ,_TEXT("No daily output available")
      ,IDOK);

      // CONTINUE here
      //find all daily *@daily$d.UED files
      //for each file export/collate

   }
   delete daily_Excel_TDF_filename;
   WinExec(gen_command.c_str(),SW_NORMAL);
*/
}
//_output_generate_daily_report_______________________________2018-10-29__2015_/
CORN::OS::File_name *
CS_Explorer_engine::output_generate_custom_report
(const std::string &TDF_filename
,const std::string &target_filetype
,const std::string &target_interface
)                                          const
{
   CORN::OS::File_name_concrete *target_filename = 0;
   CORN::OS::File_name_concrete selected_report_filename
      (TDF_filename);
   std::wstring selected_report_TDF_name(selected_report_filename.get_name());
   std::string  xxxxx_UED;
   if (selected_report_TDF_name     .find(L"daily") != std::string::npos)
   {  xxxxx_UED = "daily.UED"; }
   else if (selected_report_TDF_name.find(L"season") != std::string::npos)
   {  xxxxx_UED = "season.UED"; }
   else if (selected_report_TDF_name.find(L"annual") != std::string::npos)
   {  xxxxx_UED = "annual.UED"; }
   CORN::OS::File_name_concrete UED_filename(get_active_directory(),xxxxx_UED);
   if (!CORN::OS::file_system_engine.exists(UED_filename)) return false;

      //not needed bool Excel_conversion = selected_report_TDF_name.find("xls") != std::string::npos;
      //   if (Excel_conversion)

      std::string gen_command("\"");

      bool old_excel_TDF
         = selected_report_TDF_name.find(L"xls") != std::string::npos;
      // Previously I was going to have target format specific TDF files
      // I.e. daily.xls.TDF
      // and used the  old  text_dBase_Excel_with_TDF
      // Now, I am using the collator which can use any TDF and export
      // to a selected format
      if (old_excel_TDF)
      {
         gen_command.append(CS::Suite_directory->UED()
            .convert_text_dBase_Excel_with_TDF_executable().c_str());            //131006
         gen_command.append("\" operation=export format_filename=\"");           //171120
         selected_report_filename.append_to_string(gen_command);
         gen_command.append("\" UED_filename=\"");
         gen_command.append(xxxxx_UED); gen_command.append("\"");
      }
      else
      {  // Now the user select the target filetype (and interface)              //190213
         gen_command.append
            (CS::Suite_directory->UED().UED_collate_executable().c_str());
         gen_command.append("\"");
         gen_command.append(" --filetype=");
         gen_command.append(target_filetype);
         gen_command.append(" --interface=");
         gen_command.append(target_interface);
         gen_command.append(" format_filename=\"");           //171120
         selected_report_filename.append_to_string(gen_command);
         gen_command.append("\"");
         gen_command.append(" \""); gen_command.append(xxxxx_UED); gen_command.append("\"");

         std::wstring target_extension = L"dat"; // default                         //190330
         if (target_filetype=="Excel")                                              //190330
            target_extension = L"xls";                                              //190330

         target_filename = new CORN::OS::File_name_concrete
            (get_active_directory()                                                 //190330
            ,selected_report_filename.get_name()
            ,CORN::OS::Extension(target_extension));                                //190330

// --verbose=127 --filetype=Excel --interface=Hoerl format_filename="N:\temp\WW\Scenarios\30year\carbon_annual.tdf" "annual.UED" target_filename="annual_carbon.xls" --captions: none
      }

   // CORN::OS::Extension target_extension(Excel_conversion,"xls","dat");

   // TDF files will have the name of the form  {UED_name}.{format_extension}.TDF
   // where UED_name is UED file name without extension and format_extension
   // would be: dat xls dbf
   // so when we get_name, we get {UED_name}.{format_extension}

      gen_command.append(" target_filename=\"");
      target_filename->append_to_string(gen_command);
      gen_command.append("\" ");
      WinExec(gen_command.c_str(),SW_NORMAL);
//   target_filename.append_to_string(generated_target_return);
   return target_filename;
}
//__output_generate_custom_report___________________________________2018-10-18_/
bool CS_Explorer_engine::UED_dump_selected_UED
(bool open_output                                                                //181030
,HANDLE_OR_ERROR &handle_or_error)                                         const //181030
{  nat8 identified_context = identify_context();
   const CORN::OS::Directory_entry_name_concrete &recently_selected_UED_potential
      = recently_selected_DEN[identified_context];
   bool UED_file_selected = recently_selected_UED_potential.has_extension(L"UED");
   bool dumped = false;
   if (UED_file_selected)
      dumped = run_program_with_primary_DEN
         (CS::Suite_directory->UED().dump_executable()
         ,recently_selected_UED_potential,L"",false,true);
   else
   {
/*
      continue here dump all UED files found
      note that by using the new runner, in will present a list
      of UED files that can be selected.
      //200225 handle_or_error = (HANDLE_OR_ERROR)ERROR_FILE_NOT_FOUND;
*/
   }
   if (open_output && dumped)                                                    //181030
   {  CORN::OS::File_name_concrete yaml_filename
         (recently_selected_UED_potential,CORN::OS::Extension("yaml"));
      if (CORN::OS::file_system_engine.exists(yaml_filename))
         CORN::OS::shell_graphical.view_file(yaml_filename,handle_or_error);
   }
   return dumped;
}
//_UED_dump_selected_UED____________________________________________2018-10-18_/
nat32 CS_Explorer_engine::render_relevent_composition
(CORN::Unidirectional_list &relevent_composition)                     rendition_
{  nat32 rendered_count = 0;
   CORN::Inclusion *relevent_filename_patterns = 0;
   switch (get_identified_context())
   {  case scenario_context :
      {  relevent_filename_patterns = new CORN::Inclusion("*.CS_control",true);
         relevent_filename_patterns->include_pattern("*.CropSyst_scenario");
         relevent_filename_patterns->include_pattern(".CropSyst_scenario");
         relevent_filename_patterns->include_pattern(".CropSyst_scenario");
         relevent_filename_patterns->include_pattern("*/");
         /* Not sure if I want to include geolocation/geocooridanate
            and parameter files.
         */
      }
      break;
      /* NYI
      Eventually all parameter files will be composable especially crop,
      but I havent yet started using composposition for parameter files
      other than scenarios
      */
      /*
      default :
         find all accessible files that have extension associated
         with this directory context type
         (this should handle most parameter files).
      break;
      */
   }
   if (relevent_filename_patterns)
   {  rendered_count
         =  CORN::OS::file_system_engine.list_DENs
            (get_directory_name_qualified(),&relevent_composition,0 // ignore dirs
            ,*relevent_filename_patterns
            ,CORN::OS::File_system::superdirectory_recursion_inclusive);
            // eventually might want to get the primary project as the logical root
      delete relevent_filename_patterns;
   }
   return rendered_count;
}
//_render_relevent_composition______________________________________2018-01-18_/
bool CS_Explorer_engine::ultimate_Output_backup()
{
   // Copies the Output directory  to {date}.Output
   CORN::Date_time_clad_64 now;
   std::wstring now_str; now.append_to_wstring(now_str);
   // The standard time includes : but these are not valid for DEN
   std::replace(now_str.begin(),now_str.end(),':','_');
   CORN::OS::Directory_name_concrete source_dir(get_directory_name_qualified(),"Output");
   CORN::OS::Directory_name_concrete target_dir(get_directory_name_qualified(),now_str,"Output");
   return CORN::OS::file_system_engine
      .copy_directory(source_dir,target_dir,true);
}
//_ultimate_Output_backup___________________________________________2018-04-06_/

/* info on compressing and uncompressing folders (ZIP)

https://stackoverflow.com/questions/118547/creating-a-zip-file-on-windows-xp-2003-in-c-c/121720#121720
https://stackoverflow.com/questions/17546016/how-can-you-zip-or-unzip-from-the-script-using-only-windows-built-in-capabiliti
https://stackoverflow.com/questions/17546016/how-can-you-zip-or-unzip-from-the-script-using-only-windows-built-in-capabiliti/26843122#26843122
https://stackoverflow.com/questions/16458730/powershell-move-files-to-zip-with-system-io-compression-filesystem

____________________________________________________________
VB:

'Get command-line arguments.
Set objArgs = WScript.Arguments
Set FS = CreateObject("Scripting.FileSystemObject")
InputFolder = FS.GetAbsolutePathName(objArgs(0))
ZipFile = FS.GetAbsolutePathName(objArgs(1))

'Create empty ZIP file.
CreateObject("Scripting.FileSystemObject").CreateTextFile(ZipFile, True).Write "PK" & Chr(5) & Chr(6) & String(18, vbNullChar)

Set objShell = CreateObject("Shell.Application")

Set source = objShell.NameSpace(InputFolder).Items

objShell.NameSpace(ZipFile).CopyHere(source)

'Required to let the zip command execute
'If this script randomly fails or the zip file is not complete,
'just increase to more than 2 seconds
wScript.Sleep 2000
________________________________________________
Powershell
########################################################
# out-zip.ps1
#
# Usage:
#    To zip up some files:
#       ls c:\source\*.txt | out-zip c:\target\archive.zip $_
#
#    To zip up a folder:
#       gi c:\source | out-zip c:\target\archive.zip $_
########################################################

$path = $args[0]
$files = $input

if (-not $path.EndsWith('.zip')) {$path += '.zip'}

if (-not (test-path $path)) {
  set-content $path ("PK" + [char]5 + [char]6 + ("$([char]0)" * 18))
}

$ZipFile = (new-object -com shell.application).NameSpace($path)
$files | foreach {$zipfile.CopyHere($_.fullname)}
_______________________________________________
Other powershell  (as of .net 4.5)

Add-Type -AssemblyName System.IO.Compression.FileSystem

Add-Type -A System.IO.Compression.FileSystem
[IO.Compression.ZipFile]::CreateFromDirectory('foo', 'foo.zip')
[IO.Compression.ZipFile]::ExtractToDirectory('foo.zip', 'bar')

*/

