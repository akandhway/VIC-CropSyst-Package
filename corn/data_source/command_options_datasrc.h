#ifndef command_options_datasrcH
#define command_options_datasrcH
#include "corn/data_source/vv_datasrc.h"
#include "corn/primitive.h"
#include "corn/container/text_list.h"
#include "corn/application/arguments.h"
#include "corn/OS/directory_entry_name_concrete.h"
#ifdef __BCPLUSPLUS__
#ifndef CROPSYST_VERSION
#define VCL_ARGS
#endif
#endif
namespace CORN {
//______________________________________________________________________________
class Command_options_data_source
: public extends_ VV_Data_source
{
 protected:
   // As of 2013, main() command line arguments are only ASCII.
   nat16       argc;
   const char **argv;
   CORN::Text_list args;                                                         //160826
      // rename to args_raw
 public:
   Command_options_data_source
      (nat16 argc
      ,const char *argv[]
      ,const char *response_file_extension = 0);
   Command_options_data_source
      (nat16 argc
      ,char *argv[]
      ,char *response_file_extension = 0);
   Command_options_data_source(const std::string &reponse_line);                 //161015
      // used when reading from response file
      // response file may have all options on one line
      // or on separate lines, and/or may be INI file format with sections
      // (essentially the INI format is extended to have unary lines).
   Command_options_data_source                                                   //161015
      (const CORN::OS::File_name &response_filename);
   #ifdef VCL_ARGS
   Command_options_data_source();
   #endif
   virtual VV_Data_source::Submission_status submit
      (modifiable_ Data_record &data_rec
      ,const std::string &text
      ,bool unexpected_only)                submission_;


   virtual bool get(modifiable_ Data_record &data_rec);
   //160222 I think just implement as get bool parse_options(modifiable_ Data_record &options_record);
   /**
      The command line argv[] array is scanned for items that match the pattern
      variable=value
      Variables are set to values for the current section.
      [Initially this is the default/primary section of the data record]
      [section_name] on the command line (argv) will set to the respective new current section.

      if an argv item as the specified response_filename extension
      and the file exists, it will be openned and values will be loaded into
      the options record (overriding any currently set values)

      There may be any number of response files specified on the command line.
      This this options typically such response files will have only a
      subset of the setable options as any values loaded for variable values
      that may have be specified in previous response files would override
      the previous values.

      Values for string options that contain spaces (or tabs) should be enclosed
      in double quotes  on the command line I.e.   variable="value with spaces".
      String values that do not have spaces do no require quotation.
      String values in response files must not be quoted

      There must be no space separating the equal sign the variable nor the value.
      (otherwise these will be treated as separate arguments).

      Any other encountered argument will iniciate a call to
      the virtual function  consume_argument() which takes the current argv
      and the remainder of the argv[] array.
      This is to allow derived classes to handle additional
      special command arguments.

      Unrecognized variable=value pair are ignored.
   **/
/*obsolete Arguments::get_end now does this
   bool prompt_user_for_unencountered_options
      (const char *selected_options[] = 0);
*/

   //conceptual:   bool user_select_response_files
   //   (const char *response_file_directory = 0)
   /*
      lists (enumerated) all files in the specified directory
      (default current working direory)
      with the matching response file extension
      The user can select zero, one, or more of the listed file
      by entering the enumerated number or number separated by spaces).
      The response files will be opened and loaded in the order
      the file numbers were entered.
   */
 protected:
   virtual const char *label_string(std::string &buffer)                  const; //170423
   inline virtual bool set_data
      (Data_record &/* data_rec */,bool /* append*/)
      { return false; }
      // We don't write command line options.
      // Indicate error is attempted.
   virtual bool inherit                                                          //160128
      (Data_record       &/*data_rec*/
      ,const std::string &/*key*/
      ,const std::string &/*URL*/)
      { return true;}                                                            //160222
      // The inheritance mechanism is not currently provided
      // for command line options (unlikely to be needed/used).
      // It is not an error if not implemented.
      // However response file could be considered/works like inheritence.
};
//_Command_options_data_source______________________________________2013-04-01_/
}//_namespace_CORN_____________________________________________________________/
#endif
