#ifndef outputH
#define outputH
#include "corn/parameters/parameters_datarec.h"
#include "corn/OS/filtered_filename.h"

// This is for V4 only.
// In V5 in any outputs in the Scenario/Database/Output will be automatically enabled.
// Use symbolic links to attach to the master Simulation database.
// CS Explorer will provide functions to link to the master simulation database.

//______________________________________________________________________________
namespace CS { class Desired_variables;}
//_________________________________________________________forward declaration_/
namespace CropSyst                                                               //120112
{
//______________________________________________________________________________
class Scenario_output_options
: public Common_parameters_data_record
{
 public:
   class Conversion
   : public CORN::Item
   {
   public:
      bool always_convert;
      CORN::OS::File_name_concrete /*iltered_file_name*/ xxx_filename;
      CORN::OS::File_name_concrete /*iltered_file_name*/ ued_filename;
      CORN::OS::File_name_concrete /*iltered_file_name*/ tdf_filename;
   public:
      inline Conversion()
         : CORN::Item()
         , always_convert(true)
         , xxx_filename("daily.xls") // ,"xls","Formatted file type","*.*" )
         , ued_filename("daily.ued") // ,"ued","Universal evironmental database","*.ued")
         , tdf_filename("daily.tdf") // ,"tdf","Tabular data format","*.tdf")
         {}
   inline virtual bool is_key_string(const std::string &key)              affirmation_  //180820
      { return key.compare(xxx_filename.c_str()) == 0; }

         // Note that the xxx_filename is used by the Advanced manager for the list of reports to be generated
      
      virtual const char *label_string(std::string &buffer)               const; //170423
      virtual bool  setup_structure(CORN::Data_record &,bool for_write) modification_; //020617
   };
   CORN::Bidirectional_list conversions;
 public:
   Scenario_output_options();
   virtual bool expect_structure(bool for_write) ;
   virtual bool get_end();                                                       //010109
   inline virtual const char *get_primary_section_name()                   const {return "output";}
   void list_required_variables(/*190204 CS::Desired_variables &required_variables*/) const;//170225_041021
   virtual const char *get_type_label()                                    const {return "CropSyst scenario output options";}  //051011
};
//_Scenario_output_options__________________________________________2004-10-21_/
}//_namespace_CropSyst______________________________________________2012-01-12_/
#endif

