#ifndef indexH
#define indexH
#ifndef CORN_BINARY_TREE
#  include "corn/container/binary_tree.h"
#endif
#include "UED/library/UED_options.h"
#include "UED/library/UED_types.h"
#include "UED/library/datasetrec.h"
#define UED_FOR_ALL_VARIABLES    ((UED::Variable_code)0)
#define UED_FOR_ALL_RECORD_TYPES ((Record_code)0)
#define BINARY_RECORD_INDEX_NODE Binary_record_index_node_dynamic
using namespace CORN;
namespace CORN
{  class Date_time_abstract;
};
namespace UED {
// Forward declarations
class Time_stamped_record_list;                                                  //990108
class Variable_index_item;                                                       //000707
class Binary_data_record_cowl;
class Time_query;
class Database_file_indexed;                                                     //070718
//______________________________________________________________________________
class _UED_DLL Time_stamped_record_item : public Item
{
   friend class Time_stamped_record_list;                                        //990108
   friend class Variable_index_item;                                             //000707
   datetime64 earliest_date_time; // Index on this                               //030714
   datetime64 latest_date_time;   // Find between both both                      //030714
   UED_units_code time_step;                                                     //040125
   BINARY_RECORD_INDEX_NODE *index_node; // probably could be &since index nodes are always in memory. 000705
 public: // constructor
   inline Time_stamped_record_item
      (datetime64 _earliest_date_time                                            //030714
      ,datetime64 _latest_date_time                                              //030714
      ,UED_units_code _time_step                                                 //040125
      ,BINARY_RECORD_INDEX_NODE *_index_node)                                    //000705
       : Item()                                                                  //980210
      , earliest_date_time (_earliest_date_time)
      , latest_date_time   (_latest_date_time)
      , time_step          (_time_step)                                          //040125
      , index_node         (_index_node)                                         //000705
      {  }
      // This constructor is used when adding index items.
   virtual int compare(const Item  &other)                                const; //980210
   inline virtual const char *label_cstr_deprecated(char *buffer)          const { return buffer; }  //030123
   inline virtual const char *label_string(std::string &buffer)            const { return buffer.c_str(); }  //030123
   inline datetime64 get_earliest_date_time()     { return earliest_date_time; } //970915
   inline datetime64 get_latest_date_time()         { return latest_date_time; } //970915
   inline UED_units_code get_time_step()                   { return time_step; } //040125
   Binary_data_record_cowl *get_record(Binary_record_file_dynamic &binary_record_file); //050522
   inline modifiable_ Binary_data_record_cowl *mod_record                        //170323
      (Binary_record_file_dynamic &binary_record_file)             modification_
      {  return const_cast<Binary_data_record_cowl *>
            (get_record(binary_record_file)); }
   inline const Binary_data_record_cowl *ref_record                              //170323
      (const Binary_record_file_dynamic &binary_record_file)               const
      { return const_cast<Time_stamped_record_item *>
         (this)->get_record(const_cast<Binary_record_file_dynamic &>(binary_record_file)); }
   virtual bool write(std::ostream &)                                stream_IO_;
};
//_Time_stamped_record_item_________________________________________1998-02-10_/
class _UED_DLL Time_stamped_record_list
: public Binary_tree
{
public:
   inline Time_stamped_record_list()
      : Binary_tree()                                                            //051010
      {}
   Time_stamped_record_item *find_index
      ( const Time_query &time_query)                             modification_; //990118
   Binary_data_record_cowl *get_record                                           //000707
      (const Time_query &time_query
      ,Database_file_indexed &database);
   void update                                                                   //990108
      (datetime64 earliest_date_time                                             //030714
      ,datetime64 latest_date_time                                               //030714
      ,UED_units_code            time_step                                       //040125
      ,BINARY_RECORD_INDEX_NODE *index_node );                                   //000705
         // probably could be &since index nodes are always in memory.
 public:
   virtual Time_stamped_record_item *get_earliest_record_pos_info(datetime64 &earliest_date_time); //990909
      //D <BR>This method returns the record position info for the lowest date record in the index.
   virtual Time_stamped_record_item *get_latest_record_pos_info(datetime64 &latest_date_time);     //990909
      //D  <BR>This method returns the record position info for the latesst date record in the index.
};
//_Time_stamped_record_list_________________________________________1999-01-08_/
class Variable_index_list;
//__________________________________________________________________1999-01-08_/
class _UED_DLL Variable_index_item
: public Item
, public Time_stamped_record_list  // list of Time_stamped_record_item
{
   friend class Variable_index_list;                                             //990108
   Record_code record_type_code;                                                 //990108
      // We need this because the variable may be used for different kinds of records
   UED::Variable_code variable_code;                                             //971014
      // We may want here a reference to the variable definition so we can label
   BINARY_RECORD_INDEX_NODE *latest_index_node;
public:
   inline Variable_index_item
      (Record_code         record_type_code_
      ,UED::Variable_code  variable_code_)
      : Item()
      ,Time_stamped_record_list                                               ()
      , record_type_code                                     (record_type_code_)
      , variable_code                                           (variable_code_) //971014
      , latest_index_node                                                    (0) //000706
      {}
   inline virtual int compare(const Item  &other)                          const
   {  Variable_index_item &other_index = (Variable_index_item &)other;
      return (record_type_code < other_index.record_type_code)
      ? -1 : (record_type_code > other_index.record_type_code)
           ? 1
           :  (variable_code < other_index.variable_code)
              ? -1 : (variable_code > other_index.variable_code) ? 1 : 0;
   }
   inline virtual const char *label_cstr_deprecated(char *buffer)          const //030123
      {  return buffer; }// Not currently used but may eventually  get variable definition.
   inline virtual const char *label_string(std::string &buffer)            const //030123
      {  return buffer.c_str(); }// Not currently used but may eventually  get variable definition.
   void update                                                                   //000706
      (datetime64       earliest_date_time
      ,datetime64       latest_date_time
      ,UED_units_code   time_step                                                //040125
      ,BINARY_RECORD_INDEX_NODE *index_node                                      //000705
         // probably could be &since index nodes are always in memory.
      ,Database_file_indexed       &database);
   Binary_data_record_cowl *get_record                                                //000707
      (const Time_query       &time_query
      ,Database_file_indexed  &database);
   Time_stamped_record_item *find_record_index                                   //990111
      (const Time_query &time_query);
   Binary_data_record_cowl *find_record_in_memory(const Date_time_abstract &date_time);//990113
   Binary_data_record_cowl *get_matching_record_buffer(const Date_time_abstract &date_time);
   int32 find_record_for_year(const int16 year) const;
   int32 find_record_for_date_time(const Date_time_abstract &date_time
      ,const Record_type_distinguisher_interface::time_stamp_codes time_stamp_resolution) const;
 public: // accessors
   inline virtual UED::Variable_code get_variable_code()
      { return variable_code; }
};
//_Variable_index_item______________________________________________1999-01-08_/
class _UED_DLL Variable_index_list
: public Binary_tree                                                             //040109
{public:
   inline Variable_index_list()
      :  Binary_tree()                                                        {} //040109
   Variable_index_item *find_index
      (Record_code            record_type_code
      ,UED::Variable_code      variable_code)                     modification_;
   Binary_record *append_record                                                  //000706
      (Binary_data_record_cowl     *current_record
      ,Database_file_indexed  &parent_database)                    submission_;
   Binary_data_record_cowl *get_record                                           //000706
      (Record_code            record_type_code
      ,UED::Variable_code      variable_code
      ,const Time_query      &time_query
      ,Database_file_indexed &database);
   // Returns 0 if nothing matches in the indexes
   void update                                                                   //990108
      (Record_code            rec_type_code
      ,Variable_code      variable_code
      ,datetime64             earliest_date_time
      ,datetime64             latest_date_time
      ,UED_units_code         time_step                                          //040125
      ,BINARY_RECORD_INDEX_NODE *index_node // probably could be &since index nodes are always in memory. //000705
      ,Database_file_indexed  &database);
   virtual Time_stamped_record_item *get_earliest_record_pos_info                //990909
      ( datetime64      &earliest_date_time
      , Record_code     &for_record_type_code  //<- not const
      , Variable_code   &for_var_code );
   virtual Time_stamped_record_item *get_latest_record_pos_info                  //990909
      ( datetime64      &latest_date_time
      , Record_code     &for_record_type_code // return the type code and variable for ealiest record found
      , Variable_code   &for_var_code );
   virtual void finish();                                                        //001026
      //	When finishing the database we need to make sure that
      // latest index node pointers are cleared
 private:
   bool record_and_variable_code_matches                                         //141031
      ( Record_code          &record_type_code
      , UED::Variable_code    &var_code
      , Record_code          &for_record_type_code
      , UED::Variable_code   &for_var_code )                       affirmation_;
};
//_Variable_index_list______________________________________________1997-10-03_/
}//_namespace UED______________________________________________________________/
#endif
//index.h

