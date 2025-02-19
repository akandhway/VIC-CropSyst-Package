//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UED/convert/ED_tabular_format.h"
#include "form_tabular_format_editor.h"
#include "form_column_field.h"
#include "form_header_field.h"
#include "GUI/parameter/form_param_file.h"
#include "corn/application/user/appdata_directory_OS_FS.h"
#include "CS_suite/application/CS_suite_directory.h"

USERES("tabular_format_editor.res");
USEFORM("..\..\..\form_column_field.cpp", column_field_form);
USEUNIT("..\..\..\..\..\..\corn\format\html\form_IO\content.cpp");
USEFORM("..\..\..\form_delineation_layout.cpp", delineation_layout_form);
USEFORM("..\..\..\form_header_field.cpp", header_field_form);
USEFORM("..\..\..\form_tabular_format_editor.cpp", tabular_format_editor_form);
USEFORM("..\..\..\..\..\..\GUI\parameter\form_param_file.cpp", parameter_file_form);
USEFORM("..\..\..\..\..\..\GUI\recent\form_recent_file.cpp", recent_file_selector_form);
USEUNIT("..\..\..\..\..\..\GUI\recent\recent.cpp");
USEUNIT("..\..\..\..\ED_tabular_format.cpp");
USEUNIT("..\..\..\..\..\..\corn\tabular\tabular_format.cpp");
USEUNIT("..\..\..\..\..\..\corn\tabular\tabular_text.cpp");
USEUNIT("..\..\..\..\..\..\corn\tabular\tabular_format_identifier.cpp");
USEUNIT("..\..\..\..\..\..\corn\tabular\tabular_text_parser.cpp");
USEUNIT("..\..\..\..\..\..\corn\tabular\tabular_parser.cpp");
USEUNIT("..\..\..\..\ED_tabular_parser.cpp");
USELIB("..\..\..\..\..\..\components\TMS_4.7\source_BCB5\wininet.lib");
USELIB("C:\temp\lib\BCB5\release\CS_directory.lib");
USEFORM("..\..\..\..\..\..\components\AutoControls\AutoUnitsForm.cpp", AutoUnits_form);
USELIB("C:\temp\lib\BCB5\release\UED_static.lib");
USEUNIT("..\..\..\..\..\..\GUI\parameter\form_param_file_common.cpp");
USEUNIT("..\..\..\..\..\..\GUI\translator\translator_windows.cpp");
USEUNIT("..\..\..\..\..\..\CS_suite\CS_suite_directory.cpp");
USEUNIT("..\..\..\..\..\..\CS_suite\UED\UED_directory.cpp");
USELIB("C:\temp\lib\BCB5\release\corn_complete_static.lib");
USEUNIT("..\..\..\..\..\..\corn\tabular\delineation.cpp");
USEFORM("..\..\..\..\..\..\components\AutoControls\AutoFileEditBar.cpp", AutoFileEditBar); /* TFrame: File Type */
USEFORM("..\..\..\..\..\..\components\AutoControls\AutoMetricUnitsForm.cpp", AutoMetricUnits_form);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
   try
   {
      CORN::OS::File_name_logical program_file_name(ParamStr(0).c_str());
      CS::Suite_directory = new CS::Suite_Directory(program_file_name);

      CORN::OS::File_name_logical tdf_filename(ParamStr(1).c_str());             //140601
       Application->Initialize();
       Application->CreateForm(__classid(Tparameter_file_form), &parameter_file_form);
       Application->CreateForm(__classid(Ttabular_format_editor_form), &tabular_format_editor_form);
       Application->CreateForm(__classid(Tcolumn_field_form), &column_field_form);
       Application->CreateForm(__classid(Theader_field_form), &header_field_form);
       Application->CreateForm(__classid(Trecent_file_selector_form), &recent_file_selector_form);
       Application->CreateForm(__classid(TAutoUnits_form), &AutoUnits_form);
       Application->CreateForm(__classid(TAutoMetricUnits_form), &AutoMetricUnits_form);
       ED_tabular_file_format format_definition;
       tabular_format_editor_form->bind_to(&format_definition, parameter_file_form);

      bool filename_specified_on_command_line =  (ParamCount() >= 1);
      /*140601
      if (!filename_specified_on_command_line)
      {
         tdf_filename.set_name("*.TDF");
         tdf_filename.set_path
            (CS::Suite_directory->UED().convert_formats());
         CS::Suite_directory->UED().convert_formats().
            set_as_current_working_directory();
      }
      */

      Association_list file_type_descriptions;                                   //140601
      file_type_descriptions.append(new File_type_description("TDF"         ,"Tabular data format description"              ,"*.TDF"));

      CORN::OS_FS::Application_data_directory user_app_dir(program_file_name);   //140601
       parameter_file_form->bind_to
       (tabular_format_editor_form
       ,&format_definition
       ,&tdf_filename
       ,&file_type_descriptions                                                  //140601
       ,tabular_format_editor_form->get_main_page_control()
       ,filename_specified_on_command_line
       ,&user_app_dir);

      Application->Run();
      delete CS::Suite_directory;
   }
   catch (Exception &exception)
   {
       Application->ShowException(&exception);
   }
   return 0;
}
//---------------------------------------------------------------------------
