#include "project/GIS/pat.h"
#include "corn/math/compare.hpp"

const char *Polygon_attribute_data_record::get_record_soil_filename(const char *template_soil_file_name)
{  // Extension will already be set in template (User does not have to use .SIL)
   CORN::OS::Directory_name_concrete fname_for_path(template_soil_file_name);
   CORN::OS::Directory_name_concrete fname_path(fname_for_path,CORN::OS::Directory_entry_name::include_qualification_designated);
   CORN::OS::File_name_concrete soil_file_name(fname_path,sim_params.soil_code.c_str(),
        #if ((CROPSYST_VERSION >0) && (CROPSYST_VERSION < 5))
        "sil"
        #else
        "CS_soil"
        #endif
        );
   return filename_buffer.c_str();
}
//_get_record_soil_filename____________________________________________________/
float Polygon_attribute_table::get_total_polygon_area()
{  if (!tallied) tally();
   return total_polygon_area;
}
//_get_total_polygon_area______________________________________________________/
void Polygon_attribute_table::tally()
{  total_polygon_area = 0;
   total_polygon_run_count = 0;
   Polygon_attribute_data_record PA_DR
      (polygon_ID_field.c_str() // The name of the field corresponding to the xxx_ID polygon ID field
      ,soil_field.c_str()       // The name of the field corresponding to the mapped soils
      ,steepness_field.c_str()  // The name of the field corresponding to the steepness
         ,uniform_sand_field.c_str()
      ,uniform_clay_field.c_str()
      ,soil_depth_field.c_str()
      ,init_PAW_field.c_str()   // The name of the field corresponding to the initial plant available water
      ,init_NO3_field.c_str()   // The name of the field corresponding to the initial NO3
      ,weather_field.c_str()
      ,rotation_field.c_str()
      ,recalibration_field.c_str()
      ,water_table_field.c_str());
   bool at_eof = !goto_BOF();
   while (!at_eof)
   {  get(PA_DR);
//unused      int polygon_ID = PA_DR.sim_params.polygon_ID;
      int record_num  // 1 based
      = get_current_index() + 1;
#define equiv_polygon_ran -1
      if (!record_equivalences // <- In the case of watershed, all cells are run so equivelences are not used
          || (polygon_equivalences[record_num] >= equiv_polygon_ran))
      {  float32 area = PA_DR.area;
         if (area > 0 )
         {  total_polygon_run_count++;
            total_polygon_area += area;
         }
      }
      at_eof = !goto_next();
   }
   tallied = true;
}
//_tally_______________________________________________________________________/
Polygon_simulation_parameters::Polygon_simulation_parameters(const Polygon_simulation_parameters &copy_from)
: polygon_ID_field   ("ID")
, soil_field         ("soil" )
, steepness_field    ("steepness")
, uniform_sand_field ("sand")                                                    //050630
, uniform_clay_field ("clay")                                                    //050630
, soil_depth_field   ("depth")                                                   //050630
, init_PAW_field     ("init_PAW")
, init_NO3_field     ("init_NO3")
, weather_field      ("weather" )
, rotation_field     ("rotation" )
, recalibration_field("recalibr" )                                               //060411
, water_table_field  ("watertab" )                                               //060706
, soils_mapped                (copy_from.soils_mapped)
, uniform_soil_profile_mapped (copy_from.uniform_soil_profile_mapped)            //050531
, soil_sand_mapped            (copy_from.soil_sand_mapped)
, soil_clay_mapped            (copy_from.soil_clay_mapped)                       
, soil_depth_mapped           (copy_from.soil_depth_mapped)                      
, weather_mapped              (copy_from.weather_mapped)                         
, rotations_mapped            (copy_from.rotations_mapped)
, recalibration_mapped        (copy_from.recalibration_mapped)                   //060411
, water_table_mapped        (copy_from.water_table_mapped)                       //060706
, steepness_mapped            (copy_from.steepness_mapped)                       
, aspect_mapped               (copy_from.aspect_mapped)                          
, init_PAW_mapped             (copy_from.init_PAW_mapped)                        
, init_NO3_mapped             (copy_from.init_NO3_mapped)                        
, polygon_ID         (copy_from.polygon_ID)                                      
, weather_code       (copy_from.weather_code)                                    
, rotation_code      (copy_from.rotation_code)
, recalibration_code (copy_from.recalibration_code)                              //060411
, water_table_code (copy_from.water_table_code)                                  //060706
, soil_code          (copy_from.soil_code)                                       
, sow_DOY            (copy_from.sow_DOY)                                         
, steepness          (copy_from.steepness)                                       
, uniform_sand       (copy_from.uniform_sand)                                    
, uniform_clay       (copy_from.uniform_clay)                                    
, soil_depth_cm      (copy_from.soil_depth_cm)                                   
, init_PAW           (copy_from.init_PAW)                                        
, init_NO3           (copy_from.init_NO3)                                        
, MUID               (copy_from.MUID)
, SEQNUM            (copy_from.SEQNUM)
, COMPNAME           (copy_from.COMPNAME)                                        //051115
, site_aspect_N0_E90 (copy_from.site_aspect_N0_E90) // Souther exposure          //020611
, combine_similar_sequences(copy_from.combine_similar_sequences)                 //051115
{ }
//_Polygon_simulation_parameters____________________________________2005-06-30_/
Polygon_simulation_parameters::Polygon_simulation_parameters(bool expect_everything)
: polygon_ID_field   ("ID")
, soil_field         ("soil" /*LABEL_soil*/)
, steepness_field    ("steepness"/*LABEL_steepness*/)
, uniform_sand_field ("sand")                                                    //050630
, uniform_clay_field ("clay")                                                    //050630
, soil_depth_field   ("depth")                                                   //050630
, init_PAW_field     ("init_PAW"/*LABEL_init_PAW*/)
, init_NO3_field     ("init_NO3"/*LABEL_init_NO3*/)
, weather_field      ("weather" /*LABEL_weather*/)
, rotation_field     ("rotation" /*LABEL_rotation*/)
, recalibration_field("recalib" )                                                //060411
, water_table_field  ("watertab" )                                               //060706
, soils_mapped       (expect_everything)
, uniform_soil_profile_mapped(expect_everything)                                 //050531
, soil_sand_mapped   (expect_everything)
, soil_clay_mapped   (expect_everything)
, soil_depth_mapped  (expect_everything)
, weather_mapped     (expect_everything)
, rotations_mapped   (expect_everything)
, recalibration_mapped(expect_everything)                                        //060411
, water_table_mapped(expect_everything)                                          //060706
, steepness_mapped   (expect_everything)
, aspect_mapped      (expect_everything)
, init_PAW_mapped    (expect_everything)
, init_NO3_mapped    (expect_everything)
, polygon_ID         (-1) // default offsite
, weather_code       ("none")
, rotation_code      ("none")
, recalibration_code ("none")                                                    //060411
, water_table_code ("none")                                                      //060706
, soil_code          ("none")
, sow_DOY            (0)
, steepness          (0)
, uniform_sand       (0)
, uniform_clay       (0)
, soil_depth_cm         (0)
, init_PAW           (0)
, init_NO3           (0)
, MUID               ("")
, SEQNUM             (0)
, site_aspect_N0_E90 (180) // Souther exposure                                   //020611
, COMPNAME           ("")                                                        //051115
, combine_similar_sequences(false)                                               //051115
{}
//_Polygon_simulation_parameters_______________________________________________/
Polygon_simulation_parameters::Polygon_simulation_parameters
(const char *_polygon_ID_field
,const char *_soil_field
,const char *_steepness_field
,const char *_uniform_sand_field                                                 //050630
,const char *_uniform_clay_field                                                 //050630
,const char *_soil_depth_field                                                   //050630
,const char *_init_PAW_field                                                     
,const char *_init_NO3_field                                                     
,const char *_weather_field                                                      
,const char *_rotation_field
,const char *_recalibration_field                                                //060411
,const char *_water_table_field                                                  //060706
)
: polygon_ID_field   (_polygon_ID_field)                                         
, soil_field         (_soil_field)                                               
, steepness_field    (_steepness_field)
, uniform_sand_field (_uniform_sand_field)                                       //050630
, uniform_clay_field (_uniform_clay_field)                                       //050630
, soil_depth_field   (_soil_depth_field)                                         //050630
, init_PAW_field     (_init_PAW_field)                                           
, init_NO3_field     (_init_NO3_field)                                           
, weather_field      (_weather_field)                                            
, rotation_field     (_rotation_field)
, recalibration_field(_recalibration_field)                                      //060411
, water_table_field  (_water_table_field)                                        //060706
, soils_mapped       (true)
, uniform_soil_profile_mapped(true)                                              //050531
, soil_sand_mapped   (true)                                                      
, soil_clay_mapped   (true)                                                      
, soil_depth_mapped  (true)                                                      
, weather_mapped     (true)                                                      
, rotations_mapped   (true)
, recalibration_mapped   (true)                                                  //060411
, water_table_mapped   (true)                                                    //060706
, steepness_mapped   (true)                                                      
, aspect_mapped      (true)                                                      
, init_PAW_mapped    (true)                                                      
, init_NO3_mapped    (true)
, polygon_ID         (-1) // default offsite                                     
, weather_code       ("none")                                                    
, rotation_code      ("none")
, recalibration_code      ("none")                                               //060411
, water_table_code      ("none")                                                 //060706
, soil_code          ("none")                                                    
, sow_DOY            (0)                                                         
, steepness          (0)                                                         
, uniform_sand       (0)                                                         
, uniform_clay       (0)                                                         
, soil_depth_cm         (0)                                                      
, init_PAW           (0)
, init_NO3           (0)
, MUID               ("")                                                        
, SEQNUM             (0)
, site_aspect_N0_E90(180) // Souther exposure                                    //020611
, COMPNAME           ("")                                                        //051115
, combine_similar_sequences(false)                                               //051115
{}
//_Polygon_simulation_parameters_______________________________________________/
Polygon_attribute_data_record::Polygon_attribute_data_record
(const char *_polygon_ID_field
,const char *_soil_field
,const char *_steepness_field
,const char *_uniform_sand_field                                                 //050630
,const char *_uniform_clay_field                                                 //050630
,const char *_soil_depth_field                                                   //050630
,const char *_init_PAW_field
,const char *_init_NO3_field
,const char *_weather_field
,const char *_rotation_field
,const char *_recalibration_field                                                //060411
,const char *_water_table_field)                                                 //060706
: Data_record("PAT")
, sim_params                                                                     //050630
   (_polygon_ID_field
   ,_soil_field
   ,_steepness_field
   ,_uniform_sand_field                                                          //050630
   ,_uniform_clay_field                                                          //050630
   ,_soil_depth_field                                                            //050630
   ,_init_PAW_field
   ,_init_NO3_field
   ,_weather_field
   ,_rotation_field
   ,_recalibration_field                                                         //060411
   ,_water_table_field)                                                          //060706
