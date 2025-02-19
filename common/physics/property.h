#ifndef propertyH
#define propertyH
#include "corn/const.h"
#include "corn/primitive.h"
#include "corn/metrology/units_code.hpp"
#include "corn/metrology/units_clad.h"
#include "corn/quality.hpp"
#include <assert.h>
#include <limits>
#define INTENSIVE false
#define EXTENSIVE true
namespace Physical
{
//______________________________________________________________________________
interface_ Property_interface
{
   inline virtual ~Property_interface() {}                                       //151204
      // compilers complain about classes with virtual methods with virtual destructor.
   virtual CORN::Units_code get_units()                                 const=0;
   virtual float64 get_with_units(CORN::Units_clad &units)              const=0; //100930
      ///< \return the mass in the native units. the native units are return
   virtual float64 get_in_units(CORN::Units_code desired_units)         const=0; //150121
      ///< \return the mass converted to the desired units
      ///< The desired units must be a unit of mass otherwise the result is undefined
   virtual float64 in_prescribed_units()                                const=0;
   virtual float64 &ref_in_prescribed_units()                           const=0; //190812
   virtual bool update_value_with_units                                          //151124_150123
      (float64 _amount, CORN::Units_code _units)               rectification_=0;
   virtual float64 update_amount_in_preset_units
      (float64 new_amount)                                     rectification_=0; //150122
   virtual float64 multiply_by(float64 multiplier)              modification_=0;
   virtual bool clear()                                         modification_=0; //160126
   virtual bool is_empty()                                              const=0;
          virtual bool is_extensive()                            affirmation_=0;
   inline virtual bool is_intensive()   affirmation_ { return !is_extensive(); }
   virtual float64 force_amount(float64 new_amount)            rectification_=0; //151122
};
//_Property_interface_______________________________________________2011-08-18_/
struct Measure_datum
{
   mutable float64                 amount;
   mutable CORN::Units_code        units;
 public:
   Measure_datum();                                                              //150122
   Measure_datum
      (float64                 amount
      ,CORN::Units_code        units);
   Measure_datum(const Measure_datum &copy_from);
};
//_Measure_datum____________________________________________________2015-01-20_/
}//_namespace Physical_________________________________________________________/
#endif

