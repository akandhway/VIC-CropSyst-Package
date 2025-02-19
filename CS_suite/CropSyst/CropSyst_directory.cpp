#include "CropSyst_directory.h"
#include "CS_suite/application/CS_suite_directory.h"
#include "corn/OS/file_system_engine.h"

/*180925
#ifdef _Windows
#if (CROPSYST_VERSION == 4)
#  define cropsyst_X_exe "CropSyst_4.exe"
#else
#  define cropsyst_X_exe "CropSyst_5.exe"
#endif
#else
#if (CROPSYST_VERSION == 4)
#  define cropsyst_X_exe "CropSyst_4"
#else
#  define cropsyst_X_exe "CropSyst_5"
#endif

#endif
*/
//______________________________________________________________________________
namespace CS
{
CropSyst_Directory::CropSyst_Directory
(const CS::Suite_Directory &path_name,const wchar_t * directory_name)
: CS::Application_directory(path_name,directory_name)
, cropsyst_model_exe    (0) //(cropsyst_X_exe)
, crop_editor_exe       (0) //("crop_editor.exe")
, crop_calibrator_exe   (0) //("crop_calibrator.exe")
, soil_editor_exe       (0) //("soil_editor.exe")
, management_editor_exe (0) //("management_editor.exe")
, biomatter_editor_exe  (0) //("biomatter_editor.exe")
, rotation_editor_exe   (0) //("rotation_editor.exe")
, scenario_editor_exe   (0) //("scenario_editor.exe")
//150419 , CPF_controller_exe    (0) //("CPF_controller.exe")
, CPF_runner_exe    (0) //("CPF_controller.exe")
, schedule_viewer_exe   (0) //("schedule_viewer.exe")
, fastgraph_exe         (0) //("fastgrph.exe")
, soil_convertor_exe    (0) //("soil_convertor.exe")
, CropSyst_wizard_exe   (0)
, carbon_sequestration_optimizer_exe(0)
, carbon_sequestration_viewer_exe(0)
, GIS_runner_exe        (0)
#if (CS_VERSION == 4)
// V5 will not have .FMT or .LOC files
, location_editor_exe   (0) //("location_editor.exe")
, format_editor_exe     (0) //("format_editor.exe")
, database_editor_exe   (0) //("database_editor.exe")
, project_editor_exe     (0)
#endif
, output_manager_advanced_exe     (0) //("output_manager_advanced.exe")
{
   parent_directory = &path_name;                                                //140422
   name = directory_name;                                                        //140422
   construction_compose(); //140422 <- Acctually this should be done by
   // CS::Application_directory(*this) but I don't see that happening
}
//________________________________________________________________constructor__/
CropSyst_Directory::~CropSyst_Directory()
{  reset();
}
//_________________________________________________________________destructor__/
provide_file_name_implementation(CropSyst_Directory,crop_editor_executable,crop_editor_exe,"crop_editor.exe");
provide_file_name_implementation(CropSyst_Directory,crop_calibrator_executable   ,crop_calibrator_exe          ,"crop_calibrator.exe");
provide_file_name_implementation(CropSyst_Directory,soil_editor_executable       ,soil_editor_exe              ,"soil_editor.exe");
provide_file_name_implementation(CropSyst_Directory,management_editor_executable ,management_editor_exe        ,"management_editor.exe");
provide_file_name_implementation(CropSyst_Directory,biomatter_editor_executable  ,biomatter_editor_exe         ,"biomatter_editor.exe");
provide_file_name_implementation(CropSyst_Directory,rotation_editor_executable   ,rotation_editor_exe          ,"rotation_editor.exe");
provide_file_name_implementation(CropSyst_Directory,scenario_editor_executable   ,scenario_editor_exe          ,"scenario_editor.exe") ;
//150419 provide_file_name_implementation(CropSyst_Directory,CPF_controller_executable    ,CPF_controller_exe           ,"CPF_controller.exe");
provide_file_name_implementation(CropSyst_Directory,CPF_runner_executable        ,CPF_runner_exe               ,"CPF_runner.exe");
provide_file_name_implementation(CropSyst_Directory,schedule_viewer_executable   ,schedule_viewer_exe          ,"schedule_viewer.exe");
provide_file_name_implementation(CropSyst_Directory,fastgraph_executable         ,fastgraph_exe                ,"fastgrph.exe");
provide_file_name_implementation(CropSyst_Directory,soil_convertor_executable    ,soil_convertor_exe           ,"soil_convertor.exe");
provide_file_name_implementation(CropSyst_Directory,carbon_sequestration_optimizer_executable,carbon_sequestration_optimizer_exe           ,"carbon_sequestration_optimizer.exe");
provide_file_name_implementation(CropSyst_Directory,carbon_sequestration_viewer_executable  ,carbon_sequestration_viewer_exe           ,"carbon_sequestration_viewer.exe");
provide_file_name_implementation(CropSyst_Directory,GIS_runner_executable        ,GIS_runner_exe           ,"GIS_runner.exe");
provide_file_name_implementation(CropSyst_Directory,CropSyst_wizard_executable   ,CropSyst_wizard_exe  ,"CropSyst_wizard.exe");
provide_file_name_implementation(CropSyst_Directory,output_manager_advanced_executable   ,output_manager_advanced_exe  ,"output_manager_advanced.exe");
//______________________________________________________________________________
const CORN::OS::File_name &CropSyst_Directory::cropsyst_model_executable() provision_
{  if (!cropsyst_model_exe)
   {
      const char *CropSyst_X_exe =
         (this->find(L"uite_4") != std::string::npos)
         #ifdef _Windows
         ? "CropSyst_4.exe" : "CropSyst_5.exe";
         #else
         ? "CropSyst_4" : "CropSyst_5";
         #endif
      cropsyst_model_exe = new CORN::OS::File_name_concrete(*this,CropSyst_X_exe);
// Even though compiled for version 5.
// the version 5 model is yet to be developed so we may need to set this
// back down to version 4.
/*180925
      cropsyst_model_exe = new CORN::OS::File_name_concrete(*this,cropsyst_X_exe);
      if (!CORN::OS::file_system_engine.exists(*cropsyst_model_exe))
      {  delete cropsyst_model_exe;
         cropsyst_model_exe = new CORN::OS::File_name_concrete(*this,cropsyst_X_exe);
      }
*/
   }
   return *cropsyst_model_exe;
}
//_cropsyst_model_executable___________________________________________________/
void CropSyst_Directory::reset()                                       provision_
{  delete cropsyst_model_exe;          cropsyst_model_exe         = 0;
   delete crop_editor_exe;             crop_editor_exe            = 0;
   delete crop_calibrator_exe;         crop_calibrator_exe        = 0;
   delete management_editor_exe;       management_editor_exe      = 0;
   delete biomatter_editor_exe;        biomatter_editor_exe       = 0;
   delete rotation_editor_exe;         rotation_editor_exe        = 0;
   delete soil_editor_exe;             soil_editor_exe            = 0;
   delete scenario_editor_exe;         scenario_editor_exe        = 0;
//150419    delete CPF_controller_exe;          CPF_controller_exe         = 0;
   delete CPF_runner_exe;              CPF_runner_exe         = 0;
   delete schedule_viewer_exe;         schedule_viewer_exe        = 0;
   delete fastgraph_exe;               fastgraph_exe              = 0;
   delete soil_convertor_exe;          soil_convertor_exe         = 0;
   delete carbon_sequestration_optimizer_exe;          carbon_sequestration_optimizer_exe         = 0;
   delete carbon_sequestration_viewer_exe;          carbon_sequestration_viewer_exe         = 0;
   delete GIS_runner_exe;              GIS_runner_exe         = 0;

   #if (CS_VERSION == 4)
   delete location_editor_exe;         location_editor_exe        = 0;
   delete format_editor_exe;           format_editor_exe          = 0;
   delete project_editor_exe;          project_editor_exe          = 0;
   delete database_editor_exe;         database_editor_exe        = 0;
   #endif
   delete output_manager_advanced_exe; output_manager_advanced_exe= 0;
}
//_reset_______________________________________________________________________/
} // namespace CS

