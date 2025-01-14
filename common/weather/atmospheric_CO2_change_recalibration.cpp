#include "atmospheric_CO2_change_recalibration.h"
#include <fstream>
#include <string>
#include "corn/math/numerical/interpolation/interpolator_temporal.h"
#include <assert.h>
using namespace CORN;
//______________________________________________________________________________
Atmospheric_CO2_change_recalibration::Atmospheric_CO2_change_recalibration
(const char *CO2_recalibration_filename                                          //121126
, const CORN::Date_const &start_date
, const CORN::Date_const &end_date
, const CORN::date32 &simdate_raw_                                                 //171207
, float32 initial_CO2_conc
, bool &CO2_data_encountered)
: Atmospheric_CO2_change_element(simdate_raw_)                                     //171207
, curr_CO2_conc()                                                                //150507
, interpolator(0)
{
   curr_CO2_conc.set_ppm(initial_CO2_conc,CORN::measured_quality);               //150507
      // presuming measured quality
   //caller already checks for exists and if not then recal_file will not be good //120125
   std::ifstream recal_file(CO2_recalibration_filename);
   interpolator = new CORN::Interpolator_temporal //180430 Interpolation_curve_dated
      (linear_curve,(float32)initial_CO2_conc,&start_date);                      //150326RLN
   int read_year; int doy; std::string code;                                     //120125 char code[100];
   float32 value;                                                                //090709
   while ((! recal_file.eof()) && recal_file.good())
   {  recal_file >> read_year >> doy>> code >>   value;
      Year start_every_year = (read_year == 0) ?start_date.get_year(): read_year;//000504
      Year end_every_year   = (read_year == 0) ?end_date.get_year()  : read_year;//000504
      for (Year  year = start_every_year ; year <= end_every_year ; year++)      //000504
      {  CORN::Date_clad_32 observation_date(year,doy);                          //000504
         if (code == "CO2")                                                      //000504
         {  CO2_data_encountered = true;                                         //080213
            interpolator->set_by_date(observation_date,value);                   //000504
            if (year == start_date.get_year())                                   //120125
               curr_CO2_conc.set_ppm(value,CORN::measured_quality);              //150507
   }  }  }
   if (!CO2_data_encountered)                                                    //120125_080212
   {  // The interpolation record is empty, so just delete it                    //080212
         // maybe we will then default to a single constant value                //080212
      delete interpolator;                                                       //080212
      interpolator = 0;                                                          //080212
   }
}
//_Atmospheric_CO2_change_recalibration:constructor____________________________/
Atmospheric_CO2_change_recalibration::~Atmospheric_CO2_change_recalibration()
{  delete interpolator;
}
//_Atmospheric_CO2_change_recalibration:destructor_____________________________/
bool Atmospheric_CO2_change_recalibration::end_year()              modification_
{
   if (interpolator)
      curr_CO2_conc.set_ppm                                                      //150507
         (interpolator->get_ordinate_by_date(simdate)
         ,CORN::interpolated_quality);
      //150507 conceptual: probably should set quality of observation when the point is fixed
      //150507 curr_CO2_conc_ppm = interpolation->get_ordinate_by_date(Dec_25);
   return Atmospheric_CO2_change_element::end_year();                            //171207
}
//_end_year____________________________________________________________________/
bool Atmospheric_CO2_change_recalibration::start_day()             modification_
{  if (interpolator)
      curr_CO2_conc.set_ppm
         (interpolator->get_ordinate_by_date(simdate),CORN::interpolated_quality); //150507
      //150507 conceptual probably should set quality of observation when the point is fixed
      //150507 curr_CO2_conc_ppm = interpolation->get_ordinate_by_date(today);
   return Atmospheric_CO2_change_element::start_day();                           //171207
 }
//_start_day________________________________________________________2015-03-15_/
