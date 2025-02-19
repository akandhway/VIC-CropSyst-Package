#include "evaporator.h"
#include "common/weather/hour/weather_hours.h"
#include "CS_suite/observation/CS_inspector.h"
namespace CS
{
//______________________________________________________________________________
Evaporator_intervals::Evaporator_intervals
(modifiable_ CORN::Dynamic_array<float64>  &evaporation_potential_remaining_)
: evaporation_potential_remaining(evaporation_potential_remaining_)
, evaporation_actual()
, evaporation_potential()
, output_evaporation_potential_daily(0)
, output_evaporation_actual_daily   (0)
{}
//_Evaporator_intervals:constructor_________________________________2016-07-18_/
bool Evaporator_intervals::start_day()                             modification_
{
   evaporation_actual.clear();
   evaporation_potential.clear();
   output_evaporation_potential_daily = 0;
   output_evaporation_actual_daily = 0;
   return true;
}
//_start_day________________________________________________________2016-07-18_/
bool Evaporator_intervals::end_day()                               modification_
{
   output_evaporation_potential_daily  = evaporation_potential.sum();            // m
   output_evaporation_actual_daily     = evaporation_actual.sum();               // m
   return true;
}
//_end_day__________________________________________________________2016-07-18_/
float64 Evaporator_intervals::evaporate_interval(nat8 interval)    modification_
{  // This is usually overridden by derived classes.
   // It should normally never be called.
   // The default simply evaporates all potential remaining for the interval.
   float64 evap_pot = evaporation_potential_remaining.get(interval);
   evaporation_potential.set(interval,evap_pot);
   evaporation_potential_remaining.set(interval,0.0);
   float64 evap_act = evap_pot;
   evaporation_actual.set(interval,evap_act);
   return evap_act;
}
//_evaporate_interval_______________________________________________2016-07-18_/
RENDER_INSPECTORS_DEFINITION(Evaporator_intervals)
{
   #ifdef CS_OBSERVATION
   inspectors.append(new CS::Inspector_scalar(output_evaporation_actual_daily         ,UC_m   ,*context,"evaporation/act"    ,value_statistic /*sum_statistic*/,GENERATE_VARIABLE_CODE));
   inspectors.append(new CS::Inspector_scalar(output_evaporation_potential_daily      ,UC_m   ,*context,"evaporation/pot"    ,value_statistic /*sum_statistic*/,GENERATE_VARIABLE_CODE));
   #endif
   return 0;
}
//_RENDER_INSPECTORS_DEFINITION(Evaporator_intervals)_______________2016-07-18_/
}//_namespace CS_______________________________________________________________/
