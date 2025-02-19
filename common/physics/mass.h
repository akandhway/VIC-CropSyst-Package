//______________________________________________________________________________
#ifndef massH
#define massH
#include "corn/const.h"
#include "corn/measure/measures.h"
#include "physics/property.h"
namespace CORN
{  typedef uint32 Units_code;
   class Units_clad;
}
namespace Physical
{
//______________________________________________________________________________
interface_ Mass
: public extends_interface_ Physical::Property_interface                         //150121
{// Accessors
   inline virtual float64 mg()        const { return get_in_units(UC_mg_mass); }
   inline virtual float64 g()         const { return get_in_units(UC_g_mass);  }
   inline virtual float64 kg()        const { return get_in_units(UC_kg_mass); }
   inline virtual float64 set_mg (float64 value_mg)                modification_
                        { return update_value_with_units(value_mg,UC_mg_mass); }
   inline virtual float64 set_g  (float64 value_g)                 modification_
                          { return update_value_with_units(value_g,UC_g_mass); }
   inline virtual float64 set_kg (float64 value_kg)                modification_
                        { return update_value_with_units(value_kg,UC_kg_mass); }

 // arithmetic
          virtual float64 add(const Mass &addend)               modification_=0;
   inline virtual float64 calc_concentration_percent(float64 quantity_kg)  const
                                        { return (quantity_kg / kg()) * 100.0; }
   inline virtual float64 calc_concentration_fraction(float64 quantity_kg) const
                                                 { return  quantity_kg / kg(); }
 protected:
   virtual float64 merge_intensive_property
     (float64 augend_quality, float64 addend_quality, float64 addend_mass) const; //030916
};
//_Mass________________________________________________________________________/
interface_ Matter            // interface
{  //  Matter has mass and occupies volume (http://en.wikipedia.org/wiki/Matter)
   virtual const Mass &ref_mass()                                       const=0;
   inline virtual float64 get_volume_m3()                                  const
                               { return ref_mass().kg() / get_density_kg_m3(); }
   inline virtual float64 m3()                 const { return get_volume_m3(); }
   inline virtual float64 liter()     const { return get_volume_m3() * 1000.0; }
   inline virtual float64 get_density_kg_m3()                              const
                                      { return ref_mass().kg()/get_volume_m3();}
      // kg/m3   //2010-02-16
      // Warning density may change depending on temperature,
      // currently assuming about 20'C.
      // Derived classes may override if they have a fixed density.
      // I believe this is 'actual density'

   // get quantity accessors
   //(implemented methods because they depend only on interface methods)
   float64 calc_concentration_kg_m3(float64 quantity_kg)                const;   //021018
   float64 calc_concentration_g_m3(float64 quantity_kg)                 const;   //021018
   virtual float64 get_temperature_Celcius()                            const=0;
   inline virtual float64 get_temperature_kelvin()                      const
                               { return C_to_kelvin(get_temperature_Celcius());}
   virtual float64 set_temperature_Celcius(float64 temp_C)      modification_=0;
   inline virtual bool is_exhausted()                               affirmation_
                                                 {return ref_mass().is_empty();} //190731
};
//_Matter______________________________________________________________________/
interface_ Gas
: public extends_interface_ Matter
{  static const float64 ideal_constant_m3_mol;   // 8.314472 J/(K mol)
   static const float64 ideal_gas_pressure;   // 1.0 pascals
   //Nicole is going to change to temperature reference point
   static const float64 ideal_gas_temperature_C;// 5'C   This is too low for most uses
   static const float64 ideal_volume_m3_mol; // m3/mol
   inline virtual float64 calc_volume_m3()                                 const
      { return get_moles() * calc_volume_m3_mol(); };
   inline virtual float64 get_moles()                                      const
      {  return ref_mass().g() / get_molar_mass_g_mole(); }; // g/m
   inline virtual float64 calc_volume_m3_mol()                             const
      { return (ideal_constant_m3_mol*get_temperature_kelvin())/get_pressure_pascal();}
   inline virtual float64 get_pressure_pascal() const{return ideal_gas_pressure;}
      ///< Derived class may have a set pressure. but often 1 atmosphere is sufficient.
   inline virtual float64 get_volume_m3()     const { return calc_volume_m3(); }
      ///< Derived class may have a set volume.
   virtual float64 get_molar_mass_g_mole()                            const = 0;
};
//_Gas_________________________________________________________________________/
#define water_density_kg_m3 1000.0
class Water; // forward decl
//______________________________________________________________________________
interface_ Matter_wet       // with water                               //100819
: public extends_interface_ Matter
{  virtual float64 get_wet_in(CORN::Units_code preferred_units=UC_kg_mass)const;
   virtual float64 wet_kg()                                               const;
      ///< Derived classes may override using
      ///< Dry mass plus the mass of water (wet weight).
   virtual float64 get_moisture_percent()                                 const;
   virtual float64 set_moisture_percent(float64 new_moisture)  modification_= 0;
      /// sets the water mass with respect to the new moisture
      /// \return simply the new percent_moisture
   virtual float64 subtract_water_vol(float64 water_volume_m3) modification_= 0; //020815
      ///< \return the act vol subtracted.  May be limited by available water
   virtual const Water &add_water(const Water &addend)            modification_; //110701
   virtual const Water &get_water()                                   const = 0;
   virtual modifiable_ Water &mod_water()                                   = 0;

   inline virtual float64 get_specific_gravity_true()                      const
      { return get_density_kg_m3() / water_density_kg_m3; };
      // Warning derived classes must either implement get_density_kg_m3()
      // or get_specific_gravity_true();
   // NYI inline virtual float64 get_specific_gravity_apparent()                  const;
   virtual float64 get_density_kg_m3()                                    const; //110727
   virtual float64 set_mass_wet_at_percent_moisture                              //090729
      (float64 new_mass_wet
      ,CORN::Units_code new_units
      ,float64 at_moisture_percent)                             modification_=0;
   inline virtual float64 get_dry_matter_decimal_percent()                 const
      {  return 1.0 - get_moisture_percent(); };
      ///< \return The mass of water (without dry solids mass).
   virtual float64 get_volume_m3()                                        const;
   // calculators (amounts of this biomass given specific properties or conditions)
   // These are valid interface methods because they are based on interface methods
   virtual float64 calc_wet_weight_kg_from_volume_m3(float64 volume_m3)   const; // Based on virtual methods
   virtual float64 calc_volume_m3_from_wet_weight_kg(float64 wet_wght_kg) const; // Based on virtual methods
protected :
   inline virtual float64 convenient_mass_dry() const { return ref_mass().kg();}
   inline virtual bool has_known_specific_gravity()      const { return false; }
};
//_Matter_wet_______________________________________________________2010-08-19_/
}//_namespace Physical_________________________________________________________/
#endif

