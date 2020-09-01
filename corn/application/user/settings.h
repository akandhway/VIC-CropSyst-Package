#ifndef settingsH
#define settingsH
#include "corn/primitive.h"
#if ((__BCPLUSPLUS__ > 0) && (__BCPLUSPLUS__ <= 0x0550))
#include "corn/string/ustring.h"
#else
#include <string>
#endif
namespace CORN
{  class Data_record;
//---------------------------------------------------------------------------
// These are common user GUI preferences and settings
// that are typically saved in the user's personal application data directory settings file.
// See (user/application_directory.h)
//______________________________________________________________________________
class User_settings
{
public: // Position of single document model edit window
   // This is optional, the user can specify a URL that may be displayed on the details page of the parameter file form
   // if a local file is not available.
   // The following can be used by parameter editors to remember
   // where the position on the screen the editor was last located.
   int16    GUI_top;                // These MUST be signed ints
   int16    GUI_left;               // Because in the case of multi monitor laptops
   int16    GUI_width;              // Display elements to the left and above
   int16    GUI_height;             // the main monitor have negative position
   int16    GUI_activepageindex;
   std::string GUI_active_control;   // 080125 eventually this will replace activepageindex (once I get it working in all cases)
   bool     auto_explore_created_context; // This is is used with Explorer context. 101126
public: // options section
   bool     advanced_mode;    // May be used by derived parameter class editor to show/hide advanced user mode parameters 060808
   #if ( (__BCPLUSPLUS__ > 0)&& (__BCPLUSPLUS__ <= 0x0550))
   std::string most_recent_context_directory;    // This is used in Explorer programs to restore the editing session context directory. 090811_
   std::string language;                                                       //100518
   #else
   std::string most_recent_context_directory;    // This is used in Explorer programs to restore the editing session context directory. 090811_
   std::string language;                                                         //100518
   #endif
public: //
   User_settings();
   virtual bool setup_structure(CORN::Data_record &data_rec,bool for_write = false);
};
//_2008-05-13___________________________________________________________________
};
#endif

