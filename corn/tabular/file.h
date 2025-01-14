#ifndef fileH
#define fileH
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/data_source/data_table.h"
#include "corn/tabular/delineation.h"
#include "corn/container/text_list.h"

/*980611
   This class is a base class for all table oriented files
   such as delineated text file, dBase, and Spreadsheets.

   A number of header lines can be specified.
*/

namespace CORN {
//______________________________________________________________________________
struct Tabular_format_common
{
   Delineation   data_line_delineation;  // delineation of data lines
   Extent        file_extent_layout;    // This is so we can know where the file actual starts (to skip pretext_
};
//_struct Tabular_format_common_____________________________________2016-10-13_/


// Derive this from data source
//______________________________________________________________________________
class Tabular_file            // should rename to CORN::Data_table_tabular_file
: public Data_table           // should rename to Table_data_source  050406
{
 protected:
   std::string *filename;
      // Optional, will be 0 if std::cout
   bool          keep_read_records_in_memory; // This variable may be obsolete
   // These lists will store the header and data records if we keep records in memory
   Text_list header_records;
 protected:  // The following are for write mode
   mutable nat32   current_raw_line;       // 1 indexed
   mutable nat32   current_field_column;   // 1 indexed
      // but may be 0 indexed  if not in field position
      // (for text files, FORTRAN/COBOL carridge control column is 0
      // since it is not a data field)
 public:
   inline Tabular_file
      (const std::string *filename_
      ,bool keep_read_records_in_memory_
      ,Indexer *optional_indexer = 0)                                            //111024
      : Data_table(optional_indexer)                                             //111024
      ,filename(filename_ ? new std::string(*filename_) : 0)                     //190506
         // sorting path is OS::File_system
      ,keep_read_records_in_memory(keep_read_records_in_memory_)
      ,header_records("")
      ,current_raw_line(1)
      ,current_field_column(0)
      {}
   inline virtual bool open_for_output_if_necessary()                 stream_IO_ //090713
      {return true;}
      // This method is mainly for text files
   inline virtual bool redelineate_if_necessary()                { return true;} //141210
      // This method is mainly for text files
      // Classes derived from this class will handle the actual file foramt
      // This class allows for a specified number of rows or record lines
      // to be reserved for header, comments, column headers, labels etc..
   virtual int get_column_count()                                            =0;
      // returns the number of columns identified in the table.
   virtual int get_column_width(int column)                                  =0;
      // returns the width (in characters) of the specified column
      // I think it is OK to return 0 if column width is
      // not significant (I.e. spreadsheets) 061018
//NYN (currently only used in CORN_tabular_text_file
//NYN /*050404_*/    virtual int get_data_start_row() const = 0;
   inline virtual const std::string *get_Nth_header(int /*header_row_1based*/)
      { return 0;} // Generally only text file may have header text lines
      // 1 indexed
      // returns a pointer to the specified header row string.
      // return 0 if no header at the specified row.
 public: // The following methods apply to writing files
   inline virtual bool goto_raw_line(nat32 line_number_1_based)                  //070203
      {  current_raw_line = line_number_1_based;
         current_field_column = 0; return true;}
      // This goes to the raw line in a file.
      // This is used to position the data at a particular line in the file
      // This is primarily used in outputing any header fields
      // and column header lines in the derived classes.
      // Returns false if cannot goto the specified line.
      // For example, you cannot reposition text files to a previous line.
      //   (This is allowed for Spreadsheets, but this should be avoided
      //    if you wan't to beable to swap different file types).
      // Database tables would always return 0 because they only store
      // datarecords  and have no concept of a raw format.
      // The current field column is set to 0
      // (because text files should not reposition or redelimit on new lines.)
   inline virtual bool goto_field_column(nat32 column_number_1_based)            //070203
      { current_field_column = column_number_1_based; return true;}
   // This advances to the specified column
   // For text files, this is the field number (not the character column)
   // Text files will insert sufficient delimitation to match the column.
   // In Spreadsheets this is the cell column.
   // Does not apply to database tables
   inline virtual bool goto_sheet(const std::string & /*sheet*/) {return false;} //100110 Only applicable to Workbooks
   virtual nat32 write_delimiter_string(const char *c_str)                   =0; //070203
   virtual nat32 write_raw_string(const char *c_str)                         =0; //070203
   // This writes the specified string to the current position.
   // returns the actual number of characters written including quotes
   virtual nat32 write_field_string                                              //070203
      (const char *c_str
      //200220 ,Delineation::Text_quote_mode quote_mode
      ,const std::string &quotation)                                         =0; //200220
 public:
/*190106 This is generally applicable so move to delineation as standlone function
   virtual const std::string &compose_formatted_string_from_cstr                 //140922
      (std::string &formatted_string
      ,const char *c_str
      ,CORN::Delineation::Text_quote_mode quote_mode)          const = 0;
      // problably should pass delimit mode
*/
};
//_class Tabular_file__________________________________________________________/
}//_namespace_CORN_____________________________________________________________/
#endif

