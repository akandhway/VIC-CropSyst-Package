#ifndef binary_record_fileH
#define binary_record_fileH

#ifndef bi_listH
#  include "corn/container/bilist.h"
#endif
#  include "corn/format/binary/binary_file_interface.h"
#ifndef binary_recordH
#  include "corn/format/binary/binary_record.h"
#endif
//______________________________________________________________________________
namespace CORN {
// Binary record format class is the primitive binary file I/O used for chuck based formats.

// This probably should be defined from a more general Binary_chunk_file format class.
// May eventually have a format derived from this for SDXF


// This is use in files such as  Lotus and Biff (Excel) file formats and UED.
//   Now allows for record body length of either 16 or 32 bits
// Spreadsheets use 16 bit, UED uses 32bits for body size indicator.
//______________________________________________________________________________
class Binary_record_file_dynamic
{
 protected:
   std::string *filename_BRF;                                                    //140119_050522
      // The filename may be null indicating the binary records
      // are kept in memory and not written to a file.
   mutable Binary_file_interface  *binary_file;                                  //070628
      // The file may be null indicating the records are kept in memory.
   //140119 char *filename_BRF;                                                           //050522
 protected:
   nat16  BOF_code;                                                              //030208
   nat16  EOF_code;                                                              //030208
   nat16  free_code;                                                             //030208
 protected:
   contribute_ bool     at_eof;
   bool     big_body_sizes;  // If true indicates the default body can be stored up to 2^32 bytes. 990106
      // 081010 Eventually replace this with a mode
      // I.e.  2^8 or 2^16 or 2^32
      // Also allow each chunk to indicate the body length indicator size
   std::ios_base::openmode openmode;                                             //130311
       // (std::ios_base::in | std::ios_base::out | std::ios_base::binary)
   bool     big_endian; // need to store here now because we may now open and close the file dynamically //050522
      // move big_endian to abstract Binary_file_abstract
 public:
   Binary_record_file_dynamic
      (const char *_filename
      ,bool _big_body_sizes   // True if record header size field is 32bits
      ,std::ios_base::openmode _openmode /* = (std::ios_base::in | std::ios_base::out)*/ //130311
      ,bool _big_endian);       // Most MS-DOS or Windows base software files are not big_endian
   virtual ~Binary_record_file_dynamic();                                        //040104
 public:  // accessors
   inline bool is_big_body_sizes() { return big_body_sizes; }
   inline bool is_read_only()
      { return openmode == std::ios_base::in; }                                  //130311
 public:  // reading methods
   virtual Binary_record *instanciate_record(Record_code code)          const=0; //990114
      // This must be provided by derived classes.
      // This will create an instance of a record of type specified by code
      // and read the data from the file from the current file position.
      // The record is return
 public:  // identification methods
   virtual nat16 get_free_code(Record_code current_code)                  const; //000703
      // Some database formats allow for deleted records (free spaces) to exist
      // in the database file (this is convenient for dynamic variable sized records)
      // when the record size changes, the record will be stored at the
      // end of the file (or perhaps some free deleted area in the file).
      // And the old position in the file will be marked as deleted.
      // Current codes is pass and will be returned if get_free_code is not overridden
   virtual void reset();                                                         //000703
      // Positions file pointer to beginning of the file.
   virtual inline const char *get_file_name()                              const //001206
      {  return filename_BRF ? filename_BRF->c_str() : 0; }                      //140119
   Binary_file_interface *provide_binary_file()                      provision_; //070628
   void done_with_stream();                                                      //050522
};
//_Binary_record_file_dynamic_______________________________________1998-10-15_/
class Binary_record_file_dynamic_indexed
:public Binary_record_file_dynamic
{
protected:
   Unidirectional_list index;                                                    //100127
   bool initialized;                                                             //000705
   Binary_record_index_node_dynamic *index_node;                                 //030710
public:
   Binary_record_file_dynamic_indexed
      (const char *filename                                                      //000211
      ,bool i_big_body_sizes   // True if record header size field is 32bits
      ,std::ios_base::openmode _openmode                                         //130311
         // (std::ios_base::in | std::ios_base::out)
      ,bool big_endian);     // Most MS-DOS or Windows base software files are not big_endian
   virtual ~Binary_record_file_dynamic_indexed();                                //040104
      // Traverse the B.R. index nodes and commit any modified records.
      // Any records that current size does not match the size in the
      // File will be appended to the end of the file as a new record and the record
      // code will be changed to deleted.
   virtual Binary_record *append_record
      (Binary_record *record_to_append,bool write_immediately)      submission_; //000706
      //160609 rename to submit_record because we take ownership
      // of the record, but the caller may keep reference to it

   virtual bool initialize();                                                    //140119
   virtual void reset();                                                         //000703
      // Move to the first B.R.index node
      // call B.R._file reset.
   virtual Binary_record *read_record(std::fstream::seekdir from=std::fstream::cur);   //000703
      // If from is beg then the file is reset.
      // if from is curr read continues from current position.
      // current B.R._index_node record will be returned (loaded if not in memory).
    virtual Binary_record  *read_record_of_type
         (Record_code rec_code, std::fstream::seekdir from = std::fstream::cur); //070209
       // If from is beg then the file is reset.
       // if from is curr read continues from current position.
       // scan from the current B.R._index_node until one if found with a matching record type code.
       // record will be returned (loaded if not in memory).
   Binary_record * replace(Binary_record *existing_record
      , Binary_record *with_record,bool free_existing_record=true);              //000710
      // This replaces an existing record with the specified record
      // If if the replacement is not the same size as the original,
      // the existing record space will be freed in the file and with_record append.
      // The existing record must be just previously read (with read_record) or appended and not finished_with().
      // If free_existing_record is true, the existing record will be relinquished and returned.
      // Otherwise it will be deleted and 0 returned.
      // If the record could not be found 0 is returned.
   void finished_with(Binary_record *record);                                    //000703
      // This method is used when a database or program indicates it no longer needs
      // to keep record in memory.
      // traverse the list of B.R. index nodes, find the one having the matching record pointer.
      // call that node's finish() method.
      // Record will be deleted so the calling object must never reference
      // the record memory again.
   Binary_record *relinquish(Binary_record *record);                             //000703
      // This is used when the application is going to keep the
      // record data locally it will no longer be pointed to by R.B. index
      // So the record in the corresponding record node list
      // will be replace with just the header info (it will not be deleted).
      // The object the record is relinquished to is responsible for
      // deleting the record data structure.
   Binary_record_index_node_dynamic *get_index_node_for(const Binary_record *record);  //000706
      // This is used when a derived class database wants to maintain an index
      // of records base on some search criteria.  The derived index should
      // keep pointers to index nodes not the record itself, so that the
      // update and I/O of the record and database is done by this class not the derived class.
      // This method will return 0 if record is not found.
      // Normally you should only call this method immediately after appending
      // the record to the database so it should always find the index_node.
   nat32 get_position_of(const Binary_record *record)                     const;
   virtual bool finalize/*190717 finish*/();                                     //000828
      // This flushes all the records to the file.
      // Record bodies are removed from memory.
   virtual void dumpX(std::ostream &txt_strm,bool full_record_dump);
      // This is used to debug a database.
   virtual void write_YAML
      (std::ostream &YAML_strm,nat16 indent_level,bool full_record_dump);        //180108
   virtual Binary_record *find_duplicate_record
      (const Binary_record *record_to_find)                        performs_IO_; //091021
private:
   contribute_ Container::Iterator *read_interator;                              //100128
   Container::Iterator *provide_read_interator()                     provision_; //100128
};
//_Binary_record_file_dynamic_indexed_______________________________1999-01-07_/
}//_namespace_CORN_____________________________________________________________/

#endif

