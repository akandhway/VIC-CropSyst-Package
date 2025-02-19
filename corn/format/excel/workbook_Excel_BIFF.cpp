#include "corn/format/excel/workbook_Excel_BIFF.h"
#include "corn/format/excel/worksheet_Excel_BIFF.h"
namespace CORN {
namespace Excel_BIFF                                                             //190107
{
//______________________________________________________________________________
Binary_record *Workbook::instanciate_record(Record_code code) const
{
   Binary_record *record = 0;

   switch (code)
   {
//     case BIFF_CODE_model                   :
//        record = new BIFF_rec_model();
//     break;

//NYI       case BIFF_CODE_DIMENSIONS                   0x00
//NYI       case BIFF3_CODE_DIMENSIONS                0x0200
//NYI       case BIFF_CODE_BLANK                        0x01
//NYI       case BIFF_CODE_INTEGER                      0x02
//NYI       case BIFF_CODE_NUMBER                       0x03
//NYI       case BIFF_CODE_LABEL                        0x04
//NYI       case BIFF_CODE_BOOLERR                      0x05
//NYI       case BIFF_CODE_FORMULA                      0x06
//NYI       case BIFF_CODE_STRING                       0x08

      case BIFF578_CODE_BOF :
         // To distinguish BIFF5 and BIFF7 from BIFF8 we need to read also the vers value from the file
         // STUPID
         // But the length will give us a clue
         record = new BIFF57_rec_BOF();
      break;

      case BIFF4_CODE_BOF : case BIFF3_CODE_BOF : case BIFF2_CODE_BOF:
         record = 0; // No body
      break;

//NYI       case BIFF4_CODE_BOF                       0x0409
//NYI       case BIFF3_CODE_BOF                       0x0209


//NYI       case BIFF_CODE_EOF                        0x0A
//NYI       case BIFF_CODE_INDEX                      0x0B
//NYI       case BIFF_CODE_CALCCOUNT                  0x0C
//NYI       case BIFF_CODE_CALCMODE                   0x0D
//NYI       case BIFF_CODE_PRECISION                  0x0E
//NYI       case BIFF_CODE_REFMODE                    0x0F
//NYI       case BIFF_CODE_DELTA                      0x10
//NYI       case BIFF_CODE_ITERATION                  0x11
//NYI       case BIFF_CODE_PROTECT                    0x12
//NYI       case BIFF_CODE_PASSWORD                   0x13
//NYI       case BIFF_CODE_HEADER                     0x14
//NYI       case BIFF_CODE_FOOTER                     0x15
//NYI       case BIFF_CODE_EXTERNCOUNT                0x16
//NYI       case BIFF_CODE_EXTERNSHEET                0x17
//NYI       case BIFF_CODE_NAME                       0x18
//NYI       case BIFF_CODE_WINDOW_PROTECT             0x19
//NYI       case BIFF_CODE_VERTICAL_PAGE_BREAKS       0x1A  /* VERTICALPAGEBREAKS - Explicit Column Page Breaks */
//NYI       case BIFF_CODE_HORIZONTAL_PAGE_BREAKS     0x1B /* HORIZONTALPAGEBREAKS - Explicit Row Page Breaks */
//NYI       case BIFF_CODE_NOTE                       0x1C
//NYI       case BIFF_CODE_SELECTION                  0x1D
//NYI       case BIFF_CODE_FORMAT                     0x1E
//NYI       case BIFF_CODE_FORMATCOUNT                0x1F
//NYI       case BIFF_CODE_COLUMN_DEFAULT             0x20
//NYI       case BIFF_CODE_ARRAY                      0x21
//NYI       case BIFF_CODE_1904                       0x22
//NYI       case BIFF_CODE_EXTERNNAME                 0x23
//NYI       case BIFF_CODE_COLWIDTH                   0x24
//NYI       case BIFF_CODE_DEFAULT_ROW_HEIGHT         0x25
//NYI       case BIFF_CODE_LEFT_MARGIN                0x26
//NYI       case BIFF_CODE_RIGHT_MARGIN               0x27
//NYI       case BIFF_CODE_TOP_MARGIN                 0x28
//NYI       case BIFF_CODE_BOTTOM_MARGIN              0x29
//NYI       case BIFF_CODE_PRINT_ROW_HEADERS          0x2A
//NYI       case BIFF_CODE_PRINT_GRIDLINES            0x2B
//NYI       case BIFF_CODE_FILEPASS                   0x2F
//NYI       case BIFF_CODE_FONT                       0x31 /* @ FONT - Font Description */
//NYI       case BIFF_CODE_FONT2                      0x32
//NYI       case BIFF_CODE_TABLE                      0x36
//NYI       case BIFF_CODE_TABLE2                     0x37
//NYI       case BIFF_CODE_CONTINUE                   0x3C /* CONTINUE - Continues Long Records */
//NYI       case BIFF_CODE_WINDOW1                    0x3D /* WINDOW1 - Window Information */
//NYI       case BIFF_CODE_WINDOW2                    0x3E
//NYI       case BIFF_CODE_BACKUP                     0x40 /* BACKUP - Save Backup Version of the File */
//NYI       case BIFF_CODE_PANE                       0x41  /* PANE - Number of Panes and Their Position */

// The following are BIFF3 or later   @ indicates I could probably implement this record

/* The following records are required for BIFF5 files
Required Records
BOF - Set the 6 byte offset to 0x0005 (workbook globals)
Window1
FONT - At least five of these records must be included
XF - At least 15 Style XF records and 1 Cell XF record must be included
STYLE
BOUNDSHEET - Include one BOUNDSHEET record per worksheet
EOF
BOF - Set the 6 byte offset to 0x0010 (worksheet)
INDEX
DIMENSIONS
WINDOW2
EOF
*/

//NYI       case  BIFF4_CODE_CODEPAGE            0x42 /* @ CODEPAGE - Default Code Page */
// 0x43-0x4C not used
//NYI       case   BIFF4_CODE_PLS                   0x4D /* PLS - Environment-Specific Print Record */
// 0x4E-0x4F not used
//NYI       case   BIFF4_CODE_DCON                   0x50 /* DCON - Data Consolidation Information */
//NYI       case   BIFF4_CODE_DCONREF                   0x51     /* DCONREF - Data Consolidation References */
//NYI       case   BIFF4_CODE_DCONNAME                   0x52 /* DCONNAME - Data Consolidation Named References */
// 0x53-0x54 not used
//NYI       case   BIFF4_CODE_DEFCOLWIDTH                   0x55 /* DEFCOLWIDTH - Default Width for Columns */
// 0x65-0x58 not used
//NYI       case   BIFF4_CODE_XCT                   0x59 /* XCT - CRN Record Count */
//NYI       case   BIFF4_CODE_CRN                   0x5A /* CRN - Nonresident Operands */
//NYI       case   BIFF4_CODE_FILESHARING                   0x5B  /* FILESHARING - File-Sharing Information */
//NYI       case   BIFF4_CODE_WRITEACCESS                   0x5C  /* @ WRITEACCESS - Write Access User Name */
//NYI       case   BIFF4_CODE_OBJ                   0x5D  /* OBJ - Describes a Graphic Object */
//NYI       case   BIFF4_CODE_UNCALCED                   0x5E /* UNCALCED - Recalculation Status */
//NYI       case   BIFF4_CODE_SAVERECALC                   0x5F /* SAVERECALC - Recalculate Before Save */
//NYI       case   BIFF4_CODE_TEMPLATE                   0x60 /* TEMPLATE - Workbook Is a Template */
// 0x61-0x62 not used
//NYI       case   BIFF4_CODE_OBJPROTECT                   0x63 /* OBJPROTECT - Objects Are Protected */
// 0x64-0x7C not used
//NYI       case   BIFF4_CODE_COLINFO                   0x7D /* COLINFO - Column Formatting Information */
//NYI       case   BIFFx_CODE_RK                       0x7E  /* RK - Cell values in RK format */
//NYI       case   BIFF4_CODE_IMDATA                   0x7F /* IMDATA - Image Data */
//NYI       case   BIFF4_CODE_GUTS                   0x80    /* GUTS - Size of Row and Column Gutters */
//NYI       case   BIFF4_CODE_WSBOOL                   0x81 /* WSBOOL - Additional Workspace Information */
//NYI       case   BIFF4_CODE_GRIDSET                   0x82 /* GRIDSET - State Change of Gridlines Option */
//NYI       case   BIFF4_CODE_HCENTER                   0x83 /* HCENTER - Center Between Horizontal Margins */
//NYI       case   BIFF4_CODE_VCENTER                   0x84 /* VCENTER - Center Between Vertical Margins */
//NYI       case   BIFF5_CODE_BOUNDSHEET                   0x85   /* @ BOUNDSHEET - Sheet Information */
//NYI       case   BIFF4_CODE_WRITEPROT                   0x86 /* WRITEPROT - Workbook Is Write-Protected */
//NYI       case   BIFF4_CODE_ADDIN                   0x87  /* ADDIN - Workbook Is an Add-in Macro */
//NYI       case   BIFF4_CODE_EDG                   0x88 /* EDG - Edition Globals */
//NYI       case   BIFF4_CODE_PUB                    0x89  /* PUB - Publisher */
// 0x8A-0x8B not used
//NYI       case   BIFF4_CODE_COUNTRY                   0x8C /* COUNTRY - Default Country and WIN.INI Country */
//NYI       case   BIFF4_CODE_HIDEOBJ                   0x8D /* @ HIDEOBJ - Object Display Options */
// 0x8E-0x8F not used
//NYI       case   BIFF4_CODE_SORT                   0x90  /* SORT - Sorting Options */
//NYI       case   BIFF4_CODE_SUB                   0x91  /* SUB - Subscriber */
//NYI       case   BIFF4_CODE_PALETTE                   0x92  /* PALETTE - Color Palette Definition */

//NYI       case   BIFF4_CODE_LHRECORD                   0x94    /* LHRECORD - .WK? File Conversion Information */
//NYI       case   BIFF4_CODE_LHNGRAPH                   0x95 /* LHNGRAPH - Named Graph Information */
//NYI       case   BIFF4_CODE_SOUND                   0x96    /* SOUND - Sound Note */
//NYI       case   BIFF4_CODE_LPR                   0x98   /* LPR - Sheet Was Printed Using LINE.PRINT */
//NYI       case   BIFF4_CODE_STANDARDWIDTH                   0x99  /* STANDARDWIDTH - Standard Column Width */
//NYI       case   BIFF4_CODE_FNGROUPNAME                   0x9A  /* FNGROUPNAME - Function Group Name */
//NYI       case   BIFF4_CODE_FILTERMODE                   0x9B   /* FILTERMODE - Sheet Contains Filtered List */
//NYI       case   BIFF4_CODE_FNGROUPCOUNT                   0x9C /* FNGROUPCOUNT - Built-in Function Group Count */
//NYI       case   BIFF4_CODE_AUTOFILTERINFO                   0x9D  /* AUTOFILTERINFO - Drop-Down Arrow Count */
//NYI       case   BIFF4_CODE_AUTOFILTER                   0x9E /* AUTOFILTER - AutoFilter Data */

// 0x9F - 0xFE

//NYI       case   BIFF4_CODE_SCL                   0xA0 /* SCL - Window Zoom Magnification */
//NYI       case   BIFF4_CODE_SETUP                   0xA1  /* SETUP - Page Setup */

//NYI       case   BIFF4_CODE_COORDLIST                   0xA9  /* COORDLIST - Polygon Object Vertex Coordinates */

//NYI       case   BIFF4_CODE_GCW                   0xAB  /* GCW - Global Column-Width Flags */

//NYI       case   BIFF4_CODE_SCENMAN                   0xAE  /* SCENMAN - Scenario Output Data */
//NYI       case   BIFF4_CODE_SCENARIO                   0xAF  /* SCENARIO - Scenario Data� */
//NYI       case   BIFF4_CODE_SXVIEW                   0xB0  /* SXVIEW - View Definition */
//NYI       case   BIFF4_CODE_SXVD                   0xB1  /* SXVD - View Fields */
//NYI       case   BIFF4_CODE_SXVI                   0xB2   /* SXVI - View Item */

//NYI       case   BIFF4_CODE_SXIVD                   0xB4  /* SXIVD - Row/Column Field IDs */
//NYI       case   BIFF4_CODE_SXLI                   0xB5 /* SXLI - Line Item Array */
//NYI       case   BIFF4_CODE_SXPI                   0xB6  /* SXPI - Page Item */

//NYI       case   BIFF4_CODE_DOCROUTE                   0xB8  /* DOCROUTE - Routing Slip Information */
//NYI       case   BIFF4_CODE_RECIPNAME                   0xB9 /* RECIPNAME - Recipient Name */


//NYI       case   BIFF4_CODE_SHRFMLA                   0xBC   /* SHRFMLA - Shared Formula */
//NYI       case   BIFF4_CODE_MULRK                   0xBD    /* @ MULRK - Multiple RK Cells */
//NYI       case   BIFF4_CODE_MULBLANK                   0xBE    /* * MULBLANK - Multiple Blank Cells */


//NYI       case   BIFF4_CODE_MMS                   0xC1   /* MMS - ADDMENU/DELMENU Record Group Count */
//NYI       case   BIFF4_CODE_ADDMENU                   0xC2   /* ADDMENU - Menu Addition */
//NYI       case   BIFF4_CODE_DELMENU                   0xC3  /* DELMENU - Menu Deletion */
//      case   BIFFx_CODE_                   0xC4
//NYI       case   BIFF4_CODE_SXDI                   0xC5 /* SXDI - Data Item */
//NYI       case   BIFF8_CODE_SXDB                   0xC6 /* SXDB - PivotTable Cache Data (May have been in BIFF4)*/
//      case   BIFFx_CODE_                   0xC7
//      case   BIFFx_CODE_                   0xC8
//      case   BIFFx_CODE_                   0xC9
//      case   BIFFx_CODE_                   0xCA
//      case   BIFFx_CODE_                   0xCB
//      case   BIFFx_CODE_                   0xCC
//NYI       case   BIFF4_CODE_SXSTRING                   0xCD /* SXSTRING - String */
//      case   BIFFx_CODE_                   0xCE
//      case   BIFFx_CODE_                   0xCF
//NYI       case   BIFF4_CODE_SXTBL                   0xD0  /* SXTBL - Multiple Consolidation Source Info */
//NYI       case   BIFF4_CODE_SXTBRGIITM                   0xD1   /* SXTBRGIITM - Page Item Name Count */
//NYI       case   BIFF4_CODE_SXTBPG                   0xD2    /* SXTBPG - Page Item Indexes */
//NYI       case   BIFF4_CODE_OBPROJ                   0xD3    /* OBPROJ - Visual Basic Project */
//      case   BIFFx_CODE_                   0xD4
//NYI       case   BIFF4_CODE_SXIDSTM                   0xD5   /* SXIDSTM - Stream ID */
//NYI       case   BIFF4_CODE_RSTRING                   0xD6   /* @ RSTRING - Cell with Character Formatting */
//NYI       case   BIFF4_CODE_DBCELL                   0xD7  /* DBCELL - Stream Offsets */
//      case   BIFFx_CODE_                   0xD8
//      case   BIFFx_CODE_                   0xD9
//NYI       case   BIFF4_CODE_BOOKBOOL                   0xDA /* BOOKBOOL - Workbook Option Flag */
//      case   BIFFx_CODE_                   0xDB
//NYI       case   BIFF8_CODE_SXEXT                   0xDC /* PARAMQRY-SXEXT - Query Parameters-External Source Information (May have been in BIFF4)*/
//NYI       case   BIFF4_CODE_SCENPROTECT                   0xDD  /* SCENPROTECT - Scenario Protection */
//NYI       case   BIFF4_CODE_OLESIZE                   0xDE  /* OLESIZE - Size of OLE Object */
//NYI       case   BIFF4_CODE_UDDESC                   0xDF   /* UDDESC - Description String for Chart Autoformat */
//      case   BIFFx_CODE_                   0x
//      case   BIFFx_CODE_                   0x
//NYI       case   BIFF4_CODE_XF                   0xE0  /* XF - Extended Format */
//NYI       case   BIFF4_CODE_INTERFACEHDR                   0xE1    /* @ INTERFACEHDR - Beginning of User Interface Records */
//NYI       case   BIFF4_CODE_INTERFACEEND                   0xE2   /* INTERFACEEND - End of User Interface Records */
//NYI       case   BIFF4_CODE_SXVS                   0x0xE3     /* SXVS - View Source */
//      case   BIFFx_CODE_                   0xE4
//NYI       case   BIFF4_CODE_UNKNOWN_E5                   0xE5  /* ?UNKNOWN1 - Guessing */
//      case   BIFFx_CODE_                   0xE6
//      case   BIFFx_CODE_                   0xE7
//      case   BIFFx_CODE_                   0xE8
//      case   BIFFx_CODE_                   0xE9
//NYI       case   BIFF4_CODE_TABIDCONF                   0xEA /* TABIDCONF - Sheet Tab ID of Conflict History */
//NYI       case   BIFF8_CODE_MSODRAWINGGROUP                   0xEB   /* MSODRAWINGGROUP - Microsoft Office Drawing Group (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_MSODRAWING                   0xEC /* MSODRAWING - Microsoft Office Drawing (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_MSODRAWINGSELECTION                   0xED /* MSODRAWINGSELECTION - Microsoft Office Drawing Selection (May have been in BIFF4)*/
//      case   BIFFx_CODE_                   0xEF
//NYI       case   BIFF8_CODE_SXRULE                   0xF0  /* SXRULE - PivotTable Rule Data (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_SXEX                   0xF1 /* SXEX - PivotTable View Extended Information (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_SXFILT                   0xF2  /* SXFILT - PivotTable Rule Filter (May have been in BIFF4)*/
//      case   BIFFx_CODE_                   0xF3
//      case   BIFFx_CODE_                   0xF4
//      case   BIFFx_CODE_                   0xF5
//NYI       case   BIFF8_CODE_SXNAME                   0xF6 /* SXNAME - PivotTable Name (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_SXSELECT                   0xF7 /* SXSELECT - PivotTable Selection Information (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_SXPAIR                   0xF8  /* SXPAIR - PivotTable Name Pair (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_SXFMLA                   0xF9    /* SXFMLA - PivotTable Parsed Expression (May have been in BIFF4)*/
//      case   BIFFx_CODE_                   0xFA
//NYI       case   BIFF8_CODE_SXFORMAT                   0xFB    /* SXFORMAT - PivotTable Format Record (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_SST                   0xFC /* @ SST - Shared String Table (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_LABELSST                   0xFD /* @ LABELSST - Cell Value, String Constant/SST (May have been in BIFF4)*/
//      case   BIFFx_CODE_                   0xFE
//NYI       case   BIFF8_CODE_EXTSST                   0xFF  /* EXTSST - Extended Shared String Table (May have been in BIFF4)*/

//NYI       case   BIFF8_CODE_SXVDEX                   0x100  /* SXVDEX - Extended PivotTable View Fields (May have been in BIFF4)*/
//      case   BIFFx_CODE_                   0x101
//      case   BIFFx_CODE_                   0x102
//NYI       case   BIFF8_CODE_SXFORMULA                   0x103 /* SXFORMULA - PivotTable Formula Record (May have been in BIFF4)*/
//      case   BIFFx_CODE_                   0x
//NYI       case   BIFF8_CODE_SXDBEX                   0x122 /* SXDBEX - PivotTable Cache Data (May have been in BIFF4)*/
//      case   BIFFx_CODE_                   0x
//NYI       case   BIFF4_CODE_TABID                   0x13D
//      case   BIFFx_CODE_                   0x
//NYI       case   BIFF8_CODE_USESELFS                   0x160  /* USESELFS - Natural Language Formulas Flag (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_DSF                   0x161       /* DSF - Double Stream File (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_XL5MODIFY                   0x162  /* XL5MODIFY - Flag for DSF (May have been in BIFF4)*/
//      case   BIFFx_CODE_                   0x
//NYI       case   BIFF4_CODE_FILESHARING2                   0x1A5 /* FILESHARING2 - File-Sharing Information for Shared Lists */
//      case   BIFFx_CODE_                   0x1A6
//      case   BIFFx_CODE_                   0x1A7
//      case   BIFFx_CODE_                   0x1A8
//NYI       case   BIFF8_CODE_USERBVIEW                   0x1A9 /* USERBVIEW - Workbook Custom View Settings (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_USERSVIEWBEGIN                   0x1AA   /* USERSVIEWBEGIN - Custom View Settings (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_USERSVIEWEND                   0x1AB     /* USERSVIEWEND - End of Custom View Records (May have been in BIFF4)*/
//NYI       case   BIFF4_CODE_unused_1AC                 0x1AC
//NYI       case   BIFF8_CODE_QSI                   0x1AD    /* QSI - External Data Range (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_SUPBOOK                   0x1AE   /* SUPBOOK - Supporting Workbook (May have been in BIFF4)*/
//NYI       case   BIFF4_CODE_PROT4REV                   0x1AF /* Shared Workbook Protection Flag */
//NYI       case   BIFF8_CODE_CONDFMT                   0x1B0 /* CONDFMT - Conditional Formatting Range Information (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_CF                   0x1B1 /* CF - Conditional Formatting Conditions (May have been in BIFF4) */
//NYI       case   BIFF8_CODE_DVAL                   0x1B2 /* DVAL - Data Validation Information (May have been in BIFF4)*/

//NYI       case   BIFF8_CODE_DCONBIN                   0x1B5 /* DCONBIN - Data Consolidation Information (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_TXO                   0x1B6 /* TXO - Text Object (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_REFRESHALL                   0x1B7   /* REFRESHALL - Refresh Flag (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_HLINK                   0x1B8 /* HLINK - Hyperlink (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_CODENAME                   0x1BA /* CODENAME - xxx*/
//NYI       case   BIFF8_CODE_SXFDBTYPE                   0x1BB          /* SXFDBTYPE - SQL Datatype Identifier (May have been in BIFF4)*/
//NYI       case   BIFF8_CODE_PROT4REVPASS                   0x1BC  /* PROT4REVPASS - Shared Workbook Protection Password (May have been in BIFF4)*/
////      case   BIFFx_CODE_                   0x1BD
//NYI       case   BIFF8_CODE_DV                   0x1BE /* DV - Data Validation Criteria (May have been in BIFF4)*/

// 0x1BF - 1FF not yet defined

//NYI       case   BIFF4_CODE_DIMENSIONS                   0x200 /* DIMENSIONS - Cell Table Size */
//NYI       case   BIFF4_CODE_BLANK                   0x201 /* BLANK - Cell Value, Blank Cell */
//      case   BIFFx_CODE_                   0x202
//NYI       case   BIFF4_CODE_NUMBER                   0x203 /* NUMBER - Cell Value, Floating-Point Number */
//NYI       case   BIFF4_CODE_LABEL                   0x204 /* LABEL - Cell Value, String Constant */
//NYI       case   BIFF4_CODE_BOOLERR                   0x205 /* BOOLERR - Cell Value, Boolean or Error */
//      case   BIFFx_CODE_                   0x206
//NYI       case   BIFF4_CODE_STRING                   0x207  /* STRING - String Value of a Formula */
//NYI       case   BIFF4_CODE_ROW                   0x208 /* ROW - Describes a Row */
//      case   BIFFx_CODE_                   0x20A
//NYI       case   BIFF4_CODE_INDEX                   0x20B /* INDEX - Index Record */
//NYI       case   BIFF4_CODE_NAME                   0x218  /* NAME - Defined Name */
////      case   BIFFx_CODE_                   0x
////      case   BIFFx_CODE_                   0x
////      case   BIFFx_CODE_                   0x
////      case   BIFFx_CODE_                   0x
//NYI       case   BIFF4_CODE_ARRAY                   0x221  /* ARRAY - Array-Entered Formula */
////      case   BIFFx_CODE_                   0x222
//NYI       case   BIFF4_CODE_EXTERNNAME                   0x223 /* EXTERNNAME - Externally Referenced Name */
////      case   BIFFx_CODE_                   0x224
//NYI       case   BIFF4_CODE_DEFAULTROWHEIGHT                   0x225 /* DEFAULTROWHEIGHT - Default Row Height */

//NYI       case   BIFF4_CODE_FONT                  0x231
//NYI
//NYI       case   BIFF4_CODE_TABLE                   0x236 /* TABLE - Data Table (enchanced)*/

//NYI       case   BIFF4_CODE_WINDOW2                   0x23E /* WINDOW2 - Sheet Window Information (enhanced WINDOW2)*/

//NYI       case   BIFF4_CODE_RK                   0x27E /* RK - Cell Value, RK Number */

//NYI       case   BIFF4_CODE_STYLE                   0x293     /* STYLE - Style Information */

//NYI       case   BIFFx_CODE_FORMULA                 0x406 /* FORMULA - Cell formula */


//NYI       case   BIFF4_CODE_FORMAT                   0x41E  /* FORMAT - Number Format */

////      case   BIFFx_CODE_                   0x4BC /* ?FORMULA-RELATED=?(BC=SHRFMLA)) - Formula related, always before there are 0x06 (FORMULA) */



      default :
      {
//								  record = new XXX_record_base(); // <- this catchall should be avoided
      }
   };
   return record;
};
//_1999-01-14___________________________________________________________________
uint8 DEF_BIFF57_XF[22][16] =
{{0x00,0x00,0x00,0x00,0xF5,0xFF,0x20,0x00,0xC0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x01,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x01,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x02,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x02,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x00,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x00,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x00,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x00,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x00,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x00,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x00,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x00,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x00,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x00,0x00,0x00,0x00,0xf5,0xff,0x20,0xf4,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x00,0x00,0x00,0x00,0x01,0x00,0x20,0x00,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x01,0x00,0x2b,0x00,0xf5,0xff,0x20,0xf8,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x01,0x00,0x29,0x00,0xf5,0xff,0x20,0xf8,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x01,0x00,0x2c,0x00,0xf5,0xff,0x20,0xf8,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x01,0x00,0x2a,0x00,0xf5,0xff,0x20,0xf8,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x01,0x00,0x09,0x00,0xf5,0xff,0x20,0xf8,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
,{0x00,0x00,0xa4,0x00,0x01,0x00,0x20,0x04,0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00}
};
//______________________________________________________________________________
Workbook::Workbook
(const char    *_filename
,std::ios_base::openmode _openmode                                               //130401
,BIFF_Format    _format                                                          //100110
)
:Binary_record_file_dynamic_indexed
   (_filename,false,_openmode,false)
,CORN::Workbook()                                                                //190107
,format(_format)
,window1(0)
,cell_extended_formats()
,style_extended_formats()
,worksheets()
,charts()
,FRT_records()
{  // May need to move this to initialize
   {  if (format >= BIFF5)
         window1 = new BIFF578_rec_WINDOW1();
      for (uint8 f = 0; f < 5; f++)
            font[f] =
#ifdef NYI
                 (format == BIFF2) ?                        new BIFF2_rec_FONT
               : ((format == BIFF3) || (format == BIFF4)) ?  new BIFF34_rec_FONT
               : ((format == BIFF5) || (format == BIFF7)) ?  new BIFF57_rec_FONT
               :  (format == BIFF8)  ? NULL // NYI           new BIFF8_rec_FONT
               :
#endif
                                NULL;
      for (uint8 x = 0; x < 22; x++)
            xf[x] = ((format == BIFF5) || (format == BIFF7)) ? new BIFF57_rec_XF(DEF_BIFF57_XF[x])
               // NYI  ((format == BIFF2) ) ? new BIFF3_rec_XF(DEF_BIFF2_XF[x])
               // NYI  ((format == BIFF3) ) ? new BIFF4_rec_XF(DEF_BIFF3_XF[x])
               // NYI  ((format == BIFF4) ) ? new BIFF5_rec_XF(DEF_BIFF4_XF[x])
               // NYI  ((format == BIFF8) ) ? new BIFF8_rec_XF(DEF_BIFF8_XF[x])
               : 0;
      if (format >= BIFF3)
      {  style[0] = new BIFF34578_rec_STYLE(true,0x10,BIFF34578_rec_STYLE::Comma,0xFF);
         style[1] = new BIFF34578_rec_STYLE(true,0x11,BIFF34578_rec_STYLE::Comma_0,0xFF);
         style[2] = new BIFF34578_rec_STYLE(true,0x12,BIFF34578_rec_STYLE::Currency,0xFF);
         style[3] = new BIFF34578_rec_STYLE(true,0x13,BIFF34578_rec_STYLE::Currency_0,0xFF);
         style[4] = new BIFF34578_rec_STYLE(true,0x00,BIFF34578_rec_STYLE::Normal,0xFF);
         style[5] = new BIFF34578_rec_STYLE(true,0x14,BIFF34578_rec_STYLE::Percent,0xFF);
      }
      else
      {  style[0] = 0;
         style[1] = 0;
         style[2] = 0;
         style[3] = 0;
         style[4] = 0;
         style[5] = 0;
      }
   }
}
//_Excel_workbook:constructor__________________________________________________/
Workbook::Sheet *Workbook::provide_sheet(const std::string &sheet_name) provision_
{
   std::string valid_sheet_name(sheet_name);                                     //190107
   if (valid_sheet_name.empty())                                                 //190107
      valid_sheet_name.assign("Sheet 1");                                        //190107
   if (recent_sheet &&
       recent_sheet->is_key_string(valid_sheet_name))                            //180820
      return recent_sheet;                                                       //100110
   Worksheet /*190113 Excel_worksheet*/ *sheet = dynamic_cast<Worksheet /*190113 Excel_worksheet*/ *>
      (worksheets.find_string(valid_sheet_name));
   if (!sheet)
   {  // If the named sheet doesn't exist, create it.
      sheet = new Worksheet //190113Excel_worksheet
         (valid_sheet_name,BIFF_rec_BOUNDSHEET_common::WORKSHEET,format);
      worksheets.take/*190113 append*/(sheet);
   }
   recent_sheet = sheet;
   return recent_sheet;
}
//_provide_sheet_______________________________________________________________/
//190112Excel_worksheet *Workbook::take_sheet(Excel_worksheet *sheet_to_add)
Worksheet *Workbook::take_sheet(Worksheet *sheet_to_add)
{  worksheets.append(sheet_to_add);
   recent_sheet = sheet_to_add;                                                  //100110
   return sheet_to_add;
}
//_take_sheet__________________________________________________________________/
void Workbook::commit()
{
   index.delete_all(); // Delete all the current index nodes //101126 index is now a member instead of superclass

/*
http://support.microsoft.com/kb/147732
The records listed below must be included in order for Microsoft Excel to
recognize the file as a valid BIFF5 file. Because BIFF5 files are OLE compound document files,
these records must be written using OLE library functions.
For information on how to output an OLE docfile, see the OLE 2 Programmer's Reference Volume One.
BOF - Set the 6 byte offset to 0x0005 (workbook globals)
Window1
FONT - At least five of these records must be included
XF - At least 15 Style XF records and 1 Cell XF record must be included
STYLE
BOUNDSHEET - Include one BOUNDSHEET record per worksheet
EOF
BOF - Set the 6 byte offset to 0x0010 (worksheet)
INDEX
DIMENSIONS
WINDOW2
EOF
*/
   if (format >= BIFF5)
   {  // BIFF5 and greater require workbook globals
      BIFF_rec_BOF *BOF_globals_rec = 0;
      switch (format)
      {  case BIFF5 : // case BIFF6 :
            BOF_globals_rec = new BIFF57_rec_BOF(BIFF_rec_BOF::WORKBOOK_GLOBALS);
         break;
         case BIFF7 :
            BOF_globals_rec = new BIFF57_rec_BOF(BIFF_rec_BOF::WORKBOOK_GLOBALS);
         break;
         case BIFF8 :
            BOF_globals_rec = new BIFF8_rec_BOF(BIFF_rec_BOF::WORKBOOK_GLOBALS);
         break;
      }
      if (BOF_globals_rec) // should always be the case.

         append_record(BOF_globals_rec,true); // Work book globals

/*  Example of a minimum global section (Excel 95)
0000C: [INTERFACEHDR] (E1h  225dec)
00000  e1 00 00 00 -- -- -- -- -- -- -- -- -- -- -- --  a...
00010: [MMS] (C1h  193dec)
00000  c1 00 02 00 00 00 -- -- -- -- -- -- -- -- -- --  A.....
00016: [TOOLBARHDR] (BFh  191dec)
00000  bf 00 00 00 -- -- -- -- -- -- -- -- -- -- -- --  ?...
0001A: [TOOLBAREND] (C0h  192dec)
00000  c0 00 00 00 -- -- -- -- -- -- -- -- -- -- -- --  @...
0001E: [INTERFACEEND] (E2h  226dec)
00000  e2 00 00 00 -- -- -- -- -- -- -- -- -- -- -- --  b...
00022: [WRITEACCESS] (5Ch  92dec)
00000  5c 00 70 00 0c 52 6f 67 65 72 20 4e 65 6c 73 6f  \.p..Roger Nelso
00010  6e 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20  n
00020  20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
00030  20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
00040  20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
00050  20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
00060  20 20 20 20 20 20 20 20 20 20 20 20 20 20 20 20
00070  20 20 20 20 -- -- -- -- -- -- -- -- -- -- -- --
00096: [CODEPAGE] (42h  66dec)
00000  42 00 02 00 e4 04 -- -- -- -- -- -- -- -- -- --  B...d.
0009C: [FNGROUPCOUNT] (9Ch  156dec)
00000  9c 00 02 00 0e 00 -- -- -- -- -- -- -- -- -- --  ......
000A2: [WINDOWPROTECT] (19h  25dec)
00000  19 00 02 00 00 00 -- -- -- -- -- -- -- -- -- --  ......
000A8: [PROTECT] (12h  18dec)
00000  12 00 02 00 00 00 -- -- -- -- -- -- -- -- -- --  ......
000AE: [PASSWORD] (13h  19dec)
00000  13 00 02 00 00 00 -- -- -- -- -- -- -- -- -- --  ......
*/

      if (window1) // In workbook versions, window1 appears in globals
                  // in earlier versions window1 appears in the sheet
         append_record(window1,true);     // WINDOW
/*  NYI  records that are optional?
000CA: [BACKUP] (40h  64dec)
00000  40 00 02 00 00 00 -- -- -- -- -- -- -- -- -- --  @.....
000D0: [HIDEOBJ] (8Dh  141dec)
00000  8d 00 02 00 00 00 -- -- -- -- -- -- -- -- -- --  ......
000D6: [1904] (22h  34dec)
00000  22 00 02 00 00 00 -- -- -- -- -- -- -- -- -- --  ".....
000DC: [PRECISION] (Eh  14dec)
00000  0e 00 02 00 01 00 -- -- -- -- -- -- -- -- -- --  ......
000E2: [REFRESHALL] (1B7h  439dec)
00000  b7 01 02 00 00 00 -- -- -- -- -- -- -- -- -- --  7.....
000E8: [BOOKBOOL] (DAh  218dec)
00000  da 00 02 00 00 00 -- -- -- -- -- -- -- -- -- --  Z.....
*/
   if (format >= BIFF7)
   {
      for (uint8 f = 0; f < 5; f++)
         if (font[f])
            append_record(font[f],true);  // FONT - At least five of these records must be included


/* NYI FORMAT (optional?) In BIFF7  some formats are intrinsic
0014E: [FORMAT] (1Eh  30dec)
00000  1e 04 1a 00 05 00 17 22 24 22 23 2c 23 23 30 5f  ......."$"#,##0_
00010  29 3b 5c 28 22 24 22 23 2c 23 23 30 5c 29 -- --  );\("$"#,##0\)
0016C: [FORMAT] (1Eh  30dec)
00000  1e 04 1f 00 06 00 1c 22 24 22 23 2c 23 23 30 5f  ......."$"#,##0_
00010  29 3b 5b 52 65 64 5d 5c 28 22 24 22 23 2c 23 23  );[Red]\("$"#,##
00020  30 5c 29 -- -- -- -- -- -- -- -- -- -- -- -- --  0\)
0018F: [FORMAT] (1Eh  30dec)
00000  1e 04 20 00 07 00 1d 22 24 22 23 2c 23 23 30 2e  .. ...."$"#,##0.
00010  30 30 5f 29 3b 5c 28 22 24 22 23 2c 23 23 30 2e  00_);\("$"#,##0.
00020  30 30 5c 29 -- -- -- -- -- -- -- -- -- -- -- --  00\)
001B3: [FORMAT] (1Eh  30dec)
00000  1e 04 25 00 08 00 22 22 24 22 23 2c 23 23 30 2e  ..%...""$"#,##0.
00010  30 30 5f 29 3b 5b 52 65 64 5d 5c 28 22 24 22 23  00_);[Red]\("$"#
00020  2c 23 23 30 2e 30 30 5c 29 -- -- -- -- -- -- --  ,##0.00\)
001DC: [FORMAT] (1Eh  30dec)
00000  1e 04 35 00 2a 00 32 5f 28 22 24 22 2a 20 23 2c  ..5.*.2_("$"* #,
00010  23 23 30 5f 29 3b 5f 28 22 24 22 2a 20 5c 28 23  ##0_);_("$"* \(#
00020  2c 23 23 30 5c 29 3b 5f 28 22 24 22 2a 20 22 2d  ,##0\);_("$"* "-
00030  22 5f 29 3b 5f 28 40 5f 29 -- -- -- -- -- -- --  "_);_(@_)
00215: [FORMAT] (1Eh  30dec)
00000  1e 04 2c 00 29 00 29 5f 28 2a 20 23 2c 23 23 30  ..,.).)_(* #,##0
00010  5f 29 3b 5f 28 2a 20 5c 28 23 2c 23 23 30 5c 29  _);_(* \(#,##0\)
00020  3b 5f 28 2a 20 22 2d 22 5f 29 3b 5f 28 40 5f 29  ;_(* "-"_);_(@_)
00245: [FORMAT] (1Eh  30dec)
00000  1e 04 3d 00 2c 00 3a 5f 28 22 24 22 2a 20 23 2c  ..=.,.:_("$"* #,
00010  23 23 30 2e 30 30 5f 29 3b 5f 28 22 24 22 2a 20  ##0.00_);_("$"*
00020  5c 28 23 2c 23 23 30 2e 30 30 5c 29 3b 5f 28 22  \(#,##0.00\);_("
00030  24 22 2a 20 22 2d 22 3f 3f 5f 29 3b 5f 28 40 5f  $"* "-"??_);_(@_
00040  29 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --  )
00286: [FORMAT] (1Eh  30dec)
00000  1e 04 34 00 2b 00 31 5f 28 2a 20 23 2c 23 23 30  ..4.+.1_(* #,##0
00010  2e 30 30 5f 29 3b 5f 28 2a 20 5c 28 23 2c 23 23  .00_);_(* \(#,##
00020  30 2e 30 30 5c 29 3b 5f 28 2a 20 22 2d 22 3f 3f  0.00\);_(* "-"??
00030  5f 29 3b 5f 28 40 5f 29 -- -- -- -- -- -- -- --  _);_(@_)
002BE: [FORMAT] (1Eh  30dec)
00000  1e 04 06 00 a4 00 03 30 2e 30 -- -- -- -- -- --  ....$..0.0
*/
     for (uint8 x = 0; x < 22; x++)
        if (xf[x])
            append_record(xf[x],true);    // XF - At least 15 Style XF records and 1 Cell XF record must be included

     for (uint8 s = 0; s < 6; s++)
        if (style[s])
            append_record(style[s],true); // STYLE

/*  NYI (PALETTE optional?)
004B0: [PALETTE] (92h  146dec)
00000  92 00 e2 00 38 00 00 00 00 00 ff ff ff 00 ff 00  ..b.8...........
00010  00 00 00 ff 00 00 00 00 ff 00 ff ff 00 00 ff 00  ................
00020  ff 00 00 ff ff 00 80 00 00 00 00 80 00 00 00 00  ................
00030  80 00 80 80 00 00 80 00 80 00 00 80 80 00 c0 c0  ..............@@
00040  c0 00 80 80 80 00 99 99 ff 00 99 33 66 00 ff ff  @..........3f...
00050  cc 00 cc ff ff 00 66 00 66 00 ff 80 80 00 00 66  L.L...f.f......f
00060  cc 00 cc cc ff 00 00 00 80 00 ff 00 ff 00 ff ff  L.LL............
00070  00 00 00 ff ff 00 80 00 80 00 80 00 00 00 00 80  ................
00080  80 00 00 00 ff 00 00 cc ff 00 cc ff ff 00 cc ff  .......L..L...L.
00090  cc 00 ff ff 99 00 99 cc ff 00 ff 99 cc 00 cc 99  L......L....L.L.
000a0  ff 00 e3 e3 e3 00 33 66 ff 00 33 cc cc 00 99 cc  ..ccc.3f..3LL..L
000b0  00 00 ff cc 00 00 ff 99 00 00 ff 66 00 00 66 66  ...L.......f..ff
000c0  99 00 96 96 96 00 00 33 66 00 33 99 66 00 00 33  .......3f.3.f..3
000d0  00 00 33 33 00 00 99 33 00 00 99 33 66 00 33 33  ..33...3...3f.33
000e0  99 00 33 33 33 00 -- -- -- -- -- -- -- -- -- --  ..333.
*/


                  // Include one BOUNDSHEET record per worksheet
//100111                  for (Excel_worksheet *sheet = (Excel_worksheet *)worksheets.move_to_head()
//100111                      ;sheet
//100111                      ;sheet = (Excel_worksheet *)worksheets.move_to_next())
         FOR_EACH_IN(sheet,Worksheet /*190113 Excel_worksheet*/,worksheets,each_worksheet)               //100111
         {   append_record(sheet->relinquish_boundsheet(),true);
         } FOR_EACH_END(each_worksheet)
      }
       append_record(new BIFF_rec_EOF(),true);
   }
   //190113while (Excel_worksheet *sheet = dynamic_cast<Excel_worksheet *>(worksheets.pop_at_head()))
   while (Worksheet *sheet = dynamic_cast<Worksheet *>(worksheets.pop_at_head()))
      sheet->commit(*this);
   for (uint8 f = 0; f < 5; f++)    font[f] = 0;
   for (uint8 x = 0; x < 22; x++)   xf[x] = 0;
   for (uint8 s = 0; s < 6; s++)    style[s] = 0;
}
//______________________________________________________________________________
const std::string &Workbook::get_name(std::string &return_name)  const
{
  //  BIG WARNING  currently I am simply returning the unqualified filename
  //  BUT there is indeed a name record (218h) that defines the workbook name
  //  that I should be getting name name from

  return_name.assign(get_file_name());   // THIS NEEDS TO BE REDONE
  return return_name;
}
//_2010-01-10___________________________________________________________________
Workbook::~Workbook()
{
   if (!is_read_only())
      commit();
}
//______________________________________________________________________________
bool Workbook::save(bool display_alerts)                              stream_IO_
{  commit();
   return true;
}
//_save_____________________________________________________________________________2019-01-13_/
bool Workbook::save_as
(const std::wstring &filename,bool display_alerts)                    stream_IO_
{
   assert(false); // NYI need to reassign filename
   commit();
   return true;
}
//_save_____________________________________________________________________________2019-01-13_/

/*
reading work book


Binary record file constructor will build index

while not empty
pop each Binary_record_index_node off the index
load the record into memory.

if the file is an early (single sheet) version of BIFF
simply create the sheet
otherwise
   get the workbook globals block
   get each of the worksheets
   currently ignore charts ect.
*/
//______________________________________________________________________________
} // namespace Excel_BIFF                                                             //190107
}//_namespace_CORN_____________________________________________________________/

