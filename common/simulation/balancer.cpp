#include "common/simulation/balancer.h"
#include "corn/math/compare.h"
#include "corn/measure/measures.h"
namespace Simulation
{
//______________________________________________________________________________
Balancer::Accumulator::Accumulator(const char *balance_description, const char *accumulator_description)
: description(balance_description)
, initial(0)
, pluses (0 ,(uint32)10,0)
, minuses(0 ,(uint32)10,0)
, final  (0)
, balance(0)
, start_initial(0)
{
   description.append(" ");
   description.append(accumulator_description);
}
//______________________________________________________________________________
//void Balancer::append_caption(bool plus_else_minus,string &caption)    mutates
//{
//   if (plus_else_minus) plus_captions.add(caption);
//   else                 minus_captions.add(caption);
//}
//______________________________________________________________________________
void Balancer::Accumulator::clear()
{  pluses.clear();
   minuses.clear();
   initial = 0;
   final = 0;
   balance = 0;
   start_initial = 0;
}
//______________________________________________________________________________
bool Balancer::Accumulator::start_timestep()
{
   pluses.clear();
   minuses.clear();
   initial = start_initial;
   final = 0.0;
   return true;
}
//______________________________________________________________________________
void Balancer::Accumulator::reinitialize(float32 _initial)         modification_
{  // Note, this has not been testest
   start_initial = _initial;
}
//______________________________________________________________________________
float32 Balancer::Accumulator::add(const Accumulator &augend)
{
   float32 pluses_sum  = pluses.increment_by(augend.pluses);
   float32 minuses_sum = minuses.increment_by(augend.minuses);
   float32 all_pluses = pluses_sum + initial;
   float32 all_minuse = minuses_sum+ final;
   balance = all_pluses - all_minuse;
/*
if (!CORN::is_approximately<float32>(balance,0.0,0.00001))
cerr << endl <<"Accumulation balance error " << description.c_str() <<  ":" << balance << endl;
*/
   return balance;
}
//______________________________________________________________________________
float32 Balancer::Accumulator::update_balance()
{  float32 pluses_sum = pluses.sum();
   float32 minuses_sum = minuses.sum();
   float32 all_pluses = pluses_sum + initial;
   float32 all_minuses = minuses_sum+ final;
   balance = all_pluses - all_minuses;
/*
if (!CORN::is_approximately<float32>(balance,0.0,0.00001))
cerr << endl << "Update balance error " << description.c_str() <<  ":" << balance << "\n"; // endl;
*/
   return balance;
}
//______________________________________________________________________________
void Balancer::Accumulator::write(std::ostream &balance_report_file) performs_IO_
{
   balance_report_file
      << initial; // plus
   for (uint32 p = 0; p < pluses.get_count(); p++)
      balance_report_file << '\t' << pluses.get(p,false);
   for (uint32 p = 0; p < minuses.get_count(); p++)
      balance_report_file << '\t' << minuses.get(p,false);
   balance_report_file
      << '\t' << final   // minus
      << '\t' << balance;
}
//______________________________________________________________________________
Balancer::Balancer
(Units_code  _units, const char *balance_description)
: CORN::Item()
, units(_units)
, daily     (balance_description,"journal")
, seasonal  (0) //has_seasonal? new Balancer::Accumulator(balance_description,"seasonal") : 0)
, annual    (balance_description,"annual")
{}
//______________________________________________________________________________
Balancer::~Balancer()
{  if (seasonal) delete seasonal; seasonal = 0;
}
//______________________________________________________________________________
bool Balancer::start_day()                        modification_
{  daily.start_timestep();
   return true;  //130618 WARNING need to check that callers now get the balance value
}
//______________________________________________________________________________
float32 Balancer::start_season()                                   modification_
{
   //130618  C++ already checks null if (seasonal) delete seasonal;
   seasonal = new Balancer::Accumulator(/*Here I should keep the description */"","seasonal");
      seasonal->start_timestep();
   return 0.0;
}
//______________________________________________________________________________
bool Balancer::deactivate_seasonal()                                modification_
{
   if (!seasonal)
      return false; // nothing to deactivate
   delete seasonal;
   seasonal = 0;
   return true;
}
//______________________________________________________________________________
bool Balancer::start_year()                       modification_
{  annual.start_timestep();
   return true;   //130618 WARNING need to check that callers now get the balance value
}
//______________________________________________________________________________
bool Balancer::end_day() modification_
{
   float32 current_final = get_final();
   daily.final = current_final;
   if (seasonal) seasonal->final = current_final;
   annual.final = current_final;
   daily.update_balance();
   if (seasonal) seasonal->add(daily);
   annual   .add(daily);
   if (balance_file_daily)
   {
      write_daily(*balance_file_daily);
      (*balance_file_daily) << "\t";
   }
   daily.reinitialize(daily.final);
   return true;                                                                  //150117
}
//______________________________________________________________________________
bool Balancer::end_year()                                          modification_
{
   if (balance_file_annual)
   {  write_annual(*balance_file_annual);
      (*balance_file_annual) << "\t";
   }
   annual.reinitialize(annual.final);
   return true; //130618 WARNING need to check that callers now get the balance value
}
//______________________________________________________________________________
float32 Balancer::end_season()                                     modification_
{  float32 seasonal_balance = 0.0;
   if (seasonal)
   {
      seasonal_balance = seasonal->balance;
      // NYI 090409 eventually will add seasonal balance output
      seasonal->reinitialize(seasonal->final);
   }
   #if (CS_VERSION == 4)
   return seasonal_balance;
   #else
   return true;  //130618 WARNING need to check that callers now get the balance value
   #endif
}
//______________________________________________________________________________
void Balancer::write_column_headers(std::ostream &balance_report_file) performs_IO_
{  bool more_column_headers = true;
   balance_report_file
      << "\"initial\"";
   for (uint32 ch = 0; more_column_headers; ch++)
//   for (uint32 p = 0; p < pluses.get_count(); p++)
   {
      std::string caption;
      more_column_headers = get_caption_plus (ch ,caption);
      if (more_column_headers)
         balance_report_file << "\t\"" << caption.c_str() << " (+)\"";
   }
   more_column_headers = true;
   for (uint32 ch = 0; more_column_headers; ch++)
//   for (uint32 m = 0; m < minuses.get_count(); m++)
   {
      std::string caption;
      more_column_headers = get_caption_minus (ch ,caption);
      if (more_column_headers)
         balance_report_file << "\t\"" << caption.c_str() << " (-)\"";
   }
   balance_report_file
      << "\t\"final (-)\""
      << "\t\"balance\"";
}
//______________________________________________________________________________
void Balancer::write_daily(std::ostream &balance_report_file)       performs_IO_
{  daily.write(balance_report_file);
}
//______________________________________________________________________________
void Balancer::write_annual(std::ostream &balance_report_file)      performs_IO_
{  annual.write(balance_report_file);
}
//______________________________________________________________________________
void Balancer::write_seasonal(std::ostream &balance_report_file)    performs_IO_
{  if (seasonal) seasonal->write(balance_report_file);
}
//______________________________________________________________________________
}//namespace Simulation________________________________________________________/


