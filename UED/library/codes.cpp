#include "corn/primitive.h"
#include "UED/library/codes.h"
#ifndef validtypH
#  include "corn/validate/validtyp.h"
#endif
#ifndef qualityHPP
#   include "corn/quality.hpp"
#endif

using namespace CORN;

//______________________________________________________________________________
CORN::Quality_code validation_status_to_UED_quality_attribute(CORN::Validation_status validation_status)
{  CORN::Quality_code quality = CORN::measured_quality;
   switch (validation_status)
   {  case CORN::unavailable_status    : quality = CORN::measured_quality;         break; // with no other information, assume measured;
      case CORN::missing_status        : quality = CORN::missing_quality;          break;
      case CORN::error_low_status      : quality = CORN::out_of_range_low_error_quality;      break;
      case CORN::error_status          : quality = CORN::out_of_range_error_quality;          break;
      case CORN::error_high_status     : quality = CORN::out_of_range_high_error_quality;     break;
      case CORN::pot_error_low_status  : quality = CORN::out_of_range_pot_low_error_quality;  break;
      case CORN::pot_error_status      : quality = CORN::out_of_range_pot_error_quality;      break;
      case CORN::pot_error_high_status : quality = CORN::out_of_range_pot_high_error_quality; break;
      case CORN::warning_low_status    : quality = CORN::out_of_range_low_warning_quality;    break;
      case CORN::warning_status        : quality = CORN::out_of_range_warning_quality;        break;
      case CORN::warning_high_status   : quality = CORN::out_of_range_high_warning_quality;   break;
      case CORN::ready_status          : quality = CORN::measured_quality;                    break;
      case CORN::good_status           : quality = CORN::measured_quality;                    break;
      default : break;
//NYI      case disabled_status       : quality = UED_xxxxx;           break;
   }
   return quality;
}
//______________________________________________________________________________
CORN::Validation_status UED_quality_attribute_to_validation_status(CORN::Quality_code quality)
{  CORN::Validation_status status = good_status;
   switch (quality)                                                              //000830
   {  case  CORN::measured_quality                  : status = good_status         ;break;
          // estimated, generated and forcasted conditions are handled below     //010602
      case  CORN::not_valid_quality                 : status = error_status        ;break;
      case  CORN::out_of_range_warning_quality      : status = warning_status      ;break;//050414
      case  CORN::out_of_range_low_warning_quality  : status = warning_low_status  ;break;//050414
      case  CORN::out_of_range_high_warning_quality : status = warning_high_status ;break;//050414
      case  CORN::out_of_range_error_quality        : status = error_status        ;break;//050414
      case  CORN::out_of_range_low_error_quality    : status = error_low_status    ;break;//050414
      case  CORN::out_of_range_high_error_quality   : status = error_high_status   ;break;//050414
      #if (CS_VERSION==4)
      case  CORN::out_of_range_deprecated_quality   : status = pot_error_status    ;break; /*eventually obsolete */
      #endif
      case  CORN::missing_quality :  status = missing_status      ;break;
      case  CORN::unknown_quality :  status = unavailable_status  ;break;
      default : // I.e. estimated or generated
      {  if      (CORN::is_optimal_quality(quality)) status= good_status;        //050609
         else if (is_valid_quality(quality))         status = ready_status;
      } break;
   }
   return status;
}
//______________________________________________________________________________
