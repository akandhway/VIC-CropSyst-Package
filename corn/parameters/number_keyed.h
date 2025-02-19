#ifndef number_keyedH
#define number_keyedH
#include <string>
#ifndef primitiveH
#  include "corn/primitive.h"
#endif

#ifndef Data_type_enumH
#  include "corn/data_type_enum.h"
#endif

namespace CORN {
//______________________________________________________________________________
class Number_keyed
{
 public:
   CORN::Data_type mode;    // Not only the primitive numeric (and date time types are considered here)
   union Value
   {
      nat8    *as_nat8;
      nat16   *as_nat16;
      nat32   *as_nat32;     // Time32 times may also be stored as nat32
      int8    *as_int8;
      int16   *as_int16;
      int32   *as_int32;      // Date32 dates may also be stored as int32
      float32 *as_float32;
      float64 *as_float64;    // time and datetime are stored as float64
      const nat8    *as_const_nat8;
      const nat16   *as_const_nat16;
      const nat32   *as_const_nat32;     // Time32 times may also be stored as nat32
      const int8    *as_const_int8;
      const int16   *as_const_int16;
      const int32   *as_const_int32;      // Date32 dates may also be stored as int32
      const float32 *as_const_float32;
      const float64 *as_const_float64;    // time and datetime are stored as float64
   } value;
   union  key_cstr
   {  char   const *as_cstr;
      wchar_t const *as_wstr;
   } mutable key; contribute_ bool key_is_wide;

 public:
   Number_keyed();
   inline virtual ~Number_keyed() {}

   Number_keyed(int8    &_value);
   Number_keyed(nat8    &_value);
   Number_keyed(int16   &_value);
   Number_keyed(nat16   &_value);
   Number_keyed(int32   &_value);
   Number_keyed(nat32   &_value);
   Number_keyed(float32 &_value);
   Number_keyed(float64 &_value);

   Number_keyed(const int8    &_value);
   Number_keyed(const nat8    &_value);
   Number_keyed(const int16   &_value);
   Number_keyed(const nat16   &_value);
   Number_keyed(const int32   &_value);
   Number_keyed(const nat32   &_value);
   Number_keyed(const float32 &_value);
   Number_keyed(const float64 &_value);

   const char    * know_key_cstr(const char     *_key_cstr)          cognition_;
   const wchar_t * know_key_wcstr(const wchar_t  *_key_wstr)          cognition_;
 public:

   // In the following virtual is not actually needed,
   // virtual is just temporary until RNAutoControls gets recompiled
   // before which time virtual should be removed


   /*virtual */ float64 set_value_as_float64(float64 new_value)    modification_;
   /*virtual */ float32 set_value_as_float32(float32 new_value)    modification_;//150111
   virtual bool set_str(const char *value_as_cstr)                 modification_;
   virtual bool set_wstr(const wchar_t *value_as_wstr)             modification_;//150112
   /*virtual */ bool is_float()                                                 const;
   Number_keyed *check_abstract() { return new Number_keyed;}
 public:
   /*virtual */ float64 get_value_as_float64()                                  const;
   /*virtual */ float32 get_value_as_float32()                                  const;//150111
   const char *append_value_in_radix_to(std::string &buffer,nat8 precision,nat8 radix)       const;
   inline CORN::Data_type get_mode()                                       const {return mode;}

    virtual bool is_key_string(const std::string &key)             affirmation_; //180820
    virtual bool is_key_wstring(const std::wstring &key)           affirmation_; //180820

/*180820  was probably only used for find_cstr now using is_key
   
   virtual const char     *get_key()                                       const;//150111
   const wchar_t  *get_key_wide()                                          const;//150111
*/   
};
//_Number_keyed_____________________________________________________2015-01-03_/
}//_namespace_CORN_____________________________________________________________/
#endif // PARAMETER_NUMERIC_H
