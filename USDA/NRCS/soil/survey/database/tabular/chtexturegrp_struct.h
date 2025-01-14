#ifndef chtexturegrp_structH
#define chtexturegrp_structH
#include <string>
namespace USDA_NRCS
{
//_____________________________________________________________________________
struct CHtexturegrp_struct
{
   std::string          texture;       //Tex Mod & Class
   std::string /*bool*/ stratextsflag; //Stratified? (not null)
   std::string /*bool*/ rvindicator;   //RV? (not null)
   std::string          texdesc;       //Texture Description
   std::string          chkey;         //Chorizon Key (not null)
   std::string          chtgkey;       //Chorizon Texture Group Key (not null)
};
//_CHtexturegrp_struct______________________________________________2011-03-24_/
}//_namespace USDA_NRCS________________________________________________________/
#endif

