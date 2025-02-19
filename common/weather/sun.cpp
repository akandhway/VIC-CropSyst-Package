#  include "math.h"
#  include "corn/math/moremath.h"
#  include "corn/measure/measures.h"
#  include "weather/sun.h"
#  include "geodesy/geocoordinate.h"

// Lat and long are positive north and west.
// Azimuthal Angle of 0 is south.

//#define USE_EXACT_LOCAL_TIME
// Exact local time not yet implemented

#ifdef USE_EXACT_LOCAL_TIME
#  define EXACT_LT
#  define ANY_LT  _COMMENT_
#else
#  define EXACT_LT   _COMMENT_
#  define ANY_LT
#endif

//______________________________________________________________________________
float normalize_to_360(float value)
{  return (value - floor(value / 360.0) * 360.0);
}
//______________________________________________________________________________
Sun::Sun
(const  Temporal &now_                                                           //150125
,const  Geocoordinate &geocoordinate_)                                           //150629
: now(now_)
, day_length_hours_good(false)                                                   //011129
, day_length_hours_today(12.0)                                                   //150627
, geocoordinate(geocoordinate_)                                                  //150629
,eot_adj(0)                                                                      //081021
,daylight_adj(0)                                                                 //081021
,local_standard_time_meridian(0)                                                 //081021
{}
//_Sun:constructor_____________________________________________________________/
Sun::~Sun() {}
//_Sun:distructor___________________________________________________2013-05-15_/
void Sun::calc_angles
(DOY doy
,float solar_minutes_after_midnight
,float64 &_declination_rad
,float64 &_elevation_angle_rad
,float64 &_azimuth_angle_rad
,float64 &hour_angle_rad)
{
   float64 doy_f64 = (float64)doy;                                               //070604
   float64 sin_declination =
      0.39785    /*071115 was 0.3979*/                                           //071115
         * sin(4.869 + 0.0172 * doy_f64+
         0.03345 * sin(6.224  + 0.0172 * doy_f64));
   _declination_rad = asin(sin_declination);

//             The equation of time is a formula used to compensate for
//             the earth's elliptical orbit around the sun and
//             its axial tilt when calculating solar time.
//             Calculate local solar time more precisely with new EOT value.
   // hour angle
   float64 hour_angle_deg = ((solar_minutes_after_midnight - 12.0 * 60.0)
      / 4.0 * -1.0);
   hour_angle_rad = deg_to_rad(hour_angle_deg);
   // altitude angle
    float64 latitude_rad = geocoordinate.get_latitude_radians();                 //010227
    float64 sine_of_elevation_angle_rad =
      (cos (latitude_rad)  *
       cos (_declination_rad)  *
       cos (hour_angle_rad)) +
      (sin (latitude_rad)  *
       sin (_declination_rad));
   _elevation_angle_rad = asin(sine_of_elevation_angle_rad);
   // azimuth angle
   float64 sine_azimuth_angle_rad
      = cos(_declination_rad) * sin(hour_angle_rad) / cos(_elevation_angle_rad);
   _azimuth_angle_rad = (asin(sine_azimuth_angle_rad));
}
//_calc_angles___________________________________________________________1999?_/
bool Sun::update()
{  // Based: Christopher Gronbeck http://www.susdesign.com/sunangle/
   Minutes minutes_after_midnight = now.get_minutes();
   DOY    doy      = now.get_DOY();
   float64 hour_angle_rad;
   day_length_hours_today = get_day_length_hours(doy);                           //150627
   calc_angles
      (doy
      ,minutes_after_midnight
      ,declination_rad
      ,elevation_angle_rad
      ,azimuth_angle_rad
      ,hour_angle_rad);
   declination_deg         = rad_to_deg(declination_rad);
   elevation_angle_deg     = rad_to_deg(elevation_angle_rad);
   azimuth_angle_deg       = rad_to_deg(azimuth_angle_rad);
   float64 hour_angle_deg = rad_to_deg(hour_angle_rad);
   if (azimuth_angle_deg * hour_angle_deg < 0)
       azimuth_angle_deg = -azimuth_angle_deg;
   return provide_sun_rise_set_times();                                          //140709_130628
}
//_update______________________________________________________________________/
float64 Sun::get_day_length_hours(DOY doy)                                 const
{

   // Conceptual, should have an initialize() method
   if (!day_length_hours_good)   setup_day_length_hours();



   if (doy == 0)                 doy = now.get_DOY();
   float64 day_length = day_length_hours[doy];
   return day_length;
}
//_get_day_length_hours________________________________________________________/
bool Sun::setup_day_length_hours()                                 contribution_
{  day_length_hours[0] = 0;
   for (DOY doy = 1; doy <= 366; doy++)
      day_length_hours[doy] = calc_day_length_hours(doy);
   day_length_hours_good = true;
   return day_length_hours_good;                                                 //140709
}
//_setup_day_length_hours___________________________________________2001-11-29_/
float64 Sun::calc_day_length_hours(DOY doy)                                const
{  // 091027 unused static const float64 radians_to_hours = 7.64;
   float64 day_length = 0.0;                                                     //101021
   float64 sin_declination =
         0.3979 * sin(4.869 + 0.0172 * doy +
                      0.03345 * sin(6.224  + 0.0172 * doy));
   float64 one_minus_sqr_sin_decliation = 1.0-CORN_sqr(sin_declination);
   if (one_minus_sqr_sin_decliation > 0.0)                                       //101021
   {
      float64 latitude_rad = geocoordinate.get_latitude_radians();               //150629_010227
      float64 x = -sin(latitude_rad) * sin_declination /
        (cos(latitude_rad) * sqrt(one_minus_sqr_sin_decliation));
      float64 one_minus_x2 =  1-x * x;                                           //101021
      float64 half_day_length_rad =
         one_minus_x2 > 0.0                                                      //101021
         ? CORN_pi/2-atan(x/sqrt(one_minus_x2))
         : 0.0;
      day_length = rad_to_hours(half_day_length_rad);                            //970521
   };
   return day_length;
}
//_calc_day_length_hours__________________________________________________1997_/
float64 Sun::get_solar_noon()                                              const
// 'This function calculates the actual time of solar noon
{
   float64 DOY = now.get_DOY();
   float64 f = (279.575 + 0.9856 * DOY) * M_PI  / 180.0;
   float64 ET = (-104.7 * sin(f) + 596.2 * sin(2.0 * f) + 4.3 * sin(3.0 * f)
        - 12.7 * sin(4 * f) - 429.3 * cos(f) - 2.0
        * cos(2.0 * f) + 19.3 * cos(3.0 * f)) / 3600.0;
   float64 longitude = geocoordinate.get_longitude_dec_deg();                    //150629
   float64 standard_meridian = (int)(longitude); // I think
   float64 logitude_correction = (standard_meridian - longitude) / 15.0;
//'logitude_correction stands for longitude correction, +1/15 hour for each degree you are EAST of the
//'Standard Meridian, and -1/15 hour for each degree WEST of the Std. Mer.
//'(Campbell and Norman, 1997)

   float64 solar_noon = 12.0 - logitude_correction - ET ;
   return solar_noon;
}
//_get_solar_noon______________________________________________________________/
// For another method See also http://souptonuts.sourceforge.net/code/sunrise.c.html
//______________________________________________________________________________
bool Sun::provide_sun_rise_set_times()                                provision_
//130629 This should be renamed to update_sun_rise_set_times
{

//Warning This only accounts for locations between the artic/antartic circles. for the respective northern and southern summers!
// During artic winters it is 24 hours (usually Dec 22 to March20)
#ifdef OBSOLETE
//081021
    float64 longitude_deg = geocoordinate.get_longitude_dec_deg();                 //010227
    Minutes sunRiseSetLSoTMinutes = rad_to_deg(acos( -1.0 *
         sin (latitude_rad) *
         sin (declination_rad) /
         cos (latitude_rad) /
         cos (declination_rad))) * 4.0;
   /*Seconds*/ sunrise_seconds_after_midnight = 60.0 *
      (12.0 * 60.0 - sunRiseSetLSoTMinutes -
      (4.0 * (longitude_deg - local_standard_time_meridian))
      - eot_adj - daylight_adj);     // Currently we have no adjustments for ??? or daylight savings time
   sun_rise_time.set_S(sunrise_seconds_after_midnight);
   /*Seconds*/ sunset_seconds_after_midnight =
      86400 -     // sunset is simply the opposite time of the day
      60.0 *
      (12.0 * 60.0 + sunRiseSetLSoTMinutes -
      (4.0 * (longitude_deg - local_standard_time_meridian))
      - eot_adj - daylight_adj);       // Currently we have no adjustments for ??? or daylight savings time
#endif
   float64 latitude_rad = geocoordinate.get_latitude_radians();
   float64 coshs = (-sin(latitude_rad) * sin(declination_rad)) /
        (cos(latitude_rad) * cos(declination_rad));
   //Auxiliar trigonometric functions

   // warning check for sqrt 0.0  (may occur in artic circle?)

   float64 sinhs = sqrt(1.0 - coshs * coshs);
   float64 half_day_length = M_PI_2 - atan(coshs / sinhs);
   float64 noon = get_solar_noon();
   float64 hours_from_noon  =  ((half_day_length * 180.0 / M_PI) / 15.0);
   float64 sunrise_hours  = noon - hours_from_noon;
   // note this will be standard 24 hour time.
   CORN::Seconds sunrise_seconds_after_midnight = sunrise_hours * (60.0 * 60.0);
   float64 sunset_hours = noon + hours_from_noon;
   CORN::Seconds sunset_seconds_after_midnight = sunset_hours * (60.0 * 60.0);
   sun_rise_time.set_S(sunrise_seconds_after_midnight);
   sun_set_time.set_S(sunset_seconds_after_midnight);
   return true;
}
//_provide_sun_rise_set_times_______________________________________1999-10-08_/
Local_time &Sun::get_sun_rise_time()                                       const
{  if (sun_rise_set_date.get_date32() != now.get_date32())
      const_cast<Sun &>(*this).    // const cast this to modifiable for provides
      provide_sun_rise_set_times();                                              //090316
   return sun_rise_time;
}
//_get_sun_rise_time________________________________________________1999-10-07_/
Local_time &Sun::get_sun_set_time()                                        const
{  if (sun_rise_set_date.get_date32() != now.get_date32())
      const_cast<Sun &>(*this).    // const cast this to modifiable for provides
      provide_sun_rise_set_times();                                              //090316
   return sun_set_time;
}
//_get_sun_set_time_________________________________________________1999-10-07_/
float Sun::get_elevation_angle_rad()                                       const
{ return deg_to_rad(elevation_angle_deg); }
//_get_elevation_angle_rad_____________________________________________________/
/*
Sundials on the Internet - the Equation of Time
"Sun time" and "clock time"
Sundials tell "sun time". Clocks and watches tell "clock time". Neither kind of time is intrinsically "better" than the other - they are both useful and interesting for their separate purposes.
"Sun time" is anchored around the idea that when the sun reaches its highest point (when it crosses the meridian), it is noon and, next day, when the sun again crosses the meridian, it will be noon again. The time which has elapsed between successive noons is sometimes more and sometimes less than 24 hours of clock time. In the middle months of the year, the length of the day is quite close to 24 hours, but around 15 September the days are only some 23 hours, 59 minutes and 40 seconds long while around Christmas, the days are 24 hours and 20 seconds long.

"Clock time" is anchored around the idea that each day is exactly 24 hours long.
This is not actually true, but it is obviously much more convenient to have a "mean sun" which takes exactly 24 hours for each day,
since it means that mechanical clocks and watches, and, more recently,
electronic ones can be made to measure these exactly equal time intervals.

Obviously, these small differences in the lengths of "sun days" and "mean days" build up to produce larger differences
between "sun time" and "clock time".
These differences reach a peak of just over 14 minutes in mid-February (when "sun time" is slow relative to "clock time")
and just over 16 minutes at the beginning of November (when "sun time" is fast relative to "clock time").
There are also two minor peaks in mid-May (when "sun time" is nearly 4 minutes fast) and in late July
(when sun time is just over 6 minutes slow) (These minor peaks have the fortunate effect,
in the Northern hemisphere, that the differences are relatively minor during most of the months when there is a reasonable amount of sunshine).

The differences do not cumulate across the years, because "clock time" has been arranged so that,
over the course of a four year cycle including a leap year,
the two kinds of time very nearly come back to the same time they started.
(The "very nearly" is because "clock time" still has to be adjusted by not having a leap year at the turn of each century,
except when the year is exactly divisible by 400, so 1900 was not a leap year, but 2000 will be).
Even with this correction, we had an extra second added to "clock time" recently.

The reasons for these differences are discussed below,
followed by some information on what the differences are at given times of year.

Why the days are of different lengths
These differences arise from two quite separate causes.
The first is that the plane of the Equator is not the same as the plane of the Earth's orbit around the sun, but is offset from it by the angle of obliquity.

The second is that the orbit of the Earth around the sun is an ellipse and not a circle, and the apparent motion of the sun is thus not exactly equal throughout the year. The sun appears to be moving fastest when the Earth is closest to the sun.

These two effects are explained in more detail in a leaflet of the Royal Greenwich Observatory and in Art Carlson's excellent article on the subject at the end of this page.

Some people like such information presented in tables rather than in graphs, so two tables are presented for your information below. These are both handy summary tables, which will give you a different view of the Equation of Time, and may help you to remember some key features, for example, that between the end of March and mid-September the sun is never more than 6 minutes away from "clock time", and for the whole of February it is 13 or 14 minutes slow! If you want to know the Equation of Time for every day of the year, there is a table in Appendix A of the book by Waugh.

Table showing the dates when "Sun Time" is (nearly) exactly a given number of minutes fast or slow on "Clock Time"


16        Nov 11                                          Oct 27
15        Nov 17                                          Oct 20
14        Nov 22                                          Oct 15
13        Nov 25                                          Oct 11
12        Nov 28                                          Oct  7
11        Dec  1                                          Oct  4

10        Dec  4                                          Oct  1
 9        Dec  6                                          Sep 28
 8        Dec  9                                          Sep 25
 7        Dec 11                                          Sep 22
 6        Dec 13                                          Sep 19

 5        Dec 15                                          Sep 16
 4        Dec 17                                          Sep 13
 3        Dec 19          May  4          May 27          Sep 11
 2        Dec 21          Apr 25          Jun  4          Sep  8
 1        Dec 23          Apr 21          Jun  9          Sep  5
The Four Days Watches tell Sun Time - exactly right!
 0         Dec 25          Apr 15          Jun 14          Sep  2
Minutes Slow
1         Dec 28          Apr 12          Jun 19          Aug 29
2         Dec 30          Apr  8          Jun 23          Aug 26
3         Jan  1          Apr  5          Jun 29          Aug 22
4         Jan  3          Apr  1          Jul  4          Aug 18
5         Jan  5          Mar 29          Jul  9          Aug 12

6         Jan  7          Mar 26          Jul 18          Aug  4
7         Jan  9          Mar 22
8         Jan 12          Mar 19
9         Jan 15          Mar 16
10        Jan 18          Mar 12

11        Jan 21          Mar  8
12        Jan 24          Mar  4
13        Jan 29          Feb 27
14        Feb  5          Feb 19

Table showing the Equation of Time on the 5th, 15th and 25th of each month, together with the average daily change in seconds (given in minutes and second, + = "Sun time" is fast on "clock time"


Eq.of time on the:   5th         15th           25th     Av. change (secs)

January           -5m03         -9m10         -12m12          20
February         -14m01        -14m16         -13m18           5
March            -11m45         -9m13          -6m16          16
April             -2m57         +0m14          +1m56          18

May               +3m18         +3m44          +3m16           4
June              +1m46         -0m10          -2m20          16
July              -4m19         -5m46          -6m24          20
August            -5m59         -4m33          -2m14          11

September         +1m05         +4m32          +8m04          20
October          +11m20        +14m01         +15m47          13
November         +16m22        +15m28         +13m11          10
December          +9m38         +5m09          +0m13          27

The equation of time
Written by Art Carlson . October, 1995 .
The rotation of the Earth makes a good clock because it is, for all practical purposes, constant. Of course, scientists are not practical and care about the fact that the length of the day increases by one second every 40 000yrs. For the rest of us, it's just a matter of finding a convenient way to determine which way the Earth is pointing. Stars would be good, but they are too dim (and too many) at night and go away during the day. A useful aid is the Sun, which is out and about when we are and hard to overlook. Unfortunately, the apparent position of the sun is determined not just by the rotation of the Earth about its axis, but also by the revolution of the Earth around the Sun. I would like to explain exactly how this complication works, and what you can do about it.

The diameter of the Sun as seen from the Earth is 1/2 degree, so it moves by its own radius every minute.

    24hrs   60min   1
   ------ x ----- x -deg = 1min
   360deg     1hr   4

That means it will be hard to read a sundial to better than the nearest minute, but then, we don't bother to set our clocks much more accurately than that either. Unfortunately, if we define the second to be constant (say, the fraction 1/31 556 925.974 7 of the year 1900, the "ephemeris second"), then we find that some days (from high noon to high noon) have more than 86,400 seconds, and some have less. The solar Christmas day, for example, is 86,430 seconds long. The discrepancy between "apparent time" and "mean time" can add up to +/- 15min. How does it come about?

The inclination of the ecliptic
First note that the Earth rotates on its axis not once in 24hrs but once in 23hrs 56min 4sec. It's just that in the course of a 365dy year, the Earth must turn an extra time to make up for its orbit about the sun.

   1day  24hrs   60min
   --- x ----- x ----- = 3min 56sec
   366    1dy     1hr

The trouble comes in because this 3min 56sec is only an average value. Think of an observer sitting at the north pole on a platform which rotates once every 23hrs 56min 4sec. She will see the stars as stationary and the sun as moving in a circle. The plane of this circle is called the "ecliptic" and is tilted by 23.45deg relative to the equatorial plane. The observer will see the sun move from the horizon, up to 23.45deg, then back down to the horizon. The sun will move at a constant speed (I'm lying, but wait till later) along its circle, but the shadow cast by the North Pole (the one with the red and white candy stripes) will not move at a constant rate. When the sun is near the horizon, it must climb at a 23.45deg angle, so that it has to move 1.09deg before the shadow moves 1deg.

       1deg
   ------------- = 1.0900deg
   cos(23.45deg)

On the other hand, in the middle of summer, the sun is high in the sky taking a short cut, so it must move only 1deg along its circle to cause the shadow to move 1.09deg. This effect generalizes to more temperate climates, so that in spring and fall the 3min 56sec is reduced by the factor 1.09 to 3min 37sec, whereas in summer and winter it is correspondingly increased to 4min 17sec. Thus a sundial can gain or lose up to 20sec/dy due to the inclination of the ecliptic, depending on the time of year. If it is accurate on one day, six weeks later it will have accumulated the maximum error of 10min.


   20sec            2    1min
   ----- x 45dys x -- x ----- = 10min
    1dy            pi   60sec

The seasonal correction is known as the "equation of time" and must obviously be taken into account if we want our sundial to be exact to the minute.
If the gnomon (the shadow casting object) is not an edge but a point (e.g., a hole in a plate), the shadow (or spot of light) will trace out a curve during the course of a day. If the shadow is cast on a plane surface, this curve will (usually) be a hyperbola, since the circle of the sun's motion together with the gnomon point define a cone, and a plane intersects a cone in a conic section (hyperbola, parabola, ellipse, or circle). At the spring and fall equinox, the cone degenerates to a plane and the hyperbola to a line. With a different hyperbola for each day, hour marks can be put on each hyperbola which include any necessary corrections.


Unfortunately, each hyperbola corresponds to two different days, one in the first half and one in the second half of the year, and these two days will require different corrections. A convenient compromise is to draw the line for the "mean time" and add a curve showing the exact position of the shadow points at noon during the course of the year. This curve will take the form of a figure eight and is known as an "analemma". By comparing the analemma to the mean noon line, the amount of correction to be applied generally on that day can be determined. At the equinox, we found that the solar day is closer to the sidereal day than average, that is, it is shorter, so the sundial is running fast. That means in fall and spring the correct time will be earlier than the shadow indicates, by an amount given by the curve. In summer and winter the correct time will be later than indicated.


The eccentricity of the Earth's orbit
If you look at such a figure eight calculated correctly, you will see that the fall and winter loop is actually somewhat larger than the spring and summer loop. This is due to the lie I told above. The Earth does not actually orbit at a constant speed around the sun. On January 2, the Earth is 1.7% closer to the Sun than average and thus the angular velocity is 3.4% larger (conservation of angular momentum). This make the solar day longer than the sidereal day by about 8sec more than average,

   3min 56sec
   ---------- x 0.034 = 8.0sec/dy
      1dy

and in the course of 3 months a sundial accumulates an error of 8min due to the eccentricity of the Earth's orbit.


   8.0sec             2    1min
   ------ x 91dys x -- x ----- = 8min
     1dy             pi   60sec

Thus the correct time will be later than the shadow indicates at the spring equinox and earlier at the fall equinox. This shifts the dates at which the sundial is exactly right from the equinoxes into the summer, making the summer loop of the figure eight smaller.
The 20sec/dy error due to the inclination of the ecliptic and the 8sec/dy error due to the eccentricity work in the same direction around Christmas time and add up exactly (well, almost) to the 30sec/dy mentioned earlier. The accumulated errors of 10min and 8min due to these two effects don't add up quite so neatly, so the maximum accumulated error turns out to be somewhat less than 18min. If you calculate everything correctly, you find that during the course of a year a sundial will be up to 16min 23sec fast (on November 3) and up to 14min 20sec slow (on February 12).

Suppose in October you start a 15min coffee break at 10:45 by the wall clock. If you believe the sundial outside, without accounting for the equation of time. you will already be late for the 11:00 session as soon as you step out the door.

Other pages on the Internet which are concerned with the Equation of Time are from the Royal Greenwich Observatory
http://www.ast.cam.ac.uk/pubinfo/leaflets/equation/equation.html,
and from a number of individual pages, including
http://ourworld.compuserve.com/homepages/patrick_powers/sundials.htm , from
http://cpcug.org/user/jaubert/sundial.html (this link now changed or superseded)
http://www.ipp.mpg.de/~awc/sundial.html

We would appreciate an E-mail from you if you know of any others. Thank you
*/

