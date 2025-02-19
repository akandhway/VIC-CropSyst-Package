#include <corn/format/html/table.h>
#include <corn/format/html/body.h>

namespace HTML
{
//______________________________________________________________________________
const char *scope_label_table[] =
{"DEFAULT"
,"ROW"
,"Col"
,"ROWGROUP"
,"Colgroup"
,0};
//______________________________________________________________________________
Table::Table(Body *i_parent_element)
   :Element(M_TABLE,i_parent_element)
   ,Dimension_options()
   ,Align_option()
   ,border        ("BORDER")
   ,cellspacing   ("CELLSPACING")
   ,cellpadding   ("CELLPADDING")
{}
//______________________________________________________________________________
void Table::options()
{
   Element::options();
   Dimension_options::write(document);
   Align_option::write(document);
   border.write(document);
   cellspacing.write(document);
   cellpadding.write(document);;
   /* NYI
   Frame          frame;
   rules
   CORN_string    summary;
   HTML_color     bgcolor;
   HTML_color     bordercolor;
   HTML_color     bordercolordark;
   HTML_color     bordercolorlight;
   uint16         cols;
   uint16         datapagesize;     // ???
   */
}
//______________________________________________________________________________
void Table::Table_element::options()
{
   Element::options();
   Attribute_options::write(document);
   Align_option::write(document);
   Valign_option::write(document);
}
//______________________________________________________________________________
void Table::Ext_table_element::options()
{
   Table_element::options();
   bgcolor.write(document);

   // NYI char
   // NYI charoff

}
//______________________________________________________________________________
void Table::Table_element_block::options()
{
   Ext_table_element::options();
   Height_option::write(document);
   nowrap.write(document);
   bordercolor.write(document);
   bordercolordark.write(document);
   bordercolorlight.write(document);
   background.write(document);
}
//______________________________________________________________________________
void Table::Colgroup::options()
{
   Ext_table_element::options();
   Width_option::write(document);
   span.write(document);
}
//______________________________________________________________________________
void Table::Col::options()
{
   Ext_table_element::options();
   Width_option::write(document);
   span.write(document);
}
//______________________________________________________________________________
void Table::Tr::Table_cell::options()
{
   Table_element_block::options();
   Width_option::write(document);

   colspan.write(document);
   rowspan.write(document);
   abbr.write(document);
   axis.write(document);
   scope.write(document);
//NYI         headers.write(document);
}
//______________________________________________________________________________
}//_namespace_HTML_____________________________________________________________/

