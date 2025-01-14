
// This is abandoned, for the next version of CropSyst
// we'll have a general parameter editor

//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "form_crop_editor.h"
#include "static_phrases.h"
#include "CropSyst/source/crop/crop_param_V5.h"
#  include "corn/OS/file_system_engine.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvGroupBox"
#pragma link "RNAutoBitCheckListBox"
#pragma link "RNAutoCheckBox"
#pragma link "RNAutoFloatEdit"
#pragma link "RNAutoParameterEditBar"
#pragma link "RNAutoRadioGroup"
#pragma link "RNAutoDateButton"
#pragma link "SHDocVw_OCX"
#pragma link "RNAutoComboBox"
#pragma resource "*.dfm"
Tcrop_editor_form *crop_editor_form;
#include "CS_Suite/CropSyst/crop_editor/form_crop_editor_inc.h"
CROP_EDITOR_INCLUDES_AND_FORWARD_DECLS
//---------------------------------------------------------------------------
__fastcall Tcrop_editor_form::Tcrop_editor_form(TComponent* Owner)
   : TForm(Owner)
{ }
//---------------------------------------------------------------------------
void __fastcall Tcrop_editor_form::Update()
{  if (!crop) return;
   Update_V4_and_V5();
   checklistbox_view_panels->Update();
   radiogroup_resolution->ItemIndex =  crop->thermal_time.resolution_time_step.get() == UT_hour
      ? 0 : 1;
}
//______________________________________________________________________________
void __fastcall Tcrop_editor_form::bind_to(CropSyst_Crop_parameters *i_crop,Tparameter_file_form *i_parameter_form,const char *i_crop_model)
{  bind_to_V4_and_V5(i_crop,i_parameter_form,i_crop_model);
   checklistbox_view_panels->bind_to(&(crop->show_section),0);
   edit_detrition_time   ->bind_to(&(crop->residue_decomposition.p_detrition_time_50)  /*: 0,2*/ /*V4_HELP_URL(HELP_P_crop_residue_detrition_time)*//*,parameter_form->compose_manual_subdirectory_filename(HELP_P_crop_residue_detrition_time).c_str()*/);
   button_start_valid_date    ->bind_to(&(crop->inactive_period.start_DOY)          ,button_start_valid_date->Caption.c_str()/*,true*/ V4_HELP_URL(HELP_P_crop_dormancy_first_date_start)/*,parameter_file_form->compose_manual_subdirectory_filename(HELP_P_crop_dormancy_first_date_start).c_str()*/);
   button_leave_valid_date    ->bind_to(&(crop->inactive_period.end_DOY)          ,button_leave_valid_date->Caption.c_str()/*,true*/ V4_HELP_URL(HELP_P_crop_dormancy_first_date_restart)/*,parameter_file_form->compose_manual_subdirectory_filename(HELP_P_crop_dormancy_first_date_restart).c_str()*/);
   radiogroup_canopy_growth->bind_to(&(crop->morphology.canopy_growth_cowl));
   radiogroup_absission->bind_to(&(crop->abscission));
   edit_carbon_fraction                      ->bind_to(&(crop->residue_decomposition.p_carbon_fraction)                     V4_HELP_URL(HELP_P_crop_residue_carbon_fraction));
   edit_stubble_area_covered_to_mass_ratio   ->bind_to(&(crop->residue_decomposition.p_stubble_area_covered_to_mass_ratio)  V4_HELP_URL(HELP_P_crop_residue_area_mass_ratio));
   edit_surface_area_covered_to_mass_ratio   ->bind_to(&(crop->residue_decomposition.p_surface_area_covered_to_mass_ratio)  V4_HELP_URL(HELP_P_crop_residue_area_mass_ratio));

   combobox_RUSLE2_description            ->bind_to(&(crop->RUSLE2_description));
}
//______________________________________________________________________________
void __fastcall Tcrop_editor_form::radiogroup_resolutionClick(TObject *Sender)
{  if (!crop) return;
   crop->thermal_time.resolution_time_step.set
      ((radiogroup_resolution->ItemIndex == 1) ? UT_hour : UT_day);
   edit_cutoff_temp->Visible = crop->thermal_time.resolution_time_step.get() == UT_day;
   edit_opt_temp   ->Visible = crop->thermal_time.resolution_time_step.get() == UT_hour;
   edit_max_temp   ->Visible = crop->thermal_time.resolution_time_step.get() == UT_hour;
}
//_2011-05-28___________________________________________________________________
void __fastcall Tcrop_editor_form::update_root_biomass_checksum(TObject *Sender)
{  if ((!crop) || (!crop->has_residue_decomposition()))  return;
   root_checksum = crop->residue_decomposition.fast_cycling_root_fraction
                 + crop->residue_decomposition.slow_cycling_root_fraction
                 + crop->residue_decomposition.lignified_root_fraction;
   handle_update_root_biomass_checksum();
}
//_2011-05-29___________________________________________________________________
void __fastcall Tcrop_editor_form::update_top_biomass_checksum(TObject *Sender)
{
   if ((!crop) || (!crop->has_residue_decomposition()))  return;
   top_checksum = crop->residue_decomposition.fast_cycling_top_fraction
                + crop->residue_decomposition.slow_cycling_top_fraction
                + crop->residue_decomposition.lignified_top_fraction;
  handle_update_top_biomass_checksum();
}
//_2011-05-29___________________________________________________________________
void __fastcall Tcrop_editor_form::FormKeyPress(TObject *Sender, wchar_t &Key)
{  if (Key==VK_RETURN)
   {  Key = 0;
      Perform(WM_NEXTDLGCTL,0,0);
   };
}
//_2011-05-29___________________________________________________________________
void __fastcall Tcrop_editor_form::FormActivate(TObject *Sender)
{  CORN::OS::file_system_engine.set_current_working_directory(crop_directory);
}
//_2011-05-29___________________________________________________________________
void __fastcall Tcrop_editor_form::checkbox_grapeClick(TObject *Sender)
{  show_hide_controls(); }
//_2011-05-29___________________________________________________________________
void __fastcall Tcrop_editor_form::radiogroup_life_cycleClick(TObject *Sender)
{show_hide_controls(); }
//_2011-05-29___________________________________________________________________
void __fastcall Tcrop_editor_form::radiogroup_emergence_modelClick(TObject *Sender)
{  show_hide_controls(); }
//_2011-05-29___________________________________________________________________
void __fastcall Tcrop_editor_form::checkbox_maturity_significantClick(TObject *Sender)
{  show_hide_controls(); }
//_2011-05-29___________________________________________________________________
void __fastcall Tcrop_editor_form::radiogroup_WUE_submodelClick(TObject *Sender)
{  show_hide_controls(); }                                               //V4&V5
//_2011-05-29___________________________________________________________________
void __fastcall Tcrop_editor_form::radiogroup_max_RUE_modeClick(TObject *Sender)
{  show_hide_controls();}                                                //V4&V5
//_2011-05-29___________________________________________________________________
void __fastcall Tcrop_editor_form::radiogroup_absissionClick(TObject *Sender)
{  edit_linger->Visible =  crop->abscission.get() == semi_deciduous; }  //V5
//_2011-05-29___________________________________________________________________
void __fastcall Tcrop_editor_form::radiogroup_photoperiod_considerationClick(TObject *Sender)
{  show_hide_controls(); }                                               //V4&V5
//_2011-05-29___________________________________________________________________
void __fastcall Tcrop_editor_form::pagecontrol_season_dormancy_modeChange(TObject *Sender)
{  crop->season_dormancy_mode_labeled.set_int32(pagecontrol_season_dormancy_mode->ActivePageIndex);
   show_hide_controls(); }                                               //V4&V5
