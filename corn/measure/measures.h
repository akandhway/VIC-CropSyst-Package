#ifndef measuresH
#define measuresH

#ifndef primitiveH
#  include "corn/primitive.h"
#endif
//#define ENGLISH 0 obs 970625
//#define METRIC  1 obs

#define over_m2_to_ha  10000.0
#define over_ha_to_m2  0.0001

#define seconds_per_day   86400
#define minutes_per_day    1440
#define hours_per_day        24
#define seconds_per_hour   3600

#define deg_to_rad(deg)    ((deg) * 0.017453293)
#define rad_to_deg(rad)    ((rad) * 57.29578)

#define hours_to_rad(hours)((hours) / 7.64)
#define rad_to_hours(rad)  ((rad) * 7.64)

#define m_to_inch(m)       ((m) / 0.0254)
#define inch_to_m(inch)    ((inch) * 0.0254)

#define cm_to_inch(cm)     ((cm) / 2.54)
#define inch_to_cm(inch)   ((inch) * 2.54)

#define mm_to_inch(mm)     ((mm) / 25.4)
#define inch_to_mm(inch)   ((inch) * 25.4)

#define kg_ha_to_lbs_acre(kgha)     ((kgha) * 0.89206066)
#define lbs_acre_to_kg_ha(lbsacre)  ((lbsacre) * 1.121)

#define kg_ha_to_tons_acre(kgha)  ((kgha) * 4.46089560809852E-04)

#define ha_to_acre(ha)     ((ha) / 0.468564224)
#define acre_to_ha(acre)   ((acre) * 0.468564224)

#define are_to_acre(ha)     ((ha) / 40.468564224 )
#define acre_to_are(acre)   ((acre) * 40.468564224 )

#define acre_to_f2(acre)   ((acre) * 43560.0)
#define f2_to_acre(f2)     ((f2) / 43560.0)

#define ha_to_m2(ha)    ((ha) * 10000.0)
#define m2_to_ha(m2)    ((m2) / 10000.0)

#define per_ha_to_per_m2(per_ha) ((per_ha) / 10000.0)
#define per_m2_to_per_ha(per_m2) ((per_m2) * 10000.0)
//191212 #define per_m2_to_per_ha(per_m2) ((per_m2) / 0.0001)
#define per_mm2_to_per_ha(per_m2) ((per_m2) / 10000000000.0)
//140623  Warning needed to check this units conversion
#define per_mm2_to_per_m2(per_m2) ((per_m2) / 1000000.0)

#define g_m2_to_kg_m2(g_m2)      (g_to_kg(g_m2))
#define g_m2_to_kg_ha(g_m2)      (per_m2_to_per_ha(g_to_kg(g_m2)))

#define g_ha_to_kg_m2(g_ha)      (per_ha_to_per_m2(g_to_kg(g_ha)))

#define m_to_foot(m)       ((m) * 3.048)
#define foot_to_m(foot)    ((foot) / 3.048)

#define m_to_yard(m)       ((m) / 0.9144 )
#define yard_to_m(yard)    ((yard) * 0.9144 )

#define F_to_C(f)          (5.0/9.0 * ((f)-32))
#define C_to_F(c)          ((c) * (9.0 / 5.0) + 32)

#define f_to_inch(f)       ((f) * 12.0)
#define inch_to_f(i)       ((i) / 12.0)

// RLN I think the correct value should be  273.15  was  273.16
#define kelvin_to_C(K)  ((K) - 273.15)
#define C_to_kelvin(C)  ((C) + 273.15)

#define km_to_m(km)      ((km) * 1000.0)
#define m_to_km(m)     ((m) / 1000.0)

#define m_to_mm(m)      ((m) * 1000.0)
#ifndef mm_to_m
#define mm_to_m(mm)     ((mm) / 1000.0)
#endif
#define mm_to_cm(m)      ((m) / 10.0)
#define cm_to_mm(cm)     ((cm) * 10.0)

#define m_to_cm(m)      ((m)  * 100.0)
#define cm_to_m(cm)     ((cm) / 100.0)

#define cm2_to_m2(cm2)  (cm2 * 0.0001)
#define m2_to_cm2(m2)   (m2  * 10000.0)

