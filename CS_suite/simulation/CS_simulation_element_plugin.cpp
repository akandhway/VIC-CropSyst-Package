#include "CS_suite/simulation/CS_simulation_element_plugin.h"
#if (defined(_WIN32)  || defined(_WIN64) || defined(_Windows))
#define load_function_handle(library_hdl,function_name) GetProcAddress(library_hdl,function_name)
#endif
#ifdef __unix
#define load_function_handle(library_hdl,function_name) dlsym(library_hdl,function_name)
#include <dlfcn.h>
#endif

// DLL plugin references
// https://en.wikipedia.org/wiki/Dynamic_loading
// http://eli.thegreenplace.net/2012/08/24/plugins-in-c

// See also making a plugin with C++ (currently using simple C functions).
// http://www.cplusplus.com/articles/48TbqMoL/
// http://www.drdobbs.com/cpp/building-your-own-plugin-framework-part/204202899?pgno=4

namespace CS
{
//______________________________________________________________________________
Simulation_element_plugin::Simulation_element_plugin
(const CORN::OS::File_name    &DL_filename_
,const CORN::Container        *inspectors_
,const CORN::date32           &simdate_raw_)                                     //170524
: Simulation_element_abstract(simdate_raw_)
, DL_filename(DL_filename_)
, inspectors(inspectors_)
, library_handle(0)
{
}
//______________________________________________________________________________
Simulation_element_plugin::~Simulation_element_plugin()
{
   #if (defined(_WIN32)  || defined(_WIN64))
   FreeLibrary
   #endif
   #ifdef __unix
   dlclose
   #endif
      (library_handle);
}
//______________________________________________________________________________
bool Simulation_element_plugin::is_valid()                          affirmation_
{  bool valid = false;
   valid =
      (library_handle   != NULL)
    &&(DL_initialize  != NULL)
    &&(DL_start       != NULL)
    &&(DL_start_year  != NULL)
    &&(DL_start_day   != NULL)
    &&(DL_process_day != NULL)
    &&(DL_end_day     != NULL)
    &&(DL_end_year    != NULL)
    &&(DL_stop        != NULL);
   return valid;
}
//______________________________________________________________________________
bool Simulation_element_plugin::load()                           initialization_
{
   library_handle =
   #if (defined(_WIN32)  || defined(_WIN64)|| defined(_Windows))
      #ifdef __GNUC__
      LoadLibrary(DL_filename.w_str());
      #else
      LoadLibrary(DL_filename.c_str());
      #endif
   #endif
   #ifdef __unix
      dlopen(DL_filename.c_str(), RTLD_LAZY);
   #endif
   DL_initialize   = (bool_func_parameterless)load_function_handle(library_handle,"initialize");
   DL_start        = (bool_func_parameterless)load_function_handle(library_handle,"start");
   DL_start_year   = (bool_func_parameterless)load_function_handle(library_handle,"start_year");
   DL_start_day    = (bool_func_parameterless)load_function_handle(library_handle,"start_day");
   DL_process_day  = (bool_func_parameterless)load_function_handle(library_handle,"process_day");
   DL_end_day      = (bool_func_parameterless)load_function_handle(library_handle,"end_day");
   DL_end_year     = (bool_func_parameterless)load_function_handle(library_handle,"end_year");
   DL_stop         = (bool_func_parameterless)load_function_handle(library_handle,"stop");
   return is_valid();
}
//______________________________________________________________________________
bool Simulation_element_plugin::initialize()                     initialization_
{  return is_valid() && DL_initialize();
}
//______________________________________________________________________________
bool Simulation_element_plugin::start()                            modification_
{  return DL_start(); }
//______________________________________________________________________________
bool Simulation_element_plugin::start_year()                       modification_
{  return DL_start_year(); }
//______________________________________________________________________________
bool Simulation_element_plugin::start_day()                        modification_
{  return DL_start_day(); }
//______________________________________________________________________________
bool Simulation_element_plugin::process_day()                      modification_
{  return DL_process_day(); }
//______________________________________________________________________________
bool Simulation_element_plugin::end_day()                          modification_
{  return DL_end_day(); }
//______________________________________________________________________________
bool Simulation_element_plugin::end_year()                         modification_
{  return DL_end_year(); }
//______________________________________________________________________________
bool Simulation_element_plugin::stop()                             modification_
{  return DL_stop(); }
//______________________________________________________________________________
}// namespace CS

