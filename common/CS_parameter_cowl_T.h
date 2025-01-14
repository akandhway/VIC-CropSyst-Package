#ifndef PARAMETER_COWL_H
#define PARAMETER_COWL_H
#include "physics/property_cowl_T.h"
#include "common/CS_parameter.h"
#include "corn/const.h"
#include "corn/quality.hpp"
namespace CS
{
//______________________________________________________________________________
template <typename Real>
class Parameter_cowl_T                       // abstract
: public extends_ Physical::Property_cowl<Real>
// May want to derive from Item so it can be listed
, public CORN::Quality_cowl                                                      //150121
, public implements_ CS::Parameter                                               //151122
{
   // This is the base class for all weather parameters
protected:  // contributes
   mutable bool                    updating; // This is to prevent recursively updating when we are already updating
public: //temporarily made public should use an accessor
   mutable bool                    modified;
public:
   //___________________________________________________________________________
   inline Parameter_cowl_T
      (bool                 extensive_
      ,Real                &amount_
      ,CORN::Units_code    &units_
      ,CORN::Quality_code  &quality_code_)                                       //150121
   : Physical::Property_cowl<Real>(extensive_,amount_,units_)
   , CORN::Quality_cowl(quality_code_)                                           //151129
   , updating(false)
   , modified(false)
   {}
   //_Parameter_cowl_T:constructor_____________________________________________/
   inline virtual ~Parameter_cowl_T() {}                                         //130515
public:
   // The update methods reset the value only if the
   // the new quality is superior to the old quality.
   virtual float64 update_value_qualified
   (float64 new_value
   ,const CORN::Quality &new_quality
   ,bool override_safety = false)                                 rectification_
   {  float64 updated_value = new_value;
      if (!modified || (modified && new_quality.is_same_or_better_than(*this))
          || override_safety )
      {  updated_value = this->force_amount(new_value);
         assume(new_quality);
         modified = true;
      } else updated_value = get_value();
      return updated_value;
   }
   //_update_value_qualified________________________________________2015-01-23_/
   virtual float64 update_value_qualified_code
      (float64 new_value
      ,CORN::Quality_code new_quality_code
      ,bool override_safety = false)                              rectification_
   {  CORN::Quality_clad new_quality(new_quality_code);
      return update_value_qualified(new_value,new_quality,override_safety);
   }
   //_update_value_qualified_code___________________________________2015-01-23_/
 public:
   // The force methods reset the value irrespective of the existing quality.
   // These should only be used when the quality and value are known/assured.
   // I.e. when loading original source data.
   virtual float64 force_value_qualified
      (float64 new_value
      ,const CORN::Quality &new_quality)                          rectification_ //200227 modification_
   {  assume(new_quality);
      return this->force_amount(new_value);
   }
   //_force_value_qualified_________________________________________2015-01-30_/
   virtual float64 force_value_qualified_code
      (float64 new_value
      ,CORN::Quality_code new_quality_code)                       rectification_ //200227 modification_
   {  CORN::Quality_clad new_quality(new_quality_code);
      return force_value_qualified(new_value,new_quality);
   }
   //_force_value_qualified_code____________________________________2015-01-30_/
   virtual float64 copy(const Parameter_cowl_T<Real> &copy_from) initialization_
   {
      // Not sure if I should get and set amount from this method

      assume(copy_from.get_quality());
      updating = false;
      modified = true;
      return force_amount(copy_from.get_value());

      // Not sure if I should get and set amount from this method
   }
   //_copy______________________________________________2015-01-24__2014-07-17_/
   float64 get_value_update_if_necessary()                        rectification_
   {  // mechanism to get the current value checking if it is currently updating
      // THIS IS NEEDED FOR CLIMGEN!
      float64 value = 0.0;
      if (updating)
          value = 0.0;
            // This should be NaN but I need to trace down daily update errorstd::numeric_limits<float64>::quiet_NaN();
      else
      {  if (!is_valid()) // We may beable to have a calculated value.
               update();
         value = get_value();
      }
      return value;
   }
   //_get_value_update_if_necessary____________________________________________/
   virtual void unknown()                                          modification_
      {  force_value_qualified_code(0.0,CORN::unknown_quality);
      }
   //_unknown_______________________________________________________2015-01-20_/
   virtual CORN::Quality_code invalidate(bool absolutely = true)  rectification_
   {  if (CORN::Quality_cowl::invalidate(absolutely) == CORN::not_valid_quality)
         this->force_amount(0.0);
            // This should be NaN but I need to trace down daily update error (std::numeric_limits<double>::quiet_NaN());
      return get_quality_code();                                                 //
   }
   //_invalidate____________________________________________________2015-01-22_/
   virtual bool is_updating()                  affirmation_ { return updating; }
   virtual bool is_modified()                  affirmation_ { return modified; }
   //_affirmations_____________________________________________________________/
   virtual const CORN::Quality &modify_quality_code(CORN::Quality_code quality_)
   {  modified = true; assume_code(quality_); return *this; }
   //_modify_quality_code___________________________________________2015-01-23_/
   virtual const CORN::Quality &modify_quality(const CORN::Quality &quality_)
   {  modified = true; assume(quality_);   return *this; }
   //_modify_quality________________________________________________2015-01-23_/
   inline virtual const CORN::Quality &update()                   rectification_
      { return (const CORN::Quality &)(*this); }
   //_update________________________________________________________2015-01-21_/
      // Derived classes must override if values are calculated as needed.
 protected:  // These must only be called by Parameter_cowl
   virtual float64 get_value()                                             const
      {  return Physical::Property_cowl<Real>::get_amount(); }
   //_get_value_____________________________________________________2015-01-23_/
 protected: // These must only be called by Property
   inline virtual float64 set_amount(float64 new_amount)          rectification_
      { return this->force_amount((Real)new_amount); }
   //_set_amount_______________________________________________________________/
   virtual float64 constrain_to_range(float64 low, float64 high)  rectification_
   {  float64 curr_value = get_value();
      float64 constrained_value = CORN::must_be_between<float64>(curr_value,low,high);
      return this->force_amount(constrained_value);
   }
   //_constrain_to_range____________________________________________2015-01-25_/
 public: //Property_cowl overrides
   inline virtual float64 in_prescribed_units()                            const
      { return get_value_update_if_necessary(); }
   //_in_prescribed_units___________________________________________2015-01-26_/
};
//_Parameter_cowl_T_____________________________________2015-01-25__2014-04-16_/
}//_namespace CS______________________________________________PARAMETER_COWL_H_/
#endif


