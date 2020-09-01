//______________________________________________________________________________
#include <vcl.h>
#pragma hdrstop
#include "CropSyst/source/options.h"

#include "form_CS_explorer.h"
//______________________________________________________________________________
#pragma package(smart_init)
#pragma link "FlCtrlEx"
#pragma link "AdvEdBtn"
#pragma link "AdvEdit"
#pragma link "AdvFileNameEdit"
#pragma link "AdvCombo"
#pragma resource "*.dfm"
Tform_CS_context *form_CS_context;
#include "../CS_explorer_engine.h"
//______________________________________________________________________________
#include <corn/application/program_directory.h>
#include <corn/container/text_list.h>
#include <CS_suite/application/CS_suite_directory.h>
#include "CropSyst/source/cs_project_directory.h"
#include <cropsyst/cpp/output.h>
#include <corn/datetime/date.h>
#include <stdlib.h>
#include "GUI\explorer\frame_navigation_JAM.h"
//______________________________________________________________________________

__fastcall Tform_CS_context::Tform_CS_context(TComponent* Owner)
   : TForm(Owner)
   , explorer_engine(0)
   , CS_explorer_directory(CORN::Smart_file_name(ParamStr(0).c_str()))
{

   CORN::Smart_file_name program_exe(ParamStr(0).c_str());
   CORN::Directory_name prog_dir_buffer;



   explorer_engine = new CS_Explorer_engine(/*Now global *CS::Suite_directory*/);

   button_version->Caption = "Version: "  CS_VERSION_STRING;

   filenameedit_simulation_model->FileName = CS::Suite_directory->CropSyst().cropsyst_model_executable().c_str();
   button_documentation_help     ->Visible = CS_explorer_directory.documentation_directory.help_document().exists();
   button_documentation_FAQ      ->Visible = CS_explorer_directory.documentation_directory.FAQ_document().exists();
   button_documentation_manual   ->Visible = CS_explorer_directory.documentation_directory.manual_document().exists();
   button_documentation_tutorial ->Visible = CS_explorer_directory.documentation_directory.tutorial_document().exists();

   navigation_frame = new Tnavigation_frame(this); // I am pretty sure frame are relinquished to this form.
   navigation_frame->Parent = panel_navigation;
   navigation_frame->Show();
   navigation_frame->Top = 0;
   navigation_frame->Left = 0;
//   navigation_frame->BorderStyle = bsNone;
   navigation_frame->bind_to(this,explorer_engine);
   navigation_frame->Align= alClient;

//   navigation_frame->Width = panel_navigation->Width;
//   navigation_frame->Height  =  panel_navigation->Height;

   button_lauch_CS_suite->Visible =  CS::Suite_directory->CS_suite_executable().exists();

   checkbox_UED_tools_mode->Checked = explorer_engine->UED_tools_mode;
      //bind_to(&(explorer_engine->UED_tools_mode),0/*NO_HELP_YET*/);
      // This applies only to version 4.
      // It is obsolete in version 5.
}
//______________________________________________________________________________
#define FORM_SCENRIO_OUTPUT_CONTEXT Tform_CS_context
#define output_options explorer_engine->provide_output_options()
#define scenario_output_directory explorer_engine->get_directory_name_qualified()
#include "CS_suite/CS_explorer/form_common_output_management.cpp"
#undef scenario_output_directory
#undef output_options
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_arbitrary_project_newClick(TObject *Sender)
{
   if (edit_arbitrary_project_name->Text.Length() == 0)
      // NYI should also validate that the project name is a valid directory
      Application->MessageBox("You must enter a valid directory name to create a new project","Project directory name invalid",IDOK);
   else
   {
      CORN::Smart_directory_name *new_project_dir = explorer_engine->create_project(edit_arbitrary_project_name->Text.c_str());
      navigation_frame->select_folder(new_project_dir? new_project_dir->c_str() : 0);
   }
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_project_scenario_createClick(TObject *Sender)
{
   command_history_memo->Lines->Append(explorer_engine->project_create_scenario(edit_project_scenario_name->Text.c_str()).c_str());
   const CORN::Smart_directory_name &created_scenario_dir = explorer_engine->get_directory_name_qualified();
   navigation_frame->select_folder(created_scenario_dir.c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_scenarios_create_CropSyst_scenarioClick(TObject *Sender)
{
   command_history_memo->Lines->Append(explorer_engine->project_scenarios_create_scenario
      (edit_scenarios_scenario_name->Text.c_str()
      ,".CropSyst_scenario" // CS_scenario_ext
      ,CS::Suite_directory->CropSyst().scenario_editor_executable()
      ).c_str());
   const CORN::Smart_directory_name &created_scenario_dir = explorer_engine->get_directory_name_qualified();
   navigation_frame->select_folder(created_scenario_dir.c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_scenarios_create_CANMS_scenarioClick(TObject *Sender)
{
   command_history_memo->Lines->Append(explorer_engine->project_scenarios_create_scenario
      (edit_scenarios_scenario_name->Text.c_str()
      ,".CANMS_scenario"
      ,CS::Suite_directory->CANMS().scenario_editor_exe).c_str());
   const CORN::Smart_directory_name &created_scenario_dir = explorer_engine->get_directory_name_qualified();
//   if (created_scenario_dir)
   navigation_frame->select_folder(created_scenario_dir.c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_project_scenariosClick(TObject *Sender)
{
   CORN::Smart_directory_name *scenarios_dir = explorer_engine->goto_subdirectory("Scenarios");
   navigation_frame->select_folder(scenarios_dir ? scenarios_dir->c_str() : 0);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_project_databaseClick(TObject *Sender)
{
   CORN::Smart_directory_name *database_dir = explorer_engine->goto_subdirectory("Database");
   navigation_frame->select_folder(database_dir ? database_dir->c_str() : 0);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_project_GIS_simulationClick(TObject *Sender)
{  command_history_memo->Lines->Append(explorer_engine->project_GIS_simulation().c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_project_watershed_simulationClick(TObject *Sender)
{  command_history_memo->Lines->Append(explorer_engine->project_watershed_simulation().c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_project_cabon_sequestrationClick(TObject *Sender)
{  command_history_memo->Lines->Append(explorer_engine->project_carbon_sequestration_for_grazing_regimen().c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_project_statsClick(TObject *Sender)
{
// command_history_memo->Lines->Append(explorer_engine->project_view_statistical_tallies().c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_arbitrary_create_simulation_folderClick(TObject *Sender)
{
   AnsiString simulation_folder_name(combobox_abritrary_simulation_folder_name ->Text);
   AnsiString projects_folder_name  (combobox_arbitrary_projects->Text);
   CORN::Smart_directory_name *sim_projects_dir = explorer_engine->create_simulation_directory
      (simulation_folder_name.c_str()
      ,projects_folder_name.c_str());
   navigation_frame->select_folder(sim_projects_dir ? sim_projects_dir->c_str() : 0);
   groupbox_arbitrary_simulation_folder->Visible = false;
   // Once we've created a Simulation\Projects folder,
   // we don't necessarily want do do this again in this editing session.
};
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_format_createClick(TObject *Sender)
{
   command_history_memo->Lines->Append(explorer_engine->
      edit_context_file(edit_format_name->Text.c_str()).c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::panel_database_add_weatherClick(TObject *Sender)
{  CORN::Smart_directory_name *weather_dir =explorer_engine->add_database_context("Weather");
   navigation_frame->select_folder(weather_dir ? weather_dir->c_str() : 0);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::panel_database_add_soilClick(TObject *Sender)
{
   CORN::Smart_directory_name *soil_dir =explorer_engine->add_database_context("Soil");
   navigation_frame->select_folder(soil_dir ? soil_dir->c_str() : 0);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::panel_database_add_rotationClick(TObject *Sender)
{
   CORN::Smart_directory_name *rotation_dir =explorer_engine->add_database_context("Rotation");
   navigation_frame->select_folder(rotation_dir ? rotation_dir->c_str() : 0);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::panel_database_add_cropClick(TObject *Sender)
{
   CORN::Smart_directory_name *crop_dir =explorer_engine->add_database_context("Crop");
   navigation_frame->select_folder(crop_dir ? crop_dir->c_str() : 0);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::panel_database_add_managementClick(TObject *Sender)
{
   CORN::Smart_directory_name *management_dir =explorer_engine->add_database_context("Management");
   navigation_frame->select_folder(management_dir ? management_dir->c_str() : 0);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::panel_database_add_biomatterClick(TObject *Sender)
{
   CORN::Smart_directory_name *biomatter_dir =explorer_engine->add_database_context("Biomatter");
   navigation_frame->select_folder(biomatter_dir ? biomatter_dir->c_str() : 0);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::panel_database_add_formatClick(TObject *Sender)
{
   CORN::Smart_directory_name *format_dir =explorer_engine->add_database_context("Format");
   navigation_frame->select_folder(format_dir ? format_dir->c_str() : 0);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_scenario_databaseClick(TObject *Sender)
{
   CORN::Smart_directory_name *database_dir = explorer_engine->goto_subdirectory("Database");
   navigation_frame->select_folder(database_dir ? database_dir->c_str() : 0);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_create_weatherClick(TObject *Sender)
{
   command_history_memo->Lines->Append(explorer_engine->
      edit_context_file(edit_weather_name->Text.c_str()).c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_weather_import_UEDClick(TObject *Sender)
{
   command_history_memo->Lines->Append(explorer_engine->launch_UED_import().c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_weather_export_UEDClick(TObject *Sender)
{  command_history_memo->Lines->Append(explorer_engine->launch_UED_export().c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_weather_climgenClick(TObject *Sender)
{   command_history_memo->Lines->Append(explorer_engine->launch_ClimGen().c_str());
};
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_weather_UED_spatialClick(TObject *Sender)
{   command_history_memo->Lines->Append(explorer_engine->launch_UED_spatial_interpolator().c_str());
}
//______________________________________________________________________________
void Tform_CS_context::show_hide_controls()
{
   switch (explorer_engine->get_identified_context()) // was identify_context()
   {
      case CS_Explorer_engine::arbitrary_context           : break;
      case CS_Explorer_engine::project_context             : break; //  1
      case CS_Explorer_engine::database_context            :
      {
         panel_database_add_weather    ->Caption = explorer_engine->database_has_directory("Weather")    ? "Goto Weather"     : "Add Weather";
         panel_database_add_soil       ->Caption = explorer_engine->database_has_directory("Soil")       ? "Goto Soil"        : "Add Soil";
         panel_database_add_rotation   ->Caption = explorer_engine->database_has_directory("Rotation")   ? "Goto Rotation"    : "Add Rotation";
         panel_database_add_crop       ->Caption = explorer_engine->database_has_directory("Crop")       ? "Goto Crop"        : "Add Crop";
         panel_database_add_management ->Caption = explorer_engine->database_has_directory("Management") ? "Goto Management"  : "Add Management";
         panel_database_add_biomatter  ->Caption = explorer_engine->database_has_directory("Biomatter")  ? "Goto Biomatter"   : "Add Biomatter";
         panel_database_add_format     ->Caption = explorer_engine->database_has_directory("Format")     ? "Goto Format"      : "Add Format";
/*
         panel_database_add_weather    ->Visible = !explorer_engine->database_has_directory("Weather");
         panel_database_add_soil       ->Visible = !explorer_engine->database_has_directory("Soil");
         panel_database_add_rotation   ->Visible = !explorer_engine->database_has_directory("Rotation");
         panel_database_add_crop       ->Visible = !explorer_engine->database_has_directory("Crop");
         panel_database_add_management ->Visible = !explorer_engine->database_has_directory("Management");
         panel_database_add_biomatter  ->Visible = !explorer_engine->database_has_directory("Biomatter");
         panel_database_add_format     ->Visible = !explorer_engine->database_has_directory("Format");
*/
      } break; //  2
      case CS_Explorer_engine::scenarios_context           : break; //  3
      case CS_Explorer_engine::enterprises_context           : break; //  3
      case CS_Explorer_engine::scenario_context            : break; //  4
      case CS_Explorer_engine::generated_scenarios_context : break; //  5
      case CS_Explorer_engine::scenario_output_context     :
      {
         fast_graph_button       ->Enabled = explorer_engine->scenario_output_has_file(   "daily.UED");
         formated_schedule_button->Enabled = explorer_engine->scenario_output_has_file("schedule.txt");
         text_schedule_button    ->Enabled = explorer_engine->scenario_output_has_file("schedule.txt");
         harvest_report_button   ->Enabled = explorer_engine->scenario_output_has_file( "harvest.htm");
         //NYI groupbox_reports_to_be_generated->Visible = check if this is a simulation directory

      } break; //  6
      case CS_Explorer_engine::weather_context             : break; //  7
      case CS_Explorer_engine::soil_context                : break; //  8
      case CS_Explorer_engine::rotation_context            : break; //  9
      case CS_Explorer_engine::crop_context                : break; // 10
      case CS_Explorer_engine::management_context          : break; // 11
      case CS_Explorer_engine::biomatter_context           : break; // 12
#if (CS_VERSION==4)
      case CS_Explorer_engine::format_context              : break; // 13
      case CS_Explorer_engine::STATSGO_context             : break; // 14
#endif
   };
   {  CORN::Smart_file_name desktop_ini("desktop.ini");
      const CORN::Smart_directory_name &engine_dir_name = explorer_engine->get_directory_name_qualified();
//      if (engine_dir_name)
      desktop_ini.set_path(engine_dir_name);
      button_restore_window_position->Visible = desktop_ini.exists();
   };
   {
      bool CANMS_installed = CS::Suite_directory->CANMS()/*_directory*/.exists();;
      CORN::Smart_file_name CANMS_scenario_file(".CANMS_scenario");
      bool CANMS_files_found = CANMS_scenario_file.exists();
      CANMS_scenario_file.set_path(explorer_engine->get_directory_name_qualified());
//      button_scenario_run_CANMS->Visible = CANMS_scenario_file.exists();
      button_project_CANMS_enterprises->Visible =CANMS_installed;
      button_scenario_edit_CANMS->Visible =CANMS_installed || CANMS_files_found;
//      button_CANMS->Visible = CANMS_installed
//    tabsheet_enterprises->TabVisible = CANMS_installed;
      button_scenario_run_CANMS->Visible = CANMS_files_found;
   };
   edit_scenarios_report_filename->Visible = groupbox_scenarios_format->ItemIndex == 1;
   edit_scenario_report_filename->Visible = groupbox_scenario_format->ItemIndex == 1;
};
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_soil_newClick(TObject *Sender)
{   command_history_memo->Lines->Append(explorer_engine->
      edit_context_file(edit_soil_name->Text.c_str()).c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_biomatter_createClick(TObject *Sender)
{   command_history_memo->Lines->Append(explorer_engine->
      edit_context_file(edit_biomatter_name->Text.c_str()).c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_management_createClick(TObject *Sender)
{   command_history_memo->Lines->Append
      (explorer_engine->
         edit_context_file(edit_management_name->Text.c_str()).c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_crop_createClick(TObject *Sender)
{  command_history_memo->Lines->Append
      (explorer_engine->
         edit_context_file(edit_crop_name->Text.c_str()).c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::create_generic_crop_buttonClick(TObject *Sender)
{  command_history_memo->Lines->Append
      (explorer_engine->edit_context_file(edit_crop_name->Text.c_str(),"crop").c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::create_fruit_crop_buttonClick(TObject *Sender)
{  command_history_memo->Lines->Append
      (explorer_engine->edit_context_file(edit_crop_name->Text.c_str(),"fruit").c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::create_CROPGRO_crop_buttonClick(TObject *Sender)
{  command_history_memo->Lines->Append
      (explorer_engine->edit_context_file(edit_crop_name->Text.c_str(),"cropgro").c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_rotation_createClick(TObject *Sender)
{  command_history_memo->Lines->Append
      (explorer_engine->edit_context_file(edit_rotation_name->Text.c_str()).c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::fast_graph_button_Click(TObject *Sender)
{  explorer_engine->launch_output_fast_graph();
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::formated_schedule_button_Click(TObject *Sender)
{  explorer_engine->launch_output_schedule_viewer();
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::text_schedule_button_Click(TObject *Sender)
{  explorer_engine->launch_output_schedule_text_viewer();
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::harvest_report_button_Click(TObject *Sender)
{  explorer_engine->launch_output_harvest_report_viewer();
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::output_reexport_buttonClick(TObject *Sender)
{
   const CropSyst_scenario_output_options *output_options = explorer_engine->provide_output_options();
   if (!output_options) return;
   progress_bar->Visible=true;
   progress_bar->StepBy(1);
   progress_bar->Min = 0;
   progress_bar->Max = (output_options->conversions.count());
   progress_bar->Position = 0;
   FOR_EACH_IN(conversion,CropSyst_scenario_output_options::Conversion,output_options->conversions,each_conversion)
   {  bool export_needed = report_out_of_sync(conversion);
      if (export_needed)
          explorer_engine->output_export_UED_to_XLS_file_command_line(conversion->ued_filename,conversion->xxx_filename,conversion->tdf_filename,false);
      progress_bar->StepIt();
      Application->ProcessMessages();
   } FOR_EACH_END(each_conversion);
   progress_bar->Visible=false;
   update_list_of_pending_reports();
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_soil_importClick(TObject *Sender)
{  command_history_memo->Lines->Append
      (explorer_engine->launch_soil_convertor().c_str());
};
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_soil_SOILPAR2_Click(TObject *Sender)
{  Application->MessageBox("This function is not yet implemented: soil SOILPAR","Not yet implemented",IDOK);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::FormCreate(TObject *Sender)
{  bool target_directory_on_command_line = false;

//   for (int arg = 1; arg < ParamCount(); arg++)
//      if (ParamStr(arg) == "UED_tools")

   if (ParamCount() == 1)
   {
      if (ParamStr(1) == "UED_tools")
      {
         explorer_engine->UED_tools_mode = true;
      }
      else
      {
         // The command line may have an optional directory name
         // If specified, we go to that directory.
         navigation_frame->select_folder(ParamStr(1).c_str());
         edit_current_folder_path->Text = ParamStr(1).c_str();
         // button_restore_window_positionClick(Sender);
         target_directory_on_command_line = true;
      };
   }
   if (!target_directory_on_command_line)
   {
      // Since the explorer was launched without a directory name,
      // we restore the position from the saved user setting.

      explorer_engine->user_application_directory.load_user_settings(explorer_engine->user_settings/*,save_dialog->FileName.c_str()*/);
      if (  explorer_engine->user_settings.GUI_top
          &&explorer_engine->user_settings.GUI_left
          &&explorer_engine->user_settings.GUI_width
          &&explorer_engine->user_settings.GUI_height)
      {
         Top   = explorer_engine->user_settings.GUI_top;
         Left  = explorer_engine->user_settings.GUI_left;
         Width = explorer_engine->user_settings.GUI_width;
         Height= explorer_engine->user_settings.GUI_height;
         Position = poDesigned;
      };

      // Jump to the last accessed directory.
      CORN::Smart_directory_name start_in_directory;
      if (explorer_engine->user_settings.most_recent_context_directory.length())
         start_in_directory.set(explorer_engine->user_settings.most_recent_context_directory);

      navigation_frame->select_folder(start_in_directory.c_str());
      edit_current_folder_path->Text = start_in_directory.c_str();
   };
//090806 moved to navigator   shellnavigator_current_directory->Update();
//   checkbox_UED_tools_mode->Checked = explorer_engine->UED_tools_mode;
   checkbox_UED_tools_mode->Update();
}
//______________________________________________________________________________
void Tform_CS_context::navigation_file_selected(const CORN::Smart_file_name &selected_filename)
{
// set the recently selected filename
#if (CS_VERSION==4)
//090826 In version 4 we work with .csn files which are listed in Scenarios folder
// In version 5 there will be no .csn files.
   bool selected_is_scenario
      = selected_filename.ext_is("csn");
   button_senerios_run->Enabled = selected_is_scenario;
   // NYI set recent file for the current context
   if (selected_is_scenario)
   {
//      explorer_engine->recently_selected_filename[scenarios_context].set(Item->Path.c_str());
      filenameedit_project_combinatorial_scenario_template->FileName = selected_filename.get_name(true).c_str(); // 090802 Item->DisplayName;
   } else
#endif
   {  // currently doing nothing, but might want to set an output indicating the
      // file selection for the current context.
      // I don't currently do this because the file will already be shown in the file list view
   // Note that the navigation frame already set the most recently selected file for the current context
   };
};
//______________________________________________________________________________
void Tform_CS_context::navigation_folder_selected(const CORN::Smart_directory_name &selected_folder)
{
   CS_Explorer_engine::Directory_context selected_folder_context = explorer_engine->identify_context_of(selected_folder);
   show_context(selected_folder_context);
   switch (selected_folder_context)
   {  case CS_Explorer_engine::scenario_context :
           button_senerios_run->Enabled = true;
      break;
   };
#ifdef NYN
// set the recently selected filename
   CS_Explorer_engine::Directory_context context = explorer_engine->identify_context();
      ,scenarios_context            //  3
      ,enterprises_context          //  4
      ,scenario_context             //  5
      ,generated_scenarios_context  //  6

   if (== xxxx)
   {
      bool selected_is_scenario
         = selected_filename.ext_is("csn")
         ||(selected_filename.compare("CS_scenario") == 0);
      button_senerios_run->Enabled = selected_is_scenario;
      // NYI set recent file for the current context
      if (selected_is_scenario)
      {
   //      explorer_engine->recently_selected_filename[scenarios_context].set(Item->Path.c_str());
         filenameedit_project_combinatorial_scenario_template->FileName = selected_filename.get_name(true).c_str(); // 090802 Item->DisplayName;
      };
   };
#endif
};
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_senerios_runClick(TObject *Sender)
{
   CORN::Text_list selected_scenario_filenames;
   //Currently get_list_of_selected_files returns fully qualified filename
   if (navigation_frame->get_list_of_selected_files(selected_scenario_filenames))
   {
      explorer_engine->project_scenarios_run_scenarios
         (selected_scenario_filenames
         ,edit_scenarios_report_filename->Text.c_str()
         ,true // Currently we disable runtime graph for multiple run mode.
         ,checkbox_scenarios_run_simultaneously->Checked
         );
      command_history_memo->Lines->Append
      (explorer_engine->get_command().c_str());
   };
};
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_scenario_runClick(TObject *Sender)
{  explorer_engine->scenario_run
         (checkbox_scenario_display_runtime_graph->Checked
         ,groupbox_scenario_format->ItemIndex == 1 ?  edit_scenario_report_filename->Text.c_str() : ""
         );
   command_history_memo->Lines->Append
      (explorer_engine->get_command().c_str());
};
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_scenarios_resetClick(TObject *Sender)
{
   Application->MessageBox("This function is not yet implemented: Scenarios reset","Not yet implemented",IDOK);

//NYI copy parameters from    recently_selected_filename[
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::Panel8Click(TObject *Sender)
{
   Application->MessageBox("This function is not yet implemented: Analysis compare scenarios","Not yet implemented",IDOK);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::combinatorial_project_scenario_generation_buttonClick(TObject *Sender)
{
   if (Application->MessageBox(

      "NOTE: This function is implemented but has not yet been fully tested.\n\n"

      "This operation generates all possible combinations of scenarios\n"
      "given parameter files in this project's database.\n"
      "This may or may not replace all files in the Generated scenarios folder.\n\n"
      "You may first want to copy or move the Generated scenarios folder\n"
      "to another directory as a backup measure.\n\n"
      "Continue with combinatorial scenario generation?"
      ,"Conform combinatorial scenario generation"
      ,MB_YESNO)
      == ID_YES)
   {
      CORN::Smart_file_name template_scenario_filename(filenameedit_project_combinatorial_scenario_template->FileName.c_str());
      // Note that the current directory context here will be the project directory.

      CropSyst_project_directory *project_dir = explorer_engine->provide_project_directory();
      if (!project_dir)
      {  Application->MessageBox("Could not get the project directory or its 'Scenarios' directory for scenario generation","Combinatorial scenario generation aborted",MB_OK);
         return;
      } else
         template_scenario_filename.set_path(project_dir->get_created_scenarios_dir_qual(false));

      if (!template_scenario_filename.exists())
      {
         if (Application->MessageBox
               ("A template scenario has not yet been selected.\n\n"
               "The most recently selected or edited scenario is automatically selected as the template,\n"
               "so a scenario can be selected by entering the Scenarios folder and selecting a scenario file.\n\n"
               "Would you like to create a new scenario with the default name to be used as the template scenario now.\n","Template scenario not selected",MB_YESNO) == ID_YES)
         {
            edit_project_scenario_name->Text = filenameedit_project_combinatorial_scenario_template->Text;
            button_project_scenario_createClick(Sender);
         };
         return;
      };

      int potential_scenario_generation_count = explorer_engine->project_combinatorial_scenario_generation_generate_all(template_scenario_filename,true);
      if (potential_scenario_generation_count)
      {
         // Before running, goto the generated scenarios so we can see the scenarios as they are generated
         // Note that project_combinatorial_scenario_generation_generate_all in precount mode will create this folder above.
         CORN::Smart_file_name generated_scenarios_dir(project_dir->get_generated_scenarios_dir_qual(true).c_str()/*"Generated_scenarios"*/);
//         generated_scenarios_dir.set_path(explorer_engine->get_directory_name_qualified());
         navigation_frame->select_folder
//09080obs          shelltreeview_current_directory->SelectFolder
            (generated_scenarios_dir.c_str());
         int actual_scenario_generation_count = explorer_engine->project_combinatorial_scenario_generation_generate_all(template_scenario_filename,false);

         Application->MessageBox("The scenarios have been generated.","Complete",MB_OK);

      } else
         Application->MessageBox
            ("Either this project does not (yet) have a parameter database directory,"
             " or there are no parameter files in the database to perform combination"
            ,"No database parameter files for combination"
            ,MB_OK);
   };
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_webpageClick(TObject *Sender)
{
  Smart_file_name cropsyst_web_page("http://www.bsyse.wsu.edu/cropsyst");
   cropsyst_web_page.invoke_viewer();
}
//______________________________________________________________________________
#ifdef MOVED
to TFrame_navigation
void __fastcall Tform_CS_context::shelllistview_current_directoryItemDblClick
(TObject *Sender, TStShellItem *Item, bool &DefaultAction)
{
   if (!Item->IsFolder)
   {
      if (explorer_engine->edit_file_if_known_type(Item->Path.c_str()).length())
         DefaultAction = false;
      else
      {
         CORN::Smart_file_name file_name_to_edit(Item->Path.c_str());
         file_name_to_edit.invoke_open();
         DefaultAction = false;

         NYI I might want to set explorer_engine recently selected file.

      };
   } else
      DefaultAction = true;
}
#endif
//______________________________________________________________________________
void __fastcall Tform_CS_context::listbox_pending_reportsClick(TObject *Sender)
{
   const CropSyst_scenario_output_options *output_options = explorer_engine->provide_output_options();
   if (!output_options) return;
   Ustring selected_pending_report(listbox_pending_reports->Items->Strings[listbox_pending_reports->ItemIndex].c_str());
   CropSyst_scenario_output_options::Conversion  *conversion = (CropSyst_scenario_output_options::Conversion  *)output_options->conversions.find(selected_pending_report.c_str());
   if (conversion)
   {  explorer_engine->output_export_UED_to_XLS_file_command_line(conversion->ued_filename,conversion->xxx_filename,conversion->tdf_filename,false);
   };
   update_list_of_pending_reports();

}
//______________________________________________________________________________
// The implementation of the following methods have been moved to
// CS_suite\CS_explorer\form_common_output_management because they are also
// used by Toutput_manager_advanced_form.
//
//______________________________________________________________________________
//______________________________________________________________________________

/* scenariou output manager functions to be implemented in CS_explorer scenario output context



//______________________________________________________________________________
__fastcall Toutput_manager_advanced_form::Toutput_manager_advanced_form(TComponent* Owner)
: TForm(Owner)
, fastgrph_exe(new Smart_file_name(cs_suite_paths->get_fastgraph_exe().c_str()))
, schedule_viewer_exe(new Smart_file_name(cs_suite_paths->get_schedule_viewer_exe().c_str()))
,scenario_directory(0)
,build_reports(0)
#ifdef GENERATE_ON_START
,initial_reports_generated(false)
#endif
,export_options(new CropSyst_scenario_output_options::Conversion())
,sim_starting_date((Year)0,(DOY)0)   // Default 0 indicates all dates in the
,sim_ending_date  ((Year)0,(DOY)0)   // UED will be exported.
{}
//______________________________________________________________________________
__fastcall Toutput_manager_advanced_form::~Toutput_manager_advanced_form()
{
   if (export_options) delete export_options; export_options = 0;
   if (scenario_directory) delete scenario_directory; scenario_directory = 0;
};
//______________________________________________________________________________
class Scenario_options_data_record
:public Common_parameters_data_record //Simulation_parameters
,public CropSyst_model_options
{  // THis is a local class because we only need to get the start and end date
public:
   Scenario_options_data_record()
   : Common_parameters_data_record(NO_ASSOCIATED_DIRECTORY,CS_VERSION_NUMBERS,"model_options")
   , CropSyst_model_options()
   {};
   virtual const char *get_primary_section_name() const { return "model_options" ; };
   virtual void expect_structure(bool for_write)
   {  Common_parameters_data_record::expect_structure( for_write);
      setup_structure(*this,for_write);
   };
};
//______________________________________________________________________________
void Toutput_manager_advanced_form::bind_to
(CropSyst_scenario_output_options *i_output_options
,const char                       *i_scenario_options_filename           // I think simply the scenario filename
,Tparameter_file_form             *_param_file_form
)
{
   param_file_form = _param_file_form;
   if (scenario_directory) delete scenario_directory; scenario_directory = 0;
   scenario_directory = new CropSyst::Scenario_directory(i_scenario_options_filename);
   output_options = i_output_options;
   scenario_directory->output_dir.set_as_current_working_directory();
   CORN::Ustring new_caption(Caption.c_str());
   new_caption.append(scenario_directory->output_dir);  // Warning may have problems with this is bind_to is called multiple times.
   Caption =  new_caption.c_str();
   Application->Title = new_caption.c_str();
   CORN::Smart_file_name scenario_options_filename (i_scenario_options_filename);
   if (scenario_options_filename.exists())
   {
      Simulation_run_period_data_record sim_ops;       //060418 was Scenario_options_data_record
      VV_File scenario_file(i_scenario_options_filename);
      scenario_file.get(sim_ops);
      sim_starting_date.set(sim_ops.starting_date);
      sim_ending_date.set(sim_ops.ending_date);
   } // else the sim_starting and ending dates default to 0, the export utility will need to query the UED file to determine date extent.
   scenario_directory->output_dir.qualify();   // 050328 This line might not be needed, probably already qualified
   if (scenario_directory->output_dir.exists())
      xls_reports_shelllistview        ->RootFolder = scenario_directory->output_dir.c_str();
   if (scenario_directory->output_soil_dir.exists())
      xls_profile_reports_shelllistview->RootFolder = scenario_directory->output_soil_dir.c_str();

   fast_graph_button       ->Enabled = scenario_directory->daily_ued.exists();
   text_schedule_button    ->Enabled = scenario_directory->schedule_txt.exists();
   formated_schedule_button->Enabled = scenario_directory->schedule_txt.exists();
   harvest_report_button   ->Enabled = scenario_directory->harvest_htm.exists();

//081001   fast_graph_button       ->bind_to((scenario_directory->daily_ued.c_str()) ,0,fast_graph_gem,fastgrph_exe->c_str());
   text_schedule_button    ->bind_to((scenario_directory->schedule_txt.c_str()) ,0,schedule_gem,0);
   formated_schedule_button->bind_to((scenario_directory->schedule_txt.c_str()) ,0,schedule_gem,schedule_viewer_exe->c_str());

   scenario_directory->CPF_dir.qualify();  // 050328 might not be needed, may already be qualified
   if (scenario_directory->CPF_dir.exists())
         CPF_dirlistbox->Directory = scenario_directory->CPF_dir.c_str();
   else  CPF_tabsheet->TabVisible=false;

   auto_export_listbox->bind_to(&(output_options->conversions),0);
   bind_export_options();

   build_button->bind_to(&build_reports,0);
   Show();
};
//______________________________________________________________________________
void Toutput_manager_advanced_form::bind_export_options()
{
   auto_export_enable_checkbox->bind_to(&(export_options->always_convert),0);
   export_UED_filename_edit->bind_to(&(export_options->ued_filename),0);
   DragAcceptFiles(export_UED_filename_edit->Handle,true);
   export_TDS_filename_edit->bind_to(&(export_options->tdf_filename),0);
   DragAcceptFiles(export_TDS_filename_edit->Handle,true);
   export_XXX_filename_edit->bind_to(&(export_options->xxx_filename),0);
   DragAcceptFiles(export_XXX_filename_edit->Handle,true);
};
//______________________________________________________________________________
void __fastcall Toutput_manager_advanced_form::Update(void)
{
#ifdef GENERATE_ON_START
   if (!initial_reports_generated)
      regenerate_reports();
#endif
   auto_export_listbox->Update();
   export_UED_filename_edit->Update();
   export_TDS_filename_edit->Update();
   export_XXX_filename_edit->Update();
   update_list_of_pending_reports();
};
//______________________________________________________________________________
void __fastcall Toutput_manager_advanced_form::explore_button_onclick(TObject *Sender)
{  Variant  ShellOLE;
   ShellOLE=Variant::CreateObject("Shell.Application");
   Function Explore("Explore");
   AnsiString directory(scenario_directory->output_dir.c_str());
   ShellOLE.Exec(Explore << Variant(directory));
}
//______________________________________________________________________________
void __fastcall Toutput_manager_advanced_form::open_selected_CPF_report(TObject *Sender)
{  CORN::Smart_file_name selected_filename(CPF_filelistbox->Items->Strings[CPF_filelistbox->ItemIndex].c_str());
   selected_filename.set_path(CPF_dirlistbox->Directory.c_str());
   selected_filename.invoke_viewer();
}
//______________________________________________________________________________
void __fastcall Toutput_manager_advanced_form::append_export_buttonClick(TObject *Sender)
{  if (output_options)
   {  output_options->conversions.append(export_options);
      export_options = new CropSyst_scenario_output_options::Conversion();
      Update();
   };
}
//______________________________________________________________________________
void __fastcall Toutput_manager_advanced_form::reexport_buttonClick(TObject *Sender)
{  regenerate_reports();
}
//______________________________________________________________________________
void __fastcall Toutput_manager_advanced_form::delete_export_buttonClick(TObject *Sender)
{  auto_export_listbox->delete_selected();
   update_list_of_pending_reports();
}
//______________________________________________________________________________
#ifdef NYI
This isn't working for some reason memory is messed up some where
void Toutput_manager_advanced_form::export_UED_to_XLS_file
(const CORN::Smart_file_name &XXXXX_ued
,const CORN::Smart_file_name &XXXXX_xxx
,const CORN::Smart_file_name &XXXXX_tdf
,bool include_full_year
)
{
   CORN::Date include_starting_date(sim_starting_date);
   if (include_full_year) include_starting_date.set_DOY(1);
   CORN::Date include_endinging_date(sim_ending_date);
   if (include_full_year) include_endinging_date.set_DOY(sim_ending_date.days_in_year());
   CORN::Smart_file_name ued_filename(XXXXX_ued); ued_filename.qualify();
   CORN::Smart_file_name xxx_filename(XXXXX_xxx); xxx_filename.qualify();
   CORN::Smart_file_name tdf_filename(XXXXX_tdf);
   if (xxx_filename.find_substring("\\soil\\") != SUBSTRING_NOT_FOUND)
      output_soil_dir.create();
   if(!tdf_filename.is_qualified(false))
      tdf_filename.set_path(simulation_paths->get_database_output_default_dir());
   if (ued_filename.exists()      && tdf_filename.exists())
   {  UED_export_database *export_DB = new UED_export_database(ued_filename.c_str(), tdf_filename.c_str());
      export_DB->initialize();
      {  UED_period_record *simulation_period_rec = export_DB->get_period(UED_period_application_code_option|UED_period_variable_code_option,0,0,0,0,0);
         if (simulation_period_rec)
            export_DB->set_date_ranges(simulation_period_rec->start_date_time,simulation_period_rec->end_date_time);
      };
      export_DB->export_to(xxx_filename.c_str());
//      delete export_DB;  not deleting, because something is out of alignment when compiled under builder
   };
}
#endif

//______________________________________________________________________________
void __fastcall Toutput_manager_advanced_form::Xupdate_list_of_pending_reports(
      TObject *Sender, TListItem *Item, TItemChange Change)
{  update_list_of_pending_reports();
}
//______________________________________________________________________________
void __fastcall Toutput_manager_advanced_form::label_excel_problem_detailsClick(TObject *Sender)
{  param_file_form->view_FAQ("excel_display_problem.htm");
}
//______________________________________________________________________________
void __fastcall Toutput_manager_advanced_form::fast_graph_button_Click(TObject *Sender)
{  // The current working directory
   param_file_form->run_program_with_file
      ( fastgrph_exe->c_str()
      , scenario_directory->daily_ued.c_str()
      , ""
      , ""
      , false // don't wait_for_termination
      , SW_SHOWNORMAL);
}
//______________________________________________________________________________
*/
#ifdef MOVED
to Tnavigation_frame::shelllistview_current_directoryChange
void __fastcall Tform_CS_context::shelllistview_current_directoryChange
(TObject *Sender, TListItem *Item, TItemChange Change)
{  CS_Explorer_engine::Directory_context context = explorer_engine->identify_context();
   switch (context)
   {
      case CS_Explorer_engine::scenario_output_context :
      {  // In the case of the scenario output directory
         // If a report is deleted, we will want to relist the generated report options.
         update_list_of_pending_reports();
      } break;
   };
}
#endif
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_output_edit_report_generationClick(TObject *Sender)
{
/*
   NYI  The output_manager_advanced provided the editor
   for the list of conversions available for report generation.
   I need to provide a stand alone editor to replace this

   For version 4, as a temporary hack I can simply launch the output manager advanced
   in a special mode that perhaps hides the /Scenario results\ page,
   or at least set the /Report generation\ the active page.

*/
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_documentation_manualClick(TObject *Sender)
{  CS_explorer_directory.documentation_directory.manual_document().invoke_viewer();
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_command_promptClick(TObject *Sender)
{  explorer_engine->run_program_with_file
      ( "cmd.exe"
      , explorer_engine->get_directory_name_qualified().c_str()
      , ""
      , "/K"
      , false // True if this editor is held until the other program exits
      , SW_SHOWNORMAL);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_set_window_positionClick(TObject *Sender)
{  CORN::Smart_file_name desktop_ini("desktop.ini");
   desktop_ini.set_path(explorer_engine->get_directory_name_qualified());
   char int_buffer[10];
   WritePrivateProfileString("CS_explorer","top"   ,itoa(Top,int_buffer,10)     ,desktop_ini.c_str());
   WritePrivateProfileString("CS_explorer","left"  ,itoa(Left,int_buffer,10)    ,desktop_ini.c_str());
   WritePrivateProfileString("CS_explorer","height",itoa(Height,int_buffer,10)  ,desktop_ini.c_str());
   WritePrivateProfileString("CS_explorer","width" ,itoa(Width,int_buffer,10)   ,desktop_ini.c_str());
   button_restore_window_position->Visible = true;
};
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_restore_window_positionClick(TObject *Sender)
{  CORN::Smart_file_name desktop_ini("desktop.ini");
   desktop_ini.set_path(explorer_engine->get_directory_name_qualified());
   Top   = GetPrivateProfileInt("CS_explorer","top"   ,Top     ,desktop_ini.c_str());
   Left  = GetPrivateProfileInt("CS_explorer","left"  ,Left    ,desktop_ini.c_str());
   Height= GetPrivateProfileInt("CS_explorer","height",Height  ,desktop_ini.c_str());
   Width = GetPrivateProfileInt("CS_explorer","width" ,Width   ,desktop_ini.c_str());
};
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_scenario_editClick(TObject *Sender)
{  command_history_memo->Lines->Append(explorer_engine->scenario_edit().c_str());
};
//______________________________________________________________________________
void __fastcall Tform_CS_context::groupbox_scenario_formatClick(TObject *Sender)
{  show_hide_controls();
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::groupbox_scenarios_formatClick(TObject *Sender)
{  show_hide_controls();
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_documentation_helpClick(TObject *Sender)
{  CS_explorer_directory.documentation_directory.help_document().invoke_viewer();
}
//______________________________________________________________________________
void Tform_CS_context::navigation_command_processed(const std::string &command)
{
   if (command.length())
      command_history_memo->Lines->Append(command.c_str());
};
//______________________________________________________________________________
void Tform_CS_context::navigation_folder_changed(const char *folder_path)
{
   explorer_engine->change_current_directory(folder_path);
   CS_Explorer_engine::Directory_context context = explorer_engine->identify_context();
   edit_current_folder_path->Text = folder_path;
   int context_index = (int) context;
   button_senerios_run->Enabled = false; // We may have navigated out of the scenarios folder so all entries are deselected
   show_context(context_index);
#ifdef MOVED
090903 to show_context()
   tabsheet_arbitrary ->Enabled = true; // enable one of the tabsheets to ensure that something is enabled as we enable and disable the sheets.
   for (int page_index = 1; page_index < pagecontrol_context->PageCount; page_index++)
      pagecontrol_context->Pages[page_index]->TabVisible = page_index == context_index;
   pagecontrol_context->Pages[0]->TabVisible = 0 == context_index; // Now we enable/disable the arbitrary context as needed
   pagecontrol_context->ActivePageIndex = context_index;
   switch (context)
   {
      case CS_Explorer_engine::scenario_output_context :
      {
         update_list_of_pending_reports();
      } break;
      case CS_Explorer_engine::arbitrary_context :
         groupbox_arbitrary_simulation_folder->Visible = true;
      break;
      case CS_Explorer_engine::scenarios_context: case CS_Explorer_engine::generated_scenarios_context :
            // When we first enter a scenarios context we disabled the run button
            // until one or more scenarios are selected.
           button_senerios_run->Enabled = false;
      break;
   };
   show_hide_controls();
#endif
};
//______________________________________________________________________________
void Tform_CS_context::navigation_current_directory_change()
{
   CS_Explorer_engine::Directory_context context = explorer_engine->identify_context();
   show_context(context);
/*moved to show_context
   switch (context)
   {
      case CS_Explorer_engine::scenario_output_context :
      {  // In the case of the scenario output directory
         // If a report is deleted, we will want to relist the generated report options.
         update_list_of_pending_reports();
      } break;
      case CS_Explorer_engine::arbitrary_context :
         groupbox_arbitrary_simulation_folder->Visible = true;
      break;
   };
*/
   button_restore_window_positionClick(0);
};
//______________________________________________________________________________
void __fastcall Tform_CS_context::FormClose(TObject *Sender,TCloseAction &Action)
{
   explorer_engine->user_settings.GUI_top    = Top;
   explorer_engine->user_settings.GUI_left   = Left;
   explorer_engine->user_settings.GUI_width  = Width;
   explorer_engine->user_settings.GUI_height = Height;
   explorer_engine->user_settings.most_recent_context_directory = explorer_engine->get_directory_name_qualified();
   explorer_engine->user_application_directory.save_user_settings(explorer_engine->user_settings/*,save_dialog->FileName.c_str()*/);
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_scenario_edit_CANMSClick(TObject *Sender)
{
   command_history_memo->Lines->Append(explorer_engine->CANMS_scenario_edit().c_str());
}
//______________________________________________________________________________
void __fastcall Tform_CS_context::button_project_CANMS_enterprisesClick(TObject *Sender)
{
   CORN::Smart_directory_name *enterprises_dir = explorer_engine->goto_subdirectory("Enterprises");
   navigation_frame->select_folder(enterprises_dir ? enterprises_dir->c_str() : 0);
}
//---------------------------------------------------------------------------
void __fastcall Tform_CS_context::button_enterprise_createClick(TObject *Sender)
{
   command_history_memo->Lines->Append(explorer_engine->project_enterprises_create_enterprise(edit_enterprise_name->Text.c_str()).c_str());
   const CORN::Smart_directory_name &created_enterprise_dir = explorer_engine->get_directory_name_qualified();
   navigation_frame->select_folder(created_enterprise_dir.c_str());
}
//---------------------------------------------------------------------------
void __fastcall Tform_CS_context::button_scenario_run_CANMSClick(TObject *Sender)
{
    // NYI
}
//---------------------------------------------------------------------------
void __fastcall Tform_CS_context::combobox_arbitrary_assume_contextChange(TObject *Sender)
{
   CS_Explorer_engine::Directory_context assumed_context = combobox_arbitrary_assume_context->ItemIndex;
   explorer_engine->assume_context(assumed_context);
   show_context(assumed_context);
}
//---------------------------------------------------------------------------
void Tform_CS_context::show_context(int context_index)
{  CS_Explorer_engine::Directory_context context = (CS_Explorer_engine::Directory_context)context_index;
   tabsheet_arbitrary ->Enabled = true; // enable one of the tabsheets to ensure that something is enabled as we enable and disable the sheets.
   for (int page_index = 1; page_index < pagecontrol_context->PageCount; page_index++)
      pagecontrol_context->Pages[page_index]->TabVisible = true;
   pagecontrol_context->ActivePageIndex = context_index;
   for (int page_index = 1; page_index < pagecontrol_context->PageCount; page_index++)
      pagecontrol_context->Pages[page_index]->TabVisible = page_index == context_index;
   pagecontrol_context->Pages[0]->TabVisible = 0 == context_index; // Now we enable/disable the arbitrary context as needed
   pagecontrol_context->ActivePageIndex = context_index;

   switch (context)
   {
      case CS_Explorer_engine::scenario_output_context :
      {  // In the case of the scenario output directory
         // If a report is deleted, we will want to relist the generated report options.
         update_list_of_pending_reports();
      } break;
      case CS_Explorer_engine::arbitrary_context :
         groupbox_arbitrary_simulation_folder->Visible = true;
      break;
      case CS_Explorer_engine::scenarios_context: case CS_Explorer_engine::generated_scenarios_context :
            // When we first enter a scenarios context we disabled the run button
            // until one or more scenarios are selected.
           button_senerios_run->Enabled = false;
      break;
   };
   show_hide_controls();
};
//---------------------------------------------------------------------------
void __fastcall Tform_CS_context::button_launch_windows_explorerClick(TObject *Sender)
{
   explorer_engine->run_program_with_file
      ( "explorer.exe"
      , explorer_engine->get_directory_name_qualified().c_str()
      , ""
      , ""
      , false // True if this editor is held until the other program exits
      , SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall Tform_CS_context::button_lauch_CS_explorerClick(TObject *Sender)
{  command_history_memo->Lines->Append(
   explorer_engine->run_program_with_file
      ( ParamStr(0).c_str() 
      , explorer_engine->get_directory_name_qualified().c_str()
      , ""
      , ""
      , false // True if this editor is held until the other program exits
      , SW_SHOWNORMAL).c_str());
}
//---------------------------------------------------------------------------
void __fastcall Tform_CS_context::SpeedButton1Click(TObject *Sender)
{  command_history_memo->Lines->Append(explorer_engine->launch_ClimGen().c_str());
}
//---------------------------------------------------------------------------
void __fastcall Tform_CS_context::button_lauch_CS_suiteClick(TObject *Sender)
{  explorer_engine->run_program_with_file
      ( CS::Suite_directory->CS_suite_executable().c_str()
      , explorer_engine->get_directory_name_qualified().c_str()
      , ""
      , ""
      , false // True if this editor is held until the other program exits
      , SW_SHOWNORMAL);
   command_history_memo->Lines->Append(explorer_engine->get_command().c_str());
}
//---------------------------------------------------------------------------
void __fastcall Tform_CS_context::button_weather_map_UED_locationsClick(TObject *Sender)
{  command_history_memo->Lines->Append(explorer_engine->map_UED_locations().c_str());
   Application->MessageBox("The file locations_map.htm will appear in\nthe file list above when the mapping is complete\ndouble-click the file name to view the map in a web browser.\nRequires the Internet to connect with Google Maps."
      ,"locations_map.htm will be created"
      ,MB_OK);
}
//---------------------------------------------------------------------------
void __fastcall Tform_CS_context::button_launch_text_editClick(TObject *Sender)
{  command_history_memo->Lines->Append(explorer_engine->launch_notepad().c_str());
}
//---------------------------------------------------------------------------
void __fastcall Tform_CS_context::edit_current_folder_pathChange(TObject *Sender)
{  // The current path input field allows the user to enter a new
   // fully qualified directory to change the directory.
   // Simply entering a new name
   if (!explorer_engine) return;
   CORN::Smart_directory_name new_current_directory(edit_current_folder_path->Text.c_str());
   if (new_current_directory.exists())
   {  // The directory must exists or the Directory tree or file list view may crash.
      const CORN::Smart_directory_name &engine_dir_name_qualified =  explorer_engine->get_directory_name_qualified();
      if ((&engine_dir_name_qualified == 0)|| (new_current_directory != engine_dir_name_qualified))
      {  // Don't attempt to change the directory  if we are already there
         // This condition may occur when the context is updated
         navigation_frame->select_folder(new_current_directory.c_str());
      };
   }; // else the user is probably still entering the name by hand
}
//---------------------------------------------------------------------------
void __fastcall Tform_CS_context::checkbox_UED_tools_modeClick(TObject *Sender)
{  // Autocheckbox will have already automatically set UED_tools_mode
   explorer_engine->UED_tools_mode = checkbox_UED_tools_mode->Checked;
   if (explorer_engine->UED_tools_mode)
   {
      explorer_engine->assume_context(CS_Explorer_engine::weather_context);
      show_context(CS_Explorer_engine::weather_context);
   };
}
//---------------------------------------------------------------------------
#include <corn/OS/Windows/VCL/registry.h>
void __fastcall Tform_CS_context::UED_association_buttonClick(TObject *Sender)
{
   // This should be moved to CS_explorer_engine

   CORN::Registry *reginifile = new CORN::Registry("CropSyst_Suite_4");
   try
   {  // Note, should find the correct CS_Suite_4 directory location
      // Eventually may have a more generic editor

      reginifile->register_extension("UED","Universal Environment Database"
         ,CS::Suite_directory->weather_editor_executable().c_str()
//080924          ,cs_suite_paths->get_weather_editor_exe().c_str()
//051031      "C:\\CS_suite_4\\CropSyst\\weather_editor.exe"
         ,"C:\\CS_suite_4\\icons\\UED.ico");
      Application->MessageBox("UED file type registration complete","File type registration", MB_OK	);
   }
   __finally
   {   delete reginifile; }
}//---------------------------------------------------------------------------
void __fastcall Tform_CS_context::button_UED_condenseClick(TObject *Sender)
{
   explorer_engine->launch_UED_condense();
}
//---------------------------------------------------------------------------

