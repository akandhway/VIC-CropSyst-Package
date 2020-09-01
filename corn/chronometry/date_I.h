#ifndef date_IH
#define date_IH
#include "corn/chronometry/temporal.h"
namespace CORN
{
   class Date_format; // defined in Date_format.h
//______________________________________________________________________________
extern nat8 common_year_month_lengths[13];
extern nat8 leap_year_month_lengths[13];
//______________________________________________________________________________
interface_ Date_const
//#define Date_const_interface Date_const
: public extends_interface_ Temporal
{ //Conforms to ISO8601:2004
 public:
   enum Days_of_week
      {NO_DOW,SUNDAY,MONDAY,TUESDAY,WEDNESDAY,THURSDAY,FRIDAY,SATURDAY};
 public: // accessors
   virtual datetime64   get_datetime64()                                const=0;
      ///< \returns the date, time or date time as a 64 bit float (double)
   inline virtual float64 get_time64()                       const { return 0; } //170815
   virtual date32       get_date32()                                    const=0;
   virtual Year         get_year()                                      const=0;
      // Note that in ISO8601:2004 negative years are 1 - the BC year
      // I.e.  0 is 1BC  -1 is 2BC

   virtual DOY          get_DOY()                                       const=0;
   virtual Month        get_month()                                     const=0;
   virtual DOM          get_DOM()                                       const=0;
   virtual void         gregorian(Year &year, Month &month, DOM &dom)   const=0;
      ///< extracts the year month and day of month
   virtual Days_of_week day_of_week()                                   const=0;
      ///< returns the day of week (see days_of_week)
      /// 1 for Sunday, 2 Monday etc...

   // There used to be get() which was equivelent of get_date32()                //170429
   // removed because date time and datetime classes would be mutually ambiguous
 public: // math
   virtual nat32 days_between
      (const Date_const &date2,bool inclusive)                   calculation_=0;
      /**< Counts the number of days from date to date2 (or vica verse).
         I.e. The days between today and tomorrow is 1. if Inclusive, it is 2.
         Either date_num may be greater than the other.
         This always returns a positive number.
      **/
   virtual int32 days_to(const Date_const &date2)                calculation_=0;
      // Counts the number of days to date2 (or vica verse).                                                                                                                                                                                               m
      // Will return a negative
      // result of this function plus date_num gives date2.)
 public: // affirmations
   virtual bool is_relative()                                    affirmation_=0;
 public: // methods with respect to the current date
   virtual bool is_leap_this_year()                              affirmation_=0;
      ///< \return true if the year of the date is a leap year.
   virtual nat16 days_in_this_year()                                    const=0;
      ///< \return the number of days in the year respective of leap year.
   virtual DOM days_in_month()                                          const=0;
 public: // methods for arbitrary date
   virtual bool is_leap_year(Year year)                          affirmation_=0;
   virtual nat16 days_in_year(Year year)                                const=0;
   inline virtual bool is_last_DOY()                                       const
      { return get_DOY() == days_in_this_year(); }               //040524_130225
   inline virtual bool is_last_DOM()                                       const
      { return get_DOM() == days_in_month(); }
 public: // string conversion
   // text representation of the date can be practically any date format.
   //
   // ordering is one of the Date or gregorian date orderings.
   // styles may be a year format or'ed with a numeric format and month representation.
   // separator is the character used to seperate the date fields usually  / - . | or space.
   //    special cases:
   //       if separator is NULL, then no separator is used I.e: 1998Jan01
   //       if separator is ',', then a comma is used only before last
   //       field and spaces are inserted between each field:
   //       I.e:  Jan 1, 1993  or  26 January, 1993
   virtual void prefer_date_format(const Date_format *format=0) contribution_=0;
      // passing 0 defaults to the default ISO standard format
   virtual const std::string &append_to_string
      (std::string &target
      ,const Date_format *target_date_format=0                                   //180624
      ,const Time_format *target_time_format=0)                         const=0; //180625
};
//_Date_interface___________________________________________________2017-04-26_/
interface_ Date
#define Date_interface Date
: public extends_interface_ Date_const                                           //170815
{
 public: // assignations
   virtual datetime64 clear()                                 initialization_=0;
   virtual datetime64 set_now()                                  assignation_=0;
   virtual datetime64 set_date32(date32 date32)                  assignation_=0;
   virtual datetime64 set_datetime64(datetime64 value)           assignation_=0;
   virtual datetime64 set_year(Year year)                        assignation_=0;
   virtual datetime64 set_DOY(DOY doy)                           assignation_=0;
   virtual datetime64 set_YMD(Year year,Month month,DOM dom)     assignation_=0;
   virtual datetime64 set_YD(Year year,DOY doy)                  assignation_=0;
   virtual datetime64 set_month(Month month)                     assignation_=0;
   virtual datetime64 set_DOM(DOM dom)                           assignation_=0;
   virtual datetime64 set(const Temporal &other)                 assignation_=0;
 public: // autoincrement
   virtual datetime64 inc_years(int16 years)                    modification_=0;
      // Adds offset years to the date.
      // If years is negative,  years are subtracted
   inline virtual datetime64 dec_years(int16 years)             modification_
      { return inc_years(-years); }
      // Subtracts offset years to the date.
      // If years is negative,  years are added
   virtual datetime64 inc_months(int16 months)                  modification_=0;
      // Adds offset months to the date.
      //   If offset is negative, offset months are subtracted
   inline virtual datetime64 dec_months(int16 months)           modification_
      { return inc_months(-months); }
   // Subtracts offset months from the date.
   //   If offset is negative, offset months are added
   virtual datetime64 inc_days(int32 days)                      modification_=0;
      // Adds days to the date incrementing the year if necessary.
      // If offset is negative, offset days are subtracted.
   virtual datetime64 dec_days(int32 days)                      modification_=0;
      // Subtracts days from the date decrementing the year if necessary.
      //   If offset is negative, offset days are added.
   virtual datetime64  inc_day()                                modification_=0;
      // Adds days to the date incrementing the year if necessary.
      // If offset is negative, offset days are subtracted.
   virtual datetime64 dec_day()                                 modification_=0;
      // Subtracts days from the date decrementing the year if necessary.
      //   If offset is negative, offset days are added.
   // 170428 inc() and dec() are obsolete because would be ambiguouse
   // with time and date_time classes.
   // use inc_day() and dec_day()
 public: // string conversion
   virtual datetime64 set_c_str
      (const char *date_str
      ,const Date_format *from_format=0)                         assignation_=0; //180625
   virtual datetime64 set_w_str
      (const wchar_t *date_str
      ,const Date_format *from_format=0)                         assignation_=0; //180625
   inline virtual datetime64 set_string
      (const std::string &date_string
      ,const Date_format *from_format=0)                           assignation_  //180625
      { return set_c_str(date_string.c_str(),from_format); }                     //180625
 public: // operators
   //(valid interface implementations because implemented with virtual methods.
   inline virtual datetime64 operator = (const Temporal & other)
      { return set_datetime64(other.get_datetime64()); }
   inline virtual datetime64 operator ++ ()              { return inc_days(1); }
   inline virtual datetime64 operator -- ()              { return dec_days(1); }
   inline virtual datetime64 operator += (int dd) { return inc_days((int16)dd);}
   inline virtual datetime64 operator -= (int dd) { return dec_days((int16)dd);}
   // NYI  >> operators
};
//_Date_____________________________________________________________2017-05-24_/
}//_namespace CORN_____________________________________________________________/
// Roger Nelson rnelson@wsu.edu
#endif
