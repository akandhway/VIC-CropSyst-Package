#ifndef CS_identificationH
#define CS_identificationH
#include "corn/primitive.h"
#include <string>
namespace CS
{
//______________________________________________________________________________
class Identification
{
 public:
      // I need to move this ID it should be property of simulation unit
    // In GIS CS modules the ID is the unique land unit fragment ID
    int32         number;
    // signed because ArcGIS uses negative IDs to indicate offsite area
    std::string   code;
      // String representation of ID
 public:
   Identification(int32 ID);
   Identification(const std::string &ID);
   bool set_code(const std::string &ID);
   bool set_code(const std::wstring &ID);                                        //190317
   bool set_number(int32 _ID);
};
//_2015-10-26_____________________________________________class_Identification_/
} // namespace CS
#endif