, X                  (0)                                                         //050619
, Y                  (0)                                                         //050619
{}
//_Polygon_attribute_data_record____________________________________2003-02-20_/
bool Polygon_attribute_data_record::expect_structure(bool for_write)
{  bool expected = Data_record::expect_structure(for_write);                     //161025
   structure_defined = false;                                                    //120314
   expected &= sim_params.setup_structure(*this,for_write);                      //170808
   expect_float32    ("AREA"                   ,area);
   expect_int32      ("X"                      ,X);                              //051117
   expect_int32      ("Y"                      ,Y);                              //051117
   structure_defined = true;                                                     //120314
   return expected;                                                              //161025
}
//_expect_structure____________________________________________________________/
bool Polygon_simulation_parameters::setup_structure(Data_record &data_rec,bool for_write/* = false*/ ) modification_
{
   data_rec.expect_int32 (polygon_ID_field .c_str(),polygon_ID);
   if (soils_mapped)          data_rec.expect_string     (soil_field       .c_str(),soil_code,30);
   if (weather_mapped)        data_rec.expect_string     (weather_field    .c_str(),weather_code,30);
   if (rotations_mapped)      data_rec.expect_string     (rotation_field   .c_str(),rotation_code,30);
   if (recalibration_mapped)  data_rec.expect_string     (recalibration_field   .c_str(),recalibration_code,30);  //060411
   if (water_table_mapped)  data_rec.expect_string     (water_table_field   .c_str(),water_table_code,30);        //060706
   data_rec.expect_uint16     ("sow_DOY"                 ,sow_DOY,10);                                            //050630
   data_rec.set_current_section("site");                                                                          //010520
   if (init_PAW_mapped)       data_rec.expect_float32    (init_PAW_field   .c_str(),init_PAW);
   if (init_NO3_mapped)       data_rec.expect_float32    (init_NO3_field   .c_str(),init_NO3);
   if (aspect_mapped)         data_rec.expect_float32    ("aspect"                 ,site_aspect_N0_E90);          //010520
      // The label is currently constant but may need to be user selectable
   if (steepness_mapped)      data_rec.expect_float32    (steepness_field  .c_str(),steepness);
   data_rec.set_current_section("soil_profile");                                 //050531
   if (uniform_soil_profile_mapped)                                              //050531
   {  data_rec.expect_float32    (uniform_sand_field.c_str(),uniform_sand);      //050630
      data_rec.expect_float32    (uniform_clay_field.c_str(),uniform_clay);      //050630
      data_rec.expect_float32    (soil_depth_field.c_str()  ,soil_depth_cm);     //050630
   }
   data_rec.set_current_section("STATSGO");
   data_rec.expect_string     ("MUID"                   ,MUID,30);               //030224
   data_rec.expect_uint16     ("SEQNUM"                 ,SEQNUM,10);             //030224
   data_rec.expect_string     ("COMPNAME"               ,COMPNAME,30); // This is used for reference to check idential scenarios when combining sequences mode enabled. 051116
   //050921 May be needed for LADSS since I replaced GIS_table_data_record
   if (!for_write)                                                               //050921
      data_rec.expect_string("location",weather_code,30);                        //050921
   return true;
}
//_setup_structure_____________________________________________________________/
sint8 Polygon_simulation_parameters::compare(const Polygon_simulation_parameters &other) const
{  int result = 0;
//NYI     result = sim_template_fname.CORN::Ustring::compare(other.sim_template_fname);
   if (result == 0)   {  result = rotation_code.compare(other.rotation_code);
   if (result == 0)   {  result = recalibration_code.compare(other.recalibration_code);
   if (result == 0)   {  result = water_table_code.compare(other.water_table_code);  //067060
   if (result == 0)   {  result = weather_code.compare(other.weather_code);
   if (result == 0)   {  result = soil_code.compare(other.soil_code);
   if (result == 0)   {  result = MUID.compare(other.MUID);
   if (result == 0)   {  result = CORN::compare<float32>(site_aspect_N0_E90,other.site_aspect_N0_E90) ;
   if (result == 0)   {  result = CORN::compare<uint16>(sow_DOY,other.sow_DOY);
   if (result == 0)   {  result = CORN::compare<float32>(init_NO3,other.init_NO3);
   if (result == 0)   {  result = CORN::compare<float32>(init_PAW,other.init_PAW);
   if (result == 0)   {  result = CORN::compare<float32>(steepness,other.steepness) ;
   if (result == 0)   {  result = CORN::compare<float32>(uniform_sand,other.uniform_sand);
   if (result == 0)   {  result = CORN::compare<float32>(uniform_clay,other.uniform_clay) ;
   if (result == 0)   {  result = CORN::compare<float32>(soil_depth_cm,other.soil_depth_cm);  //050921
   if (result == 0)   {  result = COMPNAME.compare(other.COMPNAME);              //050921
   if (result == 0)   {  result = (combine_similar_sequences) ? 0 : CORN::compare<uint16>(SEQNUM,other.SEQNUM) ;  //120408
   } } } } } } } } } } } } } } } }
   return (sint8)result;
}
//_compare_____________________________________________________________________/

