#ifndef registryH
#define registryH
#include <vcl/registry.hpp>
namespace CORN
{
//______________________________________________________________________________
class Registry
: public TRegIniFile
{
public: // constructor
   Registry(const char *HKEY_CURRENT_USER_subkey);
   // Generally this is the unique application name
public:
   void register_extension
   (const char *extension
   ,const char *description
   ,const char *program
   ,const char *icon
   );
};
}//_namespace_CORN_____________________________________________________________/
#endif

