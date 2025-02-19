#include "common/biomatter/biomass_abstract.h"
#include "corn/math/compare.hpp"
#include <iostream>
#include "CS_suite/observation/CS_inspector.h"
//______________________________________________________________________________
Biomass_abstract::Biomass_abstract()
: Physical::Mass_common(0.0,UC_kg_mass)
,carbon_fraction        (std::numeric_limits<double>::quiet_NaN())               //131005_070726
,carbon_nitrogen_ratio  (std::numeric_limits<double>::quiet_NaN())               //131005
   // default to an arbitrarly invalid number to indicate C:N has not been set   //070726
,decomposition_constant (0) // default no decomposition
{}
//_Biomass_abstract:constructor_____________________________________2010-08-20_/
Biomass_abstract::Biomass_abstract
(float64 mass_dry_
,float64 carbon_fraction_                                                        //070726
,float64 carbon_nitrogen_ratio_                                                  //070726
,float64 decomposition_constant_)                                                //070726
: Physical::Mass_common (mass_dry_,UC_kg_mass)                                   //100819
, carbon_fraction       (carbon_fraction_)                                       //070726
, carbon_nitrogen_ratio (carbon_nitrogen_ratio_)                                 //070726
, decomposition_constant(decomposition_constant_)                                //070707
{}
//_Biomass_abstract:constructor_____________________________________2006-03-16_/
Biomass_abstract::Biomass_abstract(const Biomass_abstract &from_copy)
: Physical::Mass_common(from_copy)                                               //100819
, carbon_fraction                  (from_copy.carbon_fraction)                  //070726
, carbon_nitrogen_ratio            (from_copy.carbon_nitrogen_ratio)            //070726
, decomposition_constant           (from_copy.decomposition_constant)
{}
//_Biomass_abstract:copy_constructor___________________________________________/
Biomass_abstract::Biomass_abstract(const Biomass &from_copy)
: Physical::Mass_common                  (from_copy.kg(),UC_kg_mass)             //100819
, carbon_fraction                        (from_copy.get_carbon_fraction())       //070726
, carbon_nitrogen_ratio                  (from_copy.get_carbon_nitrogen_ratio()) //070726
, decomposition_constant(from_copy.get_decomposition_constant(OM_INTRINSIC_CYCLING)) //150629
{}
//_Biomass_abstract:constructor________________________________________________/
Biomass_abstract *Biomass_abstract::clone()                                const
{  return new Biomass_abstract(*this);
}
//_clone____________________________________________________________2009-07-02_/
bool Biomass_abstract::is_key_nat32(nat32 key)                      affirmation_
{  return key ==
   (nat32)
      #if (__LP64__ || _M_X64)
      (uint64_t)
      #endif
      this;
}
//_is_key_nat32_____________________________________________________2018-08-20_/
nat32 Biomass_abstract::get_key_nat32()                                    const
{  return (int)
      #if (__LP64__ || _M_X64)
      (uint64_t)
      #endif
      this;
}
//_get_key_nat32____________________________________________________2002-03-19_/
float64 Biomass_abstract::multiply_by(float64 multiplier)          modification_
{  // This shouldn't be needed but the compiler is complaining about ambiguous override
   Physical::Mass_common::multiply_by(multiplier);
   return kg();
}
//_multiply_by_________________________________________________________________/
float64 Biomass_abstract::reset_mass(float64 new_mass)             modification_
{  if (is_empty())
      return add_mass(new_mass);    // Warning when current mass is 0 this will only change the total mass unness add_mass if overridden.
   float64 curr_mass = kg();                                                     //100905
   float64 change = new_mass - curr_mass;
   float64 increase_by = 1.0 + (change/curr_mass);
   multiply_by(increase_by);
   return curr_mass;
}
//_reset_mass_______________________________________________________2006-07-27_/
float64 Biomass_abstract::add_similar_mass(const Biomass &added_biomass)
{  float64 mass_to_add           = added_biomass.kg();                           //060728
   if (mass_to_add > 0.0000001)                                                  //100827
   {  float64 mass_to_add_C_fraction= added_biomass.get_carbon_fraction();       //060728
      float64 mass_to_add_CN_ratio  = added_biomass.get_carbon_nitrogen_ratio(); //100316
      float64 current_CN_ratio      = carbon_nitrogen_ratio;                     //060728
      // TODO: Check difference between carbon_nitrogen_ratio and get_carbon_nitrogen_ratio()
      current_CN_ratio      = get_carbon_nitrogen_ratio();                       //150408BRC
      float64 current_mass = kg();                                               //060316
      float64 current_C    = get_carbon_kg();                                    //060316
      float64 added_C      = mass_to_add * mass_to_add_C_fraction;               //060316
      float64 current_N =  get_nitrogen_organic();                               //090826
      float64 added_N      = added_C / mass_to_add_CN_ratio;                     //060316
      update_amount_in_preset_units(in_prescribed_units()+mass_to_add);          //150123
      float64 C_mass = current_C + added_C;
      float64 N_mass = current_N + added_N;
      carbon_nitrogen_ratio = C_mass/N_mass;
      carbon_fraction = is_empty()                                               //110729
         ? 0.0 :                                                                 //100616
         C_mass / dry_kg() ;
      decomposition_constant = merge_intensive_property
         (get_decomposition_constant(OM_INTRINSIC_CYCLING)                       //150629
         ,added_biomass.get_decomposition_constant(OM_INTRINSIC_CYCLING)         //150629
         ,mass_to_add);                                                          //100316
   }
   return dry_kg();                                                              //070801
}
//_add_similar_mass_________________________________________________2006-07-27_/
float64 Biomass_abstract::add(const Physical::Mass &addend)        modification_
{  const Biomass_abstract *addend_as_same_type
      = dynamic_cast<const Biomass_abstract *>(&(addend));
   if (addend_as_same_type)
   {  const Biomass_abstract &added_biomass = *addend_as_same_type;
      float64 mass_to_add           = added_biomass.kg();
      if (mass_to_add > 0.0000001)
      {  float64 mass_to_add_C_fraction= added_biomass.get_carbon_fraction();
         float64 mass_to_add_CN_ratio=added_biomass.get_carbon_nitrogen_ratio();
         float64 current_CN_ratio      = get_carbon_nitrogen_ratio();            //150408BRC
         float64 current_mass = kg();
         float64 current_C    = get_carbon_kg();
         float64 added_C      = mass_to_add * mass_to_add_C_fraction;
         float64 current_N =  get_nitrogen_organic();
         float64 added_N      = mass_to_add_CN_ratio > 0.000
            ? added_C / mass_to_add_CN_ratio : 0.0;
         float64 new_amount = Physical::Mass_common::add(addend);
         float64 C_mass = current_C + added_C;
         float64 N_mass = current_N + added_N;
         carbon_nitrogen_ratio = (N_mass>0.00001) && (C_mass > 0.000001)
            ? C_mass/N_mass : current_CN_ratio;
         carbon_fraction =
            CORN::is_approximately<float64>(new_amount,0.0,0.000001)
            ? 0.0 : C_mass / new_amount ;
         decomposition_constant = merge_intensive_property
            (get_decomposition_constant()
            ,added_biomass.get_decomposition_constant(),mass_to_add);
         /*150408RLN
         if (CORN::is_approximately<float64>(current_CN_ratio,mass_to_add_CN_ratio,0.00001) &&
            !CORN::is_approximately<float64>(current_CN_ratio,carbon_nitrogen_ratio,0.00001))
            std::cout << "Warning: carbon nitrogen ratio mismatch in add().  Programming error." << std::endl;
        */
      }
   } else Physical::Mass_common::add(addend);
   return dry_kg();
}
//_add______________________________________________________________2010-09-08_/
Biomass_abstract *Biomass_abstract::extract_fraction
(float64 fraction)                                                 modification_
{  Biomass_abstract *extraction = clone();
   fraction = CORN::must_be_between<float64>(fraction,0.0,1.0);
   extraction->multiply_by(fraction);
   multiply_by(1.0-fraction);
   return extraction;
}
//_extract_fraction_________________________________________________2010-02-28_/
Biomass_abstract *Biomass_abstract::extract_mass_kg
(float64 mass_to_extract)                                          modification_
{   Biomass_abstract *extraction = 0;
   float64 fract_of_avail = 0.0;
   float64 mass_available = kg();
   float64 act_extr_mass = CORN::must_be_less_or_equal_to<float64>
      (mass_to_extract,mass_available);
   fract_of_avail =
        ((mass_available == 0.0) || (act_extr_mass == 0.0))
        ? 0.0 : act_extr_mass / mass_available;                                  //100621
   extraction = extract_fraction(fract_of_avail);
   return extraction;
}
//_extract_mass_____________________________________________________2003-02-27_/
float64 Biomass_abstract::get_carbon_nitrogen_ratio()                 provision_
{  //float64 CN_ratio = carbon_nitrogen_ratio;
   // If no fixed CN ration, try to compute it
   // if carbon and nitrogen mass is available
   if (CORN::is_approximately(carbon_nitrogen_ratio,0.0,0.0000001))
      carbon_nitrogen_ratio = calc_carbon_nitrogen_ratio();
   return carbon_nitrogen_ratio;
}
//_get_carbon_nitrogen_ratio________________________________________2010-08-26_/
RENDER_INSPECTORS_DEFINITION(Biomass_abstract)
{
   // will use context emanator
   #ifdef CS_OBSERVATION
   inspectors.append(new CS::Inspector_scalar(ref_amount(),UC_kg_mass,*context,"mass",CORN::value_statistic,GENERATE_VARIABLE_CODE));
   #endif
   return 0; //emanator;
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2015-08-20_/

