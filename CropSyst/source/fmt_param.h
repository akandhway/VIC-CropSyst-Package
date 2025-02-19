#if (CROPSYST_VERSION>=1 && CROPSYST_VERSION <=4)
// This is obsolete in V5 which outputs only UED files
#ifndef fmt_paramH
#define fmt_paramH

#include "cs_filenames.h"
#include "corn/parameters/parameters_datarec.h"
#include "corn/validate/validtyp.h"

#include "cs_glob.h"
#include "cs_vars.h"
#define MAX_entries           100
#define MAX_entries_alloc     101
#define MAX_PRINT_DATES       100
#define MAX_PRINT_DATES_ALLOC 101
//__________Forward_declarations________________________________________________
namespace CORN { namespace OS_FS { class Parameters_directory; } }
//______________________________________________________________________________
enum Accumulation_mode {NO_ACCUM,GROWTH_PERIOD_ACCUM,ANNUAL_ACCUM,CONTINUOUS_ACCUM,FALLOW_ACCUM};
extern const char *accumulation_mode_label_table[];                               //020324
DECLARE_ENUM_CLAD(Accumulation_mode_clad,Accumulation_mode,accumulation_mode_label_table); //020324

#define  chem_var_applied         1
#define  chem_var_transformation  2
#define  chem_var_uptake          3
#define  chem_var_leached         4
#define  chem_var_content         5
#define  chem_var_initial_content 5
#define  chem_var_depletion       6
#define  chem_var_final_content   6
#define  chem_var_balance         7
//______________________________________________________________________________
class Report_options
{public:
   std::string section;
   bool write_enabled;
   //      True if the file is to be opened for output
   //      (Format file must have been specified and output variables selected)
   bool header;
   bool column_heads;
   bool paginate       ;
   bool separate_years ; // { daily report only }
   Accumulation_mode_clad  accumulation_mode_clad; // daily report only    //020324
     // When outputing water balance accumulation the report may report either
     //   ANNL_ variables or GP_ variables.
     //   ANNL_ variables should be used when checking the water balance.
     //   GP_ variables should be used when graphing especially crops which grow in december and january.
   int16 line_width;
   int16 lines_per_page;
   int16 time_step;  // daily report only
   Valid_int16 v_lines_per_page;
   Valid_int16 v_time_step;
   bool water_budget;                                                            //000414
   bool nitrogen_budgets;
   // Not available yet
   //   P_budget       : bool;
   //   K_budget       : bool;
   bool salinity_budget;
   bool pesticide_budgets;
   //   { The following elements are not saved to the file: }
   int16 num_vars_to_print;   // int16 so we can import
   CS_UED_variable_code sel_var_code[MAX_entries_alloc];   // 1 based index 0 not used //000926
 public: // constructor
   Report_options();
   Report_options(const Report_options &copy_from); // Copy constructor          //981203
   Report_options(const char *section);                                          //981203
 public:
   virtual bool setup_structure
      (CORN::Data_record &data_rec,bool for_write)                modification_;
   void setup_write_enabled();                                                   //981203
   void require_variable(CS_UED_variable_code variable);                         //000926
//             This method is used by analysis simulation functions
//             To insure that required output variables have been selected.
   void count_SRO()                                               modification_; //991121
   void list_required_variables()                                         const; //190204_170225_040902
};
//Report_options_______________________________________________________________/
class Report_formats_parameters
: public Common_parameters_data_record                                           //020812
{public:
   std::string    description;                                                   //981124
   Report_options DLY_options;
   Report_options GS_options;
   Report_options YLY_options;
   int32     profile_print_date_data[MAX_PRINT_DATES_ALLOC];   // 1 indexed //170525_991105
   bool           create_TDF;                                                    //040927
 public: // Profile spreadsheets
   int16 profile_spreadsheet_timestep;
   Valid_int16 v_profile_spreadsheet_timestep;
   bool at_planting;
   bool at_emergence;
   bool at_flowering;
   bool at_maturity;
   bool at_harvest;
   // Available spreadsheets;
   bool SS_hydraulic_properties;
   bool SS_water_content;
   bool SS_water_potential;
   bool SS_temperature;
   bool SS_plant_residue;
   bool SS_manure_residue;
   bool SS_salinity;
   bool SS_ammonium;
   bool SS_nitrate;
   bool SS_denitrification;
   bool SS_nitrification;
   bool SS_NH4_mineralization;
   bool SS_immobilization;                                                       //160326
   bool SS_organic_matter;
   bool SS_root_fraction;
   bool SS_root_biomass;
   bool SS_salt;
 public:
   Report_formats_parameters();
   virtual bool expect_structure(bool for_write = false)          modification_;
   inline virtual const char *get_primary_section_name() const { return LABEL_format;}
   virtual bool get_start()                                       modification_; //161023
   virtual bool get_end()                                         modification_; //161023
   virtual bool set_start()                                       modification_; //161023
   bool add_profile_date(const CORN::Date_const &new_date)        modification_; // returns false if the date could not be added.
   bool delete_profile_date(const CORN::Date_const &del_date)     modification_;
   bool clear_profile_dates()                                     modification_;
   bool profile_print_date_exists(const CORN::Date_const &find_date)      const;
   void require_variable(nat8 for_report,CS_UED_variable_code variable);
      // This method is used by analysis simulation functions
      // To insure that required output variables have been selected.
   void list_required_profile_variable
      (CS_UED_variable_code base_var_code ,int sublayers)                 const;
   void list_required_variables(int sublayers)                            const;
public:   // For parameter editor
    int16 num_profile_print_dates;
    void sort_profile_print_dates();
private:
   void save_start()                                              modification_;
   #ifndef REACCH_VERSION
   virtual
      CORN::OS_FS::Parameters_directory
      *create_associated_directory
      (const char *param_file_name)                                performs_IO_;
      // This directory is created when using the TDF export option.
   #endif
   inline virtual const char *get_type_label()                            const
                                          { return "Report formats parameters";}
};
//_Report_formats_parameters___________________________________________________/
#endif
#endif
