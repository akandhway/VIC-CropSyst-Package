#include "import_column.h"
#include "UED/library/timeqry.h"
//170531#include "corn/datetime/time.h"
#include "UED/library/datasetrec_creation.h"
namespace UED
{
//______________________________________________________________________________
Import_column::Import_column
(ED_tabular_file_format::Column   &_layout
,Record_code            _record_type_code       // I.e. UED_record_type_distinguisher::UED_data_set_year_var_units
,UED_units_code         _time_step_units_code   // this is usually known from the UED_tabular_file_format
,UED_units_code         _time_stamp_step_units_code                              //050413
,CORN::Quality_code  _default_attribute_code // this is usually known from the UED_tabular_file_format
,const UED::Data_record_creation_layout *recognized_creation_layout)             //100125
: UED::Imp_exp_column(_layout)
, creation_layout(0)
, creation_layout_owned(false)                                                   //100125
{
   // In the future:
   // 1. It may be that the time_step is variable
   //    so the record read from the import file itself may specify.
   // 2. We may add an entry in the column description to indicate if the entire column is, for example, estimated)

   // Currently requireing all columns to specify the
   // UED_variable code, but it is possible that there is
   // only one variable in the file (single column)
   // and the variable is listed in the header or something.
   if (_layout.variable_code_clad.get())
   {
   // First we set the record type to what may have been specified for the overall file format description
   // the column layout may override
      Record_code layout_record_type_code = _record_type_code;
      if (!layout_record_type_code)
         layout_record_type_code = TFFC_layout.record_type_clad.get_code();
      // The import format column may (optionally) specified the preferred units to be stored in the UED file
      // (different from the units in the target file).
      // if no preference is given the data will be stored in the same units as the target file. 050518
      Units_code preferred_units_code = _layout.UED_import_to_smart_units_code_enum.get(); //050518
      if (!preferred_units_code) preferred_units_code = _layout                  //050518
         .get_units_code();                                                      //100117
      if (recognized_creation_layout)                                            //100125
      {  creation_layout_owned = false;                                          //100125
         creation_layout = recognized_creation_layout;                           //100125
      } else                                                                     //100125
      {  creation_layout = new UED::Data_record_creation_layout
            (layout_record_type_code
            ,_layout.variable_code_clad.get()
            ,preferred_units_code
            ,_time_step_units_code
            ,_time_stamp_step_units_code                                         //50413
            ,_default_attribute_code );
         creation_layout_owned = true;                                           //100125
      }
   }
}
//______________________________________________________________________________
Import_column::~Import_column()
{  if (creation_layout && creation_layout_owned)
      delete creation_layout;
   creation_layout = 0;
}
//______________________________________________________________________________
} // namespace UED
