#include <algorithm>
#include "mass_common.h"
#include "corn/measure/unitconv.h"
#include "corn/math/compare.hpp"
namespace Physical
{
//_____________________________________________________________________________/
Mass_common::Mass_common()
: Physical::Property_complete(EXTENSIVE,0.0,UC_kg_mass)                          //150120
{}
//_Mass_common:constructor__________________________________________2011-09-09_/
Mass_common::Mass_common(float64 _amount, CORN::Units_code _units)
: Physical::Property_complete(EXTENSIVE,_amount,_units)                          //150120
{}
//_Mass_common:constructor_____________________________________________________/
Mass_common::Mass_common(const Physical::Mass & from)
: Physical::Property_complete(EXTENSIVE,from.kg(), from.get_units())             //150120
{}
//_Mass_common:constructor_____________________________________________________/
float64 Mass_common::add(const Mass &addend)                       modification_
{  const Mass_common *as_mass_common = dynamic_cast<const Mass_common *> (&addend);
   float64 sum =                                                                 //150123
      (as_mass_common)
      ? update_amount_in_preset_units(in_prescribed_units() + as_mass_common->in_prescribed_units())
      : 0.0;
   return sum;
}
//_Mass_common:add__________________________________________________2010-09-08_/
Water::Water
(float64 mass, CORN::Units_code mass_units, float64 temperature, CORN::Units_code temperature_units)
: Physical::Mass_common(mass,mass_units)
,temperature(temperature, CORN::estimated_quality)                               //150529
{  switch (temperature_units)
   {
      case UC_kelvin : this->temperature.set_kelvin(kelvin_to_C(temperature), CORN::estimated_quality); break;//150529
      case UC_Celcius : break;   // temperature assumed Celcius by default
      // NYI case UC_Farenheit : F_to_C(temperature);
      // NYI default: assert(false); break;   // some other unaccounted for units
   }
}
//_Water:constructor________________________________________________2011-06-30_/
Water::Water
(float64 volume_m3
,float64 _temperature_C
,CORN::Units_code temperature_units)
: Physical::Mass_common(water_density_kg_m3 * volume_m3,UC_kg_mass)              //110818
{  temperature.set_Celcius(_temperature_C, CORN::estimated_quality);
}
//_Water:constructor________________________________________________2011-07-01_/
Water::Water(const Water &from_copy)
: Physical::Mass_common(from_copy) // .mass,from_copy.mass_units)
, temperature(from_copy.temperature.Celcius())                                   //150529
{}
//_Water:constructor________________________________________________2011-06-30_/
Water::Water()
: Physical::Mass_common(0,UC_kg_mass)
, temperature(60.0)                                                              //150529
{}
//_water:constructor________________________________________________2011-07-03_/
float64 Water::add(const Physical::Mass &addend)                             modification_
{  const Water *as_water = dynamic_cast<const Water *>(&addend);
   if (as_water && !as_water->is_empty())
   {  // mix temperatures
      //110701 need to check this,  may need to use specific heat
   temperature.set_Celcius(merge_intensive_property(temperature.Celcius(),as_water->temperature.Celcius(),addend.kg()), CORN::calculated_quality);//150529
   }
   return Physical::Mass_common::add(addend);
}
//_Water:add________________________________________________________2011-07-01_/
Constituent_of_mass::Constituent_of_mass
(float64 _amount, CORN::Units_code _units,const Physical::Mass &of_mass)
: Physical::Mass_common(_amount,_units)
, whole_mass(&of_mass)
{}
//_Constituent_of_mass:constructor__________________________________2001-08-03_/
Constituent_of_mass::Constituent_of_mass
(float64 _amount, CORN::Units_code _units,const Physical::Mass *of_mass)
: Physical::Mass_common(_amount,_units)
, whole_mass(of_mass)
{}
//_Constituent_of_mass:constructor__________________________________2001-08-03_/
Constituent_of_mass::Constituent_of_mass(const Constituent_of_mass &copy_from)
: Physical::Mass_common(copy_from)
, whole_mass(copy_from.whole_mass)
{}
//_Constituent_of_mass:constructor__________________________________2001-08-03_/
Constituent_of_matter::Constituent_of_matter
(float64 _amount, CORN::Units_code _units,const Matter &of_matter)
: Constituent_of_mass(_amount,_units,of_matter.ref_mass())
, whole_matter (&of_matter)
{}
//_Constituent_of_matter:constructor________________________________2001-08-03_/
Constituent_of_matter::Constituent_of_matter
(float64 _amount, CORN::Units_code _units,const Matter *of_matter)
: Constituent_of_mass(_amount,_units,of_matter->ref_mass())
, whole_matter (of_matter)
{}
//_Constituent_of_matter:constructor________________________________2001-08-03_/
Constituent_of_matter::Constituent_of_matter(const Constituent_of_matter &copy_from)
: Constituent_of_mass(copy_from)
, whole_matter (copy_from.whole_matter)
{}
//_Constituent_of_matter:constructor________________________________2001-08-03_/
Mass_elemental::Mass_elemental
(float64 _amount, CORN::Units_code _units,const Matter &of_matter,float64 _atomic_mass)
: Constituent_of_matter(_amount,_units,of_matter)
, atomic_mass(_atomic_mass)
{}
//_Mass_elemental:constructor_______________________________________2001-08-03_/
Mass_elemental::Mass_elemental
(float64 _amount, CORN::Units_code _units,const Matter *of_matter,float64 _atomic_mass)
: Constituent_of_matter(_amount,_units,of_matter)
, atomic_mass(_atomic_mass)
{}
//_Mass_elemental:constructor_______________________________________2001-08-03_/
Mass_elemental::Mass_elemental
(const Mass_elemental&copy_from)
: Constituent_of_matter(copy_from)
, atomic_mass(copy_from.atomic_mass)
{}
//_Mass_elemental:constructor_______________________________________2001-08-03_/
}//_namespace Physical_________________________________________________________/

