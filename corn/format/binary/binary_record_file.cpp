// for itoa
#  include "corn/string/strconv.hpp"
#  include "corn/format/binary/binary_record_file.h"
#if defined (__unix) || defined(unix) || defined(__GNUC__)
#     include <sys/stat.h>
#  else
#     include <io.h>
#  endif
#  include <stdlib.h>
#  include <sys/stat.h>
#include "corn/format/binary/binary_file_fstream.h"
namespace CORN {
//______________________________________________________________________________
nat16  Binary_record_file_dynamic::get_free_code(Record_code current_code) const
{ return free_code;}
//_Binary_record_file_dynamic::get_free_code________________________2000-07-03_/
Binary_record_file_dynamic::~Binary_record_file_dynamic()
{  done_with_stream();                                                           //050522
#if ((!defined(__unix)) && (!defined(_MSC_VER)) && (!defined(__GNUC__)))
   _rtl_chmod(get_file_name(), 0);
#endif
   /* disabled this because it is actually clearing permissions
   chmod(get_file_name(), S_IREAD | S_IWRITE);
   */
   //140119    if (filename_BRF) delete[] filename_BRF; filename_BRF = 0;                    //050522
   delete filename_BRF;                                                          //140119
}
//______________________________________________________________________________
Binary_file_interface *Binary_record_file_dynamic::provide_binary_file()provision_
{  if (!binary_file && filename_BRF)                                             //140119
      binary_file = new Binary_file_fstream(filename_BRF->c_str(),big_endian,openmode);   //130311
   return binary_file;
}
//_Binary_record_file_dynamic::provide_binary_file__________________2005-05-22_/
void Binary_record_file_dynamic::done_with_stream()
{
// Need to check if this function is finally obsolete
// BCB5 Not needed for Weather Editor/ClimGen (close and delete crashes)
// BCB5 Check if needed for dBase
// BDS  Needed
// BC5  check if needed

   // With streams we can just delete the stream it will close automatically
   delete binary_file; binary_file = 0;
}
//_Binary_record_file_dynamic::done_with_stream_____________________2005-05-22_/
Binary_record_file_dynamic::Binary_record_file_dynamic
(const char *              _filename
,bool                      _big_body_sizes   // True if record header size field is 32bits
,std::ios_base::openmode   _openmode /* = (std::ios_base::in | std::ios_base::out)*/
,bool                      _big_endian)      // Most MS-DOS or Windows base software files are not big_endian
: filename_BRF(0)
, binary_file (0)                                                                //070622
, BOF_code     (0)                                                               //030208
, EOF_code     (0xFFFF)                                                          //030208
, free_code    (-1)                                                              //030208
, openmode     (_openmode|std::ios_base::binary)                                 //170117_130311
, big_endian   (_big_endian)                                                     //050522
, at_eof       (false)
, big_body_sizes(_big_body_sizes)                                                //990106
{  // Derived classes should set these values
   BOF_code = 0;                                                                 //030208
   EOF_code = 0xFFFF;                                                            //030208
   free_code= -1;                                                                //030208
   if (_filename) filename_BRF = new std::string(_filename);                     //141019
}
//_Binary_record_file_dynamic:constructor______________________________________/
Binary_record_file_dynamic_indexed::Binary_record_file_dynamic_indexed
(const char *              _filename
,bool                      _big_body_sizes   // True if record header size field is 32bits
,std::ios_base::openmode   _openmode                                             //130311
,bool                      _big_endian)      // Most MS-DOS or Windows base software files are not big_endian
: Binary_record_file_dynamic(_filename,_big_body_sizes,_openmode,_big_endian)
, read_interator(0)                                                              //100128
, index()                                                                        //100127
, initialized(false)                                                             //000705
{}
//_Binary_record_file_dynamic_indexed:constructor______________________________/
Container::Iterator *Binary_record_file_dynamic_indexed::provide_read_interator()provision_
{  if (!read_interator)
        read_interator = index.iterator();
   return read_interator;
}
//_provide_read_interator___________________________________________2010-01-28_/
bool Binary_record_file_dynamic_indexed::initialize()
{
   Binary_file_interface* using_binary_file = provide_binary_file();             //140119_070622
   if (using_binary_file)                                                        //140119
   {
   nat32 file_length = binary_file->get_file_length();
   std::ios_base::openmode mode = binary_file->get_open_mode();                  //170117
   index_node = 0;                                                               //030710
   if (!file_length)  // If the file doesn't exits, so, this can't be for input  //990426
   {  if (mode == std::ios::in)
      {  //150130  Not sure why I returned false, because I do need the index
         //150130  return false;                                    //140728
      }
      else                                                                       //140728
      mode  =
         #ifndef __unix
         std::ios::binary |
         #endif
         std::ios::out ;
   }
   if ((mode & std::ios::in) && file_length)                                     //990409
   {  index.delete_all(); // This may actually be a reinitialization, so make sure head list is clear. 990130
      nat32 file_position = 0;
      bool at_eof_ = false;
      while (!at_eof_
         && file_position < file_length)                                         //080912
      {
         if (binary_file->seek(file_position,std::ios::beg)) // This should skip us over the body of the record to the beginning of the next record 080912
         {
         Binary_record_index_node_dynamic *new_index_node = new Binary_record_index_node_dynamic(*binary_file,big_body_sizes);
         new_index_node->know_position(file_position);                           //080911
         index.append(new_index_node);
         // Note that the EOF marker in Excel does not indicate the end of the file
         at_eof_ =
            #ifdef  __linux__
            !binary_file->good() ||
            #else
            binary_file->at_eof() ||                                             //000705
            #endif
              (file_position >= file_length); // <- Incase something goes wrong with the file.
         file_position += new_index_node->get_node_record_length(big_body_sizes);//080911
         }
      }
      binary_file->seek(0);                                                      //050522
   }
   }
   initialized = true;                                                           //000705
   return initialized;                                                           //140119
}
//_Binary_record_file_dynamic_indexed::initialize______________________________/
void Binary_record_file_dynamic::reset()
{  if (binary_file) binary_file->seek(0);                                        //050522
}
//_Binary_record_file_dynamic::reset________________________________2000-07-05_/
void Binary_record_file_dynamic_indexed::reset()
{  if (!initialized) initialize();
   Binary_record_file_dynamic::reset();
   delete read_interator;                                                        //140119
}
//_Binary_record_file_dynamic_indexed::reset___________________________________/
Binary_record  *Binary_record_file_dynamic_indexed::read_record(std::fstream::seekdir from )
{  // If from is beg then the file is reset.
   // if from is curr read continues from current position.
   // current B.R._index_node record will be returned (loaded if not in memory).
   if (!initialized) initialize();
   if (from == std::ios::beg)
      at_eof = false;
   if (at_eof) return 0;
   Container::Iterator *index_interator = provide_read_interator();              //030709
   if ((from ==  std::istream::beg) || (index_node == 0))                        //030710
   {  index_node = (Binary_record_index_node_dynamic *)index_interator->first(); //030710
   }                                                                             //030709
   else                                                                          //030710
   {  index_interator->find(index_node);                                         //040106
      Binary_record_index_node_dynamic *curr_node = (Binary_record_index_node_dynamic *)index_interator->next();  //030709
      if (!curr_node)
         at_eof = true;
      index_node = curr_node;                                                    //031212
   }
   return  index_node ? index_node->get(*this) : 0;
}
//_Binary_record_file_dynamic_indexed::read_record__________________2000-07-03_/
Binary_record  *Binary_record_file_dynamic_indexed::read_record_of_type(Record_code rec_code,std::fstream::seekdir from )
{
   // If from is beg then the file is reset.
   // if from is curr read continues from current position.
   // scan from the current B.R._index_node until one if found with a matching record type code.
   // record will be returned (loaded if not in memory).
   if (!initialized) initialize();
   Binary_record  *result = 0;
   Container::Iterator *index_interator =  provide_read_interator();             //030709
   Binary_record_index_node_dynamic *curr_node = 0;                              //020808
   curr_node = ((from == std::istream::beg) || (index_node == 0))                //030710
   ? (Binary_record_index_node_dynamic *)index_interator->first()                //030710
   : (Binary_record_index_node_dynamic *)index_interator->next();                //100128
   while (curr_node&& (curr_node->get_record_code() != rec_code))                //020808
      curr_node = (Binary_record_index_node_dynamic *)index_interator->next();   //020808
   if (curr_node)                                                                //020808
      result = curr_node->get(*this);                                            //040106
   index_node = curr_node;                                                       //030710
   return result;
}
//_Binary_record_file_dynamic_indexed::read_record_of_type__________2000-07-03_/
void Binary_record_file_dynamic_indexed::finished_with(Binary_record *record)
{
   // This method is used when a database or program indicates it no longer needs
   // to keep record in memory.
   // traverse the list of B.R. index nodes, find the one having the matching record pointer.
   // call that node's finish() method.
   // Record will be deleted so the calling object must never reference
   // the record memory again.
   Binary_record_index_node_dynamic search_node(record);                         //050522
   Binary_record_index_node_dynamic *node_to_finish = (Binary_record_index_node_dynamic *)index.find_matching(search_node);
   search_node.relinquish(); // don't let the node delete record, we just temporary borrowed it.
   if (node_to_finish)
   {  Binary_record *resized_record = node_to_finish->finish(*this);
      if (resized_record)
      {  if (provide_binary_file())                                              //140119_070622
         {
         // I am not sure if finish_index_node should be this->index_node
         Binary_record_index_node_dynamic *finish_index_node = new Binary_record_index_node_dynamic(*binary_file,resized_record,big_body_sizes,true,is_read_only());   //040210
         Binary_record *returned_record_should_be_null = finish_index_node->finish(*this); // This finish should always succeed. //040210
         //140119 already checks existence if (returned_record_should_be_null)
            delete returned_record_should_be_null;
         }
      }
   }
}
//_Binary_record_file_dynamic_indexed::finished_with________________2000-07-03_/
Binary_record *Binary_record_file_dynamic_indexed::relinquish(Binary_record *record)
{
   // This is used when the application is going to keep the
   // record data locally it will no longer be pointed to by R.B. index
   // So the record in the corresponding record node list
   // will be replace with just the header info (it will not be deleted).
   // The object the record is relinquished to is responsible for
   // deleting the record data structure.
   Binary_record_index_node_dynamic search_node(record);
   Binary_record_index_node_dynamic *node_to_relinquish = (Binary_record_index_node_dynamic *)index.find_matching(search_node);
   search_node.relinquish(); // don't let the node delete record, we just temporary borrowed it.
   if (node_to_relinquish)
      node_to_relinquish->relinquish();
   return record;
}
//_Binary_record_file_dynamic_indexed::relinquish___________________2000-07-03_/
Binary_record *Binary_record_file_dynamic_indexed::append_record
(Binary_record *record_to_append,bool write_immediately)             submission_
{  if (record_to_append)
   {  Binary_record_index_node_dynamic *app_index_node =                         //140119 0 ;
         (write_immediately &&  provide_binary_file())                           //140119_050522
      ? new Binary_record_index_node_dynamic(*binary_file,record_to_append,is_big_body_sizes(),write_immediately,is_read_only())
      : new Binary_record_index_node_dynamic(record_to_append);
      index.append(app_index_node);
   }
   return record_to_append;
}
//_Binary_record_file_dynamic_indexed::append_record________________2000-07-06_/
bool Binary_record_file_dynamic_indexed::finalize() //190717 finish()
{  // Traverse the B.R. index nodes and finish any modified records.
   // Any records that current size does not match the size in the
   // File will be appended to the end of the file as a new record and the record
   // code will be changed to deleted. 030709
   if (provide_binary_file())                                                    //140119_070622
   FOR_EACH_IN(fin_index_node,Binary_record_index_node_dynamic,index,each_node); //030709
   {  Binary_record *record_that_did_not_fit =  fin_index_node->finish(*this);
      if (record_that_did_not_fit && !is_read_only() && record_that_did_not_fit->is_valid())   //130311 050603
      {  // If finished node has a record in memory that was modified
         // and the record no longer fits, we have to write the record to the end of the file
         binary_file->seek(0,std::ios::end);
         record_that_did_not_fit->write(*binary_file ,is_big_body_sizes());
      }
      delete record_that_did_not_fit;
   } FOR_EACH_END(each_node)                                                     //030709
   return true;
}
//_Binary_record_file_dynamic_indexed::finalize_____________________2000-08-28_/
Binary_record_file_dynamic_indexed::~Binary_record_file_dynamic_indexed()
{  if (!is_read_only()) // Had problems with crop potential simulation           //080617
      finalize/*190717 finish*/();  // shouldn't need to 'finish' records when not writing.
}
//_Binary_record_file_dynamic_indexed:destructor_______________________________/
nat32 Binary_record_file_dynamic_indexed::get_position_of
(const Binary_record *record) const
{  FOR_EACH_IN(node ,Binary_record_index_node_dynamic, index,each_node)
      if (node->record == record)
         return node->position;
   FOR_EACH_END(each_node)
   return 0;
}
//_get_position_of_____________________________________________________________/
Binary_record_index_node_dynamic *Binary_record_file_dynamic_indexed
::get_index_node_for(const Binary_record *record)
{  // This is used when a derived class database wants to maintain an index of
   // records base on some search criteria.  The derived index should keep
   // pointers to index nodes not the record itself, so that the update and I/O
   // of the record and database is done by this class not the derived class.    //030709
   Binary_record_index_node_dynamic search_node(const_cast<Binary_record *>(record));  //050522
   Binary_record_index_node_dynamic *node_to_get = (Binary_record_index_node_dynamic *)index.find_matching(search_node);
   search_node.relinquish(); // don't let the node delete record, we just temporary borrowed it.
   return node_to_get;
}
//_get_index_node_for_______________________________________________2000-07-06_/
Binary_record *Binary_record_file_dynamic_indexed::replace(Binary_record *existing_record, Binary_record *with_record,bool free_existing_record)
{  if (existing_record == with_record) return existing_record;                   //100105
   // if they are the exact same record do nothing (may occur when updating records such as geolocation).
   // This replaces an existing record with the specified record
   // If  the replacement is not the same size as the original,
   // the existing record space will be freed in the file and with_record append.
   // The existing record must be just previously read (with read_record) or appended and not finished_with(). //030709
   Binary_record *result = 0;
   with_record->set_length(with_record->compute_record_body_size());             //001008
   if (with_record->get_length() != existing_record->get_length())               //050505
   {  // mark existing_record as freed
      existing_record->rec_code = get_free_code(existing_record->get_code());
      append_record(with_record,false);
   } else
   {  Binary_record_index_node_dynamic *existing_node = get_index_node_for(existing_record);
      if (existing_node)
      {  existing_node->record = with_record;
         if (free_existing_record)  delete existing_record;
         else                       result = existing_record;
   } }
   return result;
}
//_replace__________________________________________________________2000-07-10_/
void Binary_record_file_dynamic_indexed::dumpX
(std::ostream &txt_strm,bool full_record_dump)
{  FOR_EACH_IN(dmp_index_node,Binary_record_index_node_dynamic,index,each_node)  //030709
   {  if (full_record_dump)
         dmp_index_node->get(*this);
      // otherwise so far we have only read record headers
      dmp_index_node->dump(txt_strm);
   } FOR_EACH_END(each_node)
}
//_dumpX____________________________________________________________2000-10-0?_/
void Binary_record_file_dynamic_indexed::write_YAML
(std::ostream &YAML_strm,nat16 indent_level,bool full_record_dump)
{  FOR_EACH_IN(dmp_index_node,Binary_record_index_node_dynamic,index,each_node)  //030709
   {  if (full_record_dump)
         dmp_index_node->get(*this);
      // otherwise so far we have only read record headers
      //YAML_strm << "#____" << std::endl;
      char radix_10[10];
      nat32_to_cstr(dmp_index_node->position,radix_10,10);                       //001126

      // YAML_strm << "- { position: " << radix_10 << " }:" << std::endl;
      YAML_strm << radix_10 << ":" << " # position" << std::endl;
      dmp_index_node->write_YAML(YAML_strm,indent_level+1);
   } FOR_EACH_END(each_node)
}
//_write_YAML_______________________________________________________2018-01-08_/
Binary_record * Binary_record_file_dynamic_indexed::find_duplicate_record
(const Binary_record *record_to_find)   performs_IO_
{  FOR_EACH_IN(search_index_node,Binary_record_index_node_dynamic,index,each_node)     //030709
   {  if (record_to_find->Binary_record_header::compare(*(search_index_node->record)) == 0)  // we don't need to check in detail records that are different types
      {  // So far the records are of the same time
         if (!search_index_node->record->is_in_memory())
         {
#ifdef NYI
//091021 This isn't working yet, I need to provide some mechanism to compare the raw body data.
            Binary_record *search_record = search_index_node->get(*this);
            nat32 record_to_find_length = record_to_find->get_length();
            if (search_record /*length is already handled in header compare  && (record_to_find_length== search_record->get_length() )*/)
            {
               nat32 search_rec_length = search_record->get_length();
               const void *rec_to_find_body     = record_to_find->get_body_raw();
               const void *search_rec_body  = search_record->get_body_raw();
               if (memcmp(rec_to_find_body
                         ,search_rec_body
                         ,(size_t)record_to_find_length) == 0)
                  //WARNING the use of memcmp here is a hack.
                  // It assumes the binary record has the same structure
                  // In most record types this is the case or
                  // if the structure is not the same the record length will
                  // differ so we don't even get here.
                  // But the memcpy will compare contributes
                  // which in reality should be ignored.
                  // It would be better to implement a virtual function
                  // to compare the record body (the record header is already compared above)
                  return search_record;
            }
#endif
         }
      }
   } FOR_EACH_END(each_node)
   return 0; // not found
}
//_find_duplicate_record____________________________________________2009-10-21_/
}//_namespace_CORN_____________________________________________________________/
