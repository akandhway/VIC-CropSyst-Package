#include "common/geodesy/UTM.h"

#include "corn/measure/measures.h"
#include "corn/math/moremath.h"
#include "corn/string/strconv.h"

#include <math.h>
#include <stdlib.h>
#include "common/geodesy/UTM_conversion/LatLong-UTMconversion.h"

// This code was given to Roger Nelson by Javier Marcos, it was given to him by Ron Bolton
// I don't know where Ron got it
//______________________________________________________________________________
UTM_Coordinate::UTM_Coordinate()
: easting                  (0)
, northing                 (0)
, zone                     (0)
, zone_designator          ('Z')  // Z indicates out of range
, reference_ellipsoid_index(23)
{}
//_UTM_Coordinate:constructor__________________________________________________/
bool UTM_Coordinate::set_UTM(float64 _easting, float64 _northing, sint16 _zone,char _zone_designator)
{  // Negative zones are in the southern hemisphere
   easting = _easting;
   northing =  _northing;
   zone = _zone;
   zone_designator = _zone_designator;
   return true;
}
//_set_UTM_____________________________________________________________________/
bool UTM_Coordinate::set_degrees
(float32 latitude, float32 longitude)
{  char UTMZone[4];
   LLtoUTM(reference_ellipsoid_index,latitude,longitude,northing,easting,UTMZone);
   zone = atoi(UTMZone);
   zone_designator =UTMZone[3];
   if (zone_designator == 0) zone_designator =UTMZone[2];
   return true; // Currently assumes always successful
}
//_set_degrees_________________________________________________________________/
bool UTM_Coordinate::get_degrees
(float32 &latitude, float32 &longitude)
{  char UTMZone[4]; UTMZone[2] = 0; UTMZone[3] = 0;
   CORN::int16_to_cstr(zone,UTMZone,10);
   if (UTMZone[2] == 0) UTMZone[2] = zone_designator;
   else                 UTMZone[3] = zone_designator;
   UTMtoLL(reference_ellipsoid_index, northing, easting, UTMZone, latitude, longitude);
   return true;
}
//_get_degrees_________________________________________________________________/
#ifdef NYI
bool UTM_Coordinate::change_zone(int16 new_zone)
{  bool result = false;
   // Some times it is desirable to have all coordinates in the same zone
   // this will change the current coordinates to the specified zone.
   // Only adjacent zones can be reliably converted.
   // returns false if the zones are not adjacent
   bool same_zone = (new_zone == zone);
   if (same_zone)   // We are in the same zone, no need to translate
      result = true;
   else
   {  bool in_same_hemisphere = ((zone > 0) && (new_zone > 0)) || ((zone < 0) && (new_zone < 0));
      if (in_same_hemisphere)
      {  if (zone > 0) // Nothern hemisphere
         {  if (new_zone == (zone -1))        // Shouldn't translate non adjacent zones
            {  zone = new_zone;

I don't know exactly how to convert the zone

               easting += 1000000; // Warning, I think this is what is needed
               return true;
            } else result = false; // Shouldn't translate non adjacent zones
         } else // southern hemispher
         {
            // NYI Warning not sure exactly what to do with the southern hemisphere yet
            result = false;
         }
      } else result = false;  // can't translate different hemisphers
   }
   return result;
}
#endif
//_change_zone_________________________________________________________________/

