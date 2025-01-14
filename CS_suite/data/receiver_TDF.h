#ifndef receiver_TDFH
#define receiver_TDFH
#include "corn/data/receiver.h"
#include "corn/container/binary_tree.h"
#include "corn/spreadsheet/workbook.h"
//200618 #include "UED/convert/ED_tabular_format.h"
#include "corn/tabular/delineation.h"
class ED_tabular_file_format;
#define detail_date_default_value 0
namespace CS {
//____________________________________________________________________________
class Receiver_TDF
: public implements_ CORN::Receiver
{
   //200618 const ED_tabular_file_format  &format;
   const CORN::Table_definition  &format;                                        //200618
   CORN::Workbook                &workbook;
   nat32                          collated_line_offset;
   CORN::Dynamic_array<CORN::datetime64> detail_timestamps;
      // This is a list of dates encountered  the array index
      // corresponds to the record in the sheet.
      // There should actually be one for EACH sheet.
 protected:
   CORN::Workbook::Sheet *current_sheet;
 public:
   Receiver_TDF
      (
       //200618 const ED_tabular_file_format &format_
       const CORN::Table_definition &format_                                     //200618
      ,CORN::Workbook               &workbook_
      ,const std::string            &sheet_name_                                 //200706
      ,nat32                         collated_line_offset_);
   virtual nat32 take
      (float32          value
      ,CORN::Units_code units
      ,CORN::datetime64 timestamp
      ,const std::string &ontology
      ,nat32            variable_code);
      // returns the timestamp index
   virtual nat32 take_vector
      (CORN::Dynamic_array<float32> vector_values
      ,CORN::Units_code units
      ,CORN::datetime64 timestamp
      //190423 now using optation ?
      ,const std::string &ontology
      ,nat32            variable_code);
   virtual bool know_ID(const std::string &ID);
   virtual const char *Receiver_TDF::get_ID(std::string &ID);                    //190916
   virtual bool close(const std::string &ID);
   //190422 abandoed virtual bool close_current();
 protected:
   virtual nat32 set
      (float32          value
      ,nat16            offset
      ,CORN::Units_code units
      ,CORN::datetime64 timestamp
      // now using optation?
      ,const std::string &ontology
      ,nat32            variable_code
      );
      // returns the timestamp index
/*190715 abandoned (I don't think I ended up using this idiom)

 protected:
   virtual nat32 commit_detail_line(const Detail_line &detail_line)
      { return 0; } // Not applicable to this specialization
 public:

   inline virtual bool write_detail
      (const CORN::Detail_line &detail_line)                         stream_IO_
         { return false; } // currently using workbook idiom
*/
 private:
   nat16 calc_timestamp_index(CORN::datetime64 timestamp)          calculation_;
};
//_class_TDF_Receiver_abstract______________________________________2019-01-04_/
}//_namespace_CS________________________________________________________________
#endif

