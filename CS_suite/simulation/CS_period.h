#ifndef CS_periodH
#define CS_periodH
#include "corn/container/item.h"
#include "corn/chronometry/date_time_64.h"
#include <iomanip>
namespace CS {
//______________________________________________________________________________
interface_ Period
: public extends_interface_ CORN::Item
{
   virtual int compare(const CORN::Item &)                              const=0;
   virtual bool complete(CORN::datetime64 datetime)                          =0;
   virtual void write_YAML
      (std::ostream &YAML_strm,nat16 indent_level)                 stream_IO_=0;
   virtual bool end_day()                                                    =0;
   virtual nat32 get_lapse()                                            const=0;
      // lapse is the count of completed days
};
//_Period___________________________________________________________2020-04-09_/
// May want to make this a template for the initiation and completing
// as date32 or datetime64


class Period_clad
: public implements_ Period
{public:
/*200506
   CORN::datetime64  initiation_datetime; // was inception
   CORN::datetime64  completion_datetime;                                        //171114
*/
   CORN::datetime64  initiation_date; // was inception
   CORN::datetime64  completion_date;                                        //171114
            // completion_date is 0 until the period is completed
   nat8              level_or_period;
   std::string       name;
 public:
   nat32 lapse; // completed days (counted at end of day)
      // if the current day is the first day, lapse is 0
 public:
   inline Period_clad
      (CORN::datetime64        initiation_datetime_ //200409 const CORN::Temporal   &temporality_
      ,nat8                    level_or_period_
      ,const std::string      &period_name_)
      : Period()
      , initiation_date(initiation_datetime_) //200409 (temporality_.get_datetime64())
      , completion_date(0)   //will be set on period completion
      , level_or_period    (level_or_period_)
      , name               (period_name_)
      , lapse              (0)
      {}
   implmt virtual int compare(const CORN::Item &)                         const;
   inline virtual bool complete(CORN::datetime64 datetime)
      {
         completion_date = datetime;
         return true;
      }
   implmt virtual void write_YAML
      (std::ostream &YAML_strm,nat16 indent_level)                   stream_IO_; //190124
   inline virtual bool end_day()                     { lapse += 1; return true;} //200409
   inline virtual nat32 get_lapse()                        const {return lapse;} //200409
};
//_Period_clad______________________________________________________2017-11-15_/
}//_namespace_CS_______________________________________________________________/
#endif
