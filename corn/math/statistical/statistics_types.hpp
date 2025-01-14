#ifndef statistics_typesHPP
#define statistics_typesHPP
#include "corn/enum.h"
#include <string>
namespace CORN {
//______________________________________________________________________________
#include "corn/math/statistical/statistics_types.h"
//______________________________________________________________________________
DECLARE_ENUM_CLAD(Statistic_clad,Statistic,statistic_label_table);
//______________________________________________________________________________
extern nat16 statistic_units_matches_observation_units;
extern nat32 statistics_string_to_mask(const std::string &options_string);       //170318
extern nat32 statistics_mask_to_string                                           //170318
   (nat32 mask,bool code_2digit, std::string &options_string);
//______________________________________________________________________________
}//_namespace_CORN_____________________________________________________________/
#endif
