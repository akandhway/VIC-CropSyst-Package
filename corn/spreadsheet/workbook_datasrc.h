#ifndef workbook_datasrcH
#define workbook_datasrcH
#include "corn/data_source/datasrc.h"
#include "corn/spreadsheet/workbook.h"
/*
   This class will read and write data to Workbooks (I.e. Excel spreadsheets).
   The workbook can be any provided CORN::Workbook class derived object.
   Data_records are stored with variables in columns and each row representing a record.
   Data_record sections will be associated with worksheets.
   Existing workbook files are updated.
   When the first cell in a column is written all remaining cells below are cleared.
*/
namespace CORN {
//______________________________________________________________________________
class Workbook_data_source
: public Data_source_abstract   // May want to derive this from Data_table
{  class Fielded_sheet : public Item
   {  Workbook::Sheet *sheet;  // owned by workbook
      std::string       column_field_names[256];
      nat8             column_count;
      // We can limit to 256, this is what Excel limits
      // The index corresponds to the column number
    public:
      // Cell location constants for all referenced sheets
      nat32 detail_line_col; // 0 based
         // This is the column where the first variable will be stored.
         // All cols to the right are assumed to be variable values.
      nat32 detail_line_row; // 0 based
         // This is the row where the first record will be stored.
         // All rows below on a sheet are assumed to be records.
      bool  expect_field_names;
         // This will put field names in the field_name_row on output
         // This should be set to false when origin_row = 0
         // because data will be stored in row 0 in this case.
         // The field name will be stored in the respective columns.
      nat32 field_name_row; // 0 based
    public:
      inline Fielded_sheet(Workbook::Sheet *sheet_
      ,nat32  detail_line_column_
      ,nat32  detail_line_row_
      ,bool   expect_field_names_
      ,nat32  field_name_row_)
      : sheet                                                           (sheet_)
      , column_count                                       (detail_line_column_)
      , detail_line_col                                    (detail_line_column_)
      , detail_line_row                                       (detail_line_row_)
      , expect_field_names                                 (expect_field_names_)
      , field_name_row                                         (field_name_row_)
      {}
      nat8 get_column_index(const char *field_name);
      // returns the column having the specified field name.
      // If there is no such column then the next available column is assigned.
      //1801001 inline virtual const char *get_key()     const { return sheet->get_key();}  replace with is_key

      inline virtual bool is_key_string(const std::string &key)    affirmation_  //181001
      {  std::string name_buffer;
         return sheet->get_name(name_buffer) == key; }

      inline Workbook::Sheet *get_sheet()                  {return sheet;}
   };
protected:  // The following values are used when
   Workbook  *workbook;
   Bidirectional_list fielded_sheets;
   // Cell location constants for all referenced sheets
   nat32             detail_line_col; // 0 based  This is the column where the first variable will be stored.  All cols to the right are assumed to be variable values.
   nat32             detail_line_row; // 0 based This is the row where the first record will be stored.  All rows below on a sheet are assumed to be records.
   bool              expect_field_names; // This will put field names in the field_name_row on output
                                       // This should be set to false when origin_row = 0
                                       // because data will be stored in row 0 in this case.
                                       // The field name will be stored in the respective columns.
   nat32             field_name_row; // 0 based
   // Record placement
   nat32             curr_row;  // Will be the same for all sheets
                     // We save the field names for faster lookup.
protected:
   virtual bool create(Data_record &from_record)                { return true; }
// create is protected because it should not be called by the end programmer.
// create is usually called by set() the first time a Data_record is set in a new database.
// When we use set() and we have not yet created the database
// We need to create the list of actual fields based on the from_record.
// The from_record VV_entries should have all the data needed to define
// fields for most databases.
// In may be called by get() (I.e. if a record to get is required to exist in the database.
// Create is optional but most derived Data_source classes will use create
// to open the file and define any necessary fields.
public:
   Fielded_sheet *get_fielded_sheet(const char *sheet_name);
public:
   Workbook_data_source
      (Workbook         *workbook_
      ,nat32             detail_line_col_      = 0
      ,nat32             detail_line_row_      = 1
      ,bool              expect_field_names_   = true
      ,nat32             field_name_row_       = 0);
#ifdef DEPRECATED
101124 I am not using this for reading any more, now using Cell_reader

   virtual bool get(Data_record &data_rec);
// This reads data from the data source at it's current position
// (I.e. current record in the database)
// This abstract method must be provided by a derived class
// If the database is not in a state where is can read any data it returns false.
// If the database could be read, but there were not maching entries (fields)
// then it returns false.
// Returns true if any record entries could be read.
// Examine each data record entry to determine if it was encountered
// in the data source.
// Note that this abstract class ALWAYS returns false because this abstract class does not
// actually fetch the data, it only set up the the structure.
// Derived classes must return true unless the data could not be fetched.

   virtual bool get_next(Data_record &data_rec);
   // A where clause index more than one record.
   // In this case, use get() to fetch the first record
   // and repeat calling get_next() to fetch remaining records.
   // get_next will return false if there were no
   // additional records returned by the SELECT WHERE statement
   // to fetch.
   // Note that this abstract class ALWAYS returns false because this abstract class does not
   // actually fetch the data, it only set up the the structure.
   // Derived classes must return true unless the data could not be fetched.
#endif
   virtual bool set_data(Data_record &data_rec,bool append);                     //150503
// This writes data from data_rec to this data source.
// Classes derived from Data_record may provide information such
// as the position of the record in the data file an the Data_source
// may then need to position the file pointer before writing the record.
// In classes such as VV_file, a Data_record constitutes all the data in
// a file, consequently the data can be written out to the file.
// Returns true if the record could be written.
// Returns true if there was a problem.
// This abstract method must be provided by a derived class.
// Derived Set() methods should first check if the datasource database is being created
// (I.e. it doesn't already exist, or it doesn't already have field definitions).
// Note that this abstract class ALWAYS returns false because this abstract class does not
// actually fetch the data, it only set up the the structure.
// Derived classes must return true unless the data could not be fetched.

   virtual const char *label_string(std::string &buffer)                  const; //170423
// This returns a text label that could be used to identify
// The data source being edited.
// It is left to the derived data source class how this text is formatted.
// For VV_file we simply use the fully qualified filename.
// For dBase we could use the filename and the record number  C:\path\file.dbf#123
// The result of label() should not be used to look up or identify a Data_source
// it is intended only for output to the user (I.e. in window titles)

   inline virtual void where_long(const char *attribute, const long value)    {}
   inline virtual void where_cstr(const char *attribute, const char *value)   {}
   // These set the current record to the first record where
   // the named attribute has the specified value.
   // Some datasources (i.e. SQL based) only set up the where clause
   // and the record 'pointer' will not be updated until the get/set method is performed.
   // This is why where() does not return any status.
   // In datasources derived from index if attribute is the index key
   // this will be faster search.
   // If attribute is not an index key then the record will be search on at a time
   // This should be overridden by data sources that are tables or some other
   // structure that hold multiple data records.
   // Single data record structures such as ini files, HTML post and queries
   // do not need to implement where()
   inline virtual void where2_long
        (const char *attribute1, const long value1
        ,const char *attribute2, const long value2)                           {}
   // This is a where clause with two key integer attributes
   inline virtual bool delete_records()                        { return false; }
   // To select the records to be deleted, set up a where clause with
   // one of the where_xxxx() functions.
   // (Currently only implement in oracle data source (eventually make this a pure function).
   virtual bool   goto_first();
   // Sets the current row index to the first detail line row.
   virtual bool   goto_index(nat32  zero_based_index);
   // Sets the current row index to the first detail line row + zero_based_index;
   virtual bool   goto_next(int32 skip_records = 1);
   // Advances to the next row in the workbook for all sheets associated
   // with the data record.
};
//______________________________________________________________________________
} // CORN namespace

#endif