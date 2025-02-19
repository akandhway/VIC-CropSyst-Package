#ifndef datasrc_indexH
#define datasrc_indexH
#include "corn/container/unilist.h"

///051108 I don't think I ever actually used this
//anyway I am now using a more generic indexer.

namespace CORN {

class Data_table;
class Data_source;
//______________________________________________________________________________
union  Index_value_union
{
   char       *key_string;  // also for enum char_array filename;
   int8        key_int8;
   int16       key_int16;
   int32       key_int32;
   nat8        key_uint8;
   uint16      key_uint16;
   uint32      key_uint32;
   float32     key_float32;
   float64     key_float64;
   bool        key_bool;
//NYI   Date        key_date;
//nyi   byte        *key_byte;
};
//_Index_value_union_____________________________________________________________________________
class Index_item : public Item
{public: //contributes                                                           //120804
   contribute_ nat8              key_type;  // One of VV types
   contribute_ Index_value_union rec_key_value;
   contribute_ bool  selected;
 public:
   Index_item
      (Data_source &_data_source
      ,nat8         _key_type                                                    //021113
      ,const char  *_index_field_name);                                          //020125
      // This is the main constructor
   Index_item
      (nat8         _key_type
      ,const char * _index_field_name                                            //020125
      ,int          _string_size);
      // This constructor is used when we don't read the data source yet for the field info
   Index_item();
      // This constructor is used for searches
   virtual ~Index_item();
   virtual int compare(const Item &)                                      const;//980511
   inline virtual bool set_selected(bool _selected)                contribution_
                                                 { return selected = _selected;} //120804_020124
   inline virtual bool is_selected()                   const { return selected;} //011221
};
//_Index_item__________________________________________________________________/
class Record_index : public Index_item
{public:
   nat32 rec_num;   // this is 0 based rename to index_pos_0_based
 public:
    Record_index(Data_table &_data_table,const char *_index_field_name);         //020125
};
//_Record_index_____________________________________________________2002-11-13_/
class Dynamic_data_source_index
: public Unidirectional_list          // list of Index items
{
 protected:
   std::string   index_field_name; // primary key                                //000119
 public:
   Dynamic_data_source_index(const char *i_index_field_name);                    //020125
   virtual bool find_record_cstr
      (const char *search_key,bool from_first_record=true);
   virtual bool find_record_int32(int32 search_key,bool from_first_record=true); //021211
   virtual void build_index()                                    performs_IO_=0;
 protected:
   virtual bool index_built()                                             const; //010509
   virtual bool activate_record(Index_item &index_item) = 0;                     //021113
   virtual bool find_record
      (const Index_item &index_to_find,bool from_first_record)     performs_IO_;  //010509
#ifdef NYI
   virtual bool get_next(Data_record &data_rec);                                 //020914
   virtual void where_cstr(const char *attr, const char *value)   contribution_;
   virtual void where_long(const char *attr, const long value)    contribution_;
   virtual void where2_long
      (const char *attribute1, const long value1
      ,const char *attribute2, const long value2)                  contribution_;
#endif
};
//_Dynamic_data_source_index________________________________________2000-01-12_/
class Dynamic_data_table_index
: public Dynamic_data_source_index
{  Data_table         &table;
 public:
   inline Dynamic_data_table_index
      (Data_table &i_data_table,const char *i_index_field_name)
      : Dynamic_data_source_index(i_index_field_name)
      , table(i_data_table)
      {}
   virtual bool activate_record(Index_item &index_item);
   virtual void build_index()                                      performs_IO_;
};
//_Dynamic_data_table_index_________________________________________2002-11-12_/
}//_namespace_CORN_____________________________________________________________/
#endif