#define mm2_to_m2(mm2)  (mm2 * 0.000001)
#define m2_to_mm2(m2)   (m2  * 1000000.0)

#define g_to_kg(g)      ((g)  / 1000.0)
#define mg_to_kg(mg)    ((mg)  / 1000000.0)
#define mg_to_g(mg)     ((mg)  / 1000.0)
#define kg_to_g(kg)     ((kg) * 1000.0)
#define kg_to_cg(kg)     ((kg) * 100.0)
#define kg_to_mg(kg)     ((kg) * 1000000.0)

#define g_to_Mg(g)      ((g) / 1000000.0)
#define Mg_to_g(Mg)     ((Mg) * 1000000.0)

#define g_to_mg(g)      ((g) * 1000.0)

#define kg_to_tonne(kg)    ((kg)    * 0.001)
#define tonne_to_kg(tonne) ((tonne) * 1000.0)

#define kg_to_Mg(kg)    ((kg)    * 0.001)
#define Mg_to_kg(Mg)    ((Mg) * 1000.0)


#define kg_m2_to_tonne_ha(kg_m2) ((kg_m2)  * 10.0)
// kg/ha to tonne/ha

#define g_cm3_to_kg_m3(g_cm3) ((g_cm3) * 1000.0)
#define kg_m3_to_g_cm3(kg_m3) ((kg_m3) * 0.001)

#define cm_per_cm3_to_m_per_m3(cm_cm3)  ((cm_cm3) * 10000)
#define per_m3_to_cm3(m3)  ((m3) / 1000000.0)

#define mm_per_m2_to

#define kg_to_m3_water(kgwater) ((kgwater) *  0.001)
#define m3_to_kg_water(m2water) ((m3water) / 0.001)

#define tonne_to_m3(tonne) ((tonne) * 1000.0)
#define m3_to_tonne(m3)    ((m3)    / 1000.0)

#define long_ton_to_tonne(long_ton)  ((long_ton) * 1.016)
#define short_ton_to_tonne(short_ton)  ((short_ton) * 0.907)
#define short_ton_to_tonne(short_ton)  ((short_ton) * 0.907)

#define m3_to_cm3(m3)      ((m3)  * 1000000.0)
#define cm3_to_m3(cm3)     ((cm3) / 1000000.0)

#define kPa_to_Pa(kPa)     ((kPa) *1000.0)
#define Pa_to_kPa(Pa)      ((Pa)  / 1000.0)


#define MPa_to_kPa(MPa)     ((MPa) * 1000.0)
#define kPa_to_MPa(kPa)     ((kPa) / 1000.0)

#define kg_to_lbs(kg) ((kg) * 2.20462262184878)
#define lbs_to_kg(lbs) ((lbs) / 2.20462262184878)
#define lbs_per_ft3_to_kg_per_m3(lbs_per_ft3) (lbs_to_kg(lbs_per_ft3) * 35.937)

#define ft3_to_m3(ft3) ((ft3) / 35.314666721488590250438010354003)
#define ft3_per_ton_to_m3_per_tonne(ft3_per_ton) ((ft3_per_ton) * 1.102 / 35.314666721488590250438010354003 )
// Warning need to verify this conversion

#define knots_to_m_per_sec(knots)  (knots * 0.5144444444)

#define m_per_sec_to_m_per_day(m_per_sec) (m_per_sec * seconds_per_day)
#define m_per_hr_to_m_per_day(m_per_hr) (m_per_hr * hours_per_day)

#define m_per_day_to_m_per_sec(m_per_day) (m_per_day / seconds_per_day)
#define km_per_day_to_m_per_sec(km_per_day)(km_to_m(km_per_day) / seconds_per_day)

//#define mega_calorie_international_table_to_joule(cal)  (1000000 * 4.1868 * cal)
#define mega_calorie_international_table_to_mega_joule(cal)  (4.1868 * cal)

#define calorie_international_table_to_joule(cal)  (4.1868 * cal)
#define calorie_thermalchemical_to_joule(cal)      (4.184  * cal)
#define calorie_mean_to_joule(cal)                 (4.190  * cal)
#define calorie_15C_to_joule(cal)                  (4.185  * cal)
#define calorie_20C_to_joule(cal)                  (4.181  * cal)
#define Calorie_to_joule(Cal)                      (4186   * cal)

