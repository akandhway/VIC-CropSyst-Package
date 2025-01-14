#include "corn/string/strconv.h"
#include "UED/analysis.h"
//______________________________________________________________________________
UED_statistical_data_set::UED_statistical_data_set
(UED_indexed_database &database_ // The database to draw the data from
,UED_time_search       analysis_period_
,UED_variable_code     variable_code_
,UED_units_code        units_code_
)
:UED_statistical_data_set()
,UED_smart_variable_code(variable_code)
,CORN::Units_clad(units_code)
{
   // Creates the statistical data set, loading it with
   // the values from all records
   database_.goto_BOF();   // Goto beginning of file
   for (UED_data_record *data_rec = goto_next_data_record()
         ;data_rec
         ;data_rec = goto_next_data_record())
   {  // For each data record:
      if (data_rec->get_variable_code() == variable_code_)
      {  // If dataset variable code matches
         // Warning this only may only work with data set records.
         // Need to check if
         CORN_dynamic_float_array &data_values = data_rec->get_data_values();
         for (uint16 i = 0; i < data_rec->get_num_values(); i++)
         {  // For each item
            CORN_date_time date_of_i = data_rec.get_date_time_for_index(i);
            // If the date corresponding to the value at index i
            // is within the analysis query period then added it to the dataset.
            if (analysis_period_.match(date_of_i))
               append(data_values.get_no_resize(i));
} } } }
//_UED_statistical_data_set:constructor________________________________________/
