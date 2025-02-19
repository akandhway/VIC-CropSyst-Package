#include "corn/data_source/ttable_datasrc.h"
#include "corn/file_sys/filename.h"

namespace_CORN_start
//______________________________________________________________________________
TTable_data_source::TTable_data_source(TTable *i_table)
: Data_source()
, table(i_table)
{}
//______________________________________________________________________________
CORN_string TTable_data_source::label() const
{
   CORN_string result(table->TableName.c_str());
   return result;
}
//______________________________________________________________________________
bool TTable_data_source::get(Data_record &data_rec)
{
   bool result = table->Exists;

   // The first time we access the file we may need to activate the table
   if (!table->Active)
      table->Open();
   // If the database exists we can read the data from the current record
   // Otherwise we simply return false, data_rec is unmodified (presumably it has default values).
   if (result)
   {
   /*
      This reads the fields of the record current record position in the database
      (I.e. current record in the database) storing the values in provided Data_record structure.
      TTable has already insures that we have a current record.

      -  Returns false if the database is not in a state where is can read any data.
      -  Returns false if the database could be read, but there were not maching entries (fields).
      -  Returns true if any record entries could be read from the fields at the current position.
      -  The caller can examine each Data_record entry to determine if it was encountered in the data base.

      TTable will already have the current data record in a buffer (Fields).
      Copy the data into the Record_data structure provided by
      the caller for field names corresponding to record entries' names.

      The provided Data_record structure may expect fields entries that don't
      actually occur or are not 'encountered' in the database.
      These entries will be marked as not encountered.

      The database format may have fields/columns that are not expected in
      the Data_record.  These fields will not be loaded.
   */
      data_rec.set_position(table->RecNo);
      // Let Data_record know which record it corresponds to in the file.

      //    For each section in date_rec:
      FOR_EACH_IN(section,VV_Section,data_rec.sections,each_section)
      {  FOR_EACH_IN(entry,VV_base_entry,section->entries,each_entry)
         {  // lookup field in actual fields:
            std::string entry_label; entry.key_string(entry_label); //180820 
            //180820 entry->get_key(entry_label);
            TField *field = table->FindField(entry_label.c_str());
            if (field)
            {
               CORN_string value_str(field->AsString.c_str());
               entry->set_str(value_str.c_str());
               result = true; // If any field is encountered then we accept this data
               entry->set_encountered(field);
            };
            // else the field was simply not encountered
         } FOR_EACH_END(each_entry);
      } FOR_EACH_END(each_section);
   };
   return result;
}
//______________________________________________________________________________
bool TTable_data_source::set(Data_record &data_rec)
{
   bool sat = false;
   // If the data source is not an existing database table
   // we create the table before trying to set the data values.
   if (!table->Exists)
      sat = create(data_rec);
   if (!table->Active)
      table->Open();
   if (!data_rec.get_position())
   {  // If the data source doesn't yet have a position number, it hasn't been
      // recorded in the database yet
      table->Append();
      // Let the record remeber where it is
      data_rec.set_position(table->RecNo);
   };

   {  // goto data_rec's position
      FOR_EACH_IN(section,VV_Section,data_rec.sections,each_section)
      {  FOR_EACH_IN(entry,VV_base_entry,section->entries,each_entry)
         {  // lookup field in actual fields:
                std::string entry_label; entry.key_string(entry_label); //180820 
                //180820 entry->get_key(entry_label);
                // Not sure if field names are stored  uppercase (as in dBase) assuming not
                // entry_label.to_upper()

                TField *field = table->FindField(entry_label.c_str());
                {
                // According to Build documentation you are supposted to
                // be able to set te FieldValue's value directly instead of
                // using the explicit varient conversion from string like i am doing here.
                CORN_string value_str = entry->get();

                // WARNING different databases take different formats
                // for date and uint16 and bool

                table->FieldValues[entry_label.c_str()] = value_str.c_str();
                sat = true; // If any field is encountered then we accept this data
                }
                // else the field was simply not encountered
                entry->set_encountered(field);
         } FOR_EACH_END(each_entry)
      } FOR_EACH_END(each_section)
   };
   table->Post();
   return sat;
}
//______________________________________________________________________________
bool TTable_data_source::create(Data_record &data_rec)
{
   bool result = false;
   table->FieldDefs->Clear();

   // For each section in data_rec:
   for (VV_Section *section = (VV_Section *)data_rec.sections.move_to_head()
       ;section
       ;section = (VV_Section *)data_rec.sections.move_to_next()
       )
   {  //for each entry in current section:
      for (VV_base_entry *entry=(VV_base_entry *)section->entries.move_to_head()
                ;entry
                ;entry = (VV_base_entry *)section->entries.move_to_next())
      {  // lookup field in actual fields:
               std::string string entry_label; entry->key_string(entry_label); //180820 
               //10820  entry->get_key(entry_label);
               unsigned char entry_type = entry->get_type();
               TFieldDef *NewField = table->FieldDefs->AddFieldDef();
               NewField->Name = entry_label.c_str();
               NewField->Size = entry->get_max_strlen();
               switch (entry_type)
               {
                  case VV_unknown   : NewField->DataType = ftUnknown;   break;
                  case VV_string    : NewField->DataType = ftString;    break;
                  case VV_int8      : NewField->DataType = ftFixedChar; break;
                  case VV_int16     : NewField->DataType = ftSmallint;  break;
                  case VV_int32     : NewField->DataType = ftInteger;   break;
                  case VV_uint16    : NewField->DataType = ftWord   ;   break;
//NYI                         case VV_uint8     : NewField->DataType = dB_number   ;    break;
                  case VV_uint32    : NewField->DataType = ftLargeint   ; /*I thing ftLargeInt will work*/   break;
                  case VV_float32  : NewField->DataType = ftFloat   ; /*notsure if ftFloat is 32 or 64 bit*/   break;
                  case VV_float64  : NewField->DataType = ftFloat   ; /*notsure if ftFloat is 32 or 64 bit*/   break;
                  case VV_bool      : NewField->DataType = ftBoolean  ; break;
                  case VV_date      : NewField->DataType = ftDate     ; break;
                  case VV_byte      : NewField->DataType = ftFixedChar; /*I think ftFixed char will work length = 1;   */ break;
                  case VV_char_array: NewField->DataType = ftBytes;        break;
                  case VV_Filename  : NewField->DataType = ftString;   break;
                  case VV_enum      : NewField->DataType = ftString;/*The max length we would know from the label table*/ break;
                  /* the following are not yet implemented
                  case VV_XXXXXXX   : NewField->DataType = ftCurrency	Money field
                  case VV_XXXXXXX   : NewField->DataType = ftBCD	Binary-Coded Decimal field
                  case VV_XXXXXXX   : NewField->DataType = ftTime	Time field
                  case VV_XXXXXXX   : NewField->DataType = ftDateTime	Date and time field
                  case VV_XXXXXXX   : NewField->DataType = ftVarBytes	Variable number of bytes (binary storage)
                  case VV_XXXXXXX   : NewField->DataType =
                  case VV_XXXXXXX   : NewField->DataType = ftAutoInc	Auto-incrementing 32-bit integer counter field
                  case VV_XXXXXXX   : NewField->DataType = ftBlob	Binary Large OBject field
                  case VV_XXXXXXX   : NewField->DataType = ftMemo	Text memo field
                  case VV_XXXXXXX   : NewField->DataType = ftGraphic	Bitmap field
                  case VV_XXXXXXX   : NewField->DataType = ftFmtMemo	Formatted text memo field
                  case VV_XXXXXXX   : NewField->DataType = ftParadoxOle	Paradox OLE field
                  case VV_XXXXXXX   : NewField->DataType = ftDBaseOle	dBASE OLE field
                  case VV_XXXXXXX   : NewField->DataType = ftTypedBinary	Typed binary field
                  case VV_XXXXXXX   : NewField->DataType = ftCursor	Output cursor from an Oracle stored procedure (TParam only)
                  case VV_XXXXXXX   : NewField->DataType = ftFixedChar	Fixed character field
                  case VV_XXXXXXX   : NewField->DataType = ftWideString	Wide string field
                  case VV_XXXXXXX   : NewField->DataType = ftLargeint	Large integer field
                  case VV_XXXXXXX   : NewField->DataType = ftADT	Abstract Data Type field

                  case VV_XXXXXXX   : NewField->DataType = ftArray	Array field
                  case VV_XXXXXXX   : NewField->DataType = ftReference	REF field
                  case VV_XXXXXXX   : NewField->DataType = ftDataSet	DataSet field
                  case VV_XXXXXXX   : NewField->DataType = ftOraBlob	BLOB fields in Oracle 8 tables
                  case VV_XXXXXXX   : NewField->DataType = ftOraClob	CLOB fields in Oracle 8 tables
                  case VV_XXXXXXX   : NewField->DataType = ftVariant	Data of unknown or undetermined type
                  case VV_XXXXXXX   : NewField->DataType = ftInterface	References to interfaces (IUnknown)
                  case VV_XXXXXXX   : NewField->DataType = ftIDispatch	References to IDispatch interfaces
                  case VV_XXXXXXX   : NewField->DataType = ftGuid	globally unique identifier (GUID) values
                  */
               }
      }
   }
   table->CreateTable(); // create the table
   result = true;
   return result;
}
//______________________________________________________________________________
bool   TTable_data_source::goto_first()
{  table->First();
}
//______________________________________________________________________________
bool   TTable_data_source::goto_index(uint32 zero_based_index)
{  table->First();
   table->MoveBy(zero_based_index);
}
//______________________________________________________________________________
bool   TTable_data_source::goto_last()
{  table->Last();
}
//______________________________________________________________________________
bool   TTable_data_source::goto_next(int32 skip_records = 1)
{    // skip records is allowed to be signed
   table->MoveBy(skip_records);
}
//______________________________________________________________________________
uint32 TTable_data_source::get_current_index()
{  return table->RecordNo - 1;
   // Warning I don't know if RecordNo is 0 based
   // It is not documented in the Borland help!
   // (I presume it is one based).

   // returns the index value for the table's current position.
   // For database tables this is the record number
   // For memory tables, this is the memory location or object pointer);
}
//______________________________________________________________________________
#ifdef DEPRECATED
161013 field type is actually format specification,
uint8 TTable_data_source::get_field_type(const char *field_name)
{  // returns the filed type corresponding to VV_entry types
   uint8 result = VV_unknown;

   TField *found_field = FindField(field_name);
   if (found_field)
   {
      TFieldType field_type = found_field->DataType;
      switch (field_type)
      {
         case ftUnknown             : VV_unknown;           break;   // Unknown or undetermined
         case ftString              : VV_string;            break;   // Character or string field
         case ftFixedChar           : VV_int8;              break;   // Fixed character field
//       case ftFixedChar           : VV_byte;              break;   /*I think ftFixed char will work length = 1;   */ break;
         case ftSmallint            : VV_int16;             break;   // 16-bit integer field
         case ftInteger             : VV_int32;             break;   // 32-bit integer field
         case ftWord                : VV_uint16;            break;   // 16-bit unsigned integer field
//NYI    case dB_number             : VV_uint8;             break;
         case ftLargeint            : VV_uint32;            break;   // Large integer field (I think, unless this is a 64bit thing)
         case ftFloat               : VV_float32;           break;   /*notsure if ftFloat is 32 or 64 bit*/
//?      case ftFloat               : VV_float64;           break;   /*notsure if ftFloat is 32 or 64 bit*/   break;
         case ftBoolean             : VV_bool;              break;
         case ftDate                : VV_date;              break;   // Date field
         case ftBytes               : VV_char_array;        break;   // Fixed number of bytes (binary storage)
//       case ?????????             : VV_Filename;          break;
//       case ?????????             : VV_enum;              break;
/* the following are not yet implemented
ftCurrency	Money field
ftBCD	Binary-Coded Decimal field
ftTime	Time field
ftDateTime	Date and time field
ftVarBytes	Variable number of bytes (binary storage)

ftAutoInc	Auto-incrementing 32-bit integer counter field
ftBlob	Binary Large OBject field
ftMemo	Text memo field
ftGraphic	Bitmap field
ftFmtMemo	Formatted text memo field
ftParadoxOle	Paradox OLE field
ftDBaseOle	dBASE OLE field
ftTypedBinary	Typed binary field
ftCursor	Output cursor from an Oracle stored procedure (TParam only)

ftWideString	Wide string field
ftADT	Abstract Data Type field

ftArray	Array field
ftReference	REF field
ftDataSet	DataSet field
ftOraBlob	BLOB fields in Oracle 8 tables
ftOraClob	CLOB fields in Oracle 8 tables
ftVariant	Data of unknown or undetermined type
ftInterface	References to interfaces (IUnknown)
ftIDispatch	References to IDispatch interfaces
ftGuid	globally unique identifier (GUID) values
*/
   return result;
}
#endif
//______________________________________________________________________________
namespace_CORN_end

