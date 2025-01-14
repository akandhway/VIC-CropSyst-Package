#include "OS/FS/path_discovery.h"
#include "corn/data_source/datarec.h"
#include "corn/application/arguments.h"

#include "corn/OS/file_system_engine.h"
#include "corn/container/SDF_list.h"

namespace CORN {
namespace OS {
namespace FS {
//______________________________________________________________________________
//______________________________________________________________________________
Path_discovery_cowl::Path_discovery_cowl
(Path_discovery_struct &options_ref_
,const char *prefix_
)
: Path_discovery                                                              ()
, prefix                                                               (prefix_)
, options_ref                                                     (options_ref_)
, seclusion                              (options_ref.include_pattern,true,true)
, recursion_raw                                                               ()
, depth_first_raw                                                        (false)
, discovered                                                                  ()
, primary                                                                    (0)
, primary_owned                                                          (false)
{}
//_Path_discovery_common:constructor________________________________2020-08-01_/
bool Path_discovery_cowl::take(Item_string *arg)                     submission_
{  bool taken = false;

   // Currently recognizing only one filename
   // This is provided for compatibility with earlier CS utilities
   // Ideally would specify multiple files with files with SDF option
   if (seclusion.is_included_string(*arg))
   {  filename_raw = (*arg);
      delete arg;
      taken = true;
   }
   return taken;
}
//_take_____________________________________________________________2008-08-08_/
bool Path_discovery_cowl::setup_structure
(Data_record &data_rec,bool for_write)                            modification_
{  bool expected = true;

   // The following is where expecting basic arguments
   // (I.e. --target_filename  --source_filenames)
   // Note that the VV section must previously been set to the default
   std::string filenames_SDF_key(prefix);
      filenames_SDF_key+="_filenames";
   expected &= (data_rec.expect_string
      (filenames_SDF_key.c_str(),filenames_SDF_raw,2048) != NULL);

   std::string filename_key(prefix);
      filename_key+="_filename";
   expected &= (data_rec.expect_string
      (filename_key.c_str(),filename_raw,2048) != NULL);

   // This is mode where we use [prefix]
   expected &= data_rec.set_current_section(prefix.c_str())
   &&    data_rec.expect_switch(depth_first_raw     ,"--depth","-d","Depth first recursion")
   &&    data_rec.expect_string("--recursion"   , recursion_raw)
   &&    data_rec.expect_nat8  ("--maxdepth"    , options_ref.recursion.depth.max,10,"limit of subdirecties to recurse into","0 = only apply processing only to the start directory.")
   &&    data_rec.expect_nat8  ("--mindepth"    , options_ref.recursion.depth.min,10,"include subdirecties after specified depth","1 = do not include the start directory.")
   ;
   return expected;
}
//_setup_structure__________________________________________________2020-08-01_/
bool Path_discovery_cowl::get_end()
{  bool ended = true;
   {  // This is additive                                                        //191101
      if      (recursion_raw.find("descendent")    != std::string::npos) options_ref.recursion.direction |= descendent;
      else if (recursion_raw.find("ascendent")     != std::string::npos) options_ref.recursion.direction |= ascendent;
      // deprecated:
      else if (recursion_raw.find("subdirectory")  != std::string::npos) options_ref.recursion.direction |= descendent;
      else if (recursion_raw.find("superdirectory")!= std::string::npos) options_ref.recursion.direction |= ascendent;
      //obsolete used depth max/min else if (recursion_raw.find("exclusive")      != std::string::npos) recursion |= FILE_SYSTEM_RECURSION_EXCLUSIVE;
   }
   if (depth_first_raw) options_ref.recursion.direction |= depth_first;

   //abandoned if (must_exist)
   if (options_ref.require.existing > 0)
      ended &= (discover_paths() > 0);
   else
   {  if (!filename_raw.empty())
         primary = new CORN::OS::File_name_concrete(filename_raw);
   }
   return ended;
}
//__________________________________________________________________2020-08-01_/
nat32 Path_discovery_cowl::discover_paths()                        provision_
{
   CORN::OS::Directory_name *start_dir_default = 0;
   CORN::OS::Directory_name *start_dir = 0;
   if (CORN::global_arguments)
      start_dir = &CORN::global_arguments->start_directory;
   else // The program may not set global arguments.
   {  start_dir =
      start_dir_default = new CORN::OS::Directory_name_concrete; // CWD
   }
   if (!filename_raw.empty())
   {
      CORN::OS::Directory_entry_name_concrete *filename
            = new CORN::OS::Directory_entry_name_concrete(filename_raw);
      discovered.append(filename);
   }
   if (!filenames_SDF_raw.empty())
   {
      SDF_List filenames_SDF(filenames_SDF_raw.c_str(),false);
      FOR_EACH_IN(filename_raw,Item_string,filenames_SDF,filename_iter)
      {
         CORN::OS::Directory_entry_name_concrete *filename
            = new CORN::OS::Directory_entry_name_concrete(*filename_raw);
         discovered.append(filename);
      } FOR_EACH_END(filename_iter)
   }

   // discovered filename may be required to exist
   // if the file doesn't exist remove it
   if (options_ref.require.existing)
   {
      FOR_EACH_IN(filename,CORN::OS::File_name,discovered,filename_iter)
      {
         if (!file_system_engine.exists(*filename))
         {
            filename_iter->delete_current();
         }
         /*
         else if (! filename->is_qualified())
            filename.qualify();
         */
      } FOR_EACH_END(filename_iter)
   }

   /* Warning the following is applicable only to the old
      file_system_engine.list_DENs method:
      - this implementation only searches for file and directories
      - the recursion used the the concept of exclusive
        the OS::FS recursion uses a more general depth specification.
   */

   nat8 recursion_OLD = options_ref.recursion.direction;
   if (options_ref.recursion.depth.max == 0)
      recursion_OLD != exclusive_deprecated;
   if (options_ref.recursion.depth.min == 1)
      recursion_OLD != exclusive_deprecated;
   //200804 warning, I haven't check the depth logic/semantics

   nat32 found = OS::file_system_engine.list_DENs
         (*start_dir
         ,(options_ref.DE_type_mask & file_bit     ) ? &discovered : 0
         ,(options_ref.DE_type_mask & directory_bit) ? &discovered : 0
         ,seclusion
         ,recursion_OLD);
   if (!primary && discovered.count())
   {
      primary = dynamic_cast<Directory_entry_name *>(discovered.get_at(0));
   }
   return found;
}
//__________________________________________________________________2020-08-04_/
/* reimplemented
nat32 Path_discovery_cowl::discover_paths()                        provision_
{
   if (!preferred_DE_type_mask)
      preferred_DE_type_mask = options_ref.DE_type_mask;
   nat8 DE_bit = 1;
   for (nat8 DE_type_ndx = FIFO_special_type // 1
       ; DE_type_ndx < directory_entry_type_count
       ; DE_type_ndx++)
   {
      if (preferred_DE_type_mask & DE_bit)
      {
         if (!discovered[DE_type_ndx])
              discovered[DE_type_ndx] = new CORN::Unidirectional_list;
         DE_bit << 1;
      }
   }

   CORN::OS::Directory_name *start_dir_default = 0;
   CORN::OS::Directory_name *start_dir = 0;
   if (CORN::global_arguments)
      start_dir = &CORN::global_arguments->start_directory;
   else // The program may not set global arguments.
   {  start_dir =
      start_dir_default = new CORN::OS::Directory_name_concrete; // CWD
   }


   nat8 recursion_OLD = options_ref.recursion.direction;
   if (options_ref.recursion.depth.max == 0)
      recursion_OLD != exclusive_deprecated;
   if (options_ref.recursion.depth.min == 1)
      recursion_OLD != exclusive_deprecated;
   //200804 warning, I haven't check the depth logic/semantics

   nat32 found = OS::file_system_engine.list_DENs
         (*start_dir
         ,discovered[file_type]
         ,discovered[directory_type]
         ,seclusion
         ,recursion_OLD
         );
   return found;

CORN::Item_wstring *Path_discovery_cowl::provide_primary
(Directory_entry_type DE_type)                                     provision_
{

   Directory_entry_bitmask of_types_mask = 1 << (DE_type-1);
   nat32 discover_count = discover_paths(of_types_mask);
   CORN::Item_wstring *primary
   = (discover_count)
      ? discovered[DE_type]
         ? (dynamic_cast<CORN::Item_wstring *>(discovered[DE_type]->get_at(0)))
         : 0
      : 0;
   return primary;
}
//_provide_primary_____________2020-08-04_/
}
//__________________________________________________________________2020-08-04_/
*/
File_name *Path_discovery_cowl::provide_primary()                     provision_
{
   if (!primary)
   {
      if (discovered.count())
         primary = dynamic_cast<File_name *>(discovered.get_at(0));
         // return the reference to the first one discovered.
      // There may be no primary in cases where the source/target is stdin stdout
   }
   return primary;
}
//_provide_primary__________________________________________________2020-08-04_/



      // This will be the first discovered


/*conceptual
Path_discovery_clad::Path_discovery_clad
: Path_discovery_cowl(options)
, ....
{

   options.recursion.direction = 0;
   options.recursion.depth.min = 0;
   options.recursion.depth.max = 0;

}
*/


}}}//_namespace_CORN::OS::FS___________________________________________________/

