#ifndef text_dBase_Excel_with_TDF_convertorH
#define text_dBase_Excel_with_TDF_convertorH

#include "UED/convert/text_dBase_Excel_with_TDF/tabular_file_with_TDF_convertor.h"
#include "corn/spreadsheet/cell_pump/cell_receiver.h"

namespace CORN
{
   class Cell_reader_Excel_XLS;
}
//______________________________________________________________________________
class Text_dBase_Excel_with_TDF_convertor
: public Tabular_file_with_TDF_convertor
, public CORN::Cell_receiver  // In the case of Excel
{
 public:
   Text_dBase_Excel_with_TDF_convertor
      (UED::Convertor_arguments &arguments);                                                    //170328
 public: //191009 protected :
   virtual CORN::Tabular_file *provide_tabular_file(Year &file_year);            //150405
 protected : // Cell_receiver implementation  these are used with Excel import
   Cell_reader_Excel_XLS *cell_reader;                                           //110828
   virtual bool assign_numeric_metadata(Cell_numeric *cell)        assignation_; //171027_100113
   virtual bool assign_numeric_detail  (Cell_numeric *cell)        assignation_; //171027_100113
   virtual bool take_numeric           (Cell_numeric *cell)      appropriation_; //100113
   virtual bool assign_textual_metadata(Cell_textual *cell)        assignation_; //100113
   virtual bool assign_textual_detail  (Cell_textual *cell)        assignation_; //100113
   virtual bool take_textual           (Cell_textual *cell)      appropriation_; //100113
   virtual void recognize_current_sheet_name(const std::string &) modification_; //100113
   virtual void recognize_current_sheet_index(nat16 current_sheet_index)modification_;
 public :
   virtual nat32 perform_import();
 protected :
   std::string current_sheet_name;  // Could actually be wstring but currently TDF and UED only recognize ASCII text 100113
      // dynamically allocated because I want to make sure it is cleared
      // for each row (In Excel there is the possibility that cells are completely missing)
      // CORN::Date_time_64 current_row_datetime64;   // may need to be date time
      // Dynamic_array[float32] curr_detail_line_values;
      // as we read tables (
      // float32 because currently UED only stores float32.
   virtual nat32  export_metadata_fields();                                      //111212
   virtual nat32  export_column_headers();                                       //111212               //070305_
 private:
   bool commit_detail_line_record_if_new_row_encountered(nat32 cell_row);        //100116
};
//_Text_dBase_Excel_with_TDF_convertor_________________________________________/
#endif

