#ifndef watertabH
#define watertabH
#include "corn/math/numerical/interpolation/interpolator_temporal.h"
#include "CS_suite/simulation/CS_simulation_element.h"
using namespace CORN;
//______________________________________________________________________________
class Water_table_curve
: public extends_ CS::Simulation_element_abstract                                //150925
{private:                                                                        //071023
   float64     constant_value;                                                   //080213
      // When there is no water table file there may be a single constant value
   bool        valid_initial_value_or_water_table_file;                          //080213
   CORN::Interpolator_temporal *interpolator;                                    //180426_080213_150112RLN was Dated_interpolation_linear_curve
   contribute_ float64 value_today;                                              //150925
 public:
   Water_table_curve
      (const char * water_table_filename                                         //020620
      ,const char *selector
      ,const CORN::date32 &_today                                                //170525
      ,const Date_const &start_date
      ,const Date_const &end_date
      ,float64 initial_value_elemental
      ,float32 convert_to_molecular_concentration = 1.0);
   virtual bool is_valid()                                         affirmation_; //080213
   virtual float64 get(const CORN::Date_const &for_date)          modification_; //020306
 public: // These methods are used when using running date mode
   float64 get_for_today()                                        modification_; //071023
   virtual bool start_day()                                       modification_; //150925
};
//_Water_table_curve________________________________________________2000-05-04_/
#endif
//watertab.h

