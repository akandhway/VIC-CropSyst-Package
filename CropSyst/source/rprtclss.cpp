#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
// Obsolete in V5 which outputs only UED files
#  include "options.h"
#  include "static_phrases.h"
#  include "rprtclss.h"

//______________________________________________________________________________
Report_writer::Report_writer
(const char *filename                                                            //030110
,const Report_options &options_to_copy                                           //981203
,uint16 maximum_rows)                                                            //981203
:report_options(options_to_copy)                                                 //001114
/* 200122
#ifdef SOILR
,std::ofstream(filename)                                                         //030110
#else
*/
 ,BIFF234_File(filename,0,maximum_rows,0,256                                     //030110
 ,BIFF4  // 090626 the BIFF234_File class is actually writing cell records in BIFF2 format for BIFF4
         // Excel and OpenOffice is accepting this, but I probably should switch completely to BIFF4
         // I probably won't do this because this class will not be used in version 5
 ) // WARNING Need to determine the number of colums
//200122 #endif
{}
//_Report_writer:constructor___________________________________________________/
void Report_writer::write_header
(const std::string &line1,const std::string &line2,const std::string &line3,const std::string &line4)  //981211x*
{
   const char *accum_mode = TL_Continuous;
   switch (report_options.accumulation_mode_clad.get())
   { case GROWTH_PERIOD_ACCUM : accum_mode = TL_Growth_period; break;
     case ANNUAL_ACCUM        : accum_mode = TL_Annual;        break;
     case CONTINUOUS_ACCUM    : accum_mode = TL_Continuous;    break;
     case FALLOW_ACCUM        : accum_mode = TL_Fallow;        break;
   }
   std::string line1_accum_mode(line1);
   line1_accum_mode.append(" ");
   line1_accum_mode.append(TL_Accumulation_mode);
   line1_accum_mode.append(":");
   line1_accum_mode.append(accum_mode);
//   if (water_bal_accum)
//      line1 = line1 + " " + "Water balance accum"; // no translation yet remove_tilde((TL_Water__b_alance_accum));
  row = 0; col = 0; write_string_aligned(CSS_justify_left,line1);
/*200122
#ifdef SOILR
   (*this) << '\n';
#endif
*/
   row = 1; col = 0; write_string_aligned(CSS_justify_left,line2);
/*
#ifdef SOILR
   (*this) << '\n';
#endif
*/
   row = 2; col = 0; write_string_aligned(CSS_justify_left,line3);
/* 200122
#ifdef SOILR
   (*this) << '\n';
#endif
*/
   row = 3; col = 0; write_string_aligned(CSS_justify_left,line4);
/* 200122
#ifdef SOILR
   (*this) << '\n';
#endif
*/
   row = 4; col = 0;
/* 200122
#ifdef SOILR
   (*this) << '\n';
#endif
*/
    row = 0; col = 2; write_string_aligned(CSS_justify_left,"Version:");
    row = 0; col = 3;
    write_integer(CS_VERSION,2);                                                 //180626
    //180626 write_string_aligned(CSS_justify_left,CS_VERSION_STRING);

    row = 1; col = 2; write_string_aligned(CSS_justify_left,"Build date:");

   CORN::Date_format BCC_compiler_DATE_format(D_MDY,D_YYYY|D_Mmm,' ');           //180626
   CORN::Date_clad_32 version_program_date_curr;                                 //180822
   version_program_date_curr.set_c_str(__DATE__,&BCC_compiler_DATE_format);      //180626
   row = 1; col = 3; write_string_aligned(CSS_justify_left,version_program_date_curr.as_string());
   CORN::Date_clad_32 run_date;
   row = 2; col = 2; write_string_aligned(CSS_justify_left,"Run date:");
   row = 2; col = 3; write_string_aligned(CSS_justify_left,run_date.as_string());
   row = 4; col = 0;
}
//_write_header________________________________________________________________/
void Report_writer::write_column_header
(report_var_layout_with_graph *var_layout
,int top_line_row
,std::string top_line)
{
   std::string buffer;
   row = top_line_row;

   for (int chi = 1; chi <= 4; chi++)
   {     std::string full_header(top_line);
         full_header.append(" ");
         full_header.append(var_layout && var_layout->desc ? var_layout->desc : ".");
         write_string_aligned(CSS_justify_center, column_header(full_header,chi,4,buffer));
         col -= 1; //counteract auto advance
         row += 1;
   }
   write_string_aligned(CSS_justify_center,(var_layout?var_layout->u:"."));
   //on last row, dont need to counteract auto advance*/
   row += 1;
/* 200122
#ifdef SOILR
   (*this) << '"' << var_layout.desc << ' ' << (var_layout.u) << "\",";
#endif
*/
}
//_write_column_header_________________________________________________________/
void Report_writer::write_grouped_column_header
(group_report_var_layout &var_layout
,int top_line_row)
{  std::string buffer;
   row = top_line_row;
   for (int chi = 1; chi <= 4; chi++)
   {  std::string group_desc((var_layout.group));
      group_desc.append(" ");
      group_desc.append((var_layout.desc));                                       //020330
      write_string_aligned(CSS_justify_center, column_header(group_desc,chi,4,buffer));
      col -= 1; //counteract auto advance
      row += 1;
   }
   write_string_aligned(CSS_justify_center,(var_layout.u));
   //{on last row, dont need to counteract auto advance}
   row += 1;
/* 200122
#ifdef SOILR
   (*this) << '"' << (var_layout.desc) << ' ' << (var_layout.u) << "\",";
#endif
*/
}
//_write_grouped_column_header_________________________________________________/
void Report_writer::write_integer
(int16 value , int8  width)
{
   write_integer_cell(col,row,value);
/*200122
#ifdef SOILR
  (*this) << value << ',';
#endif
*/
   max_col = CORN_max(max_col,col);
   col += 1;
}
//_write_integer_______________________________________________________________/
void Report_writer::write_real
(float64 value , int8 _width , int8 _precision)
{
   write_number_cell(col,row,value,_precision);
/* 200122
#ifdef SOILR
   ofstream::precision(_precision);
   (*this) << value << ',';
#endif
*/
   max_col = CORN_max(max_col,col);
   col +=1;
}
//_write_real__________________________________________________________________/
void Report_writer::write_string_aligned
(CSS_justifications _alignment,const std::string &value )
{
   write_label_cell(col,row,_alignment,value.c_str());
/* 200122
#ifdef SOILR
  (*this) << '"' << value << "\",";
#endif
*/
   max_col = CORN_max(max_col,col);
   col += 1;
}
//_write_string_aligned_____________________________________________2000-11-16_/
void Report_writer::endl()
{  col = 0;
   row += 1;
/* 200122
#ifdef SOILR
   (*this) << '\n';
#endif
*/
}
//_endl________________________________________________________________________
Report_writer::~Report_writer()
{  // freeze pane after last column with date
   int16 freeze_column = 1;                                                      //110810
   for (int i = 1; i < MAX_entries_alloc; i++)                                   //110810
      if (  (report_options.sel_var_code[i] == CSVC_weather_date_year_day)       //110810
         || (report_options.sel_var_code[i] == CSVC_weather_date_DOM))           //110810
         freeze_column = i;                                                      //110810
   freeze_pane(freeze_column, FIRST_DATA_LINE_ROW);                              //110810_000307
   int start_row = FIRST_HEADER_LINE_ROW;
   if (! report_options.header)       start_row = FIRST_COLUMN_HEADER_LINE_ROW;
   if (! report_options.column_heads) start_row = FIRST_DATA_LINE_ROW;
   // set_print_range not implemented yet
   //if (start_row > 0)
   //   WKS_set_print_range(WS_file,0,start_row,MAXINT,MAXINT); //start_row,max_col,row);*/
   // unneeded: WKS_close_out_file(WS_file);
}
//_destructor__________________________________________________________________/
#endif

