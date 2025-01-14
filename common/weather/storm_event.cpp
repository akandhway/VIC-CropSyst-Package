#include "common/weather/storm_event.h"
#include "UED/library/std_codes.h"
//______________________________________________________________________________
Storm_event_creation_layout::Storm_event_creation_layout
(Record_code            storm_event_UED_record_type_
,Units_code             storage_units_
,Units_code             interval_time_length_
,CORN::Quality_code     default_attribute_code_)
: UED::Data_record_creation_layout
   (storm_event_UED_record_type_
   ,UED::STD_VC_precipitation.get()
   ,storage_units_
   ,interval_time_length_
   ,UT_arbitrary_datetime
   ,default_attribute_code_)
{}
//_Storm_event_creation_layout:constructor_____________________________________/
Storm_event_data_record::Storm_event_data_record
(Record_code            storm_event_UED_record_type_
,datetime64             date_time_
,Units_code             storage_units_
,Units_code             interval_time_length_
,CORN::Quality_code     default_attribute_code_)
: UED::Binary_data_record_clad
   (Storm_event_creation_layout
      (storm_event_UED_record_type_,storage_units_
      ,interval_time_length_,default_attribute_code_)
   ,date_time_
   ,true) //storm events a are all generated and large data set so I will allow float16
{}
//_Storm_event_data_record:constructor_________________________________________/
