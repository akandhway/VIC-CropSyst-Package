#  include "string.h"
#  include "corn/string/strconv.hpp"
#  include "corn/OS/filtered_filename.h"
#  include "corn/OS/file_system_engine.h"
#  include "corn/OS/file_system_types.h"

namespace CORN {
//______________________________________________________________________________
Filtered_file_name::Filtered_file_name(const Filtered_file_name &from_copy)
: OS::Directory_name_concrete(from_copy)
{  FOR_EACH_IN(file_type_desc,OS::File_type_description,from_copy.file_type_descriptions,each_desc)
      file_type_descriptions.append(new OS::File_type_description(*file_type_desc));
   FOR_EACH_END(each_desc)
}
//_Filtered_file_name:constructor___________________________________1998-02-09_/
Filtered_file_name::Filtered_file_name
(const char *fname
,const char *i_preferred_extension  // The preferred extension (without .)
,const char *i_filter_label         // Short description of the file type
,const char *i_wildcard_filter)     // I.e.  *.DAT if (0), *. and preferred extension will be used
: OS::Directory_name_concrete
(fname ? fname : "" )
{
   std::string fname_ext;                                                        //120212
   CORN::wstring_to_string(get_extension(),fname_ext);                           //120212
   CORN::OS::Extension                                                           //161101
      preferred_extension                                                        //990806
         (i_preferred_extension                                                  //020817
         ?i_preferred_extension:fname_ext.length()
         ?fname_ext.c_str()+1:"");
   if (!has_extension_any() && preferred_extension.length())                     //161101
   {
      name = get_name();
      extension = preferred_extension;
   }
   std::string def_wildcard("*.");
   CORN::append_wstring_to_string(preferred_extension,def_wildcard);             //161101
   if (!CORN::OS::file_system_engine.exists(*this))                              //120212
      extension = preferred_extension;                                           //161101_131120
   // Only add file type if label or wild card specified.                        //000204
   if ((fname == 0) || ((fname[0] == 0) && (preferred_extension == L"")))        //030925
      clear();                                                                   //120212
   if (i_filter_label || i_wildcard_filter)                                      //000204
   {  std::string preferred_extension_str; CORN::wstring_to_string(preferred_extension,preferred_extension_str);
      // eventually the filter will use wide strings
      add_file_type
      (preferred_extension_str/* preferred_extension*/ .c_str()
      ,(i_filter_label ? i_filter_label : "")
      ,(i_wildcard_filter ? i_wildcard_filter : def_wildcard.c_str()));
   }
}
//_Filtered_file_name:constructor______________________________________________/
Filtered_file_name & Filtered_file_name::set_with_filters(const  Filtered_file_name &_filename)
{  const Bidirectional_list &file_type_descs_to_copy = _filename.get_file_type_descriptions();
   FOR_EACH_IN(type_desc,OS::File_type_description,file_type_descs_to_copy,each_desc)
       add_file_type(*type_desc);
   FOR_EACH_END(each_desc)
   set_DEN(_filename);                                                           //121126
   return (*this);
}
//_set_with_filters_________________________________________________1999-08-06_/
const std::string  &Filtered_file_name::append_preferred_extension_to(std::string &result_preferred_extension) const
{  Container::Iterator *iter = file_type_descriptions.iterator();                //030709
   OS::File_type_description *first_desc = // Will almost always have at least one description
       (OS::File_type_description *)iter->first();                               //030709
   delete iter;                                                                  //030709
   if (first_desc)
      CORN::wstring_to_string(first_desc->get_preferred_extension(),result_preferred_extension); //13115
   else
      result_preferred_extension.assign("*");
   return result_preferred_extension;
}
//_append_preferred_extension_to____________________________________2000-11-09_/
const std::string &Filtered_file_name::append_filter_label_to(std::string &result_filter_label) const
{  Container::Iterator *iter = file_type_descriptions.iterator();                //030709
   OS::File_type_description *first_desc = // Will almost always have at least one description
       (OS::File_type_description *)iter->first();                               //030709
   delete iter;                                                                  //030709
   if (first_desc)
      CORN::wstring_to_string(first_desc->get_filter_label(),result_filter_label); //131115
   else  result_filter_label.assign("*");                                        //080325
   return result_filter_label;                                                   //080325
}
//_append_filter_label_to______________________________________________________/
void Filtered_file_name::set_filter_label(const std::string &new_filter_label) modification_
{  Container::Iterator *iter = file_type_descriptions.iterator();                //030709
   OS::File_type_description *first_desc = // Will almost always have at least one description
      (OS::File_type_description *)iter->first();                                //030709
   delete iter;                                                                  //030709
   if (first_desc)
         first_desc->set_filter_label_string(new_filter_label);
}
//_set_filter_label_________________________________________________2000-09-28_/
const std::string &Filtered_file_name::append_wildcard_filter_to
(std::string &wild_card_filter)                                            const
{ Container::Iterator *iter = file_type_descriptions.iterator();
   OS::File_type_description *first_desc = // Will almost always have at least one description
       (OS::File_type_description *)iter->first();                               //030709
   delete iter;                                                                  //030709
   if (first_desc)
      CORN::wstring_to_string(first_desc->get_wildcard_filter(),wild_card_filter);//080325
   else wild_card_filter.assign("*");                                            //080325
   return wild_card_filter;                                                      //080325
}
//_append_wildcard_filter_to________________________________________2000-11-09_/
const std::string &Filtered_file_name::append_file_type_pattern_for_Windows_to
(std::string &file_type_pattern)                                           const //080325
{  file_type_pattern.assign("");
   FOR_EACH_IN(type_desc,OS::File_type_description,file_type_descriptions,each_desc) //030709
   {
      type_desc->label_string(file_type_pattern);                                //170423
      //170423 char file_pattern[512];                                           //020125
      //170423 file_type_pattern.append(type_desc->label_cstr(file_pattern));    //080325
   } FOR_EACH_END(each_desc)
   return file_type_pattern;                                                     //080325
}
//_append_file_type_pattern_for_Windows_to__________________________2000-11-09_/
void Filtered_file_name::add_file_type
(const char *_preferred_extension   // The preferred extension (without DOT)
,const char *_filter_label    // Short description of the file type
,const char *_wildcard_filter)      // I.e.  *.DAT if (0), *. and preferred extension will be used
{  file_type_descriptions.append(new OS::File_type_description(_preferred_extension,_filter_label,_wildcard_filter));
}
//_add_file_type____________________________________________________1999-08-06_/
void Filtered_file_name::add_file_type(const OS::File_type_description &type_desc)
{  file_type_descriptions.append(new OS::File_type_description(type_desc));
}
//_add_file_type____________________________________________________1999-08-06_/
const Bidirectional_list &Filtered_file_name::get_file_type_descriptions() const
{ return file_type_descriptions; }
//_get_file_type_descriptions_______________________________________1999-08-06_/
}//_namespace_CORN_OS__________________________________________________________/

