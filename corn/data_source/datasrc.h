#ifndef datasrcH
#define datasrcH

/*
Data_source is an abstract class providing a common interface for
Submitting or Retrieving data records from various data sources such as
VV_files (INI files) or dBase or other database records.

This allows programs to specify what data they are expecting to use without
having to be concerned with how they are formatted.
*/
#include "corn/const.h"
#include <string>
//______________________________________________________________________________
namespace CORN {
//______________________________________________________________________________
class Data_record; // Forward declaration.
class Generic_data_record;
class Indexer;
//______________________________________________________________________________
interface_ Data_source
{
 protected:
   virtual bool create(Data_record &/*from_record*/) = 0;                         //161013
      /**<
      create is protected because it should not be called by the end programmer.
      create is usually called by set() the first time a Data_record is set in a
      new database. When using set() and not yet created the database
      it is necessary o create the list of actual fields based on the from_record.
      The from_record VV_entries should have all the data needed to define
      fields for most databases.
      In may be called by get() (I.e. if a record to get is required to exist in the database.
      Create is optional but most derived Data_source classes will use create
      to open the file and define any necessary fields.
      **/
 public:
   inline virtual ~Data_source()                                              {}
   inline virtual bool get(modifiable_ Data_record &)           { return true; }
      /**<
      This reads data from the data source at it's current position
      (I.e. current record in the database)
      This abstract method must be provided by a derived class
      If the database is not in a state where is can read any data it returns false.
      If the database could be read, but there were not maching entries (fields)
      then it returns false.
      Returns true if any record entries could be read.
      Examine each data record entry to determine if it was encountered
      in the data source.
      Note that this abstract class ALWAYS returns false because this abstract class does not
      actually fetch the data, it only set up the the structure.
      Derived classes must return true unless the data could not be fetched.
      **/
   virtual bool get_next(modifiable_ Data_record &data_rec);
      /**< A where clause index more than one record.
         In this case, use get() to fetch the first record
         and repeat calling get_next() to fetch remaining records.
         get_next will return false if there were no
         additional records returned by the SELECT WHERE statement
         to fetch.
         Note that this abstract class ALWAYS returns false because this abstract class does not
         actually fetch the data, it only set up the the structure.
         Derived classes must return true unless the data could not be fetched.
      **/
   virtual bool set
      (Data_record &data_rec
      ,bool         append=false)                                            =0; //150502
      /**< This writes data from data_rec to this data source.
       Classes derived from Data_record may provide information such
       as the position of the record in the data file an the Data_source
       may then need to position the file pointer before writing the record.
       In classes such as VV_file, a Data_record constitutes all the data in
       a file, consequently the data can be written out to the file.
       Returns true if the record could be written.
       Returns true if there was a problem.
       This abstract method must be provided by a derived class.
       Derived Set() methods should first check if the datasource database is being created
       (I.e. it doesn't already exist, or it doesn't already have field definitions).
       Note that this abstract class ALWAYS returns false because this abstract class does not
       actually fetch the data, it only set up the the structure.
       Derived classes must return true unless the data could not be fetched.
      **/
   virtual const char *label_string(std::string &buffer)                const=0; //170423

      /** \return a text label that could be used to identify the data source being edited.
      It is left to the derived data source class how this text is formatted.
      For VV_file we simply use the fully qualified filename.
      For dBase we could use the filename and the record number  C:\path\file.dbf#123
      The result of label() should not be used to look up or identify a Data_source
      it is intended only for output to the user (I.e. in window titles)
      **/
   inline virtual void where_long(const char */*attr*/,const long  /*value*/) {}
   inline virtual void where_cstr(const char */*attr*/,const char */*value*/) {}
      /**<
       These set the current record to the first record where
       the named attribute has the specified value.
       Some datasources (i.e. SQL based) only set up the where clause
       and the record 'pointer' will not be updated until the get/set method is performed.
       This is why where() does not return any status.
       In datasources derived from index if attribute is the index key
       this will be faster search.
       If attribute is not an index key then the record will be searched one at a time.
       This should be overridden by data sources that are tables or some other
       structure that hold multiple data records.
       Single data record structures such as ini files, HTML post and queries
       do not need to implement where()
      **/
   inline virtual void where2_long
        (const char * /*attribute1*/, const long /*value1*/
        ,const char * /*attribute2*/, const long /*value2*/)                  {}
      ///< This is a where clause with two key integer attributes
   inline virtual bool delete_records()                         { return false;}
      /**<
      To select the records to be deleted, set up a where clause with
      one of the where_xxxx() functions.
      (Currently only implement in oracle data source (eventually make this a pure function).
      **/
   inline virtual bool is_modified()                affirmation_{ return false;}
   inline virtual Generic_data_record
      *render_generic_data_record()                       stream_IO_{ return 0;}
      /**<
         Normally users of a datasource will define a class derived from
         Data_record and override the expect_structure() method
         for known attributes in the table.
         However, some times it is necessary to work with Data sources where
         the structure is not known up front (I.e. Tables that are from other
         sources or whose structure may be variable).
         This method can be called to provide a Data_record that generally
         matches the attributes of the Data source's data records.
         This method returns 0 if the structure of the data records used in
         the datasource cannot be determined.
         (I.e. Variable value files cannot idenfity the structure because
          attribute type information is not stored in the file).
         Derived classes can override this method if the structure of
         the data records is stored in the data source (I.e. database tables).
      **/
 protected:
   virtual bool set_data
      (Data_record &data_rec
      ,bool         append) = 0;                                                 //150502
};
//_Data_source_________________________________________________________________/
class Data_source_abstract
: public implements_ Data_source
{
 protected:  // The following values are used when
   mutable Indexer  *indexer;    // Owned by caller (don't delete)  // may eventually move to data_source
   contribute_ bool  modified;   // set to true if any part record or structure of the datasource has been modified by this session;
 public:
   bool recurse_inheritance;                                                     //170724
      // Normally we recurse inheritance, but when loading list
      // for parameter editors we don't recursively read the inherited files.
 public:
   inline Data_source_abstract(Indexer *optional_indexer = 0)
      : indexer(optional_indexer)
      , modified(false)
      , recurse_inheritance(true)
      {}
   inline virtual bool create(Data_record &/*from_record*/)
      { modified = true; return true; }
   virtual bool get(modifiable_ Data_record &data_rec);
   virtual bool set
      (Data_record &data_rec
      ,bool         append=false);
   inline virtual bool delete_records()        { modified = true; return false;}
   inline virtual bool is_modified()              affirmation_{return modified;}
};
//_Data_source_abstract_____________________________________________2016-10-13_/
}//_namespace_CORN_____________________________________________________________/
#endif

