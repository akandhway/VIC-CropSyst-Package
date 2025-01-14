#include "corn/application/progress.h"
#include "corn/string/strconv.hpp"
#include "corn/measure/unitconv.h"
#include "corn/chronometry/time_second.h"
#include <fstream>
#ifdef __linux__
#  include <sys/ioctl.h>
#endif
#if (defined(_WIN32)  || defined(_WIN64))
#include <windows.h>
#endif

namespace CORN
{
Progress_display *progress_display_global = 0;
//______________________________________________________________________________
Progress_parameter::Progress_parameter()
: indicator_count(0)
, step_mode (false)
, indicators(0)
, symbols   ("")
, throbs    (1)
, frequency (0)
, frequency_units (UC_count)
, display_units   (UC_count)
, marquee_width(0)
{}
//_Progress_parameter:constructor______________________________________________/

//200206  redo: recognize_assignment is obsolete, use expect_structure and move these to get end

bool Progress_parameter::recognize_assignment
(const std::string &variable
,const std::string &value)                                         modification_
{  bool recognized = false;

   if (variable == "--progress")
      recognized = set_indicator(value);
   /* units are not specified with the progress indicator
      and the progress frequency
   else if (variable == "--progress-units")
   {  units.set_label(value.c_str());
      recognized = true;
   }
   */
   else if (variable == "--progress-frequency")
   {
      frequency = CORN::cstr_to_nat32(value.c_str(),10);
      size_t colon_pos = value.find(':');
      if (colon_pos != std::string::npos)
      {
         std::string units(value,colon_pos+1);
         frequency_units.set_label(units.c_str());
         float frequency_in_units = frequency;
         CORN::Units_convertor convertor;
         float frequency_seconds;
         convertor.convert_time
            (frequency_in_units  ,frequency_units.get()
            ,frequency_seconds           ,UT_seconds);
         frequency = frequency_seconds;
      }
      recognized = true;
   }
   return recognized;
}
//_recognize_assignment_____________________________________________2017-03-16_/
bool Progress_parameter::set_indicator(const std::string &value)
{
   size_t colon_pos = value.find(':');
   std::string indicator_label(value,0,colon_pos);
   nat8 indicator_flag = 0;
   if      (indicator_label == "step")      step_mode = true;
   else if (indicator_label == "count")     indicator_flag = PROGRESS_COUNT;
   else if (indicator_label == "elapsed")   indicator_flag = PROGRESS_ELAPSED;
   else if (indicator_label == "throbber")
      { symbols = "|/-\\";                  indicator_flag = PROGRESS_THROBBER;}
   else if (indicator_label == "marquee")   indicator_flag = PROGRESS_MARQUEE;
   else if (indicator_label == "%")         indicator_flag = PROGRESS_PERCENT;
   else if (indicator_label == "percent")   indicator_flag = PROGRESS_PERCENT;
   else if (indicator_label == "bar")
      {  symbols = "_|";                    indicator_flag = PROGRESS_BAR;}
   else if (indicator_label == "remaining") indicator_flag = PROGRESS_REMAINING;
   else if (indicator_label == "etc ")      indicator_flag = PROGRESS_ETC;
   else if (indicator_label == "ETC")       indicator_flag = PROGRESS_ETC;
   if (indicator_flag)
   {
      indicator_order[indicator_count++] = indicator_flag;
      indicators |= indicator_flag;
      step_mode = false;
   }
   if (colon_pos != std::string::npos)
   {
      std::string option(value,colon_pos+1);
      switch (indicator_flag)
      {
         case PROGRESS_MARQUEE :
            marquee_width = (int8)CORN::cstr_to_nat32(option.c_str(),10);
         break;
         case PROGRESS_ELAPSED   :
         case PROGRESS_REMAINING :
         case PROGRESS_ETC       :
            display_units.set_label(option.c_str());
         break;
         case PROGRESS_THROBBER  :
         case PROGRESS_BAR       :
         default                 : // step
            symbols = option;
            throbs  = symbols.length();
         break;

      } // switch
   }
   return indicator_flag || step_mode;
}
//_set_indicator____________________________________________________2017-05-19_/
Progress_display::Progress_display()
: parameter()
, steps_completion(0)
, steps_completed(0)
     //170519  parameter          (parameter_)
, start_time         ()
, frequency_achieved(0)
, marquee_offset     (0)
, specific           ()
{}
//_Progress_display:constructor_____________________________________2017-05-20_/
bool Progress_display::know_specific_string
(const std::string &new_specific)                                     cognition_
{  if (specific != new_specific)
   {  marquee_offset = 0;
      specific = new_specific;
   }
   return true;
}
//_know_specific_string_____________________________________________2017-05-20_/
bool Progress_display::step()                                     rectification_
{
   // NYI check frequency condition return true if condition met
   bool milestone = false;
   //float64 step_frequency = frequency;
   if (parameter.frequency_units.is_temporal())
   {
      CORN::Date_time_clad_64 now;
      CORN::Date_time_clad_64 next_time(last_time);
      next_time.mod_time().inc_seconds(parameter.frequency);
      milestone = (next_time > now);
      if (milestone)
      {
         last_time.set_datetime64(now.get_datetime64());
         steps_completed += parameter.frequency;
            // actually should take the seconds difference between
            // now and last_time
      }
   } else
   {
      frequency_achieved++;
      steps_completed++;
      milestone = frequency_achieved > parameter.frequency;
      if (milestone)
         frequency_achieved = 0;
   }
   if (milestone)
   {  //specific.clear();
      marquee_offset++;
   }
   return milestone;
}
//_step_____________________________________________________________2017-03-16_/
bool Progress_display::stop
(std::ostream &stream, char delimiter)                                stream_IO_
{
   if (parameter.indicators & PROGRESS_ELAPSED)
   {
      CORN::Date_time_clad_64 stop_time;
         // may need to be date time because the program run may span days
      float64 time_between_64
         = start_time.get_datetime64()
         - stop_time.get_datetime64();
      CORN::Date_time_clad_64 time_between(time_between_64);
      CORN::Seconds run_seconds
      = time_between.ref_time().get_seconds_after_midnight();                    //170815
      /*
       // need to implement a function to calculate seconds between datetime
         = (stop_time.get_seconds_after_midnight()
         - start_time.get_seconds_after_midnight());
      */
      stream << std::endl
             << "info: stop=" << stop_time.as_string() << std::endl
             << "info: elapsed=" << run_seconds << " seconds" << std::endl;
   }
   return true;
}
//_stop_____________________________________________________________2017-03-16_/
void Progress_display::stream_to(std::ostream &stream, char delimiter) stream_IO_
{

   if (parameter.step_mode) stream << parameter.symbols;                         //170519
   else
   {
   // Currently only the 'program specific' indicator mode has been implemented
   // Eventually provide the other modes.
   if (parameter.step_mode) return; // we have already printed the symbol in the step
   bool console_mode = (delimiter == '\r');
   if (console_mode)    stream << delimiter;

   // compose progress display line
   std::string composed; compose(composed);                                      //180921
   stream << composed;                                                           //180921
   //180924    stream << specific;
   if (!console_mode)   stream << delimiter;
   }
}
//_stream_to________________________________________________________2017-03-16_/
bool Progress_display::start()                                     modification_
{
   if (steps_completion == 0)
   {  // We cannot use indicators that need the number of items to be completed
      nat8 valid = parameter.indicators & 0xF;
      parameter.indicators = valid;
      nat8 new_indicator_count = 0;
      for (nat8 i = 0; i < 8; i++)
      {  if (parameter.indicator_order[i] && (!parameter.indicator_order[i] &valid))
            parameter.indicator_order[new_indicator_count++] = parameter.indicator_order[i];
      }
      parameter.indicator_count = new_indicator_count;
   }
   steps_completed = 0;
   return true;
}
//_start____________________________________________________________2017-03-16_/
nat32 Progress_display::compose(std::string &composed)                     const
{
   nat16                      max_width;
   #ifdef __linux__
   struct winsize w;
   ioctl(0, TIOCGWINSZ, &w);
   max_width = w.ws_col;
   #endif
   #if (defined(_WIN32)  || defined(_WIN64))
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
   max_width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
   #endif


//   nat8 marqee_width = parameter.marquee_width;

   if (parameter.indicators)
   {
      std::string field[8];
      nat8        field_width_max[8];
      nat8        field_width_min[8];
      for (int f = 0; f < 8; f++)
      {
         field_width_max[f] = 0;
         field_width_min[f] = 0;
      }
      CORN::Date_time_clad_64 now;
      nat8 separators = 0;
      nat8 widths = 0;
      if (parameter.indicators & PROGRESS_COUNT)      // 0
      {
         CORN::append_nat32_to_string(steps_completed,field[0]);
         widths += field_width_max[0] = field_width_min[0] = field[0].length();
         separators ++;
      }
      if (parameter.indicators & PROGRESS_ELAPSED)    // 1
      {  // in this case steps_completed is the number of seconds
         Time_second_cowl time_completed(steps_completed);
         /* NYI
         set the time format corresponding to the display_units

         */
         time_completed.append_to_string(field[1] _ISO_FORMAT_DATE_TIME);
         widths += field_width_max[1] = field_width_min[1] = field[1].length();
         separators ++;
      }
      if (parameter.indicators & PROGRESS_THROBBER)   // 2
      {
         nat8 throb = steps_completed % parameter.throbs;
         field[2] = parameter.symbols[throb];
         widths += field_width_max[1] = field_width_min[1] = 1;
         separators ++;
      }
      if (parameter.indicators & PROGRESS_MARQUEE)    // 3
      {
         widths += field_width_max[3] = std::min<nat8>
            (parameter.marquee_width,max_width);
         field_width_min[3] = 10;
         separators ++;
      }
      if (parameter.indicators & PROGRESS_PERCENT)    // 4
      {

         nat8 percent_complete = (nat8)
            (100.0 * (float32)steps_completed / (float32)(steps_completion));
         append_nat8_to_string(percent_complete,field[4],10,3,' ');
         //includes % sign which will be omitted when 100%
         if (percent_complete < 100)
            field[4] += '%';
         widths += field_width_max[4] = field_width_min[4] = 3;
         separators ++;
      }
      if (parameter.indicators & PROGRESS_BAR)        // 5
      {
         if (parameter.indicators & PROGRESS_MARQUEE)
            field_width_max[5] = 10;

         widths += field_width_max[5] = max_width;
         field_width_min[5] = 10;
         separators ++;
      }
      if (parameter.indicators & PROGRESS_REMAINING)  // 6
      {
         if (parameter.display_units.is_temporal())
         {
            //NYI need to convert time remaining sectons
            datetime64 elapsed64
               = last_time.get_datetime64()-start_time.get_datetime64();
            Date_time_clad_64 elapsed_time(elapsed64);
            CORN::Seconds elapsed_seconds
               = elapsed_time.ref_time().get_seconds_after_midnight();           //170815
            CORN::Seconds needed_seconds = (CORN::Seconds)
               ((float64)elapsed_seconds * (float64)steps_completion
                / (float64)steps_completed);
            CORN::Seconds remaining_seconds = needed_seconds - elapsed_seconds;
            Date_time_clad_64 remaining_time(remaining_seconds);
            // NYI set format according to the display_units
            remaining_time.append_to_string(field[6] _ISO_FORMAT_DATE_TIME);
         }
         else
         {
            nat32 steps_remaining = steps_completion - steps_completed;
            CORN::append_nat32_to_string(steps_remaining,field[6],10,0,0);
         }
         widths += field_width_max[6] = field_width_min[6] = field[6].length();
         separators ++;
      }
      if (parameter.indicators & PROGRESS_ETC)        // 7
      {
         if (parameter.display_units.is_temporal())
         {
            //NYI need to convert time remaining sectons
            datetime64 elapsed64
               = last_time.get_datetime64()-start_time.get_datetime64();
            Date_time_clad_64 elapsed_time(elapsed64);
            CORN::Seconds elapsed_seconds
               = elapsed_time.ref_time().get_seconds_after_midnight();           //170815
            CORN::Seconds needed_seconds = (CORN::Seconds)
               ((float64)elapsed_seconds * (float64)steps_completion
                / (float64)steps_completed);
            Date_time_clad_64 needed_time(needed_seconds);
            // NYI set format according to the display_units
            needed_time.append_to_string(field[7] _ISO_FORMAT_DATE_TIME);
         }
         widths += field_width_max[7] = field_width_min[7] = field[7].length();
         separators ++;
      }
      // readjust field widths (bar and marqee)
      // until minimum or total width is < maximum width.
      nat8 reductions = 8;
      while ((widths > max_width) && reductions)
      {  // shrink fields that can be shrunk
         reductions = 0;
         for (nat8 i = 0; i < parameter.indicator_count; i++)
         {  nat8 f = parameter.indicator_order[i];
            if (field_width_max[i] > field_width_min[i])
            {
               field_width_max[f]--;
               reductions++;
               widths--;
            }
         }
      } // while

      for (nat8 f = 8; f >= 0  && (widths > max_width); f--)
      {  // clear last field
         field[f].clear();
         widths -= field_width_max[f];
         field_width_max[f] = 0;
      }
      // CONTINUE here draw the indicators

      for (nat8 f = 0; f < 8 ; f++)
      {  // this has only been tested with single count
         composed.append(field[f]);
         if (field[f+1].length())
            composed.append("|"); // separator but shouldn't append to last NYI
      }




   }
   else
   {
      composed = specific;
      // we don't trim when not using indicators,
      // print whatever the application wants even
      // if exceeds the console width.
   }
   return composed.length();
}
//_compose__________________________________________________________2017-05-20_/
Progress_display &provide_progress_display_global()
{  if (!progress_display_global)
      progress_display_global = new Progress_display();
   return *progress_display_global;
}
//_provide_progress_display_global__________________________________2017-05-19_/
Progress_display *get_progress_display_global()
{  return progress_display_global;
}
//_get_progress_display_global______________________________________2017-05-19_/
}//_namespace CORN_____________________________________________________________/

