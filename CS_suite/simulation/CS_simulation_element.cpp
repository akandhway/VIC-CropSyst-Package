#include "CS_suite/simulation/CS_simulation_element.h"
#include "CS_suite/simulation/CS_event_scheduler.h"
//______________________________________________________________________________
namespace CS
{
//______________________________________________________________________________
Simulation_element::Simulation_element()
:CORN::Item()
{}
//______________________________________________________________________________
Simulation_element_abstract::Simulation_element_abstract
(const CORN::date32 &simdate_raw_)                                               //170524
: Simulation_element()
, simdate_raw     (simdate_raw_)                                                 //170524
, simdate         (simdate_raw)                                                  //170524
, event_scheduler (0)
, status          (0)
{}
//_Simulation_element_abstract:constructor__________________________2015-06-08_/
Simulation_element_abstract::~Simulation_element_abstract()
{
   #if (CS_VERSION==5)
   get_emanators().forget((int32)this);
   #endif
}
//_Simulation_element_abstract:destructor___________________________2015-05-21_/
bool Simulation_element_abstract::is_valid()       affirmation_ { return true; } //{ return event_scheduler ? event_scheduler->is_valid()    : true; }
bool Simulation_element_abstract::start_day()                      modification_
   { return event_scheduler ? event_scheduler->start_day()   : true; }
bool Simulation_element_abstract::process_day()                    modification_
   { return event_scheduler ? event_scheduler->process_day() : true; }
//_Simulation_element_abstract::process_day_________________________2015-09-30_/
nat32 Simulation_element_abstract::take_events
(modifiable_ CORN::Container &events)                              modification_
{  nat32 taken_count
   = (event_scheduler)
   ?  event_scheduler->take_events(events)
   : 0;
   return taken_count;
}
//_take_events______________________________________________________2015-09-30_/
bool Simulation_element_abstract::start()                          modification_
{  bool started = true;
   if (event_scheduler) started &= event_scheduler->start();
   return started;
}
//_Simulation_element_abstract::start_______________________________2016-06-06_/
bool Simulation_element_abstract::start_year()                    modification_
{  bool started = true;
   if (event_scheduler) started &= event_scheduler->start_year();
   status &= ~(STATUS_ENDED_YEAR);                                               //180914
   return started;
}
//_Simulation_element_abstract::start_year__________________________2016-06-06_/
bool Simulation_element_abstract::end_year()                       modification_
{  bool ended = true;
   ended &= event_scheduler ? event_scheduler->end_year()    : true;
   status |= STATUS_ENDED_YEAR;                                                  //180914
   return ended;
}
//_Simulation_element_abstract::end_year____________________________2016-06-22_/
bool Simulation_element_abstract::end_day()                        modification_
{  bool ended = true;
   if (event_scheduler) ended &= event_scheduler->end_day();
   return ended;
}
//_Simulation_element_abstract::end_day_____________________________2016-06-06_/
bool Simulation_element_abstract::stop()                           modification_
{  bool stopped = true;
   // In the case where the simulation period ends before the end of the year
   // call end year to finish any yearly tallies.
   if (! status & STATUS_ENDED_YEAR)                                             //180914
      stopped &= end_year();                                                     //170413
   if (event_scheduler) stopped &= event_scheduler->stop();
   return stopped;
}
//_Simulation_element_abstract::stop________________________________2016-06-06_/
Simulation_element_composite::Simulation_element_composite
(const CORN::date32 &simdate_raw_)                                               //170524
:Simulation_element_abstract(simdate_raw_)                                       //170524
{}
//_Simulation_element_composite:constructor_________________________2015-06-08_/
bool Simulation_element_composite::initialize()                  initialization_
{  bool initialized = Simulation_element_abstract::initialize();
   if (!initialized) std::clog << "Simulation_element_composite::initialize() Simulation_element_abstract::initialize failed " << std::endl;
   FOR_EACH_IN_PARALLEL(sub_element,Simulation_element,sub_elements,each_element)
   {  initialized &= sub_element->initialize();
      if (!initialized) std::clog << "Simulation_element_composite::initialize() sub_element->initialize failed " << std::endl;
   } FOR_EACH_END(each_element)
   return initialized;
}
//_Simulation_element_composite::initialize_________________________2015-04-29_/
bool Simulation_element_composite::start()                         modification_
{  bool started = Simulation_element_abstract::start();
   FOR_EACH_IN_PARALLEL(sub_element,Simulation_element,sub_elements,each_element)
      started &= sub_element->start();
   FOR_EACH_END(each_element)
   return started;
}
//_Simulation_element_composite::start_________________________________________/
bool Simulation_element_composite::start_year()                    modification_
{   bool started = Simulation_element_abstract::start_year();
   // Balancer start_year() must be done before start_year() to
   // make sure accumulators are clear for the start of the year.
   // Components may then reset with any initial values.
   FOR_EACH_IN_PARALLEL(sub_element,Simulation_element,sub_elements,each_element)
   {  started &= sub_element->start_year();
   } FOR_EACH_END(each_element)
   return started;
}
//_Simulation_element_composite::start_year_________________________2013-06-18_/
bool Simulation_element_composite::end_year()                      modification_
{  bool ended = true;
   // Balancer end_year() must be done after end_year() to make sure the components have added any
   // last minute accumulations.
   FOR_EACH_IN_REVERSE_PARALLEL(sub_element,Simulation_element,sub_elements,each_element)
   {  ended &= sub_element->end_year();
   } FOR_EACH_END(each_element)
   return Simulation_element_abstract::end_year() && ended;                      //170528
}
//_Simulation_element_composite::end_year___________________________2013-06-18_/
bool Simulation_element_composite::start_day()                     modification_
{
   FOR_EACH_IN(pot_term_element,Simulation_element,sub_elements,each_term)       //180914
   {  if (pot_term_element->is_terminated())                                     //180914
         terminated_elements.take(each_term->detach_current());                  //180914
   } FOR_EACH_END(each_term)                                                     //180914
   bool started = Simulation_element_abstract::start_day();
   FOR_EACH_IN_PARALLEL(sub_element,Simulation_element,sub_elements,each_element)
      started &= sub_element->start_day();
   FOR_EACH_END(each_element)
   return started;
}
//_Simulation_element_composite::start_day__________________________2013-06-18_/
bool Simulation_element_composite::process_day()                   modification_
{  bool processed = Simulation_element_abstract::process_day();
   FOR_EACH_IN_PARALLEL(sub_element,Simulation_element,sub_elements,each_element)
      processed &= sub_element->process_day();
   FOR_EACH_END(each_element)
   return processed;
}
//_Simulation_element_composite::process_day________________________2013-06-18_/
bool Simulation_element_composite::end_day()                       modification_
{  bool ended = true;
   FOR_EACH_IN_REVERSE_PARALLEL
   (sub_element,Simulation_element,sub_elements,each_element)
      ended &= sub_element->end_day();
   FOR_EACH_END(each_element)
   return Simulation_element_abstract::end_day() && ended;                       //170528
}
//_Simulation_element_composite::end_day____________________________2013-06-18_/
bool Simulation_element_composite::stop()                          modification_
{  bool stopped = true;
   FOR_EACH_IN_REVERSE_PARALLEL
   (sub_element,Simulation_element,sub_elements,each_element)
      stopped &= sub_element->stop();
   FOR_EACH_END(each_element)
   return Simulation_element_abstract::stop() && stopped;                        //170528
}
//_stop_____________________________________________________________2014-07-14_/
RENDER_INSPECTORS_DEFINITION(Simulation_element_abstract)
{
   #ifdef CS_OBSERVATION
   KNOW_EMANATOR(simdate,false);                                                 //170319_160626
   return &emanator;
   #else
   return 0;
   #endif
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2015-08-21_/
RENDER_INSPECTORS_DEFINITION(Simulation_element_composite)
{
   #ifdef CS_OBSERVATION
   Emanator *emanator = Simulation_element_abstract
      ::render_inspectors(inspectors,context,instance);
   if (emanator)                                                                 //160616
   FOR_EACH_IN_PARALLEL(sub_element,Simulation_element,sub_elements,each_elem)
      sub_element->render_inspectors(inspectors,emanator,instance);              //160616
   FOR_EACH_END(each_elem)
   return emanator;
   #else
   return 0;
   #endif
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2015-08-19_/
}//_namespace_CS_______________________________________________________________/

