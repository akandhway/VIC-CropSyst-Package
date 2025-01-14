#ifndef seasonsH
#define seasonsH
#include "corn/chronometry/date_I.h"
#include "corn/chronometry/date_types.hpp"
#include "common/weather/weather_types.h"
namespace CS
{
//______________________________________________________________________________
   extern bool is_fall_or_winter_time  (CORN::DOY doy,bool southern_hemisphere);
   extern bool is_spring_or_summer_time(CORN::DOY doy,bool southern_hemisphere);
   extern nat8 get_season_S_S_F_W      (CORN::DOY doy,bool southern_hemisphere);
   // Returns
   // 0 = unknown
   // 1 = spring
   // 2 = summer
   // 3 = fall
   // 4 = winter
//______________________________________________________________________________
class Seasons
{
   const CORN::Date_const &today;
   bool southern_hemisphere;
 public:
   inline Seasons
      (const CORN::Date_const &today_
      ,bool southern_hemisphere_)
      : today(today_)
      , southern_hemisphere(southern_hemisphere_)
      {}
   inline virtual ~Seasons() {}                                                  //151204
           bool is_spring_or_summer_time(CORN::DOY doy = 0)        affirmation_; //980302
           bool is_summer_time(CORN::DOY doy = 0)                  affirmation_; //151028
           bool is_winter_time(CORN::DOY doy = 0)                  affirmation_; //151028
   inline  bool is_fall_or_winter_time(CORN::DOY doy=0)            affirmation_  //980302
      { return !is_spring_or_summer_time(doy);}
   virtual nat8 get_season_S_S_F_W(CORN::DOY doy=0)                        const;//040914
};
//_Seasons_____________________________________________________________________/
}//_namespace CS_______________________________________________________________/
#endif
