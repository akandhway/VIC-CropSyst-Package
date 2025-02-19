#pragma hdrstop
#include "chtexturegrp.h"
namespace USDA_NRCS
{
//______________________________________________________________________________
bool CHtexturegrp_record::expect_structure(bool for_write)
{
   bool expected = CORN::Data_record::expect_structure(for_write);
   expect_string("texture"       ,texture       ,30);
   expect_string("stratextsflag" ,stratextsflag ,3);
   expect_string("rvindicator"   ,rvindicator   ,3);
   expect_string("texdesc"       ,texdesc       ,255);
   expect_string("chkey"         ,chkey         ,30);
   expect_string("chtgkey"       ,chtgkey       ,30);
   structure_defined = true;                                                     //120314
   return expected;
}
//_2011-03-24___________________________________________________________________
}//_namespace USDA_NRCS________________________________________________________/
/*
"SIL"|"No "|"Yes"|"silt loam"|"675815:2701935"|"675815:4994643"
"VFSL"|"No "|"No "|"very fine sandy loam"|"675815:2701936"|"675815:4994644"
*/
