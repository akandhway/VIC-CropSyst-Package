#include "corn/format/VV/VV_stream.h"
namespace CORN
{
namespace VV
{
//______________________________________________________________________________

//______________________________________________________________________________
}}//_namespace_CORN::VV_________________________________________________________/
/*
   by Roger Nelson   rnelson@mail.wsu.edu
*/

/* original VV file

#if (defined(_WIN32)  || defined(_WIN64) || defined(__MSDOS__))
#     include <windows.h>
#  endif
#ifndef _INC_STDIO
#  include <stdio.h>
#endif
#ifndef _INC_STDLIB
#  include <stdlib.h>
#endif
#  ifndef strconvHPP
#     include "corn/string/strconv.hpp"
#  endif
#ifndef datarecH
#  include "corn/data_source/datarec.h"
#endif
#ifndef vv_entryH
#  include "corn/data_source/vv_entry.h"
#endif
#include "corn/OS/file_system_engine.h"
#include "corn/OS/directory_entry_name_concrete.h"


#if (defined(_WIN32)  || defined(_WIN64) || defined(__MSDOS__))
#     ifndef _INC_IO
#       include <io.h>
#     endif
#     ifdef _MSC_VER
#        define getcwd(a,b) _getcwd(a,b)
#        define chdir(x) _chdir(x)
#        define access(a,b) _access(a,b)
#     else
#        include <dir.h>
#     endif
#     include <direct.h>
#     include <dos.h>
#  endif

#ifdef __unix
#include <unistd.h>
#endif
*/
/*

//______________________________________________________________________________
bool VV_File::read(std::istream &strm,Data_record &data_rec, bool unexpected_only,bool single_section_mode)
{  nat32 data_read = false;                                                      //130401_050215
   bool started_single_section = false;                                          //050215
   int16 contiguous_blank_line_count = 0;  //    Bug fix for BC5                 //970614
   while (!strm.eof() && (contiguous_blank_line_count < 20))                     //050215
   {  nat32 this_line_pos = strm.tellg(); // remember current position in case this line is the start of a next section for single_section_mode mode
      std::string buffer;                                                        //131116
      getline(strm,buffer);                                                      //110725
      strip_string(buffer,Trailing,'\r');                                        //131116
      if (buffer.length())
      {
         Submission_status status = submit(data_rec,buffer,unexpected_only);
         data_read += (status != unrecognized);
         bool is_section = (status == section_known) || (status == section_unknown);
         if (is_section)
            if (single_section_mode)                                             //050215
            {  // This is a special mode for reading a single section at a time  //050215
               if (started_single_section)                                       //050215
               {  // this line is the start of the next section so we can return now   //050215
                  this->strm.seekg(this_line_pos);                               //050215
                  return true;                                                   //050215
               }                                                                 //050215
               started_single_section = true;                                    //050215
            }                                                                    //050215
          contiguous_blank_line_count = 0;
      } else
         contiguous_blank_line_count++;
   }
  return data_read > 0;                                                          //130401_050215
}
//______________________________________________________________________________
bool VV_File::exists() const
{  return file_name && OS::file_system_engine.exists((*file_name));              //160128
}
//______________________________________________________________________________
bool VV_File::get(Data_record &data_rec)
{  bool result = false;
   if (exists())  //  Don't try to open empty filename (this shouldn't be needed, presumably f.good() should detect file the file doesn't exist
   {  std::ifstream f
         (file_name->c_str());                                                    //160128
         //160128 (file_name);                                                //020125
      if (f.good())
      { data_rec.get_start();                                                    //010109
        read(f,data_rec,false);                                                  //010109
        data_rec.get_end();                                                      //010109
        if (!remember_structure)                                                 //030516
           data_rec.delete_structure();                                          //010109
        result = true;                                                           //010109
     }                                                                           //980405
   }                                                                             //980922
   return result;                                                                //020208
}
//_2001-01-09___________________________________________________________________
bool VV_File::set_data
(Data_record &data_rec
,bool         append)                                                            //150502
{  if (!file_name)                                                               //020125
      return false;  //  Don't try to open empty filename
      data_rec.set_current_section(data_rec.get_primary_section_name());
   bool result = false;                                                          //980405
    // Make sure we have structures for enuermated sections.
   FOR_EACH_IN(sect,VV_enumerated_section,data_rec.enumerated_sections_definitions,each_sect)//030709
     sect->setup_structure(data_rec,true);                                       //970929
   FOR_EACH_END(each_sect)                                                       //030709
   if (data_rec.get_preserve_unexpected_entries() && !append)                    //150502_080820
   {  std::ifstream strm(file_name->c_str());                                    //160218
      read(strm,data_rec,true);
   }
   std::ofstream strm(file_name->c_str()                                         //160128
      ,append?std::ios::app:std::ios::out);                                      //150502_980415
   {  data_rec.sections.write(strm);
      // Continue here I should now be able to use the new container Format
      result = true;                                                             //010109
   }
   data_rec.delete_structure();                                                  //010112
   //050705  probably want to move delete structure to Data_source::set()
   return result;                                                                //010109
}
//_2005-07-05___________________________________________________________________
VV_File::VV_File(const char *file_name_, bool _remember_structure)
: VV_Data_source()                                                               //130401_010108
,file_name(0)                                                                    //020125
,remember_structure(_remember_structure)                                         //030519
{  set_file_name_cstr(file_name_);                                               //160128_020125
}
//_1998-04-07___________________________________________________________________
const OS::File_name &VV_File::set_file_name_cstr(const char *filename_)
{  int file_name_len = filename_ ? strlen(filename_)+1 : 0;
   if (file_name_len)
      file_name = new OS::File_name_concrete(filename_);                         //160128
   return *file_name;
}
//_2016-01-28___________________________________________________________________
const OS::File_name &VV_File::set_file_name(const OS::File_name &file_name_)
{  file_name = new OS::File_name_concrete(file_name_);
   return (*file_name);
}
//_2016-01-28___________________________________________________________________
VV_File::~VV_File()
{  delete file_name;
}
//______________________________________________________________________________
bool VV_File::inherit
(Data_record &data_rec
,const std::string &key, const std::string &inherit_URL)
{
   bool inherited = file_name != 0;
   if (!inherited) return 0;
   // Just a sanity check, because at this point we should actually always have
   // filename that is currently being loaded.

   // Currently assuming URL is a file on this machine,
   // either relative or fully qualified.
   // but it would be nice to be able to load files anywhere off the Internet.
   OS::File_name_concrete inherit_filename_unresolved(inherit_URL);
   OS::File_name *inherit_filename_resolved =
      OS::file_system_engine.render_resolved_absolute_DEN //161013 directory_entry_name
      (inherit_filename_unresolved
      ,file_name->get_parent_directory_name_qualified());
   inherited
      =  (inherit_filename_resolved != 0)
      && OS::file_system_engine.exists((*inherit_filename_resolved));
   if (inherited)
   {  // Now read at this point from the inherited file

      std::ifstream VV_stream(inherit_filename_resolved->c_str());
      inherited = read(VV_stream,data_rec,false);
   } else
   {
      std::cerr << "error: " << "name=" << inherit_filename_unresolved.c_str()
         << " resolved to=" << inherit_filename_resolved->c_str()
         << " not found." << std::endl;
   }
   delete inherit_filename_resolved;
   return inherited;
}
//_2016-01-28___________________________________________________________________
}//_namespace_CORN_____________________________________________________________/

*/

