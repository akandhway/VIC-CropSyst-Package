// 161231 Might not be using this

#include "CS_suite/ontology/CS_ontology.h"
#include "corn/OS/directory_entry_name.h"
#include "corn/OS/file_system_engine.h"
#include "corn/seclusion.h"
#include "corn/format/VV/VV_stream.h"
#include "corn/format/VV/VV_document.h"
#define ONTOLOGYa "ontologia"
#define ONTOLOGYw L"ontologia"

namespace CS
{
namespace ontology
{
//______________________________________________________________________________


Elements::Elements()
// Currently documents don't have specifier but essentially
// the file name (unqualified) is document specifier
{}
//_Elements:constructor________________________________________________________/
Translation::Translation
(const structural::Pair_key_value &item)
{
   item.key_string(language);
   item.ref_value().append_to_wstring(text);
}
//_Translation:constructor_____________________________________________________/
bool Elements::get_translated_text(modifiable_ Translated_text &text)
{

/*200304 Need to reimplement this using structural

   Section &text_section = expect_section(text.name);
   FOR_EACH_IN(trans,structural::Pair_key_string,text_section.get_key_value_pairs(),each_trans)
   {
      Translation *translation = new Translation(*trans);
      text.take(translation);
   } FOR_EACH_END(each_trans)
*/
   return true;
}
//_Elements::get_translated_text_______________________________________________/
//______________________________________________________________________________

Elements_conceptual::Elements_conceptual()
: Elements()
, caption      ("caption")
, description  ("description")
, intention    ("intention")
, documentation("documentation")
{}
//_Elements_conceptual:constructor_____________________________________________/
/* Not sure if I still need this
//    bool expected = CORN::Data_record::expect_structure(for_write);
bool Elements_conceptual::get_end()
{
   bool got = Elements::get_end();
   got
   &= get_translated_text(caption)
    & get_translated_text(description)
    & get_translated_text(intention)
    & get_translated_text(documentation);
   return got;
}
*/
//______________________________________________________________________________
Elements_theoretical::Elements_theoretical()
: Elements_conceptual()
, extremo()
{}
//_Elements_theoretical:constructor____________________________________________/
/*
bool Elements_theoretical::expect_structure(bool for_write)
{  bool expected = Elements_conceptual::expect_structure(for_write);
   set_current_section("extremo");
      extremo.setup_structure(*this,for_write);
   return expected;
}
//______________________________________________________________________________


*/
Elements_operational::Elements_operational()
: Elements_theoretical()
{}
//______________________________________________________________________________
/* NYN or obsolete
bool Elements_operational::get_end()
{
   bool got = Elements_theoretical::get_end();
   got
   &= get_translated_text(application)
    & get_translated_text(provision)
   return got;
}
*/
//______________________________________________________________________________
Elements_model::Elements_model()
: Elements_operational()
{}
//______________________________________________________________________________


/*
bool Elements_model::expect_structure(bool for_write)                 provision_
{
   bool expected = Elements_operational::expect_structure(for_write);

   set_current_section("modello");
   expect_string ("exemplar",exemplar);
   // expect_enum("exemplar",exemplar);
   return expected;
};
*/
//______________________________________________________________________________
//______________________________________________________________________________
//______________________________________________________________________________
// NYI impelement get_end and get the translations
//______________________________________________________________________________
Domain::Domain
(CORN::OS::Directory_entry_name *source_directory_given)
: Definition(source_directory_given)                                                                  //170101
//, domain_directory(source_directory_given)
{
   ID = source_directory_given->get_name();
   //type = TYPE_DOMAIN;
   kind = kind_domain;
}
//_Domain:constructor_______________________________________________2016-02-27_/
Definition::Definition
(CORN::OS::Directory_entry_name *DEN_given)
: CORN::Item()
, generalization     ("")
, elements           (0)
, subdefinitions     ()
, directory          (0)
, filename           (0)
//, type               ("")
, kind               (kind_unknown)
, ID                 () // The ID for actual definitions will ultimately be composed in initialize
, definition_ID_names()
{
   if (DEN_given) // should always be the case
   {
      extended = DEN_given->get_type() == CORN::OS::directory_entry;
      if (extended)
      {  // extended
         filename  = find_extended_definition_filename(DEN_given);
         // There may be directories with no definition files
         // these would be place holders but there really should be
         // at least a conceptualization
         directory = DEN_given;
         // delete DEN_given;
      } else // simple (no directory name)
      {
         filename = DEN_given;
         std::string type;
         CORN::wstring_to_string(DEN_given->get_extension(),type);
         if      (type == TYPE_CONCEPTUALIZATION) kind = kind_conceptualization;
         else if (type == TYPE_PROPERTY)          kind = kind_property;
         else if (type == TYPE_MODEL)             kind = kind_model;
         else if (type == TYPE_PARAMETER)         kind = kind_parameter;
         else if (type == TYPE_CLASSIFICATION)    kind = kind_classification;
      }
   }
}
//_Definition:constructor___________________________________________2016-03-05_/
CORN::OS::File_name *Definition::find_extended_definition_filename
(CORN::OS::Directory_entry_name *DEN_given)
{

//debug std::string psdf;
//debug CORN::UnicodeZ_to_string(DEN_given->w_str(),psdf);
//debug std::clog << "psdf:" << psdf << std::endl;


   CORN::OS::File_name *found_def_filename = 0;
   CORN::OS::File_name_concrete *conceptualization
   = new CORN::OS::File_name_concrete(*DEN_given,TYPE_CONCEPTUALIZATION);
   if (CORN::OS::file_system_engine.exists(*conceptualization))
   {
      found_def_filename = conceptualization;
      kind = kind_conceptualization;
   }
   CORN::OS::File_name_concrete *modello
   = new CORN::OS::File_name_concrete(*DEN_given,TYPE_MODEL);
   if (CORN::OS::file_system_engine.exists(*modello))
   {  found_def_filename = modello;
      kind = kind_model;
   }
   CORN::OS::File_name_concrete *proprietate
   = new CORN::OS::File_name_concrete(*DEN_given,TYPE_PROPERTY);
   if (CORN::OS::file_system_engine.exists(*proprietate))
   {  found_def_filename = proprietate ;
      kind = kind_property;
   }
   CORN::OS::File_name_concrete *parameter
   = new CORN::OS::File_name_concrete(*DEN_given,TYPE_PARAMETER);
   if (CORN::OS::file_system_engine.exists(*parameter))
   {  found_def_filename = parameter ;
      kind = kind_parameter;
   }
   CORN::OS::File_name_concrete *classification
   = new CORN::OS::File_name_concrete(*DEN_given,TYPE_CLASSIFICATION);
   if (CORN::OS::file_system_engine.exists(*classification ))
   {  found_def_filename = classification ;
      kind = kind_classification;
   }
   return found_def_filename;
}
//_find_extended_definition_filename________________________________2017-01-17_/
bool Definition::initialize()                                    initialization_
{
   if (extended) //(directory)
   {
      directory->append_path_list_to
         (definition_ID_names,CORN::OS::Directory_entry_name::include_all);
      // NYI find any image file
      // assume the first (only) image found is a conceptualization
      // representation image.
   }
   else
   if (filename) // will be false when directory is not an ontology (place holder)
   {
      filename->append_path_list_to
         (definition_ID_names,CORN::OS::Directory_entry_name::include_qualification);
      definition_ID_names.add_wstring(filename->get_name());
   }
   // pop_defintions list up to and including the domain name
   CORN::Item_wstring /*180118 Text_list::Item*/ *def_name = 0;
   for (def_name = dynamic_cast<CORN::Item_wstring *>(definition_ID_names.pop_head())
       ;def_name && (*def_name != ONTOLOGYw)
       ;def_name = dynamic_cast<CORN::Item_wstring *>(definition_ID_names.pop_head()))
   {  delete def_name; def_name = 0; };
   delete def_name; def_name = 0;   // ontology
   def_name = dynamic_cast<CORN::Item_wstring *>(definition_ID_names.pop_head()); // domain
   if (definition_ID_names.count())
   {
      /*
      CORN::Text_list::Item *type_item = dynamic_cast<CORN::Text_list::Item *>(definition_ID_names.pop_tail());
      std::string &type = (*type_item);
      if      (type == TYPE_CLASSIFICATION)
         elements = new Elements_conceptual();
      else if (type == TYPE_PROPERTY)
         elements = new Elements_property();
      else if (type == TYPE_PARAMETER)
         elements = new Elements_parameter();
      else if (type == TYPE_CONCEPTUALIZATION)
         elements = new Elements_conceptual();
      else if (type == TYPE_MODEL)
         elements = new Elements_model();
      */

      switch (kind)
      {  case kind_conceptualization:elements = new Elements_conceptual();break;
         case kind_property         :elements = new Elements_property();  break;
         case kind_model            :elements = new Elements_model();     break;
         case kind_parameter        :elements = new Elements_parameter(); break;
         //case kind_classification   :elements = new Elements_classification(); break;
         //default : elements = new Elements_conceptual();break;
      } // switch kind
      // ID.clear();
//      if (elements)
//      {
         //delete type_item;
         if (filename && elements)
         {
            //170210 CORN::VV_File element_file(filename->c_str());
            //170210 element_file.get(*elements);

std::string debug_fname(filename->c_str());
//CORN::UnicodeZ_to_string(filename->w_str(),debug_fname);
std::clog << debug_fname.c_str() << std::endl;

            //CORN::VV::Stream_Unicode element_stream(*filename);


            /*200304 need to reimplement this using document and structure idiom

            element_stream.parse_document(*elements);

            */


         }
//      }
//      else // the item is not is not a type specified, so it must be a place holder directory
//      {
//         if (filename) // Just an arbitrary file, not a definition
//         {  delete filename; filename = 0;
//            // delete type_item; type_item = 0;
//         }
         else if (directory)
         {  // This may be a directory with no specific definition
            // All directories under ontology domain should have definition
            // but at this time I am allowing directories with no definitions
            // type = TYPE_CONCEPTUALIZATION;
            // definition_ID_names.prepend(type_item); // not a type to relist the item
  //          Elements_conceptual *conceptual_elements = new Elements_conceptual;
//            elements = conceptual_elements;
            // Use the directory name
            Translation *default_caption = new Translation
               ("en"
               // ,*type_item
               ,directory->get_name());
            Elements_conceptual *conceptual_elements = new Elements_conceptual();
            conceptual_elements->caption.append(default_caption);
            elements = conceptual_elements;
         }
      //}
      definition_ID_names.string_items(ID,L':');

      // Warning for inspectior outputs I am using
      // To denote only the domain and the ontology definition path
      // as forward slash

   } // else this does not appear to be an ontology definition
   delete def_name; def_name = 0;   // domain
   return is_valid(); // elements != 0 && ID.length();
}
//_Definition:initialize____________________________________________2017-01-01_/
bool Definition::is_valid()                                         affirmation_
{
   // NYI need to identify the type
//   return true;
   return elements != 0 && ID.length();
}
//_2017-01-17___________________________________________________________________
bool Definition::is_applicable()                                    affirmation_
{
   // NYI check if there is an seclusion file for this definition (extended)
   // search for seclusion files based on the apropos options (set up in the user interface)
   return true;
}
//_2017-01-17___________________________________________________________________
bool Definition::is_extended()                                      affirmation_
{
   return extended;
}
//_2017-01-17___________________________________________________________________
const CORN::Unidirectional_list &Definition::provide_subdefinitions() provision_
{
//debug std::string psdf;
   if (subdefinitions.count() == 0 && is_extended())
   {
   CORN::Seclusion apropos_seclusions;

   //NYI list all seclusions in the ontology apropos to the model and version
   //Read each of these seclusions into apropos_seclusions
   apropos_seclusions.exclude_pattern("*.template");
   apropos_seclusions.include_pattern("*");

   CORN::Unidirectional_list potential_simple_definition_files;
   CORN::Unidirectional_list potential_extended_definition_subdirs;
   CORN::OS::file_system_engine.list_DENs
      (*directory // domain directory
      ,&potential_simple_definition_files
      ,&potential_extended_definition_subdirs
      ,apropos_seclusions
      ,CORN::OS::File_system::none_recursion_inclusive);
   CORN::OS::File_name *potential_simple_definition_file = 0;
   while ((potential_simple_definition_file = dynamic_cast<CORN::OS::File_name *>
      (potential_simple_definition_files.pop_head()))!= 0)
   {

//debug CORN::UnicodeZ_to_string(potential_simple_definition_file->w_str(),psdf);

      Definition *subdef = new Definition(potential_simple_definition_file);
      if (subdef->initialize())
            subdefinitions.append(subdef);
      else  delete subdef;
   }
   CORN::OS::Directory_name *potential_extended_definition_subdir = 0;
   while ((potential_extended_definition_subdir = dynamic_cast<CORN::OS::Directory_name *>
      (potential_extended_definition_subdirs.pop_head()) )!= 0)
   {

//CORN::Unicodez_to_string(potential_extended_definition_subdir->w_str(),psdf);

         Definition *subdef = new Definition(potential_extended_definition_subdir);
         if (is_applicable())
         {
            //conceptual      bool include_expired = user_settings.relevence_level == 0; // display_all
            // currently always including expired parameters
            // but should
            if (subdef->initialize())
            {
               subdef->provide_subdefinitions();
               subdefinitions.append(subdef);
            } else delete subdef;
         } else delete subdef;
//      } // valid and applicable
   } // pot subdef dirs
   } // iff
//debug std::clog << "psdf:" << psdf << std::endl;
   return subdefinitions;
}
//_Definition::provide_subdefinitions_______________________________2017-01-01_/
nat32 Definition::render_specific_definitions
(CORN::Container &definitions
,Definition::Kind of_kind)                                            rendition_
{  nat32 def_count = 0;
   provide_subdefinitions();  // this should already filter by model
   // bool matches_type = (specific == "*") || (specific == type);
   bool matches_type = (of_kind == kind_unknown) || (kind == of_kind);
   if (matches_type)
   {  Definition_listable *listabled = new Definition_listable(*this);
      definitions.append(listabled);
      def_count++;
   }
   FOR_EACH_IN(subdef,Definition,subdefinitions,each_subdef)
   {  def_count += subdef->render_specific_definitions(definitions,of_kind /*specific*//*,apropos*/);
   } FOR_EACH_END(each_subdef)
   return def_count;
}
//_Definition::render_specific_definitions__________________________2017-01-01_/
Definition::~Definition()
{
   delete filename;
   delete directory;
}
//_Definition:destructor____________________________________________2016-02-27_/
bool Definition::is_parameter()                                     affirmation_
{
   return kind == kind_parameter; // type == TYPE_PARAMETER;
}
//_Definition::is_parameter_________________________________________2017-01-14_/
bool Definition::is_property()                                      affirmation_
{
   return kind == kind_property; // type == TYPE_PROPERTY;
}
//_Definition::is_property__________________________________________2017-01-14_/
const wchar_t *Definition::label_wstring(std::wstring &label)              const
{
   // Returns the element's caption in the preferred available language
   const Elements_conceptual *as_conceptual
      = dynamic_cast<const Elements_conceptual *>(elements);
   if (as_conceptual)
   {
      if (!as_conceptual->caption.translate(label))
         label = ID; // shouldn't occur
   }
   return label.c_str();
}
//_Definition::label_wstring________________________________________2017-01-14_/
const wchar_t *Definition::key_wstring(std::wstring &buffer)               const
{
      // Returns the definition ID (key) corresponding to the definition directory
   buffer.append(ID);
   return buffer.c_str();
}
//_Definition::key_wstring__________________________________________2017-01-14_/
const std::wstring &Definition::get_hint
(std::wstring &hint)                                                       const
{
   const Elements_conceptual *as_conceptual
      = dynamic_cast<const Elements_conceptual *>(elements);
   if (as_conceptual)
   {  as_conceptual->intention.translate(hint);
   }
   return hint;
}
//_Definition::get_hint_____________________________________________2017-01-14_/
Translated_text::Translated_text(const std::string &name_)
: CORN::Unidirectional_list()
, name(name_)
{}
//_Translated_text:constructor______________________________________2017-01-22_/
bool Translated_text::translate(std::wstring &translated)                  const
{  bool has_translation = false;
   const CORN::Text_list *languages = provide_language_precedence_list();
   FOR_EACH_IN(language,CORN::Item_string ,*languages,each_lang)
   {  const Translation *translation = dynamic_cast<Translation *>
         (find_string(*language));
      if (translation)
      {  translated = translation->text;
         has_translation = true;
         return has_translation; //translated
      }
   } FOR_EACH_END(each_lang)
   translated = L"????";
   return has_translation;
}
//_Translated_text::translate_______________________________________2017-01-22_/
std::string language_precedence;
CORN::Text_list *language_precedence_list = 0;
const CORN::Text_list *provide_language_precedence_list()
{
   if (!language_precedence_list)
      language_precedence_list = new CORN::Text_list(language_precedence.c_str(),' ');
   return language_precedence_list;
}
//_provide_language_precedence_list_________________________________2017-01-22_/
/* Replace these with Map idiom


200303 continue here replace with expect

bool Range::expect__mapping(modifiable_ structural::Mapping &mapping) expectation_
{
   structural::Mapping_clad &section = dynamic_cast<structural::Mapping_clad &>(mapping);


      section.expect("metric",units)
   &&

   section.expect_number("min",minimum);
   section.expect_number("max",maximum);
   return true;
}
//_Range::expect__mapping____________________________________________2017-02-12_/

bool Elements_theoretical::expect_structure()                       expectation_
{  bool expected = Elements_conceptual::expect_structure();
   //provide_section("extremo");
   // structural::Mapping &extremo_section = provide_section("extremo");
   expected &= extremo.expect_mapping(expect_section("extremo"));
   return expected;
}
//_Elements_theoretical::expect_structure______________________________________/

bool Elements_operational::expect_structure()                       expectation_
{  bool expected = Elements_theoretical::expect_structure();
   expected &= limitation.expect_mapping(expect_section("limitation"));

   return expected;
}
//_Elements_operational::expect_structure______________________________________/

bool Elements_model::expect_structure()                             expectation_
{
   bool expected = Elements_operational::expect_structure();
//   Section &modello_section = provide_section("modello");

   structural::Mapping_clad &modello_section = dynamic_cast<structural::Mapping_clad &>(expect_section("modello"));
//std::string exemplar_key("exemplar");
   modello_section.expect_string("exemplar",exemplar);


//expect &= modello_section.expect("exemplar",exemplar_debug);


//   expect &= modello_section.expect("exemplar",exemplar);
   // expect_enum("exemplar",exemplar);
   return expected;
};
//_Elements_model::expect_structure____________________________________________/

*/


}}//_namespace_CS::ontology____________________________________________________/

