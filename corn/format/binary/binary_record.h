#ifndef binary_recordH
#define binary_recordH

#include "corn/primitive.h"
#include "corn/const.h"
#include "corn/container/container.h"
#include "corn/format/binary/binary_types.h"
//______________________________________________________________________________
namespace CORN {
interface_ Binary_file_interface; // Forward declaration                         //070622
//______________________________________________________________________________
class Binary_record_header
{
public: // temporary made public, should use an accessor
   Record_code rec_code;
protected:
   nat32      rec_length;
      // there is the option of using 16 or 32 bits for length field,
      // in memory it is 32bits
   contribute_ bool modified;                                                    //100519
public:
   inline Binary_record_header
      (Binary_file_interface &in_file                                            //000129
      ,bool big_body_sizes )
      :rec_code   (0)
      ,rec_length (0)
      ,modified   (false)
      {  read_header(in_file,big_body_sizes); }
   inline Binary_record_header
      (Record_code i_rec_code
      ,nat32 i_length)
      : rec_code  (i_rec_code)
      , rec_length(i_length)
      ,modified   (false)
      {}
   inline Binary_record_header(const Binary_record_header &from_copy)            //050128
      : rec_code  (from_copy.rec_code)
      , rec_length(from_copy.rec_length)
      , modified(from_copy.modified) // Probably should be false since we are creating the copy
      {}
   inline virtual ~Binary_record_header() {}                                    //130305
      // Added because derived virtual method identify_IO is was not being called.
protected:                                                                       //061003
   bool read_header(Binary_file_interface&in_file,bool big_body_sizes);          //000129
   bool write_header(Binary_file_interface &out_file,bool big_body_sizes);       //000705
public: // accessors
   inline Record_code get_code()                    const { return rec_code; }
   inline virtual Record_code get_code_for_write()  const { return rec_code; }   //140612
   inline virtual nat32 get_length()                const { return rec_length; }
   inline virtual nat32 get_length_in_file()        const { return rec_length; } //091218
   inline nat32 set_length(nat32 new_length)                       modification_ //001008
      { rec_length = new_length; return rec_length; }
   virtual void dumpX(std::ostream &txt_strm);
   virtual void write_YAML(std::ostream &YAML_strm,nat16 indent_level);          //180108
      // This is used to debug a database.
      virtual int compare(const Binary_record_header &other)              const; //091021
      inline bool free(Record_code free_record_code)               modification_ //100519
         { rec_code = free_record_code; modified = true; return true; }
};
//_Binary_record_header_____________________________________________1999-01-07_/
 class Binary_record
: public Binary_record_header
{
protected:
   contribute_ bool in_memory;                                                   //100128
public:
   Binary_record
      (Binary_file_interface &in_file
      ,bool big_body_sizes );
   inline Binary_record
      (Record_code _rec_code
      ,nat32 _length
      ,bool _modified =false)  // true when freeing spaces
      :Binary_record_header                                  (_rec_code,_length)
      ,in_memory                                                         (false) //100128
      {  modified = _modified;                                                   //100519
      }
   inline Binary_record(const Binary_record &from_copy)                          //050128
      : Binary_record_header                                         (from_copy)
      ,in_memory                                                         (false) //100128
      {}
      // This constructor is used when freeing a record.
      // This class should always be used for deriving classes
      // This class by itself will be used when the record is not in memory.
   inline virtual ~Binary_record() { }
       // allow derived record classes do destory correctly
   inline virtual bool is_in_memory()         affirmation_ { return in_memory; }
   virtual bool set_modified(bool _modified = true)               contribution_; //970923
   inline virtual bool is_modified()            affirmation_{ return modified; } //000705
   inline virtual bool is_valid()                   affirmation_{ return true; } //050603
      // is_valid() is overridden by UED data set record so we don't
      //have to write records where all data is invalid.
   inline virtual void identify_IO()                             modification_{} //001126
      // overridden by UED which allows records to be resized
   virtual nat32 compute_record_body_size()                                const //001008
      {return rec_length;} // Default records don't resize
   inline virtual nat32 get_length()                                       const //091021
      { return compute_record_body_size(); }
   virtual void write(Binary_file_interface  &out_file,bool big_body_sizes);     //000705
   virtual void write_body(Binary_file_interface &out_file) = 0;                 //971103
      // Write body should always be overridden if any data follows
      // record header. Some records may consist only of the record header
      // (I.e. marker/controls).
   virtual Record_code read(Binary_file_interface &in_file,bool big_body_sizes); //000705
   inline virtual bool read_body(Binary_file_interface &/*in_file*/)  modification_  //971103  // actually performs IO
      {  //UNUSED_arg(in_file);
         return in_memory; }
      // Derived classes must reimplement read_body and return true;             //100128
// virtual void dump(STD_NS ostream &txt_strm);
      // This is used to debug a database.
public: // Actually this should be protected and friend to binary_record_file_index
   inline const void *get_body_raw()    const { return this + sizeof(modified);} //091021
};
//_Binary_record____________________________________________________2000-07-03_/
class Binary_record_without_body
: public Binary_record
{
public: // structors
   inline Binary_record_without_body(const Binary_record &full_record)
      :Binary_record(full_record.get_code(),full_record.get_length(),false)
      {}
   inline Binary_record_without_body
      (Binary_file_interface &in_file
      ,bool                   big_body_sizes )
      :Binary_record(in_file,big_body_sizes)
      {}
   inline Binary_record_without_body
      (Record_code   _rec_code
      ,nat32         _length
      ,bool          _modified)   // true when freeing spaces
      :Binary_record
      (_rec_code
      ,_length)
      {  set_modified(_modified);
      }
      // This constructor is used when freeing a record.
   inline virtual void write_body(Binary_file_interface &/*out_file*/)        {} //971103
   inline virtual bool is_in_memory()             affirmation_ { return false; }
};
//_Binary_record_without_body_______________________________________2000-07-05_/
class Binary_record_index_node_dynamic
: public Item
{
   friend class Binary_record_file_dynamic_indexed;
protected:
   nat32                position;
   Binary_record        *record;
   bool                 positioned;                                              //000711
public:
   Binary_record_index_node_dynamic
      (Binary_file_interface   &binary_record_file
      ,bool big_body_sizes);                                                     //050522
      // This constructor is used when creating an index from
      // an existing binary record stream.
      // The binary record (header) will be read from this position in the file.
      // (The body is not read)
      // The file position will be advanced past what would be the body of the record
      bool know_position(nat32 _position);
   Binary_record_index_node_dynamic(Binary_file_interface &binary_record_file    //050522
      ,Binary_record *record,bool big_body_sizes,bool write_it_now
      ,bool file_is_read_only);                                                  //050522
      // This constructor is used when adding a record to the file.
      // The position will be set to the current file length.
      // The record will be immediately written.
   inline Binary_record_index_node_dynamic(Binary_record *_record)              //000822
      :Item()
      ,position(0)
      ,record(_record)
      ,positioned(false)                                                      {}
      // This constructor is used when working with lists of records that
      // will be appended directly to a Binary_record_file_indexed
      // (This is used by the Excel file generation class)
   virtual ~Binary_record_index_node_dynamic();
      // Delete the record.
   inline virtual bool is_in_memory()                               affirmation_
                                    { return record && record->is_in_memory(); }
      // Returns true if record points to a binary record with header and body.
      // returns false if record points to only the header part of the record.
   Binary_record *finish(Binary_record_file_dynamic &binary_record_file);
      // This method will commit the record to the file if it is in memory
      // record will be deleted and replaced with only the header part.
      // returns the record if it was resized and will not fit in the current location
   Binary_record *relinquish();
   Binary_record *get(Binary_record_file_dynamic &binary_record_file);
      // If record is in memory it is returned
      // Otherwize record (which will be a header is deleted)
      // and a new full record will be read from the respective position;
public: // container overrides                                                   //000703
   inline virtual int          compare(const Item &other)                  const //980511
      {  const Binary_record_index_node_dynamic &other_node
         = (const Binary_record_index_node_dynamic &)other;
         return record < other_node.record ? -1 : record>other_node.record?1:0;
      }
public: // accessors                                                             //000705
   inline  Record_code get_record_code()
      { return record ? record->get_code() : (Record_code)0; }
   inline  nat32 get_position()                        const { return position;}
   nat32 get_node_record_length(bool big_body_sizes)                      const; //080911
   virtual void dump(std::ostream &txt_strm);
   virtual void write_YAML(std::ostream &YAML_strm,nat16 indent_level);          //180108
      // This is used to debug a database.
};
//_Binary_record_index_node_dynamic_________________________________2000-07-03_/
}//_namespace_CORN_____________________________________________________________/
#endif

