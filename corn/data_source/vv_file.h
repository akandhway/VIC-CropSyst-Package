#ifndef vv_fileH
#define vv_fileH
/*
  MS-Windows   INI file class.


  This object class creates/modifies  MS-Windows  INI like files.
  Sectioned variable value files.

     Please direct any comments, suggestions, modfications to:

     Roger Nelson
     Biological Systems Engr. Dept. 6120
     Washington State Univ.
     Pullman, WA 99164-6120
     (509)335-1100
     rnelson@mail.wsu.edu

  Documentation for this class is in the docs/VV_File.htm file.
*/

#ifndef vv_datasrcH
#  include "corn/data_source/vv_datasrc.h"
#endif
#include "corn/OS/directory_entry_name.h"

#ifdef VV_DEBUG
#define VV_DBG
#else
#define VV_DBG _COMMENT_
#endif

/*!
 * This set of classes implements Variable value files similar to
 * MS-Windows INI files.
 * <P>
 * The files read/written by these classes should not be used to
 * modify Windows system files.
 * <P>
 * These classes are intended only for programmers to implement their
 * own INI formatted file I/O.
 * <P>
 * INI files are formatted as in the following example:
 * <LISTING>

 * [section1]
 * variable1=value1
 * variable2=value2
 * variable3=value3
 * [section2]
 * variablea=valuea
 * variableb=valueb
 * variablec=valuec
//
 * </LISTING>
//
 * <P>
 * Section and variable names are case sensitive.
 * <P>
 * It should be noted that when reading variable value files,
 * if multiple sections are encountered with the same name,
 * entries are stored internally as if one adjacent section.
 * Additionally and succeeding variables in any named section will
 * overwrite any previously encountered variables with the same name.
 * Variables with the same name in different sections are preserved.
 * <BR>  For example:

 * <LISTING>
//
 * [section1]
 * variable1=value1
 * variable2=value2
 * variable3=value3
//
 * [section2]
 * variablea=valuea
//
 * variableb=valueb
 * variablec=valuec
//
 * [section1]
 * variableX=valueX
 * variableY=valueY
 * variable3=valueZ
//
 * [section3]
 * variablea=valuea
 * variableb=valueb
 * variablec=valuec
//
 * </LISTING>

 * Would be written as
 * <LISTING>

 * [section1]
 * variable1=value1
 * variable2=value2
 * variable3=valueZ
 * variableX=valueX
 * variableY=valueY
 * [section2]
 * variablea=valuea
 * variableb=valueb
 * variablec=valuec
 * [section3]
 * variablea=valuea
 * variableb=valueb
 * variablec=valuec
//
 * </LISTING>

 * This is in contrast to true Windows INI file classes which preserve
 * the INI file format exactly if no modifications are made.
 * <P>
 * Also note that blank line white space is not preserved.
 * <P>
 * Thus what I call the variable value (VV) format is similar in form
 * to the INI file format but some what more restrictive.
 * <P>
 * Despite the obvious disadvantage of not allowing multiple simultaneously
 * named sections and variables, these restriction allows for efficient
 * reading and writing of such files, efficient storage of the data in memory,
 * faster variable value association searching,
 * and a novel programmer friendly coding methodology for defining
 * variable value file formats, that simplifies coding and literally
 * reduces the tedium of coding INI file I/O in (at least) half
 * (also reducing program size).
 * <P>
 * Unlike most INI file classes which known how to store data in INI
 * files as only strings or integers.  The VV file classes recognize
 * a number of common data types.  This allows programmers to store and
 * retrieve data directly in the expected data type will out having to
 * tediously code conversion for data types, since the conversion is
 * performed by the VV classes as the data is read/written.
 * <P>
 * VV files classes do not limit the file data to 64k.
 * <P>
 * Additionally the VV file classes provide facilities for storing
 * lists or arrays of data will minimal additional coding.
 * <P>
 * Finally the VV file classes provides for effeciently annotating variable value
 * entries.
 * For example, any comments encountered by conventional INI file
 * classes will be stored in memory.  With VV file classes, any comments
 * encounter are discarded and not stored in memory, but will be constructed
 * from optional annotation constants as a file is written.  With the
 * advantage of foreign language translation if using my optional
 * translation class.
 * <P>
 * <H2>Expected entries</H2>
 * The VV file classes use a techinque of anticipation or expectation of
 * the data that is to be stored in the variable value file.
 * Generally INI like files are used to store a specific set of variable
 * value associations in a fixed set of sections.  This is the type of
 * application where VV file classes are intended, but there are some
 * facilities provided for preserving unexpected sections and entries
 * when reading then writing files, and also reading variable numbered
 * arrays or lists of sections.
 * <P>
 * With VV files, the programmer provides a virtual method to define
 * the expected format of sections and variable value entries. This
 * single method is used for both input and output.
 * Contrast this with conventional INI classes which would require a
 * set of get and a set of set method calls for reading and writing the
 * files, not to mention all the conversion of the data from/to character string arrays
 * for the get/set calls.
*/

namespace CORN {
class Data_record;
//______________________________________________________________________________
class VV_File
: public VV_Data_source                                                          //130401
{
 private:
   OS::File_name *file_name; //owned                                             //160128
   bool remember_structure;                                                      //030519
      ///< Sometimes we may want to preserve the data record structure
      /// that was used to load the data record.
      /// (This may actually be something the record might want it keep)
 public:
   const OS::File_name &set_file_name_cstr(const char *file_name_);              //160128
   const OS::File_name &set_file_name(const OS::File_name &file_name);           //160128
 public:
   VV_File(const char *file_name_ = 0, bool remember_structure_ = false);        //020125
      ///< These constructors specify the name of the file to be opened.
      /// The primary_section is the name of the first section.
      /// I.e. if you only have one section you can specify the primary section
      /// here and you don't need to need to call set_current_section()
      /// when setting up the structure.
      /// Note that you must have at least one section, so primary section is required
      /// (Don't use  0 or "" for the primary section parameter!).
      /// The filename may be omitted for derived classes such as HTML_form
      /// Set remember_structure to true to preserve the data record structure
      /// that is used to load the data records during get.
   ~VV_File();
 public: // Abstract method from Data_source
   virtual bool get(Data_record &data_rec);                                      //010109
   virtual const char *label_string(std::string &buffer)                  const; //170423
      ///< \return the fully qualified filename.
   virtual bool read (std::istream &instream,Data_record &data_rec
      , bool unexpected_only,bool single_section_mode=false);
      ///< This method does not normally need to be used by end programmers,
      /// but there may be situations where a programmer may want to
      /// circumvent the normal expect_structure() load() sequence.
      /// read() is called by load().
      /// The stream length is used only by HTML form it can otherwise be 0
 private:   bool exists()                                                 const; //020125
   virtual bool set_data(Data_record &data_rec,bool append);                     //150502_050705
 protected:
   bool parse_item(const std::string &buffer,Data_record &data_rec, bool unexpected_only,bool single_section_mode);
   virtual bool inherit
      (Data_record &data_rec
      ,const std::string &key, const std::string &URL);                          //160128
};
//_VV_File_____________________________________________________________________/
}//_namespace_CORN_____________________________________________________________/
#endif

