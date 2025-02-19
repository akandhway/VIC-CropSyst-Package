#  include "options.h"
#  include "cs_operation.h"
#  include "corn/data_source/datarec.h"
#  include "cs_till.h"
#  include "cs_operation_codes.h"
#  include "USDA/NRCS/RUSLE2/SDR_field_ops.h"
using namespace std;
namespace CropSyst {
//______________________________________________________________________________
Operation::Operation
(CropSyst_Op op_code)
: Common_operation()
, op_ID(0)                                                                       //031124
, op_clad(op_code)                                                               //030824
, carbon_footprint_kgCO2e_ha(0)                                                  //120427
{}
//______________________________________________________________________________
Operation::Operation(const Operation &copy_from)
: Common_operation(copy_from)
, op_ID(copy_from.op_ID)
, op_clad(copy_from.op_clad.get())
, carbon_footprint_kgCO2e_ha(copy_from.carbon_footprint_kgCO2e_ha)
{}
//_Operation:constructor____________________________________________2014-08-28_/
void Operation::setup_parameters(CORN::Data_record &data_rec,bool for_write) modification_
{  Common_operation::setup_parameters(data_rec,for_write);
   data_rec.expect_int16("OP_ID",op_ID);                                         //031124
   data_rec.expect_float32("carbon_footprint",carbon_footprint_kgCO2e_ha);       //120427
}
//_Operation::setup_parameters______________________________________1998-10-07_/
const char *Operation::label_string(std::string &result)                   const
{  // We dont call Common_operation::label because it is pure
   result = CropSyst_op_text[get_type()];
   result += ":";
   return result.c_str();
}
//_Operation::label_string__________________________________________2017-04-23_/
void Operation::log(ostream &log_file)                                     const
{  Common_operation::log(log_file);
   //060725 NYI (probably not needed since we get get the parameters from the operation lookup in the schedule file
}
//_Operation::log___________________________________________________2008-08-10_/
void Operation::log_ID_and_type(ostream &log_file)                         const
{  Common_operation::log_ID_and_type(log_file);
   log_file << "op_ID=" << op_ID << endl;
   log_file << "type=" << CropSyst_op_text[get_type()] << endl;
}
//_Operation::log_ID_and_type_______________________________________2005-01-10_/
void Operation::set_description(const char *i_description)
{    // set description is currently used only to update descriptions when importing V3 operations
   description.brief.assign(i_description);                                      //150112RLN
}
//_Operation::set_description_______________________________________2004-06-14_/
const char *recal_var_code_label_table[] =
{ "WC"   // WC_RECAL
, "NO3"  // NO3_RECAL
, "NH4"  // NH4_RECAL
, "%OM"  // POM_RECAL
, "SNOW" // SNOW_RECAL
, "PAW"  // PAW_RECAL
, "RBM"  // RBM_RECAL
, "MBM"  // MBM_RECAL
#if ((CROPSYST_VERSION > 1) && (CROPSYST_VERSION < 5))
// In version 5 CO2 data may be stored in UED files
// (Actually all of these could be in UED files
/*060605_*/ , "CO2"  // CO2_RECAL                                                         //060605
#endif
, 0
};
//_recal_var_code_label_table_______________________________________2002-08-12_/
Recalibration_operation::Recalibration_operation(ifstream &in_file)
:Operation(CS_OP_RECALIBRATION)                                                  //011207
,var_code_clad(WC_RECAL)
,values_by_horizon(10,10,0)
{  std::string var_code_str;
   //Should not need to clear, dynamic arrays are initially clear /*011116x*/    values_by_layer.clear();
   in_file >> var_code_str;
   var_code_clad.set_label(var_code_str.c_str());                                //020812
   nat8 horizon = 1;
   if (!in_file.eof())
   while ((in_file.peek() != '\n') && (!in_file.eof()))
   {  float32 value;                                                             //010228
      in_file >> value;                                                          //010228
      values_by_horizon.set(horizon,value);                                      //010228
      horizon += (uint8)1;
   }
}
//_Recalibration_operation:constructor_________________________________________/
Recalibration_operation::Recalibration_operation
   (Recal_var_code _var_code
   ,soil_horizon_array32(_values_by_horizon))
:Operation(CS_OP_RECALIBRATION)                                                  //011207
,var_code_clad(_var_code)
,values_by_horizon(10,10,0)
{  // This constructor is used for adding recalibration
   // to reinitialize the soil profile.
   values_by_horizon.set_values(_values_by_horizon,MAX_soil_horizons);           //011119
}
//_Recalibration_operation:constructor______________________________2001-11-16_/
const char *Recalibration_operation::label_string(std::string &buffer)     const
{  Operation::label_string(buffer);
   buffer += "Recalibration ";
   var_code_clad.append_label(buffer);                                           //190109
   return buffer.c_str();
}
//_Recalibration_operation::label_string____________________________2017-04-23_/
void Recalibration_operation::log(ostream &log_file)                       const
{  Operation::log(log_file);
   std::string var_code_string;                                                  //190109
   log_file << "recalibration=" <<
         var_code_clad.append_label(var_code_string)                             //190109
         //190109 var_code_labeled.get_label()
         << endl;
   // Not I could dump out the values here
}
//_Recalibration_operation::log_____________________________________2002-08-10_/
Field_operation::Field_operation
(CropSyst_Op op_code
, int16 /* _NRCS_SDR_operation_number_unused_in_this_model (used in derived classes) */
, int16 /* reserved_unused_in_this_model*/ )
: Operation(op_code)
, NRCS_operation_description("")                                                 //060802
{}
//_Field_operation:constructor_________________________________________________/
Field_operation::Field_operation(const Field_operation &copy_from)
: Operation(copy_from)
, NRCS_operation_description(copy_from.NRCS_operation_description)
{}
//_Field_operation:constructor______________________________________2014-08-28_/
void Field_operation::setup_parameters(CORN::Data_record &data_rec,bool for_write) modification_
{  Operation::setup_parameters(data_rec,for_write);
   data_rec.expect_bool(LABEL_terminate_crop,terminate_crop);                    //180327_970616P  //110115 moved from tillage/residue/harvest
   data_rec.expect_string("NRCS_field_op",NRCS_operation_description,100);       //060725
}
//_setup_parameters_________________________________________________1998-10-07_/
const char *Field_operation::label_string(std::string &result)             const
{  // We dont call Common_operation::label because it is pure
   Operation::label_string(result);
   if (NRCS_operation_description.length())
   {  result += " (field operation:" + NRCS_operation_description + ")";
   }
   result += ":";
   return result.c_str();
}
//_label_string_____________________________________________________2001-12-09_/
void Field_operation::log(std::ostream &log_file)                          const
{  Operation::log(log_file);
   if (NRCS_operation_description.length())
      log_file << "NRCS="<<NRCS_operation_description<<std::endl;                //060802
}
//_log______________________________________________________________2012-09-24_/
}//_namespace CropSyst_________________________________________________________/

