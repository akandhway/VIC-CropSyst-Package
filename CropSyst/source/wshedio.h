#ifndef wshedioH
#define wshedioH
#ifndef __linux
// Not yet implemented in Linux  (actually this was never really finished)
#include "corn/chronometry/date_types.hpp"
#include "corn/dynamic_array/dynamic_array_T.h"
#define Dynamic_float_array Dynamic_array<float32>
#include "corn/string/strconv.h"
#include "UED/library/database_file.h"
#include "common/soil/layering.h"
#include "UED/library/std_codes.h"
#include "corn/OS/directory_entry_name_concrete.h"
using namespace CORN;
extern char ID_str_buf[10];
// There are now two databases, one for input and one for output.
//______________________________________________________________________________
class Enumerated_database
: public UED::Database_file_indexed 
{
public:
   Enumerated_database(const char *i_filename);
protected:
   void get_XX_in_intervals
      (UED::Variable_code  var_code
      ,UED_units_code    units_code
      ,const CORN::Date_const &target_date
      ,Dynamic_float_array &run_XX_intervals);
   void set_XX_in_intervals
      (UED::Variable_code var_code
      ,UED_units_code    units_code
      ,const CORN::Date_const &target_date
      ,Dynamic_float_array &run_XX_intervals);
   void set_by_layer
      (UED::Variable_code var_code
      ,UED_units_code    units_code
      ,const CORN::Date_const  &target_date
      ,nat8               layer_count
      ,soil_sublayer_array_64(layer_data));
   void get_by_layer
      (UED::Variable_code var_code
      ,UED_units_code    units_code
      ,const CORN::Date_const  &target_date
      ,nat8              layer_count
      ,soil_sublayer_array_64(layer_data))                        modification_;
public:
   bool increment_from
      (Enumerated_database &contributing_database
      ,UED::Variable_code   from_var_code
      ,UED::Variable_code   to_var_code
      ,UED_units_code       units_code
      ,float32              fraction_contributed
      ,float64             &total_received);
      // Every single record for the contributing_database is read,
      // the values are multiplied by the fraction_contributed
      // and added to the corresponding values of records in this database.
      // records will created if there is no corresponding record.
   void get_runon_in_intervals
      (const CORN::Date_const &target_date
      ,Dynamic_float_array &runon_intervals);
   void get_runoff_in_intervals
      (const CORN::Date_const &target_date
      ,Dynamic_float_array &runoff_intervals);
   void set_runoff_in_intervals
      (const CORN::Date_const &target_date
      ,Dynamic_float_array &runoff_intervals);
   void get_lateral_flow_in_by_layer
      (const CORN::Date_const &target_date
      , nat8 num_layers   // sublayer
      , soil_sublayer_array_64(layer_data));
   void set_lateral_flow_out_by_layer
      (const CORN::Date_const &target_date
      , nat8 num_layers   // sublayer
      , soil_sublayer_array_64(layer_data));
   virtual bool initialize();
};
//_Enumerated_database_________________________________________________________/
class Cell_outlet_database
: public Enumerated_database
{public:
   Cell_outlet_database();
   virtual bool initialize();
   void set_outlet_in_intervals
      (const CORN::Date_const &target_date
      ,Dynamic_float_array &runoff_intervals);
};
//_Cell_outlet_database________________________________________________________/
class Channel_runin_database
: public Enumerated_database
{
   CORN::OS::Directory_name_concrete buffer;
public:
   Channel_runin_database
      (const CORN::OS::Directory_name &output_watershed_channel_dir
      ,const char *channel_ID_str);
   virtual bool initialize();
};
//_Channel_runin_database______________________________________________________/
#endif
//wshedio.h

#endif
