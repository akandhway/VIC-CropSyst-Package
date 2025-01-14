#include "corn/OS/directory_entry_name_abstract.h"
#include "corn/OS/file_system_engine.h"
#include "corn/container/text_list.h"
#include <algorithm>
#include "corn/OS/directory_entry_name.h"
#include "corn/string/strconv.hpp"

// include just for debuggin
#include "corn/OS/directory_entry_name_concrete.h"

using namespace std;
//_____________________________________________________________________________/
namespace CORN
{
namespace OS
{
//_____________________________________________________________________________/
Directory_entry_name_abstract::Directory_entry_name_abstract()
:std::wstring(L"")
,name       ()
,extension  ()
,root       (false)  // assigned in set()
,preferred_separator(0)                                                          //120824
,identified_type(unknown_entry)                                                  //131120
{}
//_Directory_entry_name_abstract:constructor(empty)____________________________/
Directory_entry_name_abstract::Directory_entry_name_abstract
(const char *raw_name
,Directory_entry_type entry_type)
:std::wstring(L"")
,name       ()
,extension  ()
,root       (false)  // assigned in set()
,preferred_separator(0)                                                          //120824
,identified_type(entry_type ? entry_type : type_determination_raw_str(raw_name)) //181218_131120
{  CORN::ASCIIZ_to_wstring(raw_name,*this);
   CORN::strip_wstring(*this,CORN::Trailing,L'\\');                              //140121
   CORN::strip_wstring(*this,CORN::Trailing,L'/');                               //140121
   decompose();                                                                  //120327
   setup_is_root();
}
//_Directory_entry_name_abstract:constructor___________________________________/
Directory_entry_name_abstract::Directory_entry_name_abstract
(const wchar_t *raw_name
,Directory_entry_type entry_type)
:std::wstring(raw_name)
,name       ()
,extension  ()
,root       (false)  // assigned in set()
,preferred_separator(0)                                                          //120824
,identified_type(entry_type?entry_type:type_determination_raw_wstr(raw_name))    //181218_131120
{  decompose();                                                                  //120327
   setup_is_root();
}
//_Directory_entry_name_abstract:constructor________________________2012-02-02_
Directory_entry_name_abstract::Directory_entry_name_abstract
(const std::string &raw_name
,Directory_entry_type entry_type)
:std::wstring(L"")
,name       ()
,extension  ()
,root       (false)  // assigned in set()
,preferred_separator(0)                                                          //120824
,identified_type(entry_type?entry_type:type_determination_raw_string(raw_name))  //181218_131120
{  CORN::string_to_wstring(raw_name,*this);
   decompose();                                                                  //120327
   setup_is_root();
}
//_Copy constructor____________________________________________________________/
Directory_entry_name_abstract::Directory_entry_name_abstract
(const std::wstring &raw_name
,Directory_entry_type entry_type)
:std::wstring(raw_name)
,name       ()
,extension  ()
,root       (false)  // assigned in set()
,preferred_separator(0)                                                          //120824
,identified_type(entry_type?entry_type:type_determination_raw_wstring(raw_name)) //181218_131120
{  decompose();                                                                  //120327
   setup_is_root();
}
//_Directory_entry_name_abstract:constructor________________________2012-02-09_/
Directory_entry_name_abstract::Directory_entry_name_abstract
(const Directory_entry_name &copy_from
,Path_inclusion inclusion
,Directory_entry_type entry_type)
:std::wstring(copy_from.w_str())                                                 //140121
,name       ()
,extension  ()
,root       (false)  // assigned in set()
,preferred_separator(0)                                                          //120824
,identified_type(entry_type ? entry_type : copy_from.get_type())                 //181218
{
   if (inclusion & include_name)
      name = copy_from.get_name();                                               //131120
   if (inclusion & include_extension)
      extension  = copy_from.get_extension();                                    //131120
   setup_is_root();
}
//_Copy constructor_________________________________________________2012-02-06_/
Directory_entry_name_abstract::Directory_entry_name_abstract
(const Directory_entry_name &copy_from
,const Extension &_extension
,Directory_entry_type entry_type)
:std::wstring(copy_from.get_components                                           //131119
      (include_all_except_extension,copy_from.get_separator()))                  //140613
,root       (false)  // assigned in set()
,preferred_separator(0)                                                          //120824
,identified_type(entry_type ? entry_type : copy_from.get_type())                 //181218
{  name = copy_from.get_name();                                                  //131120
   extension = _extension;                                                       //131120
   append(L".");                                                                 //140613
   append(extension);                                                            //140613
}
//_Copy constructor_________________________________________________2012-02-06_/
Directory_entry_name_abstract::Directory_entry_name_abstract
(const std::wstring &_name,const std::wstring &_extension
,Directory_entry_type entry_type)
:std::wstring()
,root       (false)   // If there is a path, then this cannot be a root
,preferred_separator(0)                                                          //120824
,identified_type(entry_type)                                                     //131120
{  name       = _name;                                                           //131120
   extension  = _extension;                                                      //131120
   construction_compose();
}
//_Directory_entry_name_abstract:constructor________________________2009-11-07_/
Directory_entry_name_abstract::Directory_entry_name_abstract
(const char *name_
,const std::wstring &extension_
,Directory_entry_type entry_type)
:std::wstring()
,root       (false)   // If there is a path, then this cannot be a root
,preferred_separator(0)
,identified_type(entry_type ? entry_type : type_determination_raw_str(name_))    //181218_131120
{
   extension  = extension_;                                                      //131120
   if (name_) append_ASCIIZ_to_wstring(name_,name);                              //131119
   construction_compose();
}
//_Directory_entry_name_abstract:constructor________________________2013-09-05_/
Directory_entry_name_abstract::Directory_entry_name_abstract
(const wchar_t *name_
,const wchar_t *extension_
,Directory_entry_type entry_type)
:std::wstring()
,root       (false)   // If there is a path, then this cannot be a root
,preferred_separator(0)                                                          //120824
,identified_type(entry_type)                                                     //131120
{
   name = name_;                                                                 //131120
   extension  =extension_;                                                       //131120
   construction_compose();
}
//_Directory_entry_name_abstract:constructor________________________2009-11-07_/
Directory_entry_name_abstract::Directory_entry_name_abstract
(const char *name_
,const char *extension_
,Directory_entry_type entry_type)
:std::wstring()
,root       (false)   // If there is a path, then this cannot be a root
,preferred_separator(0)                                                          //120824
,identified_type(entry_type)                                                     //131120
{  CORN::append_ASCIIZ_to_wstring(name_,name);                                   //131120
   CORN::append_ASCIIZ_to_wstring(extension_,extension);                         //131120
   construction_compose();
}
//_Directory_name:constructor_______________________________________2012-02-13_/
size_t Directory_entry_name_abstract
::find_right_most_directory_separator_pos()                                const
{  // first look for rightmost UNIX directory separator
   size_t rightmost_dir_sep_pos = std::string::npos;
   for (int16 i = length();i >= 0; i--)                                          //161201
   {
      nat16 char_i = c_str()[i];
      if ((char_i == L'/') ||  (char_i == L'\\'))
      {
         preferred_separator = char_i;
         return rightmost_dir_sep_pos = i;
      }
   }
   return rightmost_dir_sep_pos;
}
//_find_right_most_directory_separator_pos__________________________2012-03-27_/
size_t Directory_entry_name_abstract::find_name_start_pos()                const
{  size_t dir_sep_pos = find_right_most_directory_separator_pos();
   size_t name_start_pos = 0;
   if (dir_sep_pos != std::string::npos)
   {  wchar_t slash_pos_char = std::wstring::at(dir_sep_pos);
      name_start_pos =
         ( slash_pos_char == L'\\') || ( slash_pos_char == L'/')
         ? dir_sep_pos + 1   // case when there is directory separator
         : 0;      // case when no directory separator
   }
   return name_start_pos;
}
//_find_name_start_pos______________________________________________2012-03-27_/
void  Directory_entry_name_abstract::decompose()                   contribution_
{  size_t name_start_pos = find_name_start_pos();                                //120327
   size_t name_end_pos   = length();                                             //120531
   size_t ext_start_pos  = std::string::npos;
   size_t ext_end_pos    = length();
   size_t rightmost_dot_pos = rfind(L'.');
   if (rightmost_dot_pos != std::string::npos)
      if (rightmost_dot_pos >= name_start_pos)                                   //161201_120531
      {  name_end_pos = rightmost_dot_pos;                                       //120531
         if ((rightmost_dot_pos == 0)
            || (at(rightmost_dot_pos-1) == L'.'))                                //120531
         {  // The name is relative '.' or '..'                                  //120531
            name_end_pos = length();                                             //120531
            ext_start_pos = name_end_pos;                                        //120531
         } else                                                                  //120531
            ext_start_pos = rightmost_dot_pos                                    //120531
               + ((rightmost_dot_pos < std::string::npos) ?  1 : 0);             //120531
      }                                                                          //120531
   std::wstring extracted_name(*this,name_start_pos,name_end_pos-name_start_pos);//131129
   name = extracted_name;                                                        //131120
   if (!(  (rightmost_dot_pos == std::string::npos)                              //131129
       || (ext_start_pos > ext_end_pos)))                                        //120531
   {  std::wstring extracted_extension(*this,ext_start_pos,ext_end_pos-ext_start_pos+1);  //131129
      extension = extracted_extension;                                           //131129
   }
}
//_decompose________________________________________________________2012-03-27_/
void Directory_entry_name_abstract::construction_compose()         construction_
{  append(name);
   if (extension.length())
   {  append(L".");
      append(extension);
   }
   setup_is_root();
}
//_construction_compose_____________________________________________2012-02-10_/
Directory_entry_name_abstract::~Directory_entry_name_abstract()
{}
//_Directory_entry_name_abstract:destructor____________________________________/
   /*
   Cases:
   Case1 name.exe
      no path
   Case2 name
      no path and no ext
   Case3  .ext
      no path not name with dot
   Case4 C:\ancestors\parent\name.exe
      Fully qualified name where that has more than one parent directories
         (ancestors has one or more dirs).
   Case5 C:\parent\name.ext
      Single parent
   Case6 C:\name.exe
      file in designated root
   */
const std::wstring &Directory_entry_name_abstract::append_components_to
(std::wstring &buffer
,Path_inclusion inclusion
,wchar_t preferred_path_separator
,bool append_trailing_directory_separator)                                 const
{  //We do not clear the buffer, we simply append to it.
   if (is_root())
   {  if ((inclusion & include_designator) && get_name().length())
         buffer.append(get_name());
      else buffer.append(L"/");                                                  //130103
      // For Unix like filenames the root is empty string, so append /
      return buffer;
   }
   const CORN::OS::Directory_name *path_directory_name = get_path_directory_name(); //120206
   if (path_directory_name)
   {  if ( (inclusion & Directory_entry_name::include_parent)
          |(inclusion & Directory_entry_name::include_ancestors_path)
          |(inclusion & Directory_entry_name::include_designator))
      {  // In the case where this dir entry name has a path and we want any part of the path:
         Path_inclusion next_inclusion = inclusion & Directory_entry_name::include_designator;
         if (inclusion & include_parent)
         {  // When we compose path's string
            // we must enable including of name, and extension
            next_inclusion |= Directory_entry_name::include_name
               |Directory_entry_name::include_extension
               |Directory_entry_name::include_extension_dot;
         }
         if (inclusion & Directory_entry_name::include_ancestors_path)
            next_inclusion |= Directory_entry_name::include_qualification
               |Directory_entry_name::include_extension
               |Directory_entry_name::include_extension_dot;                     //131212
         path_directory_name->append_components_to
            (buffer
            ,next_inclusion // now that we are going into the parent, we need to include the name and the extension
            ,preferred_path_separator);
      }
   }
   bool include_name_and_or_extension = (inclusion & include_name) || (inclusion & include_extension);
   if (include_name_and_or_extension)
      if (buffer.length()
          && preferred_path_separator                                            //140202
          && buffer != L"/")/* In this case the file is in the root (will only occur in Unix*/ //130103
      {  wchar_t sep_cstr[3];                                                    //140202
         sep_cstr[0] = preferred_path_separator;                                 //140202
         sep_cstr[1] = 0;                                                        //140202
         buffer.append(sep_cstr);                                                //140202
      }
   nat32 name_length = name.length();
   if (inclusion & include_name && name_length)
      buffer.append(name);
   if ((inclusion & include_extension &&!empty()))                            //200511
   {  if (get_extension().length() && (inclusion & Directory_entry_name::include_extension_dot))
      {  buffer.append(L".");
         buffer.append(extension);
   }  }
   if (append_trailing_directory_separator)
      append_separator_to(buffer);                                               //140202
   return  buffer;
}
//_append_components_to________________________________________________________/
const std::string &Directory_entry_name_abstract::append_components_to_string
(std::string &buffer
,Path_inclusion inclusion
,wchar_t preferred_path_separator                                                //140202
,bool append_trailing_directory_separator)                                 const
{  std::wstring buffer_w;
   append_components_to(buffer_w,inclusion
      ,preferred_path_separator,append_trailing_directory_separator);
   append_wstring_to_string(buffer_w,buffer);
   return buffer;                                                                //120804
}
//_append_components_to_string______________________________________2012-02-09_/
const std::wstring &Directory_entry_name_abstract::get_Windows_representation
(std::wstring &buffer) const
{  return append_components_to (buffer,include_all,L'\\');
}
//_get_Windows_representation__________________________________________________/
const std::wstring &Directory_entry_name_abstract::get_Unix_representation
(std::wstring &buffer
,bool represent_any_drive_designator_to_root)                              const
{  std::wstring without_prefix;                                                  //111212
   append_components_to(without_prefix
      ,Directory_entry_name::include_qualification
      |Directory_entry_name::include_name
      |Directory_entry_name::include_extension_with_dot,L'/') ;
         // May need to exclude designator
   if (represent_any_drive_designator_to_root && is_qualified(true))             //111212
      // if has drive designator Convert drive designator to  /
      buffer.append(L"/");                                                       //111212
   buffer.append(without_prefix);                                                //111212
   return buffer;
}
//_get_Unix_representation_____________________________________________________/
const char *Directory_entry_name_abstract::c_str()                         const
{  CORN::wstring_to_string(*this,return_buffer_ASCII);
   return return_buffer_ASCII.c_str();
}
//_c_str_______________________________________________________________________/
const wchar_t *Directory_entry_name_abstract::w_str()                      const
{  return std::wstring::c_str();
}
//_w_str____________________________________________________________2012-02-09_/
bool Directory_entry_name_abstract::setup_is_root()
{  wchar_t path_separator = L'/';
   size_t path_separator_pos = find(L'\\');
   if  (path_separator_pos != std::string::npos)  // This appears to be a windows path
      path_separator = '\\';
   // else default to the compiled OS path separator setup in the constructor.

   path_separator_pos = find(path_separator);

   // NYI extract UNC

   size_t colon_pos = find(L":");    // Applies to Windows, AmigaDOS
   if (colon_pos != std::string::npos)
   {  nat32 len = length();
      if (colon_pos == len-1) // 131120 (c_str()[colon_pos + 1] == 0)
         root = true;
         // In this case, the name is the designator
         // The name which is a designator includes the colon.
   }
   // NYI   \\ In windows indicates a UNC
   if ((path_separator_pos == 0) &&
        (length() == 1))  // Applies to UNIX,
      root = true;
   if (length() == 0)
      root = true;                                                               //140201
   return root;
}
//_setup_is_root_______________________________________________________________/
bool  Directory_entry_name_abstract::is_qualified
(bool and_fully_qualified_with_drive_designator)                           const
{  const CORN::OS::Directory_name *path_directory_name = get_path_directory_name();
   if (is_root()) return true;   // root is considered to be fully qualified;    //140424
   bool qualified =                                                              //140424
      (path_directory_name)                                                      //140424
      ? (and_fully_qualified_with_drive_designator)                              //140424
         ? path_directory_name->is_qualified()                                   //140424
         : true                                                                  //140424
      : false;                                                                   //140424
   return qualified;                                                             //140424
}
//_is_qualified________________________________________________________________/
bool Directory_entry_name_abstract::has_designator_special()        affirmation_ //171204
{
   return
      (at(0) == '~')
   || (at(0) == '^')                                                             //200427
   || (at(0) == '<') || (at(0) == '>')                                           //200427
   || (at(0) == '$');
}
//_has_designator_special___________________________________________2017-12-04_/
bool Directory_entry_name_abstract::has_extension_Unicode
(const_UnicodeZ valid_ext,bool case_insensitive)                    affirmation_
{  nat32 extension_length = extension.length();                                  //170703
   if (!valid_ext)
   {
      return extension_length == 0;                                              //170703
         // If extension_length ==0 no extension is expected and no extension was found.
         //  extension_length then extension would not match

      //170703 if (extension_length == 0) return true;
   }
   if (extension_length != wcslen(valid_ext)) return false;
   // returns true if the extension matches valid_ext.
   // valid_ext can be either with . or without.
   // case is not significant for OS's without case sensitive filename.
   std::wstring compare_ext
      (valid_ext[0] == '.' ? &valid_ext[1] : valid_ext);
#if (!defined(__unix__) && (!defined(__GNUC__)))
// UNIX already case sensitive
#  if (__BCPLUSPLUS__ < 0x0550)
#     if (!defined(_MSC_VER) && !defined(__MINGW_ANSI_STDIO__))
   compare_ext.set_case_sensitive(directory_entry_name_is_case_sensitive);
#     endif
#  endif
#endif

#if ((__BCPLUSPLUS__ > 0) && (__BCPLUSPLUS__ < 0x0550))
   bool result = compare_ext.compare(extension) == 0;
#else
   const wchar_t *ext_c_str = get_extension().c_str();
   bool result =
        #if (defined(WINAPI) || defined(_Windows))
        _wcsicmp
        #else
        wcscmp
        #endif
        (compare_ext.c_str(),ext_c_str) == 0;
#endif
   return result;                                                                //990806
}
//_has_extension_Unicode____________________________________________2012-02-09_/
bool Directory_entry_name_abstract::has_extension_ASCII
(const_ASCIIZ valid_ext,bool case_insensitive)                     affirmation_
{  if (!valid_ext)
   {
      return (extension.length() == 0) ;
         // In this case no extension is expected and no extension was found.
         // then the extension is considered a match
         // otherwise no match
   }
   if (extension.length() != strlen(valid_ext)) return false;
   // returns true if the extension matches valid_ext.
   // valid_ext can be either with . or without.
   // case is not significant for OS's without case sensitive filename.
   std::string compare_ext
      (valid_ext[0] == '.' ? &valid_ext[1] : valid_ext);
   std::wstring valid_ext_unicode;
   CORN::string_to_wstring(compare_ext,valid_ext_unicode);
   return has_extension_Unicode(valid_ext_unicode.c_str(), case_insensitive);
}
//_has_extension_ASCII______________________________________________1999-07-26_/
int Directory_entry_name_abstract::compare_including
(const Directory_entry_name &other
,Path_inclusion inclusion)                                                const
{  ///< lexicorigraphically compares the respective selected elements specified in the inclusion.
   std::wstring this_string;
   std::wstring other_string;
   append_components_to(this_string,inclusion);
   other.append_components_to(other_string,inclusion);                           //140208
   return this_string.compare(other_string);
}
//_compare_including________________________________________________2009-12-09_/
const std::wstring &Directory_entry_name_abstract::get_name()              const
{  return name;
}
//_get_name_________________________________________________________2005-02-07_/
const std::wstring &Directory_entry_name_abstract::get_extension()         const
{  return extension;
}
//_get_extension____________________________________________________2005-02-07_/
const std::wstring &Directory_entry_name_abstract::get_name_with_extension
(std::wstring &result)                                                     const
{  result.clear();
   return append_name_with_extension_to(result);                                 //1301006
}
//_get_name_with_extension__________________________________________2005-02-07_/
const std::wstring &Directory_entry_name_abstract::append_name_with_extension_to
(std::wstring &result)                                                     const
{  return append_components_to(result,Directory_entry_name::include_name
      | Directory_entry_name::include_extension_with_dot);
}
//_append_name_with_extension_to____________________________________2013-10-06_/
const std::wstring &Directory_entry_name_abstract::append_name_to(std::wstring &result) const
{  return append_components_to(result,Directory_entry_name::include_name);
}
//_append_name_to___________________________________________________2013-10-07_/
const Directory_entry_name &Directory_entry_name_abstract::set_DEN
(const Directory_entry_name &to_copy_from)                         modification_
{  name = to_copy_from.get_name();                                               //131120
   extension = to_copy_from.get_extension();                                     //131120
   root = to_copy_from.is_root();
   assign(to_copy_from.w_str());
   return *this;
}
//_set_DEN__________________________________________________________2012-02-09_/
#ifdef NYN
const std::wstring &Directory_entry_name_abstract::set_cstr
(const char *to_copy_from)                                         modification_
{}
//_set_cstr_________________________________________________________2012-02-09_/
const std::wstring &Directory_entry_name_abstract::set_wstr
(const wchar_t *to_copy_from)                                      modification_
{}
//_set_wstr_________________________________________________________2012-02-09_/
#endif
//______________________________________________________________________________
const CORN::OS::Directory_name &Directory_entry_name_abstract
::get_parent_directory_name_qualified()                                    const
{  return *get_path_directory_name();
}
//_get_parent_directory_name_qualified______________________________2009-09-24_/
bool Directory_entry_name_abstract::is_root()                              const
{
   bool is_a_root = false;
   size_t FN_length = length();
   if (FN_length == 0)
   #ifdef __linux__
      is_a_root = true;                                                          //121204
   #else
      is_a_root = false;                                                         //200511
   #endif
   else
   {  char first_char = at(0);
      if (FN_length == 1)
      {  if ( (first_char == L'\\')
            ||(first_char == L'/'))
            is_a_root = true; // Windows root (without drive designator)
      } else if (name.length() == 2)
         if (name[1] == L':' )
            is_a_root = true;
   }
   return is_a_root;
}
//_is_root_____________________________________________________________________/
bool Directory_entry_name_abstract::has_parent_directory()          affirmation_
{  bool isRoot = is_root();
   bool hasPath = has_path();
   return (!isRoot && hasPath);
}
//_has_parent_directory_____________________________________________2012-05-29_/
bool Directory_entry_name_abstract::compose_relative_to
(const Directory_name &ref_dir_qual
   // Qualification does not need to be absolute (fully qualified)
   // but is is assumed this is similarly qualified
,std::wstring &composed // append to this
,Qualification qualification)                                         rendition_ //151221
{  if (!ref_dir_qual.is_qualified())
   {  // It is not sensible for ref_dir_to be relative
      append_components_to(composed);
      return false;
   }
   CORN::Unidirectional_list this_paths;  this->append_path_list_to(this_paths,include_ancestors_path);
   CORN::Unidirectional_list refs_paths;  ref_dir_qual.append_path_list_to(refs_paths,include_ancestors_path|include_name);
   std::wstring this_designator; this       ->append_designator_to(this_designator);
   std::wstring refs_designator; ref_dir_qual.append_designator_to(refs_designator);
   if (qualification == relative_partial_qualification)
      if (this_designator != refs_designator)
      {  // The files are on different file systems
         composed.append(this_designator);
      }
   std::wstring this_dir_ext; this->get_name_with_extension(this_dir_ext);
   //NA std::wstring refs_dir_ext;ref_dir_qual.get_name_with_extension(refs_dir_ext);

   CORN::Unidirectional_list rel_paths;
   std::wstring *relative_dirent = 0;
   if (is_qualified())
   {  bool paths_match = true;
      CORN::Item_wstring *this_path = dynamic_cast<CORN::Item_wstring *>(this_paths.pop_first());
      CORN::Item_wstring *refs_path = 0;
      while(this_path == ".." && refs_paths.count())
      {  delete this_path;
         refs_paths = dynamic_cast<CORN::Item_wstring *>(refs_paths.pop_first());
         this_path = dynamic_cast<CORN::Item_wstring *>(this_paths.pop_first());
      } // when we exhaust refs_paths then we essentially take the remainer of this in the following
      refs_path = dynamic_cast<CORN::Item_wstring *>(refs_paths.pop_first());
      if ((this_path && refs_path) && (*this_path) != (*refs_path))
      {  // The paths don't correspond from the start
         // the relative file is simply this
         this_paths.string_items(composed,DEFAULT_DIRECTORY_SEPARATORa_chr);
      }
      else
      {  while ((this_path && refs_path) && (*this_path) == (*refs_path))
         {  {  // so far still matching, just eat the paths
               delete this_path; this_path = 0;
               delete refs_path; refs_path = 0;
               this_path = dynamic_cast<CORN::Item_wstring *>(this_paths.pop_first());
               refs_path = dynamic_cast<CORN::Item_wstring *>(refs_paths.pop_first());
            }
          }
         while (refs_path)
         {  rel_paths.append(new CORN::Item_wstring(L".."));
            delete refs_path;
            refs_path = dynamic_cast<CORN::Item_wstring *>(refs_paths.pop_first());
         }
         while (this_path)
         {  // the ref paths are exhausted
            rel_paths.append(this_path);
            this_path = dynamic_cast<CORN::Item_wstring *>(this_paths.pop_first());
         }
         rel_paths.string_items(composed,DEFAULT_DIRECTORY_SEPARATORw_chr);
         composed += DEFAULT_DIRECTORY_SEPARATORw_chr;
      }
   } else  // this is not qualified
   {  if (qualification == relative_strict_qualification)
      {  composed += L".";
         composed += DEFAULT_DIRECTORY_SEPARATORw_str;
      }
   }
   composed.append(this_dir_ext);
   return relative_dirent;
}
//_compose_relative_to______________________________________________2012-04-30_/
/*190114 This is broken
std::wstring *Directory_entry_name_abstract::render_relative_to
(const Directory_name &qualified_reference_directory_name
,Qualification qualification)                                         rendition_ //151221
{  if (!qualified_reference_directory_name.is_qualified()) return 0;
   std::wstring *relative_dirent = 0;
   if (is_qualified())
   {  std::wstring *this_path = render_wstring(include_qualification_designated,L'/');
      std::wstring *ref_path  = render_wstring(include_qualification_designated,L'/');
      nat32 match_path_length = 0;
      bool  match_path_identified = false;
      for (size_type m = 0; m < ref_path->length() && !match_path_identified; m++)
      {  wchar_t char_m = this_path->at(m);
         if (char_m == ref_path->at(m))
         {  if (char_m == L'/')
            {  match_path_length = m;
            }
         } else
         {  match_path_identified = true;
         }
      }
      nat16 parent_count = 0;
      for (size_type r = match_path_length; r < ref_path->length(); r++)
      {  if (ref_path->at(r) == L'/')
            parent_count ++;
      }
      relative_dirent = new std::wstring;
      for (nat16 p =
            (relative_strict_qualification ? 0 : 1)                              //151221
            // I think this is it, not checked                                   //151221
          ; p < parent_count
          ; p++)
      {
         relative_dirent->append(L"..");
         relative_dirent->append(DEFAULT_DIRECTORY_SEPARATORw_str);
      }
      relative_dirent->append(this_path->substr(match_path_length+1));           //190114
      delete this_path;
      delete ref_path;
   } else
   {  relative_dirent =  new std::wstring(qualified_reference_directory_name.w_str());
      relative_dirent->append(DEFAULT_DIRECTORY_SEPARATORw_str);
      relative_dirent->append(name);
      if (extension.length())                                                    //131120
      {
      relative_dirent->append(L".");
      relative_dirent->append(extension);
      }
   }
   return relative_dirent;
}
//_2012-04-30___________________________________________________________________
*/
bool Directory_entry_name_abstract::is_in_root()                           const
{  bool in_root = !is_root();  // roots will not be in root.
   if (in_root)
   {  // so far it could be in root, but need to check the path.
      in_root = false;
      const Directory_name *parent_path = get_path_directory_name();             //140104
      in_root = parent_path && parent_path->is_root();                           //140114
   }
   return in_root;
}
//_is_in_root_______________________________________________________2009-09-25_/
const std::wstring &Directory_entry_name_abstract::append_designator_to
(std::wstring &buffer)                                                     const
{  return append_components_to(buffer,Directory_entry_name::include_designator);
}
//_append_designator_to_____________________________________________2014-02-02_/
const std::wstring &Directory_entry_name_abstract::append_separator_to
(std::wstring &buffer)                                                     const
{  wchar_t buffer_str[5];                                                        //130117
   buffer_str[0] = get_separator();
   buffer_str[1] = 0;
   buffer= buffer_str;                                                           //130117
   return buffer;
}
//_append_separator_to___________________________________2014-02-02_2012-08-24_/
wchar_t Directory_entry_name_abstract::get_separator()                provision_
{  if (!preferred_separator)
      preferred_separator =
      (has_parent_directory())
      ?   get_path_directory_name()->get_separator()
      : DEFAULT_DIRECTORY_SEPARATORw_chr;
   return preferred_separator;
}
//_get_separator____________________________________________________2014-02-02_/
CORN::Container & Directory_entry_name_abstract::append_path_list_to
(CORN::Container &buffer                                                         //181030
,Path_inclusion inclusion)                                                 const //091103
{  if (inclusion & Directory_entry_name::include_qualification_designated)
   {  // if including any part of the path
      if (has_parent_directory())                                                //120529
         get_parent_directory_name_qualified().
            append_path_list_to(buffer,inclusion | include_name | include_extension);
         // For the path we do include that name and extensions of ancestor directory names.
   }
   bool include_name= inclusion &Directory_entry_name::include_name;;
   bool include_ext = inclusion &Directory_entry_name::include_extension;
   if (is_root() && (!(inclusion &Directory_entry_name::include_designator)))    //190115
   {  include_name = false;                                                      //190115
      include_ext  = false;                                                      //190115
   }
   std::wstring name_and_or_extension;
   if (include_name)
       name_and_or_extension.append(get_name());
   if (include_ext && get_extension().length())
   {  // Phyiscal root directory (drive designators) usually
         // don't have an extension
         // however logical root directories may be a non root directory.
         // In the case of logical roots, often directory names don't have extension,
         // but that is perfectly acceptable in most OSs.
         if (inclusion | Directory_entry_name::include_extension_dot)
            name_and_or_extension.append(L".");
         name_and_or_extension.append(get_extension());
   }
   if (name_and_or_extension.length())
      buffer.append(new Item_wstring(name_and_or_extension));                    //181030
   return buffer;
}
//_append_path_list_to_________________________________________________________/
wchar_t *invalid_name_component_characters = L"<>\/?%*:|\"";
bool Directory_entry_name_abstract::is_valid_name_component
(const std::wstring &name_component)                                affirmation_
{
   // NYI actually should have  get_invalid_name_component_characters
   // so derived OS classes can specify invalid characters
   // currently both Unix/Linux and Windows have the same invalid characters
   wchar_t *invalid_char_found = wcspbrk
      (name_component.c_str(),invalid_name_component_characters);
   return invalid_char_found == 0;
}
//_is_valid_name_component__________________________________________2016-01-20_/
wchar_t *invalid_name_path_characters = L"<>?%*|";
bool Directory_entry_name_abstract::is_valid()                      affirmation_
{  size_t invalid_found_pos = find_first_of(invalid_name_path_characters);
   return (invalid_found_pos == std::wstring::npos)
      && length();                                                               //180612
}
//_is_valid_________________________________________________________2018-02-01_/
bool Directory_entry_name_abstract::has_extension_any(bool must_be_valid)  const
{  ///< Returns true if the filename has a file extension .
   bool has = get_extension().length();
   if (must_be_valid)
      has = is_valid_name_component(get_extension());
   return has;
}
//_has_extension_any____________________________________2016-01-20__1998-09-19_/
bool Directory_entry_name_abstract::has_name(bool must_be_valid)           const
{  bool has = get_name().length();
   if (has && must_be_valid)                                                     //160219
      has = is_valid_name_component(get_name());
   return has;
}
//_has_name_____________________________________________2016-01-20__1998-09-19_/
bool Directory_entry_name_abstract::has_path()                             const
{  const CORN::OS::Directory_name *path_directory_name = get_path_directory_name();
   return path_directory_name && path_directory_name->has_name();
}
//_has_path_________________________________________________________2012-02-06_/

// The caller should query the file system if needed

File_system::Directory::Type Directory_entry_name_abstract::get_type()     const
{
   if (identified_type == unknown_entry)
       identified_type = file_system_engine.identify_type(*this);
   return identified_type;
}
//_get_type_________________________________________________________2013-11-20_/
bool Directory_entry_name_abstract::is_directory()                  affirmation_
{  return get_type() == directory_entry;
}
//_is_directory____________________2018-12-18_2013-11-20_2012-04-15_2012-02-14_/
bool Directory_entry_name_abstract::is_file()                       affirmation_
{  return get_type() == file_entry;
}
//_is_file____________________________________2013-11-20_2012-04-15_2012-02-14_/
Directory_entry_name *Directory_entry_name_abstract::render_with_respect_to
(const Directory_entry_name &respect_DEN)                             rendition_
{
   Directory_entry_name *rendered = 0;
   if (Item::compare(respect_DEN) == 0)
   {  // special case where the file is the same
      return new CORN::OS::Directory_entry_name_concrete(this->w_str());  // actually simply copy this as DEN
   }
   CORN::Text_list     old_path;
   append_path_list_to(old_path,include_qualification_designated); // omitting filename.ext
   CORN::Text_list respect_path;
   respect_DEN .append_path_list_to(respect_path,include_qualification_designated); // omitting filename.ext
   nat8 old_dir_count      = old_path     .count();
   nat8 respect_dir_count  = respect_path .count();
   nat8 dir_match_start_in_respect  =255;
   nat8 dir_match_start_in_old      =255;
   nat8 dir_match_end_in_respect    =  0;
   nat8 dir_match_end_in_old =0;
   for (nat8 r = 0; r < respect_dir_count; r++) // respect index
   {  const Item_wstring *respect_r = dynamic_cast<const Item_wstring *>(respect_path.get_at(r));
      for (nat8 o = 0; o < old_dir_count; o++)  // old index
      {  const Item_wstring *respect_o = dynamic_cast<const Item_wstring *>(old_path.get_at(o));
         bool matches = respect_r->wstring::compare(*respect_o) == 0;
         if (matches)
         {  dir_match_start_in_respect = std::min<nat8>(dir_match_start_in_respect  ,r);
            dir_match_start_in_old     = std::min<nat8>(dir_match_start_in_old      ,o);
            dir_match_end_in_respect   = std::max<nat8>(dir_match_end_in_respect    ,r);
            dir_match_end_in_old       = std::max<nat8>(dir_match_end_in_old        ,o);
   }  }  }
   CORN::Text_list resolved;
   if (dir_match_start_in_respect < 255)
      for (nat8 r = 0; r <= dir_match_end_in_respect; r++)
      {  Item_wstring *respect_r = dynamic_cast <Item_wstring *>(respect_path.pop_at_head());
         resolved.take(respect_r);
      }
   if (dir_match_start_in_old < 255)
   {  for (nat8 o = 0 ; o <= dir_match_end_in_old; o++)
      {  Item_wstring *eliminate_f = dynamic_cast <Item_wstring *>(old_path.pop_at_head());
         delete eliminate_f;
      }
      resolved.transfer_all_from(old_path);
   }
   if (resolved.count())
   {
      std::wstring name (get_name());                                            //180927
      std::wstring ext  (get_extension());                                       //180927
      resolved.add_wstring(name);
      resolved.add_wstring(ext);
      rendered = new Directory_entry_name_concrete(resolved);                    //180927
   }
   return rendered;
}
//_render_with_respect_to________________________________2018-01-03_2007-01-06_/
Directory_entry_type Directory_entry_name_abstract::type_determination_raw_char
(char last_char)                                                           const
{  return ((last_char == '\\') ||(last_char == '/') ||(last_char == 0))
      ? directory_entry : unknown_entry;
}
//_type_determination_raw_char______________________________________2018-12-18_/
Directory_entry_type Directory_entry_name_abstract::type_determination_raw_str
(const char *raw_name)                                                     const
{
   size_t last_char_pos = strlen(raw_name) -1;
   return type_determination_raw_char((char)raw_name[last_char_pos]);
}
//_type_determination_raw_string____________________________________2018-12-18_/
Directory_entry_type Directory_entry_name_abstract::type_determination_raw_string
(const std::string &raw_name)                                              const
{
   size_t last_char_pos = raw_name.length() -1;
   return type_determination_raw_char((char)raw_name[last_char_pos]);
}
//_type_determination_raw_string____________________________________2018-12-18_/
Directory_entry_type Directory_entry_name_abstract::type_determination_raw_wstr
(const wchar_t *raw_name)                                                  const
{
   size_t last_char_pos = wcslen(raw_name) -1;
   return type_determination_raw_char((char)raw_name[last_char_pos]);
}
//_type_determination_raw_string____________________________________2018-12-18_/
Directory_entry_type Directory_entry_name_abstract::type_determination_raw_wstring
(const std::wstring &raw_name)                                             const
{
   size_t last_char_pos = raw_name.length() -1;
   return type_determination_raw_char((char)raw_name[last_char_pos]);
}
//_type_determination_raw_string____________________________________2018-12-18_/
}}//_namespace_CORN_OS_________________________________________________________/

