#ifndef workbook_Excel_OLE_earlyH
#define workbook_Excel_OLE_earlyH
#include <vcl.h>
#pragma hdrstop
#include "corn/spreadsheet/Excel_OLE/workbook_Excel_OLE.h"
//#include "Excel_2K_SRVR.h"
//#include "Excel_XP_SRVR.h"
#include "corn/OS/directory_entry_name_concrete.h"
// Rows and columns are all 0 base indexed

#if (__BCPLUSPLUS__ < 0x0570)
#define VARIANT TVariantInParam
#else
#define VARIANT Variant
#endif

namespace CORN {
namespace Excel_OLE {
namespace earlybinding {
//______________________________________________________________________________
extern TExcelApplication *global_application;
//______________________________________________________________________________
class Workbook
: public implements_ CORN::Excel_OLE::Workbook
{
  public:
   class Sheet // Worksheet
   : public CORN::Excel_OLE::Workbook::Sheet
   {
//use Get_Name      AnsiString    name; // name of the workbook
      // (note this is only temporary, the name should be available from vWorksheet
   public: // public because can't seem to pass these STUPID variants
      TExcelWorksheet *worksheet;  // owned by this
      bool just_added_to_workbook;
   public:
      Sheet
         (TExcelWorksheet *excelworksheet,bool just_added_to_workbook);
      virtual ~Sheet();
      // The following are for setting data in the sheet

      // Range can be either cell coded coordinate i.e A1, named cell, or coded range i.e. A1..Z999
      virtual const std::string &set_range_label(const char *range, const std::string &value);
      virtual const std::wstring &set_range_label_wstring (const char *range, const std::wstring &value); //130905
      virtual int32     set_range_date          (const char *range, const CORN::Date_const &date_value);
      virtual bool      set_range_bool          (const char *range, bool value) ;
      virtual int32     set_range_integer30     (const char *range, int32 value) ;
      virtual float64   set_range_number        (const char *range, float64 value,nat8 precision);
      virtual bool      set_range_color         (const char *range,nat8 red,nat8 green, nat8 blue);  //110815
      virtual const std::string &set_range_formula(const char *range, const std::string &formula);   //110815
      // The following are for getting data in the sheet
      inline virtual int32 get_cell_date(nat16 col, nat32 row) const {return 0;} // NYI!!!
      virtual int32        get_range_integer30(const char *range)         const; //111001
      virtual bool         get_range_bool(const char *range)              const; //130328
      virtual float64      get_range_number(const char *range)            const;
      virtual const std::wstring &get_range_label
         (const char *range,std::wstring &label_return)                   const; //130402_111001
      virtual bool clear_range(const char *range);

      inline virtual bool is_just_added_to_workbook()                     const
         { return just_added_to_workbook;}
      // returns true if the cell has no data

      // Sheet attributes:
      virtual void freeze_pane(nat16 left_most_column,nat32 top_most_row ) ;
      virtual const std::string &get_name(std::string &sheet_name) const;   // the name of the sheet
      //NYI  uint8       get_index() ;  // the sheet tab index (0 based).
      virtual bool is_key_string(const std::string &key)           affirmation_;
   private: //101229  These might not be needed in this class
      virtual nat32 get_first_define_row()                     const {return 0;}
      virtual nat32 get_first_define_column()                  const {return 0;}
      virtual nat32 get_last_define_row()                      const {return 0;}
      virtual nat32 get_last_define_column()                   const {return 0;}
      const VARIANT &get_range_Variant
         (const char *cell_coordinate,VARIANT &value)                     const; //191016
      const VARIANT &Workbook::Sheet::set_range_Variant
         (const char *cell_coordinate,VARIANT &value)                     const; //191016
   };
   //_Sheet____________________________________________________________________/
private:
   TComponent        *owner;
   TExcelWorkbook    *workbook;
public:
   CORN::OS::File_name_concrete filename_XLS;                                    //130328
public: // Workbook implementation
   Workbook
      (const CORN::OS::File_name   &workbook_filename_XLS// usually fully qualified or relative to current directory
      ,TComponent             *owner = 0);                                       //190803
   virtual ~Workbook();
   virtual Sheet *provide_sheet(const std::string &sheet_name)       provision_; //110711
   virtual Sheet *provide_sheet(const std::wstring &sheet_name)      provision_; //110711
   virtual Sheet *copy_sheet                                                     //190803
      (const std::string &from_sheet_name
      ,const std::string &to_sheet_name)                          modification_; //110711
   virtual const std::string &get_name(std::string &return_name)          const;
   virtual bool save(bool display_alerts = false)                  performs_IO_;
   virtual bool save_as_format(const std::wstring &filename,XlFileFormat format
      ,bool display_alerts = false)                                performs_IO_;
   virtual bool close()                                            performs_IO_;
   virtual bool set_updating(bool enabled)                        contribution_;
 public: // Specific to Excel_OLE
   virtual bool run_macro(const std::string &macro_name);
};
//_Workbook____________________________________________________________________/
bool application_start(bool minimize);
bool application_stop();
}}}//_namespace CORN Excel_OLE earlybinding____________________________________/
#endif

