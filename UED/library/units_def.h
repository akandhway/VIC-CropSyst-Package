#ifndef units_defH
#define units_defH
#ifndef unitsH
#  include "corn/measure/units.h"
#endif
#ifndef unilistH
#  include "corn/container/unilist.h"
#endif
#ifndef UED_typesH
#  include "UED/library/UED_types.h"
#endif
using namespace CORN;
namespace UED {                                                                  //070718
//______________________________________________________________________________
class Units_definition_format_0_record;                                          //990426
//______________________________________________________________________________
class Units_definition
: public Item
, public Units_clad                                                              //980818
{
friend class Units_definition_format_0_record;                                   //990426
   std::string description;
   std::string abbreviation;
public: // constructors
   // Constructor for reading definition from database
   inline Units_definition()                                                     //980818
      :Item()
      ,Units_clad(0)
      ,description()
      ,abbreviation()
      {}
   // Constructor for standard units definitions not stored in the database
   Units_definition(UED_units_code _code);                                       //990412
   inline Units_definition
      (UED_units_code _code
      ,const char *_description
      ,const char *_abbreviation)                                                //990115
      :Item()
      ,Units_clad       (_code)                                                  //980818
      ,description      (_description)
      ,abbreviation     (_abbreviation)                                       {} //990115
   //             Copy constructor
   inline Units_definition                                                       //990426
      (const Units_definition &to_copy)
      :Item()
      ,Units_clad       (to_copy.get_code())                                     //980818
      ,description      (to_copy.description)
      ,abbreviation     (to_copy.abbreviation)                                {} //990115
   inline bool is_local()    const { return (get() & 0x80000000) == 0x80000000;}
   inline virtual int compare(const Item  &other)                         const
      { return description.compare(((Units_definition &)other).description); }
   inline nat32 get_code()                    const { return Units_clad::get();} //980624
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual const std::string &get_abbreviation(std::string &result)       const; //020130
   virtual nat32 compute_units_format_size()                              const;
#ifdef UED_DUMP
   virtual void dumpX(std::ostream &txt_strm);
#endif
   virtual void write_YAML(std::ostream &YAML_strm,nat16 indent_level);          //180108
};
//_Units_definition____________________________________________________________/
}//_namespace UED______________________________________________________________/
#endif

