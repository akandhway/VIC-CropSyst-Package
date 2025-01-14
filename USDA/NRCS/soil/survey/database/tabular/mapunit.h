
#ifndef mapunitH
#define mapunitH
#include "mapunit_struct.h"
#include "corn/parameters/parameter.h"
#include "corn/data_source/datarec.h"
namespace USDA_NRCS
{
//______________________________________________________________________________
class  Mapunit_record
: public Mapunit_struct
, public CORN::Data_record
{
 public:
   CORN::Parameter_properties_c_str p_musym;          // Mapunit Symbol   6
   CORN::Parameter_properties_c_str p_muname;         // Mapunit Name   175
   CORN::Parameter_properties_c_str p_mukind;         // Kind   254     Mapunit_kind
   CORN::Parameter_properties_c_str p_mustatus;       // Status  254      Mapunit_status
   CORN::Parameter_number p_muacres;                  // Total Acres  0 acres

   CORN::Parameter_number p_mapunitlfw_l;             // Linear Feature Width     meters
   CORN::Parameter_number p_mapunitlfw_r;             // Linear Feature Width     meters
   CORN::Parameter_number p_mapunitlfw_h;             // Linear Feature Width     meters

   CORN::Parameter_number p_mapunitpfa_l;             // Point Feature Area 1 0.1 10 acres
   CORN::Parameter_number p_mapunitpfa_r;             // Point Feature Area 1 0.1 10 acres
   CORN::Parameter_number p_mapunitpfa_h;             // Point Feature Area 1 0.1 10 acres
   CORN::Parameter_properties_c_str p_farmlndcl;      // Farm Class   254 Farmland_classification
   CORN::Parameter_properties_c_str  p_muhelcl;       // HEL   254 Mapunit_HEL_class
   CORN::Parameter_properties_c_str  p_muwathelcl;    // HEL Water   254 Mapunit_HEL_class
   CORN::Parameter_properties_c_str  p_muwndhelcl;    // HEL Wind   254 Mapunit_HEL_class
   CORN::Parameter_properties_c_str  p_interpfocus;   // Interpretive Focus   30
   CORN::Parameter_properties_c_str  p_invesintens;   // Order of Mapping   254 Investigation_intensity
   CORN::Parameter_number p_iacornsr;                 // IA CSR   5 100
   CORN::Parameter_properties_c_str   p_nhiforsoigrp; // NH Forest Soil Grp   254 NH_Important_forest_soil_group
   CORN::Parameter_number p_nhspiagr;                 // NH SPI Agr   (prcsn 1) 0 100
   CORN::Parameter_properties_c_str p_vtsepticsyscl;  // VT Septic System   254  VT_Septic_system_class
   CORN::Parameter_properties_c_str p_mucertstat;     // Map Unit Certification Status 254  Mapunit_certification_status
   CORN::Parameter_properties_c_str p_lkey;           // Legend Key   30
   CORN::Parameter_properties_c_str p_mukey;          // Mapunit  30
 public:
   Mapunit_record();
   virtual bool expect_structure(bool for_write) ;
};
//_Mapunit_record______________________________________________________________/
}//_namespace USDA_NRCS________________________________________________________/
#endif
