#ifndef vv_datasrcH
#define vv_datasrcH
#ifndef datasrcH
#  include "corn/data_source/datasrc.h"
#endif
#include <string>
/*
  Vewsion
  1.0            Initial release.
                 The class provides polymorphic lists of entries.
                 Unlike INI entry, the VV value entry now references directly
                 the data value to be input/output so we nolonger need to
                 keep the string representation of the data.
  1.0.1          Added enumeration entries
  1.0.2  970926  Added enumerated sections
  1.0.3  980405  Moved VV_xxx_entry classes to VV_entry.h
                 (these can be used by INI_file and htmlform).
                 Switch to int16,... float64 types.
                 Documentation.
  1.0.4  980629  Added keyless entries, where there is no = in the line (the whole line is preserved)
  1.0.5  981211  Added array sections (float and integer)
*/
namespace CORN
{
class VV_Section;
//______________________________________________________________________________
class VV_Data_source
: public Data_source_abstract                                                    //161013
{public:
   enum Submission_status
      { unrecognized
      , section_unknown
      , entry_unknown
      , section_known
      , entry_known
      , comment_recognized
      };
 public:
   VV_Data_source();
   virtual Submission_status submit(modifiable_ Data_record &data_rec,const std::string &text,bool unexpected_only)  submission_;
 protected:
   contribute_  VV_Section* current_section;
   bool inheritance;                                                             //160128
   virtual bool inherit                                                          //160128
      (Data_record &data_rec
      ,const std::string &key, const std::string &URL) = 0;
};
//_VV_Data_source___________________________________________________2013-04-01_/
}//_namespace CORN_____________________________________________________________/
#endif
