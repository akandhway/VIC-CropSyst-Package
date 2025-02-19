#ifndef tabular_formatH
#define tabular_formatH

// Identifiers columns
// Knows about
//    header lines may precede the data lines, but doesn't know what data is stored there,
//    column header captions
//    column header units
//    detail lines.
// Doesn't attempt to associated data with columns except date fields.
// Doesn't assume detail line are time series (Doesn't know which column is the data/time).

#include "corn/tabular/delineation.h"
#ifndef parameters_datarecH
#include "corn/parameters/parameters_datarec.h"
#endif
#include "corn/format/html/form_IO/content.h"

#include "corn/data_source/datarec.h"
#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include "corn/math/statistical/stats.h"

#define LABEL_variable_code   "variable_code"
#include "corn/format/structural.h"

#include "corn/data_source/datarec.h"

#define LABEL_units_code      "units_code"
#define LABEL_UED_record_code    "UED_record_code"

class Desired_variables; // forward declaration

namespace CORN {
//______________________________________________________________________________
/*200220 now simply use file extension
enum Format_file_type
{FILE_TYPE_unknown,FILE_TYPE_text,FILE_TYPE_binary
, FILE_TYPE_dBase, FILE_TYPE_lotus123, FILE_TYPE_Excel};
extern const char *format_file_type_label_table[];
DECLARE_ENUM_CLAD(Format_file_type_clad,Format_file_type,format_file_type_label_table)
//200131 obs extern const char *variable_order_label_table[];
//______________________________________________________________________________
*/
//_____________________________________________________________________________/

class Tabular_format // replaces Common_tabular_file__format
//200306 : public extends_ Delineation_tabular  // Adds columns
: public extends_ Table_definition
//#if (CS_VERSION < 6)
#ifndef USE_MAP_IDIOM
, public extends_ Common_parameters_data_record                                  //040925
#endif
{
 public:
   Tabular_format();                                                             //200225
   //200226 check needed Tabular_format(const std::string &description);         //200225
   //200226 check needed Tabular_format(const Tabular_format &source);

 public:
   #if (CS_VERSION < 6)

      virtual bool expect_structure(bool for_write = false);                     //161023
      inline virtual const char *get_primary_section_name()                const
            { return "format"; }

   /*200306 unused
   bool copy_from(const Tabular_format &source);
   */
   #endif

   virtual bool get_end();                                                       //190325
      // Not sure yet if I need to reimplement this
 private:
   Tabular_format *check_abstract() { return new Tabular_format; }
   /*200220  obsolete use Map idiom
   virtual bool expect_structure(bool for_write = false);                        //161023
   virtual bool get_end();                                                       //190325
   inline virtual const char *get_primary_section_name()                   const
      { return "format"; }
   */
   /*200220 obs
   inline virtual const char *get_type_label()                             const //051011
      { return "Tabular data file format";}
   */

};
//_Tabular_format___________________________________________________2020-02-20_/
}//_namespace_CORN_____________________________________________________________/
#endif

