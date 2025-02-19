#include "corn/OS/directory_entry_name.h"
#include "corn/container/text_list.h"
#include "corn/string/strconv.hpp"
namespace CORN {
namespace OS {
//______________________________________________________________________________
std::wstring *Directory_entry_name::render_wstring
(Path_inclusion inclusion
,wchar_t preferred_seperator)                                              const
{  // Although this method is implemented in an interface, it still qualifies
   // as an interface because it is implemented using only interface methods.
   std::wstring *rendered_string = new std::wstring (L"");
   append_components_to(*rendered_string,inclusion,preferred_seperator);
   return rendered_string;
}
//_render_wstring___________________________________________________2009-11-22_/
bool Directory_entry_name::has_extension
(const std::wstring &valid_ext,bool case_insensitive)                      const
{return has_extension_Unicode(valid_ext.c_str(), case_insensitive); }
//_has_extension________________________________________2017-02-24__1999-07-26_/
bool Directory_entry_name::has_in_path_wstr
(const_UnicodeZ name_cstr
,Path_inclusion inclusion
,bool substring)                                                    affirmation_
{  std::wstring found_buffer;
   return find_in_path_wstr(name_cstr,found_buffer,inclusion,substring);
}
//_has_in_path_wstr_________________________________________________2017-12-27_/
bool Directory_entry_name::has_in_path_cstr
(const_ASCIIZ name_cstr
,Path_inclusion inclusion
,bool substring)                                                    affirmation_
{  std::wstring found_buffer;
   return find_in_path_cstr(name_cstr,found_buffer,inclusion,substring);
}
//_has_in_path_cstr_________________________________________________2012-11-21_/
bool  Directory_entry_name::find_in_path_wstr
(const_UnicodeZ name_wstr
,std::wstring &found_string_appended
,Path_inclusion inclusion
,bool substring)                                                           const
{  std::wstring name(name_wstr);
   CORN::Text_list path_list;
   append_path_list_to(path_list,inclusion);
   FIND_FOR_EACH_IN(found_element,subname,CORN::Item_wstring,path_list,true,each_subname)
   {  found_element = (*subname) == name ? subname : 0;
      if (!found_element && substring)
             found_element = subname->find(name) != std::string::npos
               ? subname : 0;
   } FOR_EACH_END(each_subname)
      // Search for the specified name in directories names of the and the name itself.
      // If found, it returns the full name of the directory element that matches
      // the specified name.
   if (found_element)
      found_string_appended += (*found_element);
   return found_element != 0;
}
//_find_in_path_wstr________________________________________________2017-12-27_/
bool  Directory_entry_name::find_in_path_cstr
(const_ASCIIZ name_cstr
,std::wstring &found_string_appended
,Path_inclusion inclusion
,bool substring)                                                           const
{
   std::wstring name; CORN::ASCIIZ_to_wstring(name_cstr,name);                   //180115
   CORN::Text_list path_list;
   append_path_list_to(path_list,inclusion);
   FIND_FOR_EACH_IN(found_element,subname
      ,Item_wstring
      ,path_list,true,each_subname)
   {  found_element = (*subname) == name ? subname : 0;
      if (!found_element && substring)
             found_element = subname->find(name) != std::string::npos            //121129
               ? subname : 0;
   } FOR_EACH_END(each_subname)
      // Search for the specified name in directories names of the and the name itself.
      // If found, it returns the full name of the directory element that matches
      // the specified name.
   if (found_element)
       found_element->append_to_wstring(found_string_appended);                  //120209
   return found_element != 0;
}
//_find_in_path_cstr________________________________________________2012-12-28_/
std::wstring Directory_entry_name::get_components
(Path_inclusion inclusion
,wchar_t preferred_seperator                                                     //140202
,bool append_trailing_directory_separator)                                 const
{  // This method is virtual because it doesn't modifiy members
   // and calls another virtual method.
   std::wstring return_value;
   append_components_to
      (return_value,inclusion,preferred_seperator
      ,append_trailing_directory_separator);
   return return_value;
}
//_get_components___________________________________________________2013-11-19_/
bool Directory_entry_name::is_relative()                            affirmation_
{  return is_qualified(false) && is_qualified_relative();                        //160121
}
//_is_relative______________________________________________________2012-04-30_/
const Path_inclusion Directory_entry_name::include_copy                      = 0xFF;  //140613
const Path_inclusion Directory_entry_name::include_designator                = 0x01;
const Path_inclusion Directory_entry_name::include_ancestors_path            = 0x02; // includes the path upto but not including the parent directory name.
const Path_inclusion Directory_entry_name::include_ancestors_path_designated = 0x03; // includes ancestors path and drive designator
const Path_inclusion Directory_entry_name::include_parent                    = 0x04; // only the immediate parent name
const Path_inclusion Directory_entry_name::include_qualification             = 0x06;   // path includes both ancestors and parent
                                                                               // up to but not including the designator  (or logical root)
const Path_inclusion Directory_entry_name::include_qualification_designated  = 0x07;   // includes path and drive designator

const Path_inclusion Directory_entry_name::include_name                      = 0x08;
const Path_inclusion Directory_entry_name::include_extension                 = 0x10;
const Path_inclusion Directory_entry_name::include_extension_dot             = 0x20;
const Path_inclusion Directory_entry_name::include_extension_with_dot        = 0x30;
const Path_inclusion Directory_entry_name::include_name_extension            = 0x38;  //130123
const Path_inclusion Directory_entry_name::include_all                       = 0xFF;
const Path_inclusion Directory_entry_name::include_all_except_extension      = 0x0F;
//_include_XXX_________________________________________________________________/
std::ostream& operator<<(std::ostream& os, const Directory_entry_name& dt)
{  os << dt.c_str();
   return os;
}
//_operator<<_______________________________________________________2014-01-24_/
}}//_namespace CORN::OS________________________________________________________/

