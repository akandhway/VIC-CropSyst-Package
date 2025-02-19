#ifndef strconvHPP
#define strconvHPP
#include "corn/string/strconv.h"
#include <string>
//______________________________________________________________________________
namespace CORN
{
//_____________________________________________________________________________
extern std::string  &wstring_to_string(const std::wstring  &wstr,std::string  &astr);
extern std::wstring &string_to_wstring (const std::string  &astr,std::wstring &wstr);
extern std::string  &UnicodeZ_to_string(const_UnicodeZ      wstr,std::string  &astr); //120205
extern std::wstring &ASCIIZ_to_wstring (const_ASCIIZ        astr,std::wstring &wstr); //120205
extern char         *wstring_to_ASCIIZ (const std::wstring &wstr,char *cstr);    //130827
//______________________________________________________________________________
extern std::string&  append_wstring_to_string (const std::wstring &wstr,std::string  &astr); //120209
extern std::wstring& append_string_to_wstring (const std::string  &astr,std::wstring &wstr); //120209
extern std::string&  append_UnicodeZ_to_string(const_UnicodeZ      wstr,std::string  &astr); //120209
extern std::wstring& append_ASCIIZ_to_wstring (const_ASCIIZ        astr,std::wstring &wstr); //120209
extern std::string & append_ASCIIZ_to_string  (const_ASCIIZ        cstr,std::string  &astr); //130417
extern char *        append_wstring_to_ASCIIZ (const std::wstring &wstr,char *cstr);   //130827
//______________________________________________________________________________
enum StripType { Leading, Trailing, Both };                                      //001021
extern std::string  &strip_string    (std::string  &astr, StripType s = Trailing, char c=' '); //130919
extern std::wstring &strip_wstring   (std::wstring &wstr, StripType s = Trailing, wchar_t c=L' '); //140121
extern std::string &append_SDF_to    (const std::string &source_string, std::string &to_string,bool append_to_result);   //130919
extern std::string &replace_character(std::string &original,char current_c, char replacement_c); //131008
//conceptual extern std::wstring &strip_wstring(std::wstring  &wstr, StripType s = Trailing, wchar_t c=' ');            //130919
//______________________________________________________________________________
extern std::string &append_float32_to_string(float32 value,nat8 decimals,std::string &to_string,nat16 str_width=0,char leading_char=' '); //190106_131213
extern std::string &append_float64_to_string(float64 value,nat8 decimals,std::string &to_string,nat16 str_width=0,char leading_char=' '); //190106_140615
extern std::string &append_int8_to_string   (int8    value,std::string &to_str,int8 radix=10   ,nat16 str_width=0,char leading_char=' '); //140216
extern std::string &append_nat8_to_string   (nat8    value,std::string &to_str,int8 radix=10   ,nat16 str_width=0,char leading_char=' '); //140216
extern std::string &append_int16_to_string  (int16   value,std::string &to_str,int8 radix=10   ,nat16 str_width=0,char leading_char=' '); //140216
extern std::string &append_nat16_to_string  (nat16   value,std::string &to_str,int8 radix=10   ,nat16 str_width=0,char leading_char=' '); //140216
extern std::string &append_int32_to_string  (int32   value,std::string &to_str,int8 radix=10   ,nat16 str_width=0,char leading_char=' '); //140216
extern std::string &append_nat32_to_string  (nat32   value,std::string &to_str,int8 radix=10   ,nat16 str_width=0,char leading_char=' '); //140216
/*NYI
extern std::wstring &append_float32_to_wstring(float32 value,nat8 decimals,std::wstring &to_string); //131213
extern std::wstring &append_float64_to_wstring(float64 value,nat8 decimals,std::wstring &to_string); //140615
extern std::wstring &append_int8_to_wstring   (int8    value,std::wstring &to_str,int8 radix=10,nat16 str_width=0,char leading_char=' '); //140216
extern std::wstring &append_nat8_to_wstring   (nat8    value,std::wstring &to_str,int8 radix=10,nat16 str_width=0,char leading_char=' '); //140216
extern std::wstring &append_int16_to_wstring  (int16   value,std::wstring &to_str,int8 radix=10,nat16 str_width=0,char leading_char=' '); //140216
extern std::wstring &append_nat16_to_wstring  (nat16   value,std::wstring &to_str,int8 radix=10,nat16 str_width=0,char leading_char=' '); //140216
*/
extern std::wstring &append_int32_to_wstring  (int32   value,std::wstring &to_str,int8 radix=10,nat16 str_width=0,char leading_char=' '); //140216
extern std::wstring &append_nat32_to_wstring  (nat32   value,std::wstring &to_str,int8 radix=10,nat16 str_width=0,char leading_char=' '); //140216
//______________________________________________________________________________
extern std::istream  &read_token(std::istream &strm,std::string &token);
//______________________________________________________________________________
extern size_t find_case_insensitive(const std::string &in_string,const std::string &sub_string);
//______________________________________________________________________________
}//_namespace CORN_____________________________________________________________/
#endif