//_2011-05-29___________________________________________________________________
/*190627 obsolete
void __fastcall Tcrop_editor_form::radiogroup_crop_modelClick(TObject *Sender)
{  handle_crop_model_radiogroup_onclick(); }
//_2011-05-29___________________________________________________________________
*/
void __fastcall Tcrop_editor_form::checkbox_vernalizationClick(TObject *Sender)
{  show_hide_controls(); }
//_2011-06-04___________________________________________________________________
void __fastcall Tcrop_editor_form::radiogroup_harvested_biomassClick(TObject *Sender)
{  show_hide_controls();  }                                              //V4&V5
//_2011-06-04___________________________________________________________________
void __fastcall Tcrop_editor_form::checkbox_sensitive_to_coldClick(TObject *Sender)
{if (crop)    bind_to_hardiness();                                       //V4&V5
   show_hide_controls(); }
//_2011-06-04___________________________________________________________________
void Tcrop_editor_form::show_hide_controls()
{
   if (!crop) return;
   show_hide_controls_V4_and_V5();                                               //110524
   panel_canopy_cover_based->Visible    = crop->morphology.canopy_growth_cowl.get() == canopy_cover_based;
   panel_leaf_area_index_based->Visible = crop->morphology.canopy_growth_cowl.get() == leaf_area_index_based;
   bool CROPSYST_OR_CROPGRO_MODEL = ((crop->crop_model_labeled.get() == CROPSYST_CROP_MODEL) || (crop->crop_model_labeled.get() == CROPGRO_MODEL));
   bool CROPSYST_FRUIT (crop->crop_model_labeled.get()== CROPSYST_ORCHARD_MODEL);//131116

   groupbox_residue        ->Visible = CROPSYST_OR_CROPGRO_MODEL;
   groupbox_nitrogen       ->Visible = CROPSYST_OR_CROPGRO_MODEL;
   groupbox_salinity       ->Visible = CROPSYST_OR_CROPGRO_MODEL;
   groupbox_vernalization  ->Visible = CROPSYST_OR_CROPGRO_MODEL;
   groupbox_photoperiod    ->Visible = CROPSYST_OR_CROPGRO_MODEL;
   groupbox_layout         ->Visible = CROPSYST_FRUIT;
   groupbox_fruit          ->Visible = CROPSYST_FRUIT;
   groupbox_CROPGRO        ->Visible = crop->crop_model_labeled.get() == CROPGRO_MODEL;
   groupbox_salinity       ->Visible = CROPSYST_OR_CROPGRO_MODEL;
   groupbox_hydrothermal_time      ->Visible = CROPSYST_OR_CROPGRO_MODEL; // does not apply to orchard
   if (CROPSYST_FRUIT)
   {
      crop->life_cycle_labeled.set(perennial_life_cycle);
      crop->stem_type_labeled.set(woody_stem);
   };
   bool is_annual = crop->life_cycle_labeled.get() == annual_life_cycle/*110605 ANNUAL*/;
   groupbox_seasonal_adjustment->Visible = !is_annual;
   edit_linger->Visible = (crop->abscission.get() == semi_deciduous);

   groupbox_grape->Visible = crop->fruit.grape;
   groupbox_fruit_other->Visible = !crop->fruit.grape;

   bool advanced = parameter_file_form->is_advanced_mode();
   for (int show_i = 0; show_i <  checklistbox_view_panels->Items->Count; show_i++)
   {  bool visible = checklistbox_view_panels->Checked[show_i] || advanced;
      {
         System::UnicodeString selected_item_name(checklistbox_view_panels->Items->Strings[show_i]);
              if (selected_item_name == "Defaults")                  groupbox_defaults->Visible = visible;
         else if (selected_item_name == "Classification")            groupbox_classification->Visible    = visible;
         else if (selected_item_name == "Thermal time accumulation") groupbox_thermal_time->Visible      = visible;
         else if (selected_item_name == "Transpiration")             {groupbox_transpiration->Visible     = visible; groupbox_max_water_uptake->Visible     = visible;  }
         else if (selected_item_name == "Vernalization")             groupbox_vernalization->Visible     = visible;
         else if (selected_item_name == "Photo-period")              groupbox_photoperiod->Visible       = visible;
         else if (selected_item_name == "Emergence")                 groupbox_hydrothermal_time->Visible = visible;
         else if (selected_item_name == "Phenology")                 groupbox_phenology->Visible         = visible;
         else if (selected_item_name == "Canopy growth")             groupbox_canopy->Visible            = visible;
         else if (selected_item_name == "Attainable growth")         groupbox_attainable_growth->Visible = visible;
         else if (selected_item_name == "Senescence")                groupbox_senescence->Visible        = visible;
         else if (selected_item_name == "Dormacy/Seasonal")          groupbox_dormancy->Visible          = visible;
         else if (selected_item_name == "Root")                      groupbox_root->Visible              = visible;
         else if (selected_item_name == "Fruit")                     groupbox_fruit->Visible             = visible;
         else if (selected_item_name == "Layout")                    groupbox_layout->Visible            = visible;
         else if (selected_item_name == "Harvest")                   groupbox_harvest->Visible           = visible;
         else if (selected_item_name == "Hardiness")                 groupbox_hardiness->Visible         = visible;
         else if (selected_item_name == "Residue")                   groupbox_residue->Visible           = visible;
         else if (selected_item_name == "Nitrogen")                  groupbox_nitrogen->Visible          = visible;
         else if (selected_item_name == "Salinity")                  groupbox_salinity->Visible          = visible;
         else if (selected_item_name == "CO2")                       groupbox_CO2->Visible               = visible;
         else if (selected_item_name == "CROPGRO")                   groupbox_CROPGRO->Visible           = visible;
      };
   };
};
//______________________________________________________________________________
void __fastcall Tcrop_editor_form::checklistbox_view_panelsClick(TObject *Sender)
{
   // goto the panel with the specified name
   System::UnicodeString selected_item_name(   checklistbox_view_panels->Items->Strings[checklistbox_view_panels->ItemIndex]);
   TWinControl *panel_win_control = 0;
   //TGroupBox *panel_win_control = 0;
        if (selected_item_name == "Defaults")                  panel_win_control = groupbox_defaults;
   else if (selected_item_name == "Classification")            panel_win_control = groupbox_classification;
   else if (selected_item_name == "Thermal time accumulation") panel_win_control = groupbox_thermal_time;
   else if (selected_item_name == "Transpiration")             panel_win_control = groupbox_transpiration;
   else if (selected_item_name == "Vernalization")             panel_win_control = groupbox_vernalization;
   else if (selected_item_name == "Photo-period")              panel_win_control = groupbox_photoperiod;
   else if (selected_item_name == "Emergence")                 panel_win_control = radiogroup_emergence_model;
   else if (selected_item_name == "Phenology")                 panel_win_control = groupbox_phenology;
   else if (selected_item_name == "Canopy growth")             panel_win_control = groupbox_canopy;
   else if (selected_item_name == "Attainable growth")         panel_win_control = groupbox_attainable_growth;
   else if (selected_item_name == "Senescence")                panel_win_control = groupbox_senescence;
   else if (selected_item_name == "Dormacy/Seasonal")          panel_win_control = groupbox_dormancy;
   else if (selected_item_name == "Root")                      panel_win_control = groupbox_root;
   else if (selected_item_name == "Fruit")                     panel_win_control = groupbox_fruit;
   else if (selected_item_name == "Layout")                    panel_win_control = groupbox_layout;
   else if (selected_item_name == "Harvest")                   panel_win_control = groupbox_harvest;
   else if (selected_item_name == "Hardiness")                 panel_win_control = groupbox_hardiness;
   else if (selected_item_name == "Residue")                   panel_win_control = groupbox_residue;
   else if (selected_item_name == "Nitrogen")                  panel_win_control = groupbox_nitrogen;
   else if (selected_item_name == "Salinity")                  panel_win_control = groupbox_salinity;
   else if (selected_item_name == "CO2")                       panel_win_control = groupbox_CO2;
   else if (selected_item_name == "CROPGRO")                   panel_win_control = groupbox_CROPGRO;

//   TControl *panel_control = FindChildControl(L"groupbox_photoperiod"); //selected_item_name);
//   TWinControl *panel_win_control = dynamic_cast<TWinControl *>(panel_control);
   if (panel_win_control&&panel_win_control->Visible)
      panel_win_control->SetFocus();
      //ActiveControl = panel_win_control;
/*
   TCustomForm:: FocusControl (TWinControl * Control);
To set focus
TCustomForm:: __property TWinControl * ActiveControl = {read=FActiveControl, write=FActiveControl};
TWinControl TControl * FindChildControl (AnsiString ControlName);
*/
}
//______________________________________________________________________________
void __fastcall Tcrop_editor_form::checklistbox_view_panelsClickCheck(TObject *Sender)
{  show_hide_controls();  }
//______________________________________________________________________________
void __fastcall Tcrop_editor_form::radiogroup_canopy_growthClick(TObject *Sender)
{  show_hide_controls();  }
//______________________________________________________________________________
void __fastcall Tcrop_editor_form::button_calibrateClick(TObject *Sender)
{  handle_calibrate_button_on_click(); }
//______________________________________________________________________________
void __fastcall Tcrop_editor_form::combobox_RUSLE2_descriptionChange(TObject *Sender)
{  bool reset_description = crop->description.empty();
   if (!reset_description)
      reset_description = Application->MessageBox(L"Set the parameter file description to the RUSLE2 description?",L"Reset description?",MB_ICONQUESTION| MB_YESNO) == IDYES;
   if (reset_description)
   {
       crop->description.assign(crop->RUSLE2_description);
      parameter_file_form->edit_description->Update();
   }
}
//_2011-08-12___________________________________________________________________
void __fastcall Tcrop_editor_form::checkbox_HI_adjustmentsClick(TObject *Sender)
{
 show_hide_controls();
}
//_2011-09-07___________________________________________________________________

