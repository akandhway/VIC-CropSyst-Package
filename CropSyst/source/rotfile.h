//#error check obsolete 120813

#ifndef rotfileH
#define rotfileH

#include "cs_filenames.h"
#include "corn/datetime/date.hpp"
#include "corn/data_source/datarec.h"

//020520  Note this format is obsolete in version 4
//       This class is maintained to import version 3 to version 4
//______________________________________________________________________________
class Rotation_entry
: public Item
{
 public:
   CORN::Date     planting_date;
   CORN::OS::File_name_concrete crop_filename;                                   //000426
   CORN::OS::File_name_concrete management_filename;                             //000426
 public:
   Rotation_entry
   (const CORN::Date    &_planting_date
   ,const CRP_filename  &_crop_filename
   ,const MGT_filename  &_management_filename);
   Rotation_entry(const Ustring &file_line);
   virtual int compare(const Item &other)                                  const;
   /*190718 obs
   virtual const char *label_cstr_deprecated(char *buffer)                 const;
   */
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual bool write(std::ostream &strm)                         modification_;
   bool qualify();                                                               //000211
      /* Currently always return true;
         This makes sure that all file names are qualified.
         This is needed for generated simulations for
         Watershed and AcsCS projects where simulation is run
         in a subdirectory of the project directory.
      */
};
//_Rotation_entry______________________________________________________________/
class Rotation_file
: public Bidirectional_list
, public Data_record                                                             //010118
{public:
   Rotation_file();
   void save(const ROT_filename &i_filename);
   void load(const ROT_filename &i_filename);
   bool qualify(const CORN::OS::Directory_name &rot_dir_name);                   //000211
//             Currently always return true;
//             This makes sure that all file names are qualified.
//             This is needed for generated simulations for
//             Watershed and AcsCS projects where simulation is run
//             in a subdirectory of the project directory.
};
//_Rotation_file________________________________________________________________
#endif

