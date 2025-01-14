#ifndef datasetrecH
#define datasetrecH
#ifndef UED_TYPES_H
#  include "UED/library/UED_types.h"
#endif
#include "corn/dynamic_array/dynamic_array_T.h"
#include "UED/library/record.h"
#include "UED/library/datasetrec_creation.h"
#include "corn/quality.hpp"
#include "common/CS_parameter.h"
using namespace CORN;

namespace UED { // forward declarations:                                         //070718
class UED_graphs_dialog;
//______________________________________________________________________________
class _UED_DLL Data_record_header
: public Record_base
{
   // This class serves two functions:
   // It is primarly the header part of a data record (scalar vector dataset)
   // but it is also used in building indexes (which needs only the
   // variable code and time stamp info
 private: // The following contributes are used when determining record size and doing I/O
   bool IO_num_values;                                                           //971103
   bool IO_year;                                                                 //971103
   bool IO_date;    // 32 bit date timestamp                                     //971103
   bool IO_time;    // 32 bit time timestamp                                     //971103
     // if both IO_date and IO_time are specified use datetime64 timestamp       //030713
   bool IO_timestep_units_code;                                                  //971103
   bool IO_variable_code;                                                        //971103
 public:
   //180211 nat16 last_num_values;
   nat16 value_count_last;                                                       //180211(renamed)
      // contribute Needs to be in header because index uses it to index date/time range
                        // Will be 0 when creating otherwise read from the file.
   UED_units_code timestep_units_code;
   Variable_code  variable_code;
   datetime64     timestamp;                                                     //030714
 public: // constructors
   Data_record_header(Record_code rec_code);                                     //990108
   Data_record_header                                                            //990108
      (const Data_record_creation_layout &creation_layout
      ,datetime64 date_time);                                                    //030714
   Data_record_header(const Data_record_header &to_be_copied);
   virtual Data_record_header* clone()                                    const; //140116

 public: // setup for I/O
   virtual void identify_IO()                                     modification_; //990113
   virtual nat32  compute_record_body_size()                              const;
   virtual bool read_body(Binary_file_interface &parent_database);               //971103
   virtual void write_body(Binary_file_interface &parent_database);              //971103
 public: // accessors
   inline datetime64 get_time_stamp()                                     const
      { return timestamp; }
   inline datetime64 set_time_stamp(datetime64 new_timestamp)                    //030723
      { return timestamp = new_timestamp;}
   inline const UED_units_code &get_time_step_units_code()                const  //140116
      { return timestep_units_code; }
   inline Variable_code get_variable_code()      const { return variable_code;}
 public: // queries
   datetime64 get_final_time_series_date();                                      //030714
   datetime64 get_final_time_series_time();                                      //030714
   bool contains_time_step
      (Variable_code check_variable_code
      ,datetime64    check_date_time);
   inline virtual nat16 count_values()       const { return value_count_last; }  //180211
   inline virtual nat16 get_num_values()     const { return value_count_last; }  //180211 renaming to count_values
   sint8 compare_header(const Data_record_header &other)                  const; //140118
};
//_Data_record_header_______________________________________________1999-01-08_/
class Binary_data_record_cowl
: public Data_record_header                                                      //990108
{  friend class UED_graphs_dialog;                                               //971103
 private:
   Dynamic_array<float32>        &data_values_ref;                               //151111_000828
 public: // These contributes are for in memory control and not saved to the file
   contribute_ bool              index_maintained;                               //990113
 private:  // The following contributes are used when determining record size and doing I/O
   contribute_ bool              IO_units_code;                                  //971103
 public:
   UED_units_code                units_code;
 protected:
   UED_quality_attribute         attribute_homogeneous;                          //021211
 protected:
   float32                       single_value_32;
   Dynamic_array<nat8>           D_attributes;                                   //120806_000518
 private: // the following are not stored to the file
   bool                          can_consolidate_attributes;                     //060807
      // currently consolidate attributes for records that have already had attributes is disabled (it is problematic, and doesn't really save if the database is not condensed or an exactly sized record replaces).
   contribute_ Dynamic_array<nat32>  *add_count;                                 //140118
      // used with add_corresponding and finalize_average()
      // Null until initial add_corresponding and delete
      // by finalize_average.  (Values not recorded to UED file).
   contribute_ bool              auto_float16;                                   //140130
   contribute_ bool              write_float16_status_known;                     //140612
   contribute_ bool              use_float16;                                    //140612
 public:  // structors
   Binary_data_record_cowl
      (Dynamic_array<float32>        &data_values_
      ,Record_code rec_code);                                                    //990107
      // This constructor a reads a record at the current file position into curr_record.
      // The file pointer must be positioned at a record header.
      // The file pointer will bce positioned to the next record.
   Binary_data_record_cowl                                                       //151111_970909
      (Dynamic_array<float32>            &data_values
      ,const Data_record_creation_layout &creation_layout                        //990107
      ,datetime64                         date_time
      ,bool                               auto_float16); //=false
      // This constructor is used when we need to add a new record.
/* NYN might want to have a constructor that doesn't use creation_layout
   Binary_data_record_cowl                                                       //160609
      (Dynamic_array<float32> &data_values
      ,CORN::Record_code      record_type
      ,UED_variable_code      variable_code
      ,UED_units_code         stored_units_code
      ,UED_units_code         time_step_units_code
      ,UED_units_code         time_stamp_step_units_code
      ,UED_quality_attribute  default_attribute_code
      ,bool _auto_float16); //=false
      // This constructor is used when we need to add a new record.
*/
   Binary_data_record_cowl
      (Dynamic_array<float32>  &data_values_                                     //151111
      ,const Binary_data_record_cowl &to_be_copied);                             //991112
 public: // accessors
   inline virtual Record_code get_code_for_write()                        const; //140612
   datetime64 get_date_time_for_index(nat16 index)                        const; //981002
      //190105 rename get_timestamp_at

      // given an index to the values array, computes the date/time for that entry. //980904
   inline Dynamic_array<float32>   &ref_data_values()                            //151111_120804_000829
      { return data_values_ref; }
   nat16 count_data_values()                                              const; //180211 replacing data_values_get_count
   inline nat16 data_values_get_count()                                   const  //000707 renaming(deprecated)
      { return count_data_values(); }                                            //180211
      //180211  rename to count_values()
      // If data values haven't been allocated yet, they will be.
      // The use of the data array should be avoided
      // When using the returned array the user should be the one creating the
      // database and can guarentee that the array will be properly bounded!
   inline modifiable_ Dynamic_array<nat8> &get_attributes()        modification_ //120806_001126
      { return D_attributes; }
   inline const Dynamic_array<nat8> &get_attributes_const()               const //120806_001126
      { return D_attributes; }
   datetime64 get_earliest_date_time(datetime64 earliest_date_time_yet)   const; //030714
   datetime64 get_latest_date_time  (datetime64 latest_date_time_yet)     const; //030714
#ifdef UED_DUMP
   virtual void dumpX(std::ostream &txt_strm);
#endif
   virtual void write_YAML(std::ostream &YAML_strm,nat16 indent_level);          //180108

 public: // UED_base_record virtual overrides
   virtual nat32  compute_record_body_size()                              const;
   virtual bool read_body (Binary_file_interface &parent_DB)      modification_; //971103
   virtual void write_body(Binary_file_interface &parent_DB)      modification_; //971103
 public:
   virtual void identify_IO()                                     modification_; //990113
   bool has_attribute_markers()                                           const;
   inline bool attribute_valid(nat8 attribute_)                           const  //981002
      { return is_valid_quality((CORN::Quality_code)attribute_); }               //150805
   void consolodate_attributes();                                                //981002
   inline UED_units_code get_units_code()                                 const
      { return units_code; }
   int16 time_step_index(datetime64 date_time_idx)                        const;//030714
   float32 get_value
      (CORN::Quality &datum_attribute
      ,datetime64 get_date_time)                                          const;
   virtual float32 set_value
      (float32 value
         // The value units at this point must be in the record specified units
      ,CORN::Quality &attribute // was a return value                            //971104
      // The program can specify the desired attribute,
      //   UED will try to give a value for the attribute
      //   attribute will be set to the attributes available
      // UED will check the current record attribute; if the attribute
      // doesn't match it will check if individual timestep attributes
      // exist and will not overwrite values of higher quality unless
      // override safty is enabled.
      ,datetime64 set_date_time                                                  //030714
      // UED will try to give the value for the specified date and time.
      // If the date and time is not available, UED will try to estimate
      // a value (the returned attribute will be set accordingly
      // If the value cannot be estimated, the attribute will also be set accordingly
      //   If a record doesn't already exist for the time period matching the date and time,
      //   A record will be created having the specified units.
      ,bool override_safety= false);
   nat32 set_missing_as
      (float32 valid_value = 0.0
      ,CORN::Quality_code valid_quality = CORN::measured_quality);
      // Change value and quality of missing values in the record to
      // the specified value and quality.
      // Returns the number of items set to the valid value.
      // This method was to allow UED convert to ensure missing precipitation
      // is recorded as 0.0 because some source files record only
      // precipitation on days with non 0.0 values.  All other days are assumed to have no precip.
   float32 get_at(nat32  index, CORN::Quality &datum_attribute)           const; //050510
   virtual float32 set_at                                                        //000323
      (nat16 index
      ,float32 value  // The value units at this point must be in the record specified units
      ,CORN::Quality  &attribute // was a return value                           //971104
         // The program can specify the desired attribute,
         //   UED will try to give a value for the attribute
         //   attribute will be set to the attributes available
      ,bool override_safety= false);                                             //000710
         // UED will check the current record attribute; if the attribute
         // doesn't match it will check if individual timestep attributes
         // exist and will not overwrite values of higher quality unless
         // override safty is enabled.
      // Set at is at a lower level than test value (used for data sets only)
   virtual CORN::Quality_code set_quality_at                                     //140615
      (nat16 index
      ,const CORN::Quality  &attribute);
      // This is used by the quality import_export utility.
   virtual bool get_parameter_at                                                 //151119
      (modifiable_ CS::Parameter &parameter
      ,nat32  index)                                                      const;
   virtual bool set_parameter_at                                                 //151221
      (const CS::Parameter &parameter
      ,nat32  index)                                              modification_;
   virtual bool get_parameter_on                                                 //151119
      (modifiable_ CS::Parameter &parameter
      ,datetime64 get_date_time)                                          const;
   virtual bool set_parameter_on                                                 //151221
      (const CS::Parameter &parameter
      ,CORN::datetime64 get_date_time)                            modification_;
/*151122 conceptual
   virtual bool setup_parameter_reference_at                                     //151119
      (modifiable_ CS::Parameter_cowl_T<float32> &parameter
      ,nat32  index)                                                      const;
   virtual bool setup_parameter_reference                                        //151119
      (modifiable_ CS::Parameter_cowl_T<float32> &parameter
      ,datetime64 get_date_time)                                          const;
*/
 public: // utility methods
   virtual nat16 get_num_values()                                         const;
   bool get_creation_layout(Data_record_creation_layout &creation_layout);       //050503
      //    This can be used to copy records.
   virtual bool is_valid()                                         affirmation_; //050503
      // is_valid() is overridden by UED data set record so we don't have to write records where all data is invalid.
   sint8 compare_meta(const Binary_data_record_cowl &other)               const; //151111_140118
   nat32 add_corresponding
      (const Binary_data_record_cowl &addend)                     modification_; //151111_140118
      // adds values from the addend's values to this (augend)
      // The number of additions performed in each item in the record is
      // recorded is additions.  This is used when average is performed.
      // Currently the data record signature must match:
      // the augend and the added records represent the same variable
      // Currently the records must have the same timestamp and
      // must cover the same period of time.
      // (In the future I may relax this requirement allowing
      //  different periods to be joined.
      // \return the number of elements that were actually added.
      // The may not necessarily be the number of corresponding items
      // (I.e. some corresponding items in both records may be invalid)
      // \return 0 if the records do not correspond.
      // Invalid quality values in this will be set to the addend if
      // the addend value is valid.
      // Invalid values in the added are not included
      // The augend keeps track of the count of additions or each corresponding
      // item. (Initially these counts are 1 (if this record's corresponding
      // initial value is valid).  These counts are used to calulate averages.
   nat32 finalize_average()                                       modification_; //140118
      // Uses the counts and the current values accumulated from previous
      // calls to calculate average values for each corresponding item.
      // The record's data values are replaced with the corresponding average value.
      // After the averages a computed, the count array is deleted.
      // If there had been no previous calls to add_corresponding
      // (and thus no add_count array) this method does nothing.
      // returns the number of items averaged.
      // The add_count array is reset.  Because the count is reset,
      // any subsequent additions will result in skewed averages
      // in any subsequent calls to 'finalize_average'.
      // So use  finalize_average only once after completing all averages.
    Dynamic_array<nat32>* provide_add_count()                     contribution_;
 protected:
   virtual bool read_data_values_float32(Binary_file_interface &parent_database)   stream_IO_;//140129
   virtual bool write_data_values_float32(Binary_file_interface &parent_database,nat16 num_values)  stream_IO_;//140129
   virtual bool read_data_values_float16(Binary_file_interface &parent_database)   stream_IO_;//140129
   virtual bool write_data_values_float16(Binary_file_interface &parent_database,nat16 num_values)  stream_IO_;//140129
   virtual bool do_all_float16_values_have_acceptable_precision()  affirmation_;
 public:
   inline bool set_automatic_float16(bool enable)                  contribution_ //140130
      { return auto_float16 = enable; }
      /**< By default dataset records are store with 32bit floats
         This option will automatically store numbers using 16bit floats
         with loss of precision if the 16 bit number does not loose
         more than the required precision for the variable.
      **/
   inline virtual bool is_auto_float16()                            affirmation_ //140130
      { return auto_float16; }
   virtual bool provide_write_float16_status()                       provision_; //140612
   //check needed virtual bool make_abstract() = 0;
};
//_Binary_data_record_cowl_____________________________________2015-11-11_1997_/
class Binary_data_record_clad
: public extends_ Binary_data_record_cowl
{
 protected:
   Dynamic_array<float32>        data_values_owned;
 public:
   Binary_data_record_clad(Record_code rec_code);
   // This constructor a reads a record at the current file position into curr_record.
   // The file pointer must be positioned at a record header.
   // The file pointer will bce positioned to the next record.
   Binary_data_record_clad
      (const Data_record_creation_layout &creation_layout
      ,datetime64 date_time_
      ,bool auto_float16_); //=false
   Binary_data_record_clad
      (const Binary_data_record_clad &copy_from);
   // This constructor is used when we need to add a new record.
 public: // interface implementations
   virtual Binary_data_record_clad *clone()                               const;
   inline virtual bool make_abstract()                         { return false; }
};
//_Binary_data_record_clad__________________________________________2015-11-11_/
class _UED_DLL Vector_data_record
: public extends_ Binary_data_record_clad
{
   // values store vector data, not a data set
public:
   inline Vector_data_record(Record_code _rec_code)                              //990108
   : Binary_data_record_clad(_rec_code) //length not yet determined              //990108
      {}                                                                         //990108
   Vector_data_record(const Vector_data_record &to_be_copied);                   //991121
   virtual Vector_data_record *clone()                                    const; //140116
   inline bool has_attribute_markers()                                    const
      { return false;}
   // There are no attributes for the vector data,
   // attribute applies to each item.
   inline nat16 get_vector_size()                                         const
      { return get_num_values(); }
   float32 set(nat16 index,float32 value,CORN::Quality &attr)     modification_;
};
//_Vector_data_record_______________________________________________1998-09-04_/
}//_namespace UED______________________________________________________________/
#endif
//datarec.h

