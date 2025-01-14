#include "corn/OS/file_type_description.h"
#include <string.h>

namespace CORN { namespace OS {
//______________________________________________________________________________
File_type_description::File_type_description
(const wchar_t *_preferred_extension // The preferred extension (without DOT)
,const wchar_t *_filter_label        // Short description of the file type
,const wchar_t *_wildcard_filter)    // I.e.  *.DAT if (0), *. and preferred extension will be used
: preferred_extension(_preferred_extension)
, filter_label       (_filter_label)
, wildcard_filter    (_wildcard_filter)
{}
//______________________________________________________________________________
File_type_description::File_type_description
(const char *_preferred_extension  // The preferred extension (without DOT)
,const char *_filter_label       // Short description of the file type
,const char *_wildcard_filter)   // I.e.  *.DAT if (0), *. and preferred extension will be used
{  CORN::ASCIIZ_to_wstring(_preferred_extension ,preferred_extension);
   CORN::ASCIIZ_to_wstring(_filter_label        ,filter_label);
   CORN::ASCIIZ_to_wstring(_wildcard_filter     ,wildcard_filter);
}
//_2013-11-19___________________________________________________________________
File_type_description::File_type_description(const File_type_description &to_copy)
: preferred_extension(to_copy.preferred_extension)
, filter_label(to_copy.filter_label)
, wildcard_filter(to_copy.wildcard_filter)
{}
//_1999-02-27___________________________________________________________________
void File_type_description::set_filter_label_string
(const std::string  &new_filter_label)                            modification_
{  CORN::string_to_wstring(new_filter_label,filter_label);
}
//_2013-11-15___________________________________________________________________
/*170424 replaced with label_string
const char *File_type_description::label_cstr(char *buffer)     const
{  CORN::append_wstring_to_ASCIIZ(filter_label,buffer);                          //131115
   strcat(buffer,"(");
   {  strcat(buffer,"*.");                                                       //101116
      CORN::append_wstring_to_ASCIIZ(preferred_extension,buffer);                //131115
   }                                                                             //101116
   strcat(buffer,")");                                                           //101116
   strcat(buffer,"|");
   CORN::append_wstring_to_ASCIIZ(wildcard_filter,buffer);                       //131114
   return buffer;
}
//_2012-05-15___________________________________________________________________
*/
const char *File_type_description::label_string(std::string &buffer)       const
{  CORN::append_wstring_to_string(filter_label,buffer);
   buffer += "(";
   {  buffer += "*.";
      CORN::append_wstring_to_string(preferred_extension,buffer);
   }
   buffer += ")";
   buffer += "|";
   CORN::append_wstring_to_string(wildcard_filter,buffer);
   return buffer.c_str();
}
//_2017-04-23___________________________________________________________________
}}//_namespace_CORN_OS_________________________________________________________/

