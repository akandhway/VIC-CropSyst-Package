#ifndef variableH
#define variableH
#include "corn/metrology/units_code.hpp"
#include "corn/primitive.h"
namespace UED {                                                                  //070718
//______________________________________________________________________________
struct Variable_definition_constant
{
   float32 min_warning;    // Defines a valid range of values before an warning is issued in an editor
   float32 max_warning;    // In graphics displays, this will also define the range of the graph.
   float32 min_error;      // Defines a valid range of values before an error is issued in an editor
   float32 max_error;      // Can be used in an import utility to take missing/invalid values
    char *description;    // // The description is the full text description without any abbreviations or any limit on text length.
    char *abbreviation;   // caption   // The abbreviation is an optional shorten descriptive text suitable for captions
       // Description and abbreviation (captions) Can be left blank if smart variable code can compose a description

   // The following preference options are used as hints for utilities
   // that writing/convering values to text.
   // These do not specify how the data is stored in the database.
   // Database records specify the units of storage
   // and value are stored as  32bit floats
   CORN::Units_code preferred_units_code;   // (eventually rename this to display_units_code) This is the units that the value is usually display in or written to text files. .If preferred_units_code is 0xFFFFFFFF  then use the preferred_units_def
// The following are not currently stored in the database but may eventually be added in the next version
   nat8   preferred_precision;  // This is the number of decimal point recommended when writing data values to text
                                 // This is NOT the precision stored in the database or truncation used.
    char *label_64;         // This is a text code used when importing from or exporting to other databases (In this case Access)
    char *label_30;         // This is a text code used when importing from or exporting to other databases (In this case Oracle or other SQL)
    char *label_11;         // This is a text code used when importing from or exporting to other databases (In this case dBase)
};
//_Variable_definition_constant_____________________________________2007-02-16_/
}//_namespace UED______________________________________________________________/
#endif
//variable.h

