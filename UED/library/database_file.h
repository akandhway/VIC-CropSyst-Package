#ifndef database_fileH
#define database_fileH
#include "UED/library/UED.h"
#ifndef binary_record_fileH
#  include "corn/format/binary/binary_record_file.h"
#endif
#ifndef unitconvH
#  include "corn/measure/unitconv.h"
#endif
#ifndef units_defH
#  include "UED/library/units_def.h"
#endif
#ifndef indexH
#  include "UED/library/index.h"
#endif
#include "corn/chronometry/date_I.h"
#ifndef enumlistH
#  include  "corn/container/enumlist.h"
#endif
#include "corn/dynamic_array/dynamic_array_T.h"
#include "common/CS_parameter.h"
#include "UED/library/variable_format.h"
//_Forward declarations_________________________________________________________
class Geolocation;
//_______________________________________________________Forward declarations__/
// Universal Environmental Database accessor class.                              //970305
// Forward declartions
namespace CORN
{  class Bidirectional_list;
   class Text_list;                                                              //091021
}
//_____________________________________________________________________________
namespace UED {
//______________________________________________________________________________
class _UED_DLL
  Database_file                                                                  //070718
: public Binary_record_file_dynamic_indexed                                      //000705
, public Units_convertor                                                         //030228
{
   friend class Database_file_indexed;
   friend class UED_variable_definition_format_0_record;                         //990114
   friend class UED_units_definition_format_0_record;                            //990114
 public:
   Record_base         *current_nondata_rec;                                     //971104 eventually rename to current_record
   Geolocation_record  *geolocation_rec_in_memory;                               //070618
      //Only an optional reference (owned by binary_record_file_dynamic_index)
 private:
   contribute_ Binary_data_record_cowl   *current_data_rec;                      //971104
   bool        auto_float16;                                                     //140612
 public: // enumerations
   std::string compose_standard_variable_description(UED::Variable_code var_code);//971106
 public:
   virtual Binary_record *append_record                                          //000706
      (Binary_record  *record_to_append,bool write_immediately)     submission_;
   virtual Binary_record *instanciate_record(Record_code code)            const; //990114
   // This method should be overridden by classes derived from Database_file
   // inorder for it to recognize and properly load any new record types.
   // Overridden instanciate_record() methods should call Database_file::instanciate_record()
   // or parent classes derived from Database_file inorder to recognize all record types.
 public:
   Database_file                                                                 //000711
      (const char *ued_filename
         // the filename may null indicating that
         // records are to be kept in memory and will not be written to a file.
      ,std::ios_base::openmode _openmode                                         //130311
      ,bool _auto_float16);                                                      //140612
   inline virtual ~Database_file() {}                                            //081129
   virtual bool initialize()                                    initialization_; //140119_990112
   virtual bool done_with_dates_before(const CORN::Date_const &earliest_date);   //141103
   bool current_data_record_is_target                                            //990119
      (Record_code   target_rec_code
      ,Variable_code target_var_code);
   virtual float32 get_value                                                     //970921
      (Variable_code variable
      ,const Time_query &time_query                                              //990118
         // UED will try to give the value for the specified date and time.
         // If the date and time is not available, UED will try to estimate
         // a value (the returned attribute will be set accordingly
         // If the value cannot be estimated, the attribute will also be set accordingly
      ,CORN::Units_code      &units
         // The program can specify the desired units.
         // UED will try to convert to those units, if it can't
         // units will be set  to the units available
      ,CORN::Quality  &attribute
         // The program can specify the desired attribute,
         // UED will try to give a value for the attribute
         // attribute will be set to the attributes available
      );
   virtual float32 set_value  // was set
      (float32 value
      ,const Time_query &time_query                                              //990118
         // UED will try to give the value for the specified date and time.
         // If the date and time is not available, UED will try to estimate
         // a value (the returned attribute will be set accordingly.
         // If the value cannot be estimated, the attribute will also be set accordingly.
         // If a record doesn't already exist for the time period matching the date and time,
         // a record will be created having the specified units.
      ,CORN::Units_code      &units
         // The program can specify the units of the variable.
         // UED will try to convert the value to the units the data is stored in
         // if it can't figure out how to do that, set fails returning false.
         // Note that units conversion is a feature of the
         // UED database class, not the record itself.
      ,CORN::Quality  &attribute    // must be a returned value                  //000710
         // The program can specify the desired attribute,
         // UED will try to give a value for the attribute
         // attribute will be set to the attributes available
         // UED will check the current record attribute; if the attribute
         // doesn't match it will check if individual timestep attributes
         // exist and will not overwrite values of higher quality unless
         // override safty is enabled.
      ,const Data_record_creation_layout &creation_layout
         // If a time series record doesn't exist for the specified
         // variable code with the specified date and time, a new record
         // will be created using this layout.
      ,bool  &record_created
      ,bool override_safety // = false
      );
   virtual bool set_vector                                                       //980925
      (const Dynamic_array<float32> &values                                      //120804
      ,datetime64 date_time                                                      //030714
         // UED will try to give the value for the specified date and time.
         // If the date and time is not available, UED will try to estimate
         // a value (the returned attribute will be set accordingly
         // If the value cannot be estimated, the attribute will also be set accordingly
         // If a record doesn't already exist for the time period matching the date and time,
         // A record will be created having the specified units.
      ,CORN::Units_code    &units
         // The program can specify the units of the variable.
         // UED will try to convert the value to the units the data is stored in
         // if it can't figure out how to do that, set fails returning false.
         // Note that units conversion is a feature of the
         // UED database class, not the record itself.
      ,CORN::Quality  &attribute    // must be a returned value                  //000710
         // The program can specify the desired attribute,
         // UED will try to give a value for the attribute
         // attribute will be set to the attributes available
         // UED will check the current record attribute; if the attribute
         // doesn't match it will check if individual timestep attributes
         // exist and will not overwrite values of higher quality unless
         // override safty is enabled.
      ,const Data_record_creation_layout &creation_layout
         // If a time series record doesn't exist for the specified
         // variable code with the specified date and time, a new record
         // will be created using this layout.
      ,bool  &record_created                                                     //000405
      );
   virtual int16 get_vector                                                      //980928
      (float32 vector_data[]
      ,Record_code rec_type                                                      //990118
      ,Variable_code  variable
      ,const Time_query &time_query                                              //990118
         // UED will try to give the value for the specified date and time.
         // If the date and time is not available, UED will try to estimate
         // a value (the returned attribute will be set accordingly
         // If the value cannot be estimated, the attribute will also be set accordingly
      ,CORN::Units_code      &units
         // The program can specify the desired units.
         // UED will try to convert to those units, if it can't
         // units will be set  to the units available
      ,CORN::Quality  &attribute
         // The program can specify the desired attribute,
         // UED will try to give a value for the attribute
         // attribute will be set to the attributes available
      ,bool  &record_created);                                                   //000405
         // Returns the size of the vector
         // The vector data is copied into buffer
   virtual bool get_parameter_on                                                 //151119
      (modifiable_ CS::Parameter &parameter
      ,Variable_code     variable
      ,const Time_query &time_query
      ,Record_code )                                                      stream_IO_; //190122const;
      // although rec_type is not used here, it is used in derived classes
   virtual bool set_parameter_on                                                 //151221
      (const CS::Parameter &parameter
      ,Variable_code     variable
      ,const Time_query &time_query
      ,Record_code       rec_type);
 public: // Annotation functions
   const  char *get_variable_description                                         //070223
      (Variable_code var_code
      ,std::string &description);                                                //170424
         // This method returns (in description) a description of the variable
         // This is a function of Database_file because new codes can be defined within the database.
   const Variable_format_abstract *get_variable_definition(Variable_code var_code) modification_;   //160620_010216
      ///< \return a variable definition for the selected code,
      /// Either from variables defined in the database or standard definitions.
      /// The returned variable definition is read only!
   nat16 get_variable_definitions
      (Variable_format_abstract *variable_definitions[]                          //160620
      ,nat16 max_allocation)     ; //                                     provision_;
      ///< Fill the provided array with pointers to the current variable definitions.
      /// The max_allocation is the maximum array size.
      /// The variable definitions are provided (loaded in memory) as needed.
      /// \return the number of variable definitions filled into the array.
      /// (The remaining pointers in the array are cleared to 0).

   nat16 get_defined_variables_codes(nat32 variable_codes[],nat16 max_alloc);    //140922
   nat16 get_defined_variables_codes(CORN::Dynamic_array<nat32>&variable_codes); //190102

   std::string &get_units_description(CORN::Units_code units_code,bool abbreviated = true);   //980818
   // This method returns (in buffer) a description of the units
   // This is a function of Database_file because new codes can be defined within the database.
   Units_definition *get_units_definition
      (CORN::Units_code units_code,bool abbreviated = true);                     //980818
   // This method returns a units definition for the selected code,
   // Either from units defined in the database or standard definitions.
   float32 generalized_conversion_factor                                         //990508
   (int16 from_general_sub_units
   ,int16 to_general_sub_units);
 public: // Periods
   const Period_record * set_period(const Period_record &copy_from_period);      //041011
   //This modifies or adds a period with the specified description and attributes.
   //Arguments for options that are not used may be 0
   //Existing periods with the same start/end dates,
   //options and options values and will be updated,
   //will get an updated period_description.
   //Returns a pointer to either the modified matching period record, or the new period record.
   //returns 0 if could not be set.
   const Period_record *get_period                                               //050120
      (nat32         consider_options
      ,datetime64    at_date
      ,nat32         application_code = 0
      ,nat32         enumeration      = 0
      ,nat32         index            = 0
      ,Variable_code variable_code    = 0);

      // actually I think should be stream_IO_

   // Searches the database for a period the date falls with in the period
   // Returns the period record.
   // If the period's date matches and the periods options
   // specify application code, enumeration, and/or index
   // these specified parameters will must also match.
   // The consider_options bit mask specifies the options that must match
   // It does not specify that the records options bit mask must be identical!
   // This returns the first period encountered that has a date time that matches the time query,
   // options and values for selected options.
   // Return 0 if no matching period is found.                                     041011
   nat32  copy_periods_to_database(Database_file &target_database);              //050125
   // This copies all the listed periods from this database to the specified target database.
   // Returns the number of periods copied.
 public:  // The following function provide a database interface similar to CORN_dbase
   Record_base *goto_BOF()                                           stream_IO_; //070209
   // This has been provided for backward compatibility
   // use read_record(istream::beg);
   // This method goes to the beginning of the file and reads the first record into the current record buffer.
   // The previous current record is first moved to aux.
   // The file pointer is positioned back to the beginning of the record (BOF).
   // Returns the record code
   Record_base *goto_next()                                          stream_IO_; //970915
   //    This method advances the file pointer to the next record and reads the record.
   //    The current record is first swapped to aux.
   //    The next record is determined by the current record's size of curr_record + header bytes.
   //    If there isn't current record, then it does a goto_BOF() instead
   //    The file pointer is positioned back to the beginning of the record.
   //    Returns the record code.
   Record_base *goto_next_record_of_type(Record_code record_type)    stream_IO_; //120509_991210
   // This goes to the next record of the specified type
   Binary_data_record_cowl *goto_next_data_record()                  stream_IO_; //990430
   // Similar to goto next, but skips non data record.
   // Returns pointer to the retrieved datarecord or 0 if no more data record encountered.
 public:   // The following methods force the database to write various local definitions
   virtual bool write_general_comment
      (const char * general_comment);                                            //971208
      ///< This is a optional comment that may occur anywhere in the document.
      /// There may be more than one comment records.
      /// The comment will not be added if the same comment already appears in the file.
      /// \return true if the comment was added, false if the comment not unique.
   virtual bool write_generating_application                                     //971208
      (nat16 version,const char *application_name);
      ///< This is a optional comment identifying the application that is generating the database file.
      /// This method will usually only be called once.
      /// The version can be any number used by the application
      /// For example the upper byte could be the version number and the lower byte the release.
      /// There may be more than one of these record
      /// \return true if the record was added, false if the a record with version and application name already exists in the file
   virtual bool write_database_description
      (const char * database_description);                                       //971208
      ///< This is a comment describing the application.
      /// There can be multiple calls to write_database_description
      /// The first occurance shall be considered the major title;
      /// any subsequence descriptions shall be considered subtitles.
      /// \return true if the record was added, false if a record with description already exists in the file.
   virtual void write_end_definitions_marker();                                  //971207
   // After writting definitions to the database, we will output
   // the end definitions marker record to indicate that there
   // will be no definitions in the file after this point.
   // This allows the database class when reading the file as input
   // to stop looking for definition records.
   // Otherwise the database object will read the entire file looking
   // for definition records.
   void include_var_code_def
      (const Variable_format_0  &variable_def);                                  //990426
   void include_var_code_def_ontological                                         //160620
      (const Variable_format_ontological &variable_def);
   void include_units_code_def
      (const Units_definition &units_def);                                       //990426
   // These include methods will output variable code definition records
   // for the specified variable/units codes.
   // The definitions will not be duplicated if duplicated codes are encountered.
/*190515
   Geolocation_record  *get_geolocation_record()                     stream_IO_; //050429
      //180214 rename this to mod_geolocation_record
   const Geolocation_record  *get_geolocation_record_immutable()     stream_IO_; //050429
      //180214 rename this to ref_geolocation_record
*/
   Geolocation_record  *mod_geolocation_record()                     stream_IO_; //050429
      //180214 rename this to mod_geolocation_record
   const Geolocation_record  *ref_geolocation_record()               stream_IO_; //050429
      //180214 rename this to ref_geolocation_record
   // Returns the location record (if present)
   // otherwise return 0 indicating no location record is present.
   // The returned record is owned by the this, the caller must not delete it, but can update it.
   Geolocation_record *take_geolocation_record(Geolocation_record *geo_location_record) modification_; //050523
   // This replace/updates any existing location record, or appends the location record if not present.
   // The location record is relinquished to this UED.
   // Returns the passed location_record if successful, otherwise return 0.
   const Geolocation_record *know_geolocation(const Geolocation &geo_location) modification_;          //050523
   // This replace/updates any existing location record, or appends the location record if not present.
   // Returns the existing or rendered location_record if successful, otherwise return 0.

   Geolocation_record  *provide_geolocation_record()                 stream_IO_; //140606
   // Gets the geolocation record.
   // If doesn't exists it creates one.

   nat16 get_descriptions (CORN::Text_list &descriptions)            stream_IO_; //091021
   nat16 get_comments     (CORN::Text_list &comments)                stream_IO_; //091021
   const Generating_application_record *get_generating_application_record() stream_IO_;
 public: // identification method overrides
   inline virtual nat16 get_BOF_code()                                     const { return UED_marker_BOF; } //990108
   inline virtual nat16 get_EOF_code()                                     const { return UED_marker_EOF; } //990108
   inline virtual nat16 get_free_code(Record_code /*current_code*/)        const { return UED_free_space; } //000703
      // Currently I only have the option of marking the record block as free
      // Eventually I will want to have a deleted bit in the header
      // so records could be undeleted

   virtual nat32 free_records_for_period(CORN::Year start_year, CORN::Year end_year); //100105
      /**< Delete all data records within the time period
         Records that have dates outside this period are not deleted.
         (data values of records that are not deleted but that are
         within the range of years are not delete and not invalidated)
      **/
   virtual nat32 free_records_for_period_variable
      (CORN::Year start_year, CORN::Year end_year,Variable_code variable_code);  //110601
      /**< Delete all data records within the time period
         having the specied variable code
         Records that have dates outside this period are not deleted.
         (data values of records that are not deleted but that are
         within the range of years are not delete and not invalidated)
      **/
};
//_Database_file_______________________________________________________________/
class _UED_DLL
Database_file_indexed : public Database_file                                     //070718
{
  friend class UED_viewer_options_dialog;  //<- uses indexes to display available variables  //971101
  friend class UED_graphs_dialog;          // These are not in the UED namespace!//971103
protected:
   mutable Variable_index_list indexes;
public:
  // The following functions return the file position for a record
  // The specified year, date, or time
  // 0 is returned if there is no record containing the specified year.
  // (0 is ok because the record at file pos is always a header)
   virtual float32 set_value // was set                                          //000710
      (float32              value
      , const Time_query   &time_query                                           //990118
      // UED will try to give the value for the specified date and time.
      // If the date and time is not available, UED will try to estimate
      // a value (the returned attribute will be set accordingly
      // If the value cannot be estimated, the attribute will also be set accordingly
      //   If a record doesn't already exist for the time period matching the date and time,
      //   A record will be created having the specified units.
      ,CORN::Units_code    &units
      // The program can specify the units of the variable.
      //   UED will try to convert the value to the units the data is stored in
      //   if it can't figure out how to do that, set fails returning false.
              //   Note that units conversion is a feature of the
              //   UED database class, not the record itself.
      ,CORN::Quality       &attribute    // was a returned value                 //000710
      // The program can specify the desired attribute,
      //   UED will try to give a value for the attribute
      //   attribute will be set to the attributes available
      // UED will check the current record attribute; if the attribute
      // doesn't match it will check if individual timestep attributes
      // exist and will not overwrite values of higher quality unless
      // override safty is enabled.
      ,const Data_record_creation_layout &creation_layout
      // If a time series record doesn't exist for the specified
      // variable code with the specified date and time, a new record
      // will be created using this layout.
      ,bool                &record_created                                       //000405
      ,bool                 override_safety = false);
      // Returns the value as set (if value could not be safetly replaced)
      // the value as fixed in the database will be returned.
   virtual float32 get_no_create                                                 //000518
      (Record_code  rec_type                                                     //040125
      ,Variable_code       variable
      ,const Time_query   &time_query                                            //990118
      ,CORN::Units_code   &requested_units
      ,CORN::Quality      &attribute)                                stream_IO_;
   virtual bool set_vector                                                       //980925
      (const Dynamic_array<float32> &values                                      //120806
      ,datetime64 date_time  // Created vectors occur only at specific types     //030714
      // UED will try to give the value for the specified date and time.
      // If the date and time is not available, UED will try to estimate
      // a value (the returned attribute will be set accordingly
      // If the value cannot be estimated, the attribute will also be set accordingly
      //   If a record doesn't already exist for the time period matching the date and time,
      //   A record will be created having the specified units.
      ,CORN::Units_code &units
      // The program can specify the units of the variable.
      //   UED will try to convert the value to the units the data is stored in
      //   if it can't figure out how to do that, set fails returning false.
              //   Note that units conversion is a feature of the
              //   UED database class, not the record itself.
      ,CORN::Quality  &attribute    // was a returned value                      //000710
      // The program can specify the desired attribute,
      //   UED will try to give a value for the attribute
      //   attribute will be set to the attributes available
      // UED will check the current record attribute; if the attribute
      // doesn't match it will check if individual timestep attributes
      // exist and will not overwrite values of higher quality unless
      // override safty is enabled.
      ,const Data_record_creation_layout &creation_layout
      // If a time series record doesn't exist for the specified
      // variable code with the specified date and time, a new record
      // will be created using this layout.
      ,bool  &record_created)                                      modification_;//000405
   int16 get_vector                                                              //980928
      (float32 vector_data[]
      ,const Time_query &time_query                                              //990118
      ,CORN::Units_code      &requested_units
      ,CORN::Quality &attribute
      ,const Data_record_creation_layout &creation_layout
      ,bool  &record_created);                                                   //000405
      // This form of get vector allows the vector to be created if not already in the database
   virtual int16 get_vector                                                      //980928
      (float32           vector_data[]
      ,Record_code       rec_type                                                //990118
      ,Variable_code     variable
      ,const Time_query &time_query                                              //990118
      // UED will try to give the value for the specified date and time.
      // If the date and time is not available, UED will try to estimate
      // a value (the returned attribute will be set accordingly
      // If the value cannot be estimated, the attribute will also be set accordingly
      ,CORN::Units_code &units
      // The program can specify the desired units.
      //   UED will try to convert to those units, if it can't
      //   units will be set  to the units available
      ,CORN::Quality  &attribute
      // The program can specify the desired attribute,
      //   UED will try to give a value for the attribute
      //   attribute will be set to the attributes available
      ,bool  &record_created);                                                   //000405
   // Returns the size of the vector
   // The vector data is copied into buffer
   // This form of get_vector doesn't allow the vector to be created.

   virtual bool get_parameter_on                                                 //151122
      (modifiable_ CS::Parameter &parameter
      ,Variable_code     variable
      ,const Time_query &time_query
      ,Record_code       rec_type)                                   stream_IO_; //160331
   virtual bool set_parameter_on                                                 //180808
      (const CS::Parameter &parameter
      ,const Time_query &time_query
      ,const Data_record_creation_layout &creation_layout);
   virtual Binary_record *append_record                                          //000706
      (Binary_record *record_to_append,bool write_immediately)      submission_;
   virtual Binary_data_record_cowl *locate_data_record                           //990114
      (Record_code  rec_type
      ,Variable_code var_code
      ,const Time_query &time_query)                                 stream_IO_; //990118
   Binary_data_record_cowl *locate_or_create_data_record                         //990111
      (const Time_query &time_query                                              //990118
      ,const Data_record_creation_layout &creation_layout
      ,bool  &record_created);                                                   //000405
   Dynamic_array<float32> * get_record_buffer_array                              //120806_990427
      (Variable_code var_code
      , const Time_query &time_query                                             //990118
      ,Dynamic_array<nat8> &attributes                                           //120806_000818
      ,Record_type_distinguisher_interface::time_stamp_codes time_step_resolution); //010525
   virtual datetime64 get_earliest_date_time                                     //970915
      ( Record_code   for_record_type_code = UED_FOR_ALL_RECORD_TYPES
      , UED::Variable_code for_var_code = UED_FOR_ALL_VARIABLES)          const; //170323
   virtual datetime64 get_latest_date_time                                       //970915
      ( Record_code   for_record_type_code = UED_FOR_ALL_RECORD_TYPES
      , Variable_code for_var_code = UED_FOR_ALL_VARIABLES)               const; //170323
 public:
   Database_file_indexed                                                         //070718
      (const char *ued_filename
      ,std::ios_base::openmode _openmode = std::ios::in                          //130311
      ,bool _auto_float16 = true);                                               //140612
   inline virtual ~Database_file_indexed()                                    {} //081129
   virtual bool initialize();                                                    //140119_990112
   virtual bool finalize();
      // deprecated finish renamed to finalize.

   //NYI void get_variable_definitions(CORN::Bidirectional_list &definition_list, bool layered_variables = false);
      // Appends to definition_list (usually initially empty,
      // Variable definitions of those records that are indexed.
};
//_Database_file_indexed_______________________________________________________/
}//_namespace UED______________________________________________________________/
#endif

