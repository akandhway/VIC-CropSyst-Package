#include "CS_document.h"
#include "corn/string/strconv.hpp"
#include "corn/container/text_list.h"
namespace CS
{
// http://yaml-online-parser.appspot.com/

//______________________________________________________________________________
Document::Document()
: structural::Mapping_clad(0) //190605Mapping_abstract(0)
{}
//______________________________________________________________________________
bool Document_configured::initialize()                                      initialization_
{  bool initialized = true;
   /*NYI
   parameter_associations.append(new CS::Parameter_assocation_text(common_parameters.description.brief         ,"description/brief"       ,NOT_ADJUSTABLE,    OBSERVABLE));
   parameter_associations.append(new CS::Parameter_assocation_text(common_parameters.description.details_URL   ,"description/details_URL" ,NOT_ADJUSTABLE,    OBSERVABLE));
   */
/*
   parameter_associations.append(new CS::Parameter_association_numeric(common_parameters.data_source_version_number      ,"version/data"       ,NOT_ADJUSTABLE,    OBSERVABLE));

   parameter_associations.append(new CS::Parameter_association_numeric(common_parameters.current_program_version_number  ,"program/version"   ,NOT_ADJUSTABLE,    OBSERVABLE));
*/
   //conceptual parameter_associations.append(new CS::Parameter_association_numeric(common_parameters.program      ,"program/name"      ,NOT_ADJUSTABLE,    OBSERVABLE));

   /*NYI
   parameter_associations.append(new CS::Parameter_assocation_text(common_parameters.data_source_URL         ,"source/URL"       ,NOT_ADJUSTABLE,    OBSERVABLE));
   */
      std::string          email;   // originator
      std::string          name;    // originator
      CORN::Date_clad_32   date;    // rename to inception
      std::string          program;
      nat32                program_version;

   initialized = initialize_parameter_associations();
      // Parameter associations are sort of temporarily,
      // eventually I want to load these from ontology

//std::ofstream debug("C:\\temp\\debugCSdoc.txt");
//   Parameter_association *parameter_assoc =0;
//   while (parameter_assoc = dynamic_cast<Parameter_association *>(parameter_associations.pop_first()))
   FOR_EACH_IN(parameter_assoc,Parameter_association ,parameter_associations, each_param_assoc)
   {

//debug << ontology << std::endl;

      know(parameter_assoc);
   } FOR_EACH_END(each_param_assoc)
   return initialized;
}
//_initialize_______________________________________________________2015-09-19_/
bool Document_configured::initialize_relevencies()               initialization_
{  bool initialized =
         initialize_parameter_pertinencies()
      && initialize_parameter_essentials();
   return initialized;
}
//_initialize_relevencies___________________________________________2016-07-01_/

/*190606 I think this is obsoltete and simply append to parameter associations list

bool Document_configured::know_parameter_association // take_parameter_association_numeric
(modifiable_ structural::Mapping    &in_mapping
,modifiable_ Parameter_association  *param_assoc_known
,modifiable_ CORN::Text_list        &key_list_given)
{
   //190605 CORN::Text_list::Item *key_label = dynamic_cast< CORN::Text_list::Item *>(key_list_given.pop_at_head());
   CORN::Item_wstring *key_label = dynamic_cast< CORN::Item_wstring *>(key_list_given.pop_at_head());
   if (!key_label)
      return false; // should never occur
   structural::Key_string *simple_key = new structural::Key_string(key_label);
   //190605 structural::Key_simple *simple_key = new structural::Key_simple(key_label);


   structural::Pair_key_value *KV_pair =0;
   bool keys_exhaused = key_list_given.is_empty();
   if (keys_exhaused) //160704 count() == 0)
   {
      // There is no further configured structure
      // we must be a terminal numeric enumeration or bool values value

      Parameter_association_numeric *numeric_param =
         dynamic_cast < Parameter_association_numeric *>(param_assoc_known);
      if (numeric_param)
      {
         KV_pair = new structural::Pair_key_value //190606 Parameter_number
               (numeric_param,simple_key);
      } else
      {  Parameter_association_enum *enum_param =
         dynamic_cast < Parameter_association_enum *>(param_assoc_known);
         if (enum_param)
         {  KV_pair = new Parameter_labeled_enum
                  (enum_param->labeled_enum,simple_key);
         } else
         {  Parameter_association_bool *bool_param =
            dynamic_cast < Parameter_association_bool *>(param_assoc_known);
            if (bool_param)
            {  KV_pair = new Parameter_bool
                     (bool_param->bool_value,simple_key);
            }
         }
      }
      //registered_parameter_associations.append(param_assoc_given);
      in_mapping.get_key_value_pairs().take(KV_pair);
            simple_key = 0; // The key has been relinquished to Parameter_number
            // NYI
            //if (param_assoc_given->observable)
            //{  create observation
            //   setup structure mapping for observation
            //
            //}
            //if (param_assoc_given->adjustable)
            //{
            //   create adjuster
            //   setup structure mapping for observation
            //}


   } else
   {
      // Here we are implementing simple key value pairs
      // however YAML keys can themselves be complex objects
      KV_pair =
         dynamic_cast<structural::Pair_key_value *>
            (in_mapping.get_key_value_pairs().find_string(*key_label));

      // At this point in CS documents, KV_pair will typically be
      // a structural::Mapping::Parameters_XXXX
      // or a simple structure
      if (KV_pair) // we've already encountered this key at this 'level'
      {
         structural::Construct *value = KV_pair->get_value();
         structural::Mapping *value_as_mapping = dynamic_cast<structural::Mapping *>(value);
         if (value_as_mapping)
         {
            // structural::Construct *unused =
               know_parameter_association
               (*value_as_mapping,param_assoc_known,key_list_given);
         } else
         {  // This may be a sequence which I haven't handled yet
            // I Don't think there is anything to process at this point
            // other than there would be some specialized structure
            // that the document should know about an handle. (NYI)
         }
      }
      else
      {

         // It is possible that in_mapping may have/know specialized
         // key value pairs (a specialized structure)
         KV_pair = in_mapping.provide_key_value_pair(simple_key);
         structural::Mapping *value_mapping = 0;
         if (KV_pair)
         {
            simple_key = 0;
            // The simple_key is submitted, and if KV_pair is instanciated
            // the simple_key is reliquished to KV_pair.

            value_mapping = dynamic_cast<structural::Mapping *>(KV_pair->get_value());
            if (!value_mapping)
            {
               value_mapping = new structural::Mapping_abstract(simple_key);
               KV_pair->take__value(value_mapping);
//160704               key_value_pairs.append(value_mapping);                                               //160703
               in_mapping.get_key_value_pairs().append(value_mapping);              //160704
            }
         } else
         {
            // In this composition mode, all subelement (values) are assumed to be mappings
            value_mapping = new structural::Mapping_abstract(simple_key);


            //key_value_pairs.append(value_mapping);                               //160703

            in_mapping.get_key_value_pairs().append(value_mapping);              //160704
         }
         if (value_mapping)
            know_parameter_association
               (*value_mapping,param_assoc_known,key_list_given);
         //simple_key = 0; // The key has been relinquished to KV_pair

      }
   }
   return KV_pair;
}
//_2015-09-19___________________________________________________________________
*/

bool Document_configured::is_pertinent_wstring(const std::wstring &parameter_or_propery_qualified) affirmation_
{
   bool pertinent = true;
   FIND_FOR_EACH_IN(found_pert, param_pert,const Parameter_pertinence,parameter_pertinencies,true,each_param_pert)
   {
      bool ontology_matches = parameter_or_propery_qualified.find(param_pert->structure) != std::string::npos;
      if (ontology_matches)
      {  // applicability indicator found for the specified structure.
         // it is now only applicable if the enumeration selection matches
         pertinent = false;
         int32 param_enum_value = param_pert->enumeration.get_int32();
         int32 enum_selection = param_pert->enum_option;
         if (param_enum_value == enum_selection)
         {  pertinent = true;
            found_pert = param_pert; // we can break the oup
         }
      }
   } FOR_EACH_END(each_param_pert)
   return pertinent;
}
//_is_pertinent_wstring_____________________________________________2016-06-30_/
bool Document_configured::is_pertinent_cstr(const char *parameter_or_propery_qualified)      affirmation_
{
   std::wstring parameter_or_propery_qualified_wstring;
   CORN::ASCIIZ_to_wstring(parameter_or_propery_qualified,parameter_or_propery_qualified_wstring);
   return is_pertinent_wstring(parameter_or_propery_qualified_wstring);
}
//_2016-06-30___________________________________________________________________
bool Document_configured::is_essential_wstring(const std::wstring &parameter_or_propery_qualified)         affirmation_
{
   bool essential = false;
       /// \returns true if the specified element is essential
       /// (critical to the model).

   FIND_FOR_EACH_IN(found_essential, param_essential,CORN::Item_string
         ,parameter_essentials,true,each_param_essential)
   {  std::wstring essential_as_wstring;
   CORN::string_to_wstring(*param_essential,essential_as_wstring );
      bool ontology_matches = parameter_or_propery_qualified.find(essential_as_wstring) != std::string::npos;
      essential = ontology_matches;
      if (essential)
         found_essential = param_essential;
   } FOR_EACH_END(each_param_essential)
   return essential;
}
//_2016-07-01___________________________________________________________________
/*190606 check still needed
bool Document_configured::get_value_as_string
(const std::wstring &parameter_or_propery_qualified
,modifiable_ std::string &value
,modifiable_ std::string &units
// NYI status
)                                           const
{
   // Eventually there will be an inherited list and we would search each interited item
   FIND_FOR_EACH_IN(found_param, param,Parameter_association_numeric,parameter_associations,true,each_param)
   {
      bool ontology_matches = false;
      if (param->key_is_wide)
      {
         ontology_matches = parameter_or_propery_qualified.find
            (param->CORN::Number_keyed::get_key_wide()) != std::string::npos;
      } else
      {
         std::string parameter_or_propery_qualified_string;
         CORN::wstring_to_string(parameter_or_propery_qualified,parameter_or_propery_qualified_string);
         ontology_matches = parameter_or_propery_qualified_string.find(param->CORN::Number_keyed::get_key()) != std::string::npos;
      }

      if (ontology_matches)
      {
         Parameter_association_numeric *as_numeric =
            dynamic_cast<Parameter_association_numeric *>
            (param);
         if (as_numeric)
         {
            as_numeric->append_value_in_radix_to(value,6,10);
               // precision needs to be obtain from the ontology/parameter
            found_param = param;
         }
      }
   } FOR_EACH_END(each_param)
   // NYI
   //if (!found_param->encountered)  // if wasn't loaded from parameter file
   //{


   //}
   return found_param != 0;;
}
*/
//_2016-07-01___________________________________________________________________

/*NYN
bool Document_configured::is_pertinent_URL(CORN::OS::Uniform_resource_locator &ontology_URL) affirmation_ //160630
{
   return is_pertinent_wstring(ontology_URL.c_str());
}
*/
//_2016-06-30___________________________________________________________________
#endif
} // namespace CS
