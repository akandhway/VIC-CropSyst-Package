#ifndef progressH
#define progressH
#include "corn/metrology/units_clad.h"
#include "corn/const.h"
#include "corn/chronometry/date_time_64.h"
namespace CORN
{
//______________________________________________________________________________
/*
This class is used to generate a textual indicator
to display program processing progress for text based user interfaces
*/

/* progress-indicator type flag
An application may also have one or more specialized progress indicators
simple percent done or progress bar or processing throbber
or ETC estimated time of completion
*/
//#define PROGRESS_STEP            0x01
/*
   step:{symbol}
   show in indication of item processed
   {symbol} is a single character.
   (for example text output represented by a . printed at each iteration)
*/
#define PROGRESS_COUNT           0x01
/*
   count
   show the number of items processed
*/
#define  PROGRESS_ELAPSED        0x02
/*
   elapsed:{units}
   running time (time will be shown in the progress-units)
*/
#define PROGRESS_THROBBER        0x04
/*
   trobber:{symbols}
   throbber symbol is printed on the line (in the order specified)
   The throbber is rendered by printing in sequence the specifyied symbols
   with preceded by (carridge return or backspace character) (after the first one).
   (The underscore is replaced with a space, giving the appearance of blinking)
   I.e.
   -/|\ would represent clock hands ticking away.
   _* would represent a flashing light
*/

#define PROGRESS_MARQUEE         0x08
/* marquee:{width}
   show progress string in rotating field.
*/
#define PROGRESS_PERCENT         0x10
/* percent or %
  show the percent done
*/

#define PROGRESS_BAR             0x20
/* bar:{symbols}
   progress bar optional
   {symbols} specify the characters used to draw the bar,
   The first character indicates not completed, the second is completed.
   I.e.
   -+ +++++-----
   _X XXXXX_____
   -= =====-----
   =# #####=====
*/
#define PROGRESS_REMAINING       0x40
/*
   remaining:{units}
   The amount of iterations or items or estimated time remaining
   as indicated by the optional progress-units.
   If progress units are also specified the units indicator will also be printed.
   Otherwise progress units show as 99 years 99 days 99 hours 99 minutes 99seconds
    (time option will show also the date if not today)
               the program is expected to finish.
*/

#define PROGRESS_ETC             0x80
/*
  etc:{units} or ETC:{units}
   The time the program is estimated to complete (ISO standard format).
   If the progress-units is:
   date: The date and time are both always printed.
   time: The time is printed (if the finish time is not today the date is included).
*/

/*
There may be multiple progress-indicator type options,
although a program is not expected to implement any of them.
The bit/flag value sort of indicates more complexity or overhead
in preparing the indicator.

Specifying progress indicators enables progress frequency
(the default progress-frequency level)
*/
/* progress-frequency indicates how often the progress indicator is updated.
The larger the number, the greater the frequency.
The interpretation of the frequency is application specific.
Typically it would be one of the following:
- The number of items processed or main loop iterations.
- Real world run time
- Simulated time

This could be a level or time interval however the program wishes to interpret.
0 = no progress indicates no program during run
   (although always show indicator at end)
Typically the value (between 0 and 255) is run time

obs For simulation models
obs 1 = show progress at yearly time steps
obs 2 = show progress at daily time steps

*/
/* progress-units specifies the units to interpret the frequency.
The units are standard UED units code such as second, or minute or hour.
The default units would be (count main loop iterations such as items processed)

The progress_units argument could be omitted if the units are
concatenated with the progress-frequency number
I.e.
--progress-frequency=10s   would be every 10 seconds
--progress-frequency=20%   would be every 20 percent of completion
*/

//______________________________________________________________________________
struct Progress_parameter
{
   nat8        indicator_count;
   bool        step_mode;
   nat8        indicator_order[8];
   nat8        indicators;  // flags/bitmask
   std::string symbols;
   nat8        throbs;
   nat32       frequency;
      // Normally frequency is steps
      // In the case of time frequence modes frequency is seconds.
   Units_clad  frequency_units;
   Units_clad  display_units;    // Should have display uses for each indicator
   nat8        marquee_width; // 0 indicates static mode
 public:
   Progress_parameter();
   bool recognize_assignment
      (const std::string &variable
      ,const std::string &value)                                  modification_;
   bool set_indicator(const std::string &value);
};
//_Progress_parameter_______________________________________________2017-03-16_/
class Progress_display
{public:
   Progress_parameter  parameter;
   mutable nat32 steps_completion; // The predetermined number of steps to complete
   mutable nat32 steps_completed;

 private: // control
//   const Progress_parameter  &parameter;

 protected: // status
   CORN::Date_time_clad_64             start_time;
      // This may need to be date time because the program run
      // could very likely span days
   mutable CORN::Date_time_clad_64     last_time;
   mutable nat32                       frequency_achieved; //steps
   mutable nat8                         marquee_offset;
 private:
   mutable std::string        specific;
   // The class using this may want to have its own
   // stepping indicator
 public:
   Progress_display();
   bool step()                                                   rectification_;
      // return true if now would meet criteria for displaying progress
      // such that the progress line was actually displayed
   inline bool know_steps_to_complete(nat32 steps)                   cognition_
      { steps_completion = steps; return true; }
   void stream_to(std::ostream &stream, char delimiter = '\r')       stream_IO_;
      // Stream to a file other than stdout.
   bool start()                                                   modification_;
   bool stop(std::ostream &stream, char delimiter = '\r')            stream_IO_;
   bool know_specific_string(const std::string &new_specific)        cognition_;
 private:
   nat32 compose(std::string &composed)                                   const;
      // returns the length of the composed string
};
//_Progress_display_________________________________________________2017-03-16_/
extern Progress_display *progress_display_global;
extern Progress_display &provide_progress_display_global();
extern Progress_display *get_progress_display_global();
//______________________________________________________________________________
}//_namespace CORN_____________________________________________________________/
#endif

