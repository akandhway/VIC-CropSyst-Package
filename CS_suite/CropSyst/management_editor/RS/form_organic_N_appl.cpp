//______________________________________________________________________________
#include <vcl.h>
#include "CropSyst/source/management/management_param_V5.h"
//#include "corn/math/moremath.h"
#include "common/biomatter/decomposition_const.h"
#include "GUI/parameter/form_param_file.h"
#pragma hdrstop
#include "form_organic_N_appl.h"
//______________________________________________________________________________
#pragma package(smart_init)
#pragma link "RNAutoCheckBox"
#pragma link "RNAutoParameterEditBar"
#pragma link "RNAutoParameterEditBar"
#pragma link "RNAutoRadioGroup"
#pragma link "RNAutoFloatEdit"
#pragma resource "*.dfm"
Torganic_N_application_form *organic_N_application_form;
//______________________________________________________________________________
__fastcall Torganic_N_application_form::Torganic_N_application_form(TComponent* Owner)
   : TForm(Owner)
   , operation(0)
   , est_org_drymatter(0)
{}
//______________________________________________________________________________
void Torganic_N_application_form::bind_to(CropSyst::Organic_nitrogen_operation_abstract *i_operation,   Tparameter_file_form       *_parameter_file_form)
{  operation = i_operation;
   parameter_file_form = _parameter_file_form;
   // Organic manure page
   org_N_amount_edit          ->bind_to(&(operation->p_org_N)                       /* No help yet*/);
//170701    org_N_decomp_time_63_edit  ->bind_to(&(operation->p_decomposition_time_63)       /* No help yet*/);
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
   org_N_decomp_time_50_edit  ->bind_to(&(operation->p_halflife)       /* No help yet*/);
   org_N_decomp_time_check    ->bind_to(&(operation->decomposition_time_calculated) /* No help yet*/);
   manure_type_radiogroup     ->bind_to(&(operation->org_N_source_labeled)          /* No help yet*/);
#endif
   NH3_ammonia_edit           ->bind_to(&(operation->p_NH3_N)                       /* No help yet*/);
   // Volatilization page
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
   NH3_volatil_loss_edit      ->bind_to(&(operation->p_long_term_org_N_volatilization_loss_percent)/* No help yet*/);
   NH3_volatil_loss_check     ->bind_to(&(operation->org_N_volatilization_calculated)              /* No help yet*/);
   org_appl_method_radiogroup ->bind_to(&(operation->org_N_appl_method_labeled)                    /* No help yet*/);
   orgN_form_radiogroup       ->bind_to(&(operation->solid_liquid_form_labeled)                    /* No help yet*/);
   edit_carbon_fraction       ->bind_to(&(operation->v_carbon_fraction)                            /* No help yet*/);
#endif

   org_drymatter_label->Caption = org_drymatter_label->Caption; //  + " (estimated from N mass)";
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
   est_org_drymatter = operation->org_N_kg_ha * 100.0 / N_in_manure_percent_dry_wt[operation->org_N_source_labeled.get()];
   est_org_drymatter_out->bind_to(&(est_org_drymatter),2,0/* No help yet*/);
#endif
   NRCS_org_N_field_op_number_combbox->Text=operation->NRCS_operation_description.c_str();
   NH3_volatil_loss_check->Update();
   org_N_decomp_time_check->Update();
   show_hide_controls();
};
//______________________________________________________________________________
void Torganic_N_application_form::show_hide_controls()
{  if (operation)
   {
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
      org_N_decomp_time_63_edit->Enabled =!operation->decomposition_time_calculated;
      org_N_decomp_time_50_edit->Enabled =!operation->decomposition_time_calculated;
// Must be always visible, the type specifies the biomass conversion     manure_type_radiogroup->Visible = operation->decomposition_time_calculated;
      NH3_volatil_loss_edit->Enabled      =!operation->org_N_volatilization_calculated;
      org_appl_method_radiogroup->Visible = operation->org_N_volatilization_calculated;
      orgN_form_radiogroup->Visible       = operation->org_N_volatilization_calculated;
#endif
   }
}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::NH3_volatil_loss_check_onclick(TObject *Sender)
{  show_hide_controls();
}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::org_N_decomp_time_checkClick(TObject *Sender)
{  show_hide_controls();
}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::NRCS_org_N_field_op_number_combboxChange(TObject *Sender)
{
   CORN::UnicodeZ_to_string
   (NRCS_org_N_field_op_number_combbox->Text.c_str()
   ,operation->NRCS_operation_description);
//170701    operation->NRCS_operation_description.assign(NRCS_org_N_field_op_number_combbox->Text.c_str());
}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::est_org_drymatter_outChange(TObject *Sender)
{  org_drymatter_label->Caption = "Organic dry matter biomass (specified)";
   org_N_amount_edit->Caption /*Description_label*/ = "Amount of nitrogen in the form of organic manure (est)";
   operation->org_N_kg_ha = est_org_drymatter / 100.0 * N_in_manure_percent_dry_wt[operation->org_N_source_cowl_or_labeled.get()];
   org_N_amount_edit->Update();
}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::org_N_amount_editExit(TObject *Sender)
{
   org_N_amount_edit->Caption/*Description_label*/ = "Amount of nitrogen in the form of organic manure";
   org_drymatter_label->Caption = "Organic dry matter biomass (estimated from N mass)";
   est_org_drymatter = operation->org_N_kg_ha * 100.0 / N_in_manure_percent_dry_wt[operation->org_N_source_cowl_or_labeled.get()];
   est_org_drymatter_out->Update();
}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::manure_type_radiogroupClick(TObject *Sender)
{
/*200220 obsolete
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
   operation->decomposition_time_50 = convert_decomposition_time_63_to_50
      (manure_decomp_time_coef[operation->org_N_source_labeled.get()]);
#endif
*/
   org_N_decomp_time_50_edit->Update();
   org_N_amount_editExit(Sender);
}
//______________________________________________________________________________
void __fastcall Torganic_N_application_form::button_NRCS_helpClick(TObject *Sender)
{  parameter_file_form->view_manual("NRCS_field_operation.htm");
}
//---------------------------------------------------------------------------

