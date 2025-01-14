#ifndef CS_adjustmentH
#define CS_adjustmentH
#include "corn/container/unilist.h"
#include "corn/data_source/datarec.h"
//#include "corn/metrology/units_code.hpp"
//#include "corn/metrology/units_clad.h"
#include "CS_suite/association/CS_associate.h"
#include "corn/OS/directory_entry_name_concrete.h"


namespace CS {
//____________________________________________________________________________
class Adjustment_properties
: public CORN::Data_record
{public:
   std::string alteration;
   struct Domain
   {
      CORN::Units_clad units;
      inline Domain()
         : units(UT_date)
         {}
   } domain;
   struct Codomain
   {
      CORN::Units_clad units;
      std::string ontology;
      #if (CS_VERSION < 6)
      nat32       UED_variable;
         // in version 6 we will only use ontology as identifier
      #endif
      inline Codomain()
         : units        (UC_unitless)
         , ontology     ("")
         #if (CS_VERSION < 6)
         , UED_variable (0)
         #endif
         {}
   } codomain;
   CORN::Unidirectional_list  adjustments;
      // Typically dated values
   std::string                assignment_operator; // for assignment model I.e. = += -= *= /=
 public:
   Adjustment_properties();
   virtual bool expect_structure(bool for_write);
   //virtual bool get_end();
};
//_Adjustment_properties____________________________________________2018-10-15_/
class Modifier_scalar // analygous to Inspector_scalar

// So an emanating object instanciates this

: public extends_ CS::Associate
{
};
//_class_Modifier___________________________________________________2018-10-12_/
/* NYN
class Modification // analygous to Inspection
: public extends_ CORN::Item
{
 public:
   Modifier_scalor &modifier;
   // conceptual const Period &period
   // Modification might only be applicable for a specified period.
   // Interpolation may be only associated with this period

};
*/
//______________________________________________________________________________
interface_ Adjustment
: public implements_ CORN::Item
{

 public:
   virtual bool commit() = 0;
   virtual bool date_matches
      (CORN::date32 simdate_raw
      ,CORN::date32 start_date_raw) = 0;
/*
      (const CORN::Date_cowl_const_32 &today
      ,const CORN::Date_cowl_32 &start_date);
*/
   virtual bool year_matches
      (CORN::Year this_year_raw
      ,CORN::Year start_year_raw) = 0;
};
//_interface_Adjustment_____________________________________________2018-10-15_/
class Adjustment_abstract
: public implements_ Adjustment
{
 public:
   nat32 temporal;  // currently date or year but other time step (I.e. year/month year/week)
 public: // Data_record implementations
   Adjustment_properties *adjustment_props; //owned
 public:
   inline Adjustment_abstract(Adjustment_properties *adjustment_props_given)
      : adjustment_props(adjustment_props_given)
      {}
   virtual bool commit();
   virtual bool date_matches
      (CORN::date32 simdate_raw
      ,CORN::date32 start_date_raw);
/*
      (const CORN::Date_cowl_const_32 &today
      ,const CORN::Date_cowl_32 &start_date);
*/
   virtual bool year_matches
      (CORN::Year this_year_raw
      ,CORN::Year start_year_raw);

};
//_class_Adjustment_abstract________________________________________2018-10-15_/
class Assignment
: public implements_ Adjustment_abstract
{
   // probably should be interpolation step
 public:
   inline Assignment(Adjustment_properties *adjustment_props_given)
      : Adjustment_abstract(adjustment_props_given)
      {}

};
//______________________________________________________________________________

/*

class Interpolation_model
{

};

class Interpolation_linear_model
{


};

template value_type
class Adjustee
{
   value_type &value;




*/

//____________________________________________________________________________
} // namespace CS
#endif