#define BTU_international_table_to_joule(btu)  (1055.056 * btu)
#define BTU_thermalchemical_to_joule(btu)  (1054.350 * btu)
#define BTU_mean_to_joule(btu)  (1055.87 * btu)

#define foot_pounds_force_to_joule(fpf)  (1.355 * fpf)
#define foot_poundals_to_joule(fpf)  (0.042 * fpf)

#define horsepower_hours_to_joule(hph)       (2684520.0 * hph)
#define centigrade_heat_units_to_joule(chu)  (1900.4 * chu)


#define Langley_to_mmEvap(langley)  ((langley) / 58.48)
#define mmEvap_to_Langley(mmEvap)   ((mmEvap) * 58.48)

#define Langley_to_MJ_m2(langley)  ((langley) * 0.041876)
#define MJ_m2_to_Langley(MJ_m2)    ((MJ_m2)   / 0.041876)

#define MJ_m2_to_kJ_m2(MJ_m2)       ((MJ_m2) * 1000.0)
#define kJ_m2_to_MJ_m2(kJ_m2)       ((kJ_m2) / 1000.0)

#define MJ_kg_to_J_kg(MJ_kg)       ((MJ_kg) * 1000000.0)
#define J_kg_to_MJ_kg(J_kg)        ((J_kg) / 1000000.0)

#define MJ_kg_to_kJ_kg(MJ_kg)       ((MJ_kg) * 1000.0)
#define kJ_kg_to_MJ_kg(kJ_kg)       ((kJ_kg) / 1000.0)

#define MPa_to_J_kg(MPa)            ((MPa) * 1000.0)
#define J_kg_to_MPa(kJ_kg)          ((kJ_kg) / 1000.0)

#define MJ_m2_to_J_m2(MJ_m2)        ((MJ_m2)* 1000000.0)
#define J_m2_to_MJ_m2(J_m2)         ((J_m2) / 1000000.0)

#define J_cm2_to_MJ_m2(J_cm2)       (J_cm2 * 0.01)

#define MJ_m2_day_to_J_m2_sec(MJ_m2_day) (MJ_m2_day * 1000000.0 /seconds_per_day)
#define MJ_m2_hour_to_J_m2_sec(MJ_m2_hr) (MJ_m2_hr * 1000000.0 /seconds_per_hour)

#define liter_to_foot3(liter)      ((liter) / 28.316)
#define foot3_to_liter(foot3)      ((foot3) * 28.316)

#define liter_to_gallon(liter)     ((liter)  / 3.785412 )
#define gallon_to_liter(gallon)    ((gallon) * 3.785412 )

#define m3_to_gallon(m3)     ((m3)  *  264.172052358148 )
#define gallon_to_m3(gallon)    ((gallon) /  264.172052358148 )

#define yard3_to_m3(yard3)    ((yard3) * 0.764554857984 )

#define atm_to_pascal(atm)    ((atm) * 0.101325)

#define liter_to_gallon_imperial(liter)               ((liter)  / 4.54609  )
#define gallon_imperial_to_liter(gallon_imperial)     ((gallon_imperial) * 4.54609  )


#define m3_to_liter(m3)            (m3 * 1000)
#define liter_to_m3(liter)         (liter / 1000)

#define kg_liter_to_kg_m3(kg_liter)(kg_liter * 1000.0)

#define hours_to_minutes(hours) ( (hours) * 60)
#define seconds_to_minutes(seconds) ( (seconds) * 60)
// Warning to to confirm kg_liter_to_kg_m3()

#define km_to_mile(km)  ( km * 0.621371192237334)
#define mile_to_km(mile)( mile *1.609344 )

/*{ Use NO_MEASURE and NO_CONVERSION if the field is unitless. }*/

extern bool NO_MEASURE;
double tn_ha_to_m(double amount, double bulk_density);
double kg_ha_to_ppm(double value_kg_ha, double thickness, double bulk_density);
double kg_m2_to_ppm(double value_kg_m2, double thickness, double bulk_density);
double ppm_to_kg_m2(double value_ppm, double thickness, double bulk_density);

#endif

//measures

