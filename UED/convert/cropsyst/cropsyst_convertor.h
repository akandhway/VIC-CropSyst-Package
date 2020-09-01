#ifndef cropsyst_convertorH
#define cropsyst_convertorH
#include "UED/convert/convertor_weather.h"
#include "common/weather/database/weather_dat_gen_database.h"
//______________________________________________________________________________
class CropSyst_text_convertor
: public UED::Convertor_weather //170111 Convertor
{
   Location_parameters   loc_params;
public:
   CropSyst_text_convertor
         (UED::Convertor_arguments /*180101 CropSyst_text_convertor::Arguments*/ &arguments);                                          //170725
   //170725 (int argc,const char *argv[]);
   virtual uint32 perform_import() ;
   virtual uint32 perform_export() ;
   virtual uint16 get_version()           const { return 0x0100; };
   virtual const char *get_application_name()  const { return "CropSyst UED import utility (Roger Nelson rnelson@wsu.edu)"; };
private:
   void transfer
      (Weather_database &from_database
      ,Weather_database &to_database
      ,Year                start_year
      ,Year                end_year
      ,bool                export_more
      ,Weather_text_file::Format_old  export_format);
};
//_2006-01-18___________________________________________________________________
#endif

