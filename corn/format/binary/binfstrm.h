#ifndef binfstrmH
#define binfstrmH
// deprecated, being replaced by Binary_file_fstream  (and Binary_file_FILE)
#include <fstream>
#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#ifndef constH
#  include "corn/const.h"
#endif
#ifndef data_type_enumH
#  include "corn/data_type_enum.h"
#endif

// CPU big endian check based on littleEndian by wscott

#if (__BCPLUSPLUS__ <= 0x340)
#define seekdir seek_dir
// 051217 Apparently seek_dir was changed to seekdir in std::ios
// Borland C++ 5.0 does not have seekdir
// GNU C++ (or Linux) dont have seek_dir
#endif

namespace CORN {

extern bool CPU_big_endian();                                                    //010608
//______________________________________________________________________________
class Binary_fstream
{
#define STRM strm.
 public:
   std::fstream   strm;
 private:
   char *file_name;                                                              //020125
 protected:
   bool    endian_swapping;                                                      //990106
   int     mode; // I stream function is not accessable to remember the open mode, so I have we'll keep it //990412
 public: // constructors
   Binary_fstream
      (const char *file_name
      ,bool big_endian);
      // Most MS-DOS or Windows base software files are not big_endian
   virtual ~Binary_fstream();                                                    //020125
 public:
      // The write methods simply return the value passed.
   int32    write_int32(int32 value);
   nat32    write_nat32(nat32 value);
   int16    write_int16(int16 value);
   nat16    write_nat16(nat16 value);
   inline   int8 write_int8(int8 value)   {  STRM put(value); return value; }
   inline   nat8 write_nat8(nat8 value)   {  STRM put(value); return value; }    //990909
   float64  write_float64(float64 value);
   float32  write_float32(float32 value);                                        //990107
   const char *write_ASCIx
     (const char *str,String_format format=ASCII_Z_string,nat32 field_length=0); //050408
      // This writes character array (string)
      // str          - is a null terminated string of any length.
      // format       - specifies how the string will be formatted in the output file.
      // field_length - is the number of bytes to reserve for the string
      // If field length is 0 the number of bytes written
      // will be the actual length will be determined based on the
      // format and the number of characters
      // If field length is specified (greater than 0) then
      // this exact number of characters will be written
      // For the ASCIC and ASCIZ format, the field_length includes the bytes
      // required for the formatting information.
      // For example if the string is  "abc" and ASCIZ format is used
      // the field size would have to be at least 4 bytes to accomodate
      // the null terminator.
      // If the ASCIC32 is used then the field length must be at least 7 bytes.
      // for the 32bit integer field size (4 bytes) plus the 3 text characters.
      // The string plus formatting information may be truncated to fit this size).
      // If the string is shorter than the field size null characters will
      // be used to pad the remainder of the field.

   const char *write_cstr(const char *str,bool null_terminated = true);          //990103
      // This is a short version of write_ASCIx(str,ASCIZ or ASCII,0):
      // write_ASCIx(str,ASCIZ,0) if null_terminated = true
      // write_ASCIx(str,ASCII,0) if null_terminated = false

   int32 read_int32();                                                           //990103
   int16 read_int16();
   nat32 read_nat32();                                                           //990103
   nat16 read_nat16();
   inline  int8  read_int8()  {  int8 value = (int8)STRM get();   return value;  }
   inline  nat8  read_nat8()  {  nat8 value = (nat8)STRM get(); return value;  } //990909
   float64 read_float64();
   float32 read_float32();                                                       //990107

   const char *read_ASCIx
      (char *str,String_format format = ASCII_Z_string,nat32 field_length=0);    //050408
      // This reads a character array (string)
      // str          - is an allocated character array for a null terminated string of any length.
      //             THE CALLER MUST ENSURE THE BUFFER SIZE IS LARGE ENOUGH TO STORE THE TEXT.
      // format       - specifies how the string has been formatted in the output file.
      // field_length - is the number of bytes reserved for the string if the file (fixed field width)
      // The field length is NOT the allocated size of the array
      // (although for the ASCIZ and ASCIC formats you will be guarenteed
      //  that the buffer will not overflow if the string is at least the field length).
      // field_length it may be 0 but then the format must be ASCIZ or one of the ASCIC formats.
      // If field length is specified (greater than 0) then
      // this exact number of characters will always be read
      // For the ASCII format, make sure the str buffer is allocated the field_length + 1 to accomodate the null terminator appended to the read string.
      // For the ASCIC and ASCIZ format, the field_length includes the bytes
      // required for the formatting information.
      // For example if the string is  "abc" and ASCIZ format is used
      // the field size would have to be at least 4 bytes to accomodate
      // the null terminator.
      // If the ASCIC32 is used then the field length must be at least 7 bytes.
      // for the 32bit integer field size (4 bytes) plus the 3 text characters.
      // The string plus formatting information may be truncated to fit this size).
      // If the string is shorter than the field size null characters will
      // be used to pad the remainder of the field.

   const char *read_cstr(char *cstr,nat16 max_string_length,bool null_terminated=true);  //990103
      // This is a short version of read_ASCIx(str,ASCIZ or ASCII,0):
      // read_ASCIx(str,ASCIZ,0) if null_terminated = true
      // read_ASCIx(str,ASCII,0) if null_terminated = false
public: // accessors
   bool seek(nat32 pos , std::ios::seekdir dir = std::ios::beg)    performs_IO_; //990107
   nat32 get_file_length()                                         performs_IO_; //990115
   nat32 tell()                                                    performs_IO_; //990103
   bool goto_EOF()                                                  performs_IO_;//990107
   inline virtual const char *get_file_name()        const { return file_name; } //001206
   inline int get_open_mode()                        const { return mode; }      //990412
   inline bool good()                                const { return true; }      //990409
   inline bool at_eof()                         stream_IO_ { return STRM eof();} //050522
};
//______________________________________________________________________________
}//_namespace_CORN_____________________________________________________________/
#endif

