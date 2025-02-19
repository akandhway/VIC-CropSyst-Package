#ifndef stormsH
#define stormsH
#include "corn/math/statistical/sample_dataset_T.h"
#include "weather/stormgen.h"
#if (CORN_FS_VERSION==2020)
#include "OS/FS/path.h"
#else
#include "corn/OS/directory_entry_name.h"
#endif
#include "UED/library/timeqry.h"
#include "corn/OS/FS/recursion.h" //LML 200819

//______________________________________________________________________________
namespace CORN
{  class Time;
}
namespace UED
{ class Database_file_indexed;
}
using namespace CORN;
using namespace CORN::OS;
using namespace CORN::OS::FS;
namespace CS
{
namespace meteorology
{
//______________________________________________________________________________
interface_ Storms  // rename this to simply storm
{
   inline virtual ~Storms() {}                                                   //151015
   virtual float64 get_eff_precipitation_mm_by_interval
      (CORN::Dynamic_array<float32> &precip_X_minute
      ,float64             &eff_precip_mm
      ,CORN::Time          &event_start_time   // time the event starts is generated.
      ,nat16               &intervals)                                  const=0;
   virtual float64 get_intensity_interval_max()                         const=0; //140217_0000615
   virtual bool    has_event_today()                             affirmation_=0; //160506
   virtual bool    start_day()                                  modification_=0; //160506
};
//_Storms___________________________________________________________2014-02-17_/
class Storms_common
: public implements_ Storms
, public extends_ Storm_generator
{public:
   Storms_common
      (const CORN::date32  &today
      ,const Precipitation &precipitation                                        //151015
      ,const float32        mean_peak_rainfall[]);
   inline virtual ~Storms_common()                                            {} //151015
 public: // simulation element
   inline virtual bool    start_day()                              modification_ //160506
      { return Storm_generator::start_day(); }
 public:
   inline virtual bool has_event_today()                            affirmation_ //160506
      { return Storm_generator::has_event_today(); }
 public:
   virtual float64 get_eff_precipitation_mm_by_interval
      (CORN::Dynamic_array<float32> &precip_X_minute
      ,float64             &eff_precip_mm
      ,CORN::Time          &event_start_time   // time the event starts is generated.
      ,nat16               &intervals)                                    const;
   inline virtual float64 get_intensity_interval_max()                    const
      { return Storm_generator::get_peak_precip_intensity_mm(); }
};
//_Storms_common____________________________________________________2014-02-17_/
class Storms_database
: public extends_ Storms_common
{protected:
   UED::Database_file_indexed* storm_database;
 public:
   Storms_database
      (const CORN::date32        &today
      ,const Precipitation       &precipitation_                                 //151015
      ,const float32              mean_peak_rainfall[]
      ,const File_name           &storm_database_filename);
   inline virtual ~Storms_database() { delete storm_database; }
   virtual float64 get_eff_precipitation_mm_by_interval
      (CORN::Dynamic_array<float32> &precip_X_minute
      ,float64             &eff_precip_mm
      ,CORN::Time          &event_start_time   // time the event starts is generated.
      ,nat16               &intervals)                                    const;
};
//_Storms_database__________________________________________________2014-02-17_/
class Storms_generation
: public extends_ Storms_common
{protected:
   CORN::Minutes       interval_minutes;
 public:
   inline Storms_generation
      (const CORN::date32  &today_
      ,const Precipitation &precipitation_                                       //151015
      ,const float32        mean_peak_rainfall_[]
      ,CORN::Minutes        interval_minutes_)
      : Storms_common   (today_,precipitation_,mean_peak_rainfall_)              //151015
      , interval_minutes(interval_minutes_)
      {}
   inline virtual ~Storms_generation() {}                                        //151015
};
//_Storms_generation________________________________________________2014-02-17_/
class Storms_hyeotographic
: public extends_ Storms_generation
{protected: // parameters:
    bool          generate_intensity;
 public:
   Storms_hyeotographic
      (const CORN::date32  &today
      ,const Precipitation &precipitation_                                       //151015
      ,const float32    mean_peak_rainfall[]
      ,CORN::Minutes    interval_minutes
      ,bool             generate_intensity);
 public:
   inline virtual ~Storms_hyeotographic() {}                                     //151015
   virtual float64 get_eff_precipitation_mm_by_interval
      (CORN::Dynamic_array<float32> &precip_X_minute                             //140264
      ,float64             &precip_mm
      ,CORN::Time          &event_start_time   // time the event starts is generated.
      ,nat16               &intervals)                                    const;
};
//_Storms_hyeotographic_____________________________________________2014-02-17_/
class Storms_uniform
: public extends_ Storms_generation
{protected: // parameters
   bool          generate_intensity;
   CORN::Minutes interval_minutes;
   CORN::Minutes default_event_duration_minutes;
 public:
   inline Storms_uniform
      (const CORN::date32  &today_
      ,const Precipitation &precipitation_                                       //151015
      ,const float32     mean_peak_rainfall_[]
      ,bool              generate_intensity_
      ,CORN::Minutes     interval_minutes_
      ,CORN::Minutes     default_event_duration_minutes_)
      :Storms_generation
         (today_
         ,precipitation_                                                         //151015
         ,mean_peak_rainfall_
         ,interval_minutes_)
      ,generate_intensity(generate_intensity_)
      ,interval_minutes(interval_minutes_)
      ,default_event_duration_minutes(default_event_duration_minutes_)
      {}
   inline virtual ~Storms_uniform() {}                                           //151015
   virtual float64 get_eff_precipitation_mm_by_interval
      (CORN::Dynamic_array<float32> &precip_X_minute                             //140264
      ,float64             &eff_precip_mm
      ,CORN::Time          &event_start_time   // time the event starts is generated.
      ,nat16               &intervals)                                    const;
};
//_Storms_uniform______________________________________________________________/
class Storm_event_search : public UED::Time_search
{public:
   Storm_event_search
      (const CORN::date32 &target_date);                                         //170524
};
//_Storm_event_search_______________________________________________1999-04-28_/
class Storm_energy
{private:
   const CORN::Dynamic_array<float32> &precip_mm_in_intervals;
   CORN::Minutes                       interval_duration;
   contribute_ CORN::statistical::Sample_dataset_clad<float32,float32,float32> energy;
      // MJ/ha (These are units of original model)
 public:
   Storm_energy
      (const CORN::Dynamic_array<float32> &precip_mm_in_intervals
      ,CORN::Minutes                       interval_duration);
   float64 get_intensity_mm_per_hour_at(nat16 interval)                   const;
   float64 get_energy_MJ_per_ha_at(nat16 interval)                        const;
   float64 get_energy_MJ_per_ha()                                         const;
 private:
   bool provide_energy()                                             provision_;
};
//_Storm_energy_____________________________________________________2016-05-11_/
}}//_namespace CS Weather______________________________________________________/
#endif

