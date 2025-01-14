#ifndef timeseriesH
#define timeseriesH

//180508 I am in the process of replacing Time_series with more compresensive
// mechanisms for specifying time series  see corn/data/time_series

// This is used in:
//    - Simulation models to create hydrographs and hyetographs
//    - UED for time series data records

// Eventually this will be derived from (abstract class) Data_set
// I Considered making this derived from Statistical_data_set,
// but I think if one needs statistical function,
// I would rather have a Statistical_data_set wrapper class
// that you could plug onto any Dynamic_float_array.

   // I have this now,  Statisical_sample_cowl_dataset  180408



#include <corn/dynamic_array/da_float32.h>
#include <corn/datetime/datetime64.h>
#include <corn/measure/units.h>
namespace CORN
{
//___________________________________________________________________________
class Time_series
: public Dynamic_float_array
{protected:  // friend Time_series
   Date_time_64   start_date_time;  // 0 indicates midnight of any abitrary day
   Units_code     time_step_units;
   // Could possibly have units of the data set
 public: // Constructors
   Time_series(Units_code     time_step_units= 0);
   Time_series(const Dynamic_float_array &dataset,Units_code     time_step_units= 0);
   Time_series(const Time_series &copy_from); // copy constructor
 public:
   Time_series * normalize_amounts_to_timesteps
      (uint32 timesteps)                       const;
   // This creates a new target dataset time series, expanding or compressing
   // this time series values into 'bins' matching the specified timestep
   // This can only be used with mass, volume, linear units.
   // This is used for hydrographs
   // Returns the created timeseries if successful
   // Returns 0 if the target dataset could not be discretely subdivided or combined.
   // The returned timeseries is relinquished to the caller.

   // Could have have normalize_amounts_to_timestep
   // where the timestep units change and the number of items is
   // adjusted to the time step.
   // This would then call normalize_amounts_to_timesteps
};
//_Time_series_________________________________________________________________/
}//_namespace_CORN_____________________________________________________________/
#endif

