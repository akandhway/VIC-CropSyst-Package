#ifndef itemH
#define itemH
#include "corn/primitive.h"
#include "corn/const.h"
#include <ostream>
#include <fstream>
#include <assert.h>
namespace CORN {                                                                 //001206
class Data_record;                                                               //010103
class Binary_file_interface;                                                     //111015
//______________________________________________________________________________
interface_ Item_interface
{  inline Item_interface()                                                    {} //120511 even though this is an interface gcc want this declared for copy constructor
   inline virtual ~Item_interface()                                           {} //130304
   virtual const char *label_string(std::string &buffer)                const=0; //170423
};
//_Item_interface___________________________________________________2009-07-28_/
interface_ Item
: public extends_interface_ Item_interface                                       //090728
{
 public:
   enum Filter
      {filter_all                          // 000
      ,filter_selected                     // 001
      ,filter_checked                      // 010
      ,filter_select_or_checked            // 011
      ,filter_not_selected_and_not_checked // 100
      ,filter_not_selected                 // 101
      ,filter_not_checked                  // 110
      ,filter_not_checked_or_not_checked   // 111
      };                                                                         //040318
   //_Filter___________________________________________________________________/
   struct Format
      {  // applied to each item in the container
         char *initializer;
         char *separator;
         char *finalizer;  // I.e. might include \n or \r (ie. linux) if line separation
         char indentor; // Usually either space or tab, or 0 if no indentation
         struct Key
         {  bool distinquished;    // If the key is specially set aside (I.e. JSON, INI files)
            bool case_sensitive;   // used only when reading
            char *separator;
            struct Quotation
            {  char *initializer;   // I.e.  JSON keys are double quoted
               char *finalizer;     // I.e.  JSON keys are double quoted
            } quotation;
         } key;
         struct Sequence_inline
         {
            char *initializer;
            char *separator;
            char *finalizer;
         } sequence_inline;
         struct Association_inline // mapping
         {
            char *initializer;
            char *separator;
            char *finalizer;
         } association_inline;
         struct Comment
         {  char *initializer;   // I.e. C style comments
            char *finalizer;
            char *endline;       // I.e. C++ style comment (Do not include newline character)
         } comment;
      };
   //_Format________________________________________________________2013-02-17_/
 public: // 'structors
   Item();                                                                       //120511
      // Even though this is an interface gcc wants this declared for copy constructor 120511
   virtual ~Item();
         // All derived classes must define compare and label for the class.
         // This destructor is required!
         // (if it is not included, items don't get deleted correctly.
   inline virtual CORN::Item *clone()                         const { return 0;} //170817
      // Currently derived classes are not required to implement clone
 public:
   inline virtual const char *label_string(std::string &buffer)            const
                                                       { return buffer.c_str();} //170101
      // In most cases ust be overridden to a
   inline virtual const wchar_t *label_wstring(std::wstring &buffer)       const
                                                       { return buffer.c_str();} //170101
      // Appends a user recognizable (representation) short caption representing
      // the object to the buffer.
      // This is usually used to display the object to the user.
          virtual int   compare(const CORN::Item &)                       const; //150211_980511
   inline virtual bool is_case_sensitive()               const { return false; } //150201_051101
   inline virtual const char    *key_string (std::string &buffer)          const
      { return append_to_string(buffer).c_str(); }                               //180802
      // Derived classes should at least implement key_string
      // if not simply use append_to_string as default.
   implmt virtual const wchar_t *key_wstring(std::wstring &buffer)         const;
   inline virtual nat32 get_key_nat32()                        const {return 0;} //110305

/*conceptual
   virtual bool has_key()                                          affirmation_; //180820
      // could default to return false;
*/

   // Derived classes should implement one of these is_key methods
   // Currently assert is used to find where not implemented but needed
/*conceptual
   virtual CORN::Data_type has_key()                            interrogation_=0;
      // Derived classes should (usually) override
      // indicating the type of the key
      // Usually this will be DT_string DT_wstring or DT_nat32
      // eventually default return DT_none
*/
   inline virtual bool is_key_cstr(const char *key)                 affirmation_ //=0; //180820
      {  UNUSED_arg(key);
         return 0;}
   inline virtual bool is_key_string(const std::string &key)        affirmation_ //180820
      { return is_key_cstr(key.c_str()); }
   inline virtual bool is_key_wstr(const wchar_t *key)              affirmation_ //=0; //180820
      {  UNUSED_arg(key);
         return 0;}
   inline virtual bool is_key_wstring(const std::wstring &key)      affirmation_ //180820
      { return is_key_wstr(key.c_str()); }
   inline virtual bool is_key_nat32(nat32 key)                      affirmation_ //180820
      { UNUSED_arg(key);
        return 0;}  //This is temporary
   virtual std::ostream &write_key(std::ostream &to_stream)          stream_IO_;

   implmt virtual const wchar_t *append_to(std::wstring &buffer)          const; //200621
   implmt virtual const char    *append_to(std::string  &buffer)          const; //200621


   // append_to_Xstring are deprecated used append_to
   implmt virtual const std::wstring &append_to_wstring
      (std::wstring &buffer)                                              const; //130103
   inline virtual const std::string &append_to_string
      (std::string &buffer)                             const { return buffer; } //130103
      // Appends a text representation of the object to the string buffer.
      // In most cases, append_to_string must be implemented in derived classes to become active.

   implmt virtual bool  write(std::ostream &)                        stream_IO_; //150728_980511
   implmt virtual bool  write_with_format                                        //150728
      (std::ostream &,const Format &,nat16 identation_level)         stream_IO_; //130217
   inline virtual nat32 write_binary(Binary_file_interface &)        stream_IO_
                                                                    { return 0;} //111015
   inline virtual nat32 read_binary(Binary_file_interface &)         stream_IO_
                                                                    { return 0;} //111015
//NYI   inline virtual nat32 write_return_bytecount(STD_NS ostream &)    performs_IO_ = 0; //{ return 0;}; //111015
      // Eventually replaced as write() in derived classes with write_return_bytecount which we can then rename
   inline virtual bool setup_structure
      (Data_record &,bool /*for_write*/)            modification_ {return true;} //020617
   inline virtual bool  is_enabled()                 affirmation_ {return true;} //011221
      /**<\fn is_enable can be overridden in derived classes
      it is intended to be used for GUI list boxes **/
   inline virtual bool  is_selected()              affirmation_  {return false;} //011221
      /**<\fn is_selected  can be overridden in derived classes
      it is intended to be used for GUI list boxes and check list boxes **/
   inline virtual bool  is_checked()               affirmation_  {return false;} //011221
      /**<\fn is_checked can be overridden in derived classes
         it is intended to be used for GUI check list boxes */
   inline virtual bool set_selected(bool)           contribution_{return false;}
      //020124 Note that this should be mutates because were are modifying control information, not attributes
      /**<\fn This is used to set the selected state of the item (for GUI listboxes).
         This can be overridden by derived classes that want to support selected state*/
   inline virtual bool set_checked(bool)            contribution_{return false;}
      //020124 Note that this should be mutates because were are modifying control information, not attributes
      /**<\fn This is used to set the checked selected state of the item (for GUI checked listboxes).
         This can be overridden by derived classes that want to support checked state.*/
   inline virtual nat8 get_checkbox_state()                    const {return 0;} //011221
      /**<\fn can be overridden in derived classes
            it is intended to be used for GUI check list boxes */
/*190730 abandoned. This was intended for AutoContainerTreeView, but never really used
   virtual nat8  get_node_path(char **node_path)                          const; //020319
      ///<\fn This is used by TAutoContainerTreeView to put Items in nodes
      ///   It can also be used by data rec to identify sections.
      ///   Node path is allocated by this function which is relinquished to the caller
      ///   The caller must delete node path.
      ///   Node path will be 0 if not supported by this object.
      ///   get_node_path returns the number of nodes in the path.
*/
/*190730 abandoned This was intended to be used for AutoControls but it was never actually used
   inline virtual int   get_image_index()                const {return (int)-1;} //020319
      /// <\fn This can be used in GUI applications for objects which may have associated images.
      ///      -1 indicates no image association.
*/
   bool matches_filter(Filter filter)                             confirmation_; //040318
};
//_Item_____________________________________________________________1999-11-05_/
typedef Item * Item_pointer;
}//_namespace CORN__________________________________________________2000-12-06_/
#endif

