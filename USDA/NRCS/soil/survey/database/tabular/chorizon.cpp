//---------------------------------------------------------------------------
#include "chorizon.h"
//---------------------------------------------------------------------------
using namespace CORN;
namespace USDA_NRCS
{
//______________________________________________________________________________
Parameter_range PPR_percent_0_100_integer  ={0,100,0,100};
Parameter_range PPR_percent_0_100_f1       ={0,100,0,100};
Parameter_range PPR_percent_0_100_f2       ={0,100,0,100};
Parameter_range PPR_db                     ={0.02,2.6,0.02, 2.6};

//1 hzname Designation String  12
Parameter_properties_literal PP_desgndisc          = {"desgndisc"       ,"desgndisc"         ,"desgndisc","Caption","Hint","HTTP:\\descriptionURL"         ,UC_unitless};    Parameter_range PPR_desgndisc={2,99,2,99}; // PPF_8_0_10;
//Parameter_properties_literal PP_desgnmaster        = {"desgnmaster"     ,"desgnmaster"       ,DFW,"Master Choice","Hint","HTTP:\\descriptionURL"   ,UC_unitless};    Parameter_range PPN_desgnmaster={min_warn,max_warn,min_error,max_error, precision,10 }; 254 horz_desgn_master
//Parameter_properties_literal PP_desgnmasterprime   = {"desgnmasterprime","desgnmasterprime"  ,DFW,"Prime Choice","Hint","HTTP:\\descriptionURL"    ,UC_unitless};    Parameter_range PPN_desgnmasterprime={min_warn,max_warn,min_error,max_error, precision,10 }; 254 horz_desgn_master_prime
Parameter_properties_literal PP_desgnvert          = {"desgnvert"       ,"desgnvert"         ,"desgnvert"         ,"Sub","Hint","HTTP:\\descriptionURL"             ,UC_unitless};    Parameter_range PPR_desgnvert={0,99999,0,9999}; // PPF_8_0_10
Parameter_properties_literal PP_hzdept_l           = {"hzdept_l"        ,"hzdept_l"          ,"hzdept_l"          ,"Top Depth - Low Value ","Hint","HTTP:\\descriptionURL",UC_cm};    Parameter_range PPR_hzdep={0,9999,0,9999}; // PPF_8_0_10
Parameter_properties_literal PP_hzdept_r           = {"hzdept_r"        ,"hzdept_r"          ,"hzdept_r"          ,"Top Depth - Representative Value","Hint","HTTP:\\descriptionURL"  ,UC_cm};
Parameter_properties_literal PP_hzdept_h           = {"hzdept_h"        ,"hzdept_h"          ,"hzdept_h"          ,"Top Depth - High Value","Hint","HTTP:\\descriptionURL"            ,UC_cm};
Parameter_properties_literal PP_hzdepb_l           = {"hzdepb_l"        ,"hzdepb_l"          ,"hzdepb_l"          ,"Bottom Depth - Low Value","Hint","HTTP:\\descriptionURL"          ,UC_cm};
Parameter_properties_literal PP_hzdepb_r           = {"hzdepb_r"        ,"hzdepb_r"          ,"hzdepb_r"          ,"Bottom Depth - Representative","Hint","HTTP:\\descriptionURL",UC_cm};
Parameter_properties_literal PP_hzdepb_h           = {"hzdepb_h"        ,"hzdepb_h"          ,"hzdepb_h"          ,"Bottom Depth - High Value","Hint","HTTP:\\descriptionURL",UC_cm};
Parameter_properties_literal PP_hzthk_l            = {"hzthk_l"         ,"hzthk_l"           ,"hzthk_l"           ,"Thickness - Low Value Integer","Hint","HTTP:\\descriptionURL",UC_cm}; Parameter_range PPR_hzthk={0,9999,0,9999}; // PPF_8_0_10
Parameter_properties_literal PP_hzthk_r            = {"hzthk_r"         ,"hzthk_r"           ,"hzthk_r"           ,"Thickness - Representative Value","Hint","HTTP:\\descriptionURL",UC_cm};
Parameter_properties_literal PP_hzthk_h            = {"hzthk_h"         ,"hzthk_h"           ,"hzthk_h"           ,"Thickness - High Value","Hint","HTTP:\\descriptionURL",UC_cm};
Parameter_properties_literal PP_fraggt10_l         = {"fraggt10_l"      ,"fraggt10_l"        ,"fraggt10_l"        ,"Rock >10 - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_fraggt10_r         = {"fraggt10_r"      ,"fraggt10_r"        ,"fraggt10_r"        ,"Rock >10 - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_fraggt10_h         = {"fraggt10_h"      ,"fraggt10_h"        ,"fraggt10_h"        ,"Rock >10 - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_frag3to10_l        = {"frag3to10_l"     ,"frag3to10_l"       ,"frag3to10_l"       ,"Rock 3-10 - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_frag3to10_r        = {"frag3to10_r"     ,"frag3to10_r"       ,"frag3to10_r"       ,"Rock 3-10 - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_frag3to10_h        = {"frag3to10_h"     ,"frag3to10_h"       ,"frag3to10_h"       ,"Rock 3-10 - High Value Integer","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sieveno4_l         = {"sieveno4_l"      ,"sieveno4_l"        ,"sieveno4_l"        ,"#4 - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sieveno4_r         = {"sieveno4_r"      ,"sieveno4_r"        ,"sieveno4_r"        ,"#4 - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sieveno4_h         = {"sieveno4_h"      ,"sieveno4_h"        ,"sieveno4_h"        ,"#4 - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sieveno10_l        = {"sieveno10_l"     ,"sieveno10_l"       ,"sieveno10_l"       ,"#10 - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sieveno10_r        = {"sieveno10_r"     ,"sieveno10_r"       ,"sieveno10_r"       ,"#10 - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sieveno10_h        = {"sieveno10_h"     ,"sieveno10_h"       ,"sieveno10_h"       ,"#10 - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sieveno40_l        = {"sieveno40_l"     ,"sieveno40_l"       ,"sieveno40_l"       ,"#40 - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sieveno40_r        = {"sieveno40_r"     ,"sieveno40_r"       ,"sieveno40_r"       ,"#40 - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sieveno40_h        = {"sieveno40_h"     ,"sieveno40_h"       ,"sieveno40_h"       ,"#40 - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sieveno200_l       = {"sieveno200_l"    ,"sieveno200_l"      ,"sieveno200_l"      ,"#200 - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sieveno200_r       = {"sieveno200_r"    ,"sieveno200_r"      ,"sieveno200_r"      ," #200 - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sieveno200_h       = {"sieveno200_h"    ,"sieveno200_h"      ,"sieveno200_h"      ,"#200 - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandtotal_l        = {"sandtotal_l"     ,"sandtotal_l"       ,"sandtotal_l"       ,"Total Sand - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandtotal_r        = {"sandtotal_r"     ,"sandtotal_r"       ,"sandtotal_r"       ,"Total Sand - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandtotal_h        = {"sandtotal_h"     ,"sandtotal_h"       ,"sandtotal_h"       ,"Total Sand - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandvc_l           = {"sandvc_l"        ,"sandvc_l"          ,"sandvc_l"          ,"vcos - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandvc_r           = {"sandvc_r"        ,"sandvc_r"          ,"sandvc_r"          ,"vcos - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandvc_h           = {"sandvc_h"        ,"sandvc_h"          ,"sandvc_h"          ,"vcos - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandco_l           = {"sandco_l"        ,"sandco_l"          ,"sandco_l"          ,"cos - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandco_r           = {"sandco_r"        ,"sandco_r"          ,"sandco_r"          ,"cos - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandco_h           = {"sandco_h"        ,"sandco_h"          ,"sandco_h"          ,"cos - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandmed_l          = {"sandmed_l"       ,"sandmed_l"         ,"sandmed_l"         ,"ms - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandmed_r          = {"sandmed_r"       ,"sandmed_r"         ,"sandmed_r"         ,"ms - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandmed_h          = {"sandmed_h"       ,"sandmed_h"         ,"sandmed_h"         ,"ms - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandfine_l         = {"sandfine_l"      ,"sandfine_l"        ,"sandfine_l"        ,"fs - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandfine_r         = {"sandfine_r"      ,"sandfine_r"        ,"sandfine_r"        ,"fs - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandfine_h         = {"sandfine_h"      ,"sandfine_h"        ,"sandfine_h"        ,"fs - High Value","Hint","HTTP:\\descriptionURL",UC_percent};

Parameter_properties_literal PP_sandvf_l           = {"sandvf_l"        ,"sandvf_l"          ,"sandvf_l"          ,"vfs - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandvf_r           = {"sandvf_r"        ,"sandvf_r"          ,"sandvf_r"          ,"vfs - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sandvf_h           = {"sandvf_h"        ,"sandvf_h"          ,"sandvf_h"          ,"vfs - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_silttotal_l        = {"silttotal_l"     ,"silttotal_l"       ,"silttotal_l"       ,"Total Silt - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_silttotal_r        = {"silttotal_r"     ,"silttotal_r"       ,"silttotal_r"       ,"Total Silt - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_silttotal_h        = {"silttotal_h"     ,"silttotal_h"       ,"silttotal_h"       ,"Total Silt - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_siltco_l           = {"siltco_l"        ,"siltco_l"          ,"siltco_l"          ,"Coarse Silt - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_siltco_r           = {"siltco_r"        ,"siltco_r"          ,"siltco_r"          ,"Coarse Silt - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_siltco_h           = {"siltco_h"        ,"siltco_h","xxxx","Coarse Silt - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_siltfine_l         = {"siltfine_l"      ,"siltfine_l","xxxx","Fine Silt - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_siltfine_r         = {"siltfine_r"      ,"siltfine_r","xxxx","Fine Silt - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_siltfine_h         = {"siltfine_h"      ,"siltfine_h","xxxx","Fine Silt - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_claytotal_l        = {"claytotal_l"     ,"claytotal_l","xxxx","Total Clay - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_claytotal_r        = {"claytotal_r"     ,"claytotal_r","xxxx","Total Clay - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_claytotal_h        = {"claytotal_h"     ,"claytotal_h","xxxx","Total Clay - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_claysizedcarb_l    = {"claysizedcarb_l" ,"claysizedcarb_l","xxxx","CaCO3 Clay - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_claysizedcarb_r    = {"claysizedcarb_r" ,"claysizedcarb_r","xxxx","CaCO3 Clay - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_claysizedcarb_h    = {"claysizedcarb_h" ,"claysizedcarb_h","xxxx","CaCO3 Clay - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_om_l               = {"om_l"            ,"om_l","xxxx","OM - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_om_r               = {"om_r"            ,"om_r","xxxx","OM - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_om_h               = {"om_h"            ,"om_h","xxxx","OM - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_dbtenthbar_l       = {"dbtenthbar_l"    ,"dbtenthbar_l","xxxx","Db 0.1 bar H2O - Low Value","Hint","HTTP:\\descriptionURL",UC_g_per_cm3};
Parameter_properties_literal PP_dbtenthbar_r       = {"dbtenthbar_r"    ,"dbtenthbar_r","xxxx","Db 0.1 bar H2O - Representative Value","Hint","HTTP:\\descriptionURL",UC_g_per_cm3};
Parameter_properties_literal PP_dbtenthbar_h       = {"dbtenthbar_h"    ,"dbtenthbar_h","xxxx","Db 0.1 bar H2O - High Value","Hint","HTTP:\\descriptionURL",UC_g_per_cm3};
Parameter_properties_literal PP_dbthirdbar_l       = {"dbthirdbar_l"    ,"dbthirdbar_l","xxxx","Db 0.33 bar H2O - Low Value","Hint","HTTP:\\descriptionURL",UC_g_per_cm3};
Parameter_properties_literal PP_dbthirdbar_r       = {"dbthirdbar_r"    ,"dbthirdbar_r","xxxx","Db 0.33 bar H2O - Representative Value","Hint","HTTP:\\descriptionURL",UC_g_per_cm3};
Parameter_properties_literal PP_dbthirdbar_h       = {"dbthirdbar_h"    ,"dbthirdbar_h","xxxx","Db 0.33 bar H2O - High Value","Hint","HTTP:\\descriptionURL",UC_g_per_cm3};
Parameter_properties_literal PP_dbfifteenbar_l     = {"dbfifteenbar_l"  ,"dbfifteenbar_l","xxxx","Db 15 bar H2O - Low Value","Hint","HTTP:\\descriptionURL",UC_g_per_cm3};
Parameter_properties_literal PP_dbfifteenbar_r     = {"dbfifteenbar_r"  ,"dbfifteenbar_r","xxxx","Db 15 bar H2O - Representative Value","Hint","HTTP:\\descriptionURL",UC_g_per_cm3};
Parameter_properties_literal PP_dbfifteenbar_h     = {"dbfifteenbar_h"  ,"dbfifteenbar_h","xxxx","Db 15 bar H2O - High Value","Hint","HTTP:\\descriptionURL",UC_g_per_cm3};
Parameter_properties_literal PP_dbovendry_l        = {"dbovendry_l"     ,"dbovendry_l","xxxx","Db oven dry - Low Value","Hint","HTTP:\\descriptionURL",UC_g_per_cm3};
Parameter_properties_literal PP_dbovendry_r        = {"dbovendry_r"     ,"dbovendry_r","xxxx","Db oven dry - Representative Value","Hint","HTTP:\\descriptionURL",UC_g_per_cm3};
Parameter_properties_literal PP_dbovendry_h        = {"dbovendry_h"     ,"dbovendry_h","xxxx","Db oven dry - High Value","Hint","HTTP:\\descriptionURL",UC_g_per_cm3};

Parameter_properties_literal PP_partdensity        = {"partdensity"     ,"Dp","xxxx","Partical density","Hint","HTTP:\\descriptionURL",UC_g_per_cm3};    Parameter_range PPR_partdensity={0.01,5,0.01,5}; // PPF_8_2_10;

Parameter_properties_literal PP_ksat_l             = {"ksat_l"          ,"ksat_l","xxxx","Ksat - Low Value","Hint","HTTP:\\descriptionURL",UC_micrometer_per_second};  Parameter_range PPR_ksat={0,705,0,705}; // PPF_8_4_10;
Parameter_properties_literal PP_ksat_r             = {"ksat_r"          ,"ksat_r","xxxx","Ksat - Representative Value","Hint","HTTP:\\descriptionURL",UC_micrometer_per_second};
Parameter_properties_literal PP_ksat_h             = {"ksat_h"          ,"ksat_h","xxxx","Ksat - High Value","Hint","HTTP:\\descriptionURL",UC_micrometer_per_second};
Parameter_properties_literal PP_awc_l              = {"awc_l"           ,"awc_l","xxxx","AWC - Low Value","?Available Water Content?","HTTP:\\descriptionURL",UC_cm3_per_cm3};  Parameter_range PPR_awc={0,0.7,0,0.7}; // PPF_8_2_10
Parameter_properties_literal PP_awc_r              = {"awc_r"           ,"awc_r","xxxx","AWC - Representative Value","?Available Water Content?","HTTP:\\descriptionURL",UC_cm3_per_cm3};
Parameter_properties_literal PP_awc_h              = {"awc_h"           ,"awc_h","xxxx","AWC - High Value","?Available Water Content?","HTTP:\\descriptionURL",UC_cm3_per_cm3};
Parameter_properties_literal PP_wtenthbar_l        = {"wtenthbar_l"     ,"wtenthbar_l","xxxx","0.1 bar H2O - Low Value","Hint","HTTP:\\descriptionURL",UC_percent}; Parameter_range PPR_wtenthbar={0,2000,0,2000}; // PPF_8_1_10
Parameter_properties_literal PP_wtenthbar_r        = {"wtenthbar_r"     ,"wtenthbar_r","xxxx","0.1 bar H2O - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_wtenthbar_h        = {"wtenthbar_h"     ,"wtenthbar_h","xxxx","0.1 bar H2O - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_wthirdbar_l        = {"wthirdbar_l"     ,"wthirdbar_l","xxxx","0.33 bar H2O - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};  Parameter_range PPR_wthirdbar={1,2000,1,2000}; // PPF_8_1_10
Parameter_properties_literal PP_wthirdbar_r        = {"wthirdbar_r"     ,"wthirdbar_r","xxxx","0.33 bar H2O - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_wthirdbar_h        = {"wthirdbar_h"     ,"wthirdbar_h","xxxx","0.33 bar H2O - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_wfifteenbar_l      = {"wfifteenbar_l"   ,"wfifteenbar_l","xxxx","15 bar H2O - Low Value","Hint","HTTP:\\descriptionURL",UC_percent}; Parameter_range PPR_wfifteenbar={0,400,0,400}; // PPF_8_1_10
Parameter_properties_literal PP_wfifteenbar_r      = {"wfifteenbar_r"   ,"wfifteenbar_r","xxxx","15 bar H2O - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_wfifteenbar_h      = {"wfifteenbar_h"   ,"wfifteenbar_h","xxxx","15 bar H2O - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_wsatiated_l        = {"wsatiated_l"     ,"wsatiated_l","xxxx","Satiated H2O - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};  Parameter_range PPR_wsatiated={10,70,10,70}; // PPF_8_0_10
Parameter_properties_literal PP_wsatiated_r        = {"wsatiated_r"     ,"wsatiated_r","xxxx","Satiated H2O - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_wsatiated_h        = {"wsatiated_h"     ,"wsatiated_h","xxxx","Satiated H2O - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_lep_l              = {"lep_l"           ,"lep_l","xxxx","LEP - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};  Parameter_range PPR_lep={0,30,0,30}; // PPF_8_1_10
Parameter_properties_literal PP_lep_r              = {"lep_r"           ,"lep_r","xxxx","LEP - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_lep_h              = {"lep_h"           ,"lep_h","xxxx","LEP - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_ll_l               = {"ll_l"            ,"ll_l","xxxx","LL - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};Parameter_range PPR_ll={0,400,0,400}; // PPF_8_1_10
Parameter_properties_literal PP_ll_r               = {"ll_r"            ,"ll_r","xxxx","LL - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_ll_h               = {"ll_h"            ,"ll_h","xxxx"," LL - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_pi_l               = {"pi_l"            ,"pi_l","xxxx","PI - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};  Parameter_range PPR_pi={0,130,0,130}; // PPF_8_1_10
Parameter_properties_literal PP_pi_r               = {"pi_r"            ,"pi_r","xxxx","PI - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_pi_h               = {"pi_h"            ,"pi_h","xxxx","PI - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_aashind_l          = {"aashind_l"       ,"aashind_l","xxxx","AASHTO Group Index - Low Value","Hint","HTTP:\\descriptionURL",UC_index}; Parameter_range PPR_aashind={0,120,0,120}; // PPF_8_0_10
Parameter_properties_literal PP_aashind_r          = {"aashind_r"       ,"aashind_r","xxxx","AASHTO Group Index -Representative Value","Hint","HTTP:\\descriptionURL",UC_index};
Parameter_properties_literal PP_aashind_h          = {"aashind_h"       ,"aashind_h","xxxx","AASHTO Group Index - High Value","Hint","HTTP:\\descriptionURL",UC_index};
//Parameter_properties_literal PP_kwfact           = {"kwfact","kwfact","xxxx","Kw","Hint","HTTP:\\descriptionURL",UC_factor}; Choice Parameter_range PPR_x={min_warn,max_warn,min_error,max_error, precision,10 }; 254 soil_erodibility_factor
//Parameter_properties_literal PP_kffact           = {"kffact","kffact","xxxx","Kf","Hint","HTTP:\\descriptionURL",UC_factor}; Choice Parameter_range PPR_x={min_warn,max_warn,min_error,max_error, precision,10 }; 254 soil_erodibility_factor
Parameter_properties_literal PP_caco3_l            = {"caco3_l"         ,"caco3_l","xxxx","CaCO3 - Low Value","Hint","HTTP:\\descriptionURL",UC_percent}; Parameter_range PPR_caco3={0,110,0,110}; // PPF_8_0_10
Parameter_properties_literal PP_caco3_r            = {"caco3_r"         ,"caco3_r","xxxx","CaCO3 - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_caco3_h            = {"caco3_h"         ,"caco3_h","xxxx","CaCO3 - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_gypsum_l           = {"gypsum_l"        ,"gypsum_l","xxxx","Gypsum - Low Value","Hint","HTTP:\\descriptionURL",UC_percent}; Parameter_range PPR_gypsum={0,120,0,120}; // PPF_8_0_10
Parameter_properties_literal PP_gypsum_r           = {"gypsum_r"        ,"gypsum_r","xxxx","Gypsum - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_gypsum_h           = {"gypsum_h"        ,"gypsum_h","xxxx","Gypsum - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_sar_l              = {"sar_l"           ,"sar_l","xxxx","SAR - Low Value","Hint","HTTP:\\descriptionURL",U_units_not_specified};  Parameter_range PPR_sar={0,9999,0,9999}; // PPF_8_1_10
Parameter_properties_literal PP_sar_r              = {"sar_r"           ,"sar_r","xxxx","SAR - Representative","Hint","HTTP:\\descriptionURL",U_units_not_specified};
Parameter_properties_literal PP_sar_h              = {"sar_h"           ,"sar_h","xxxx","SAR - High Value","Hint","HTTP:\\descriptionURL",U_units_not_specified};
Parameter_properties_literal PP_ec_l               = {"ec_l"            ,"ec_l","xxxx","EC - Low Value","Hint","HTTP:\\descriptionURL",UC_mS_per_cm};  Parameter_range PPR_ec={0,15000,0,15000}; // PPF_8_1_10
Parameter_properties_literal PP_ec_r               = {"ec_r"            ,"ec_r","xxxx","EC - Representative Value","Hint","HTTP:\\descriptionURL",UC_mS_per_cm};
Parameter_properties_literal PP_ec_h               = {"ec_h"            ,"ec_h","xxxx","EC - High Value","Hint","HTTP:\\descriptionURL",UC_mS_per_cm};
Parameter_properties_literal PP_cec7_l             = {"cec7_l"          ,"cec7_l","xxxx","CEC-7 - Low Value","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};  Parameter_range PPR_cec7={0,400,0,400}; // PPF_8_1_10
Parameter_properties_literal PP_cec7_r             = {"cec7_r"          ,"cec7_r","xxxx","CEC-7 - Representative","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};
Parameter_properties_literal PP_cec7_h             = {"cec7_h"          ,"cec7_h","xxxx","CEC-7 - High Value","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};
Parameter_properties_literal PP_ecec_l             = {"ecec_l"          ,"ecec_l","xxxx","ECEC - Low Value","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};  Parameter_range PPR_ecec={0,400,0,400}; // PPF_8_1_10
Parameter_properties_literal PP_ecec_r             = {"ecec_r"          ,"ecec_r","xxxx","ECEC - Representative Value","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};
Parameter_properties_literal PP_ecec_h             = {"ecec_h"          ,"ecec_h","xxxx","ECEC - High Value","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};
Parameter_properties_literal PP_sumbases_l         = {"sumbases_l"      ,"sumbases_l","xxxx","Sum of Bases - Low Value","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};  Parameter_range PPR_sumbases={0,300,0,300}; // PPF_8_1_10
Parameter_properties_literal PP_sumbases_r         = {"sumbases_r"      ,"sumbases_r","xxxx","Sum of Bases - Representative","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};
Parameter_properties_literal PP_sumbases_h         = {"sumbases_h"      ,"sumbases_h","xxxx","Sum of Bases - High","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};
Parameter_properties_literal PP_ph1to1h2o_l        = {"ph1to1h2o_l"     ,"ph1to1h2o_l","xxxx","pH H2O - Low Value","Hint","HTTP:\\descriptionURL",UC_pH};  Parameter_range PPR_ph1to1h2o={1.8,1,1.8,11}; // PPF_8_1_10
Parameter_properties_literal PP_ph1to1h2o_r        = {"ph1to1h2o_r"     ,"ph1to1h2o_r","xxxx","pH H2O - Representative","Hint","HTTP:\\descriptionURL",UC_pH};
Parameter_properties_literal PP_ph1to1h2o_h        = {"ph1to1h2o_h"     ,"ph1to1h2o_h","xxxx","pH H2O - High","Hint","HTTP:\\descriptionURL",UC_pH};
Parameter_properties_literal PP_ph01mcacl2_l       = {"ph01mcacl2_l"    ,"ph01mcacl2_l","xxxx","pH CaCl2 - Low Value","Hint","HTTP:\\descriptionURL",UC_pH};  Parameter_range PPR_ph01mcacl2={ 1.8 ,11, 1.8 , 11}; // PPF_8_1_10
Parameter_properties_literal PP_ph01mcacl2_r       = {"ph01mcacl2_r"    ,"ph01mcacl2_r","xxxx","pH CaCl2 - Representative Value","Hint","HTTP:\\descriptionURL",UC_pH};
Parameter_properties_literal PP_ph01mcacl2_h       = {"ph01mcacl2_h"    ,"ph01mcacl2_h","xxxx","pH CaCl2 - High Value","Hint","HTTP:\\descriptionURL",UC_pH};
Parameter_properties_literal PP_freeiron_l         = {"freeiron_l"      ,"freeiron_l","xxxx","Free Iron - Low Value","Hint","HTTP:\\descriptionURL",UC_percent};  Parameter_range PPR_freeiron={0,100,0,100}; // PPF_8_2_10
Parameter_properties_literal PP_freeiron_r         = {"freeiron_r"      ,"freeiron_r","xxxx","Free Iron - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_freeiron_h         = {"freeiron_h"      ,"freeiron_h","xxxx","Free Iron - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_feoxalate_l        = {"feoxalate_l"     ,"feoxalate_l","xxxx","Oxalate Fe - Low Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg}; Parameter_range PPR_feoxalate={0,150000,0,150000}; // PPF_8_2_10
Parameter_properties_literal PP_feoxalate_r        = {"feoxalate_r"     ,"feoxalate_r","xxxx","Oxalate Fe - Representative Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};
Parameter_properties_literal PP_feoxalate_h        = {"feoxalate_h"     ,"feoxalate_h","xxxx","Oxalate Fe - High Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};
Parameter_properties_literal PP_extracid_l         = {"extracid_l"      ,"extracid_l","xxxx","Ext Acidity - Low Value","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};  Parameter_range PPR_extracid={0,250,0,250}; // PPF_8_1_10
Parameter_properties_literal PP_extracid_r         = {"extracid_r"      ,"extracid_r","xxxx","Ext Acidity - Representative","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};
Parameter_properties_literal PP_extracid_h         = {"extracid_h"      ,"extracid_h","xxxx","Ext Acidity - High Value","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};
Parameter_properties_literal PP_extral_l           = {"extral_l"        ,"extral_l","xxxx","Extract Al - Low Value","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};  Parameter_range PPR_extral={0,150,0,150}; // PPF_8_2_10
Parameter_properties_literal PP_extral_r           = {"extral_r"        ,"extral_r","xxxx","Extract Al - Representative Value","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};
Parameter_properties_literal PP_extral_h           = {"extral_h"        ,"extral_h","xxxx","Extract Al - High Value","Hint","HTTP:\\descriptionURL",UC_mEq_per_100g};
Parameter_properties_literal PP_aloxalate_l        = {"aloxalate_l"     ,"aloxalate_l","xxxx","Oxalate Al - Low Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};  Parameter_range PPR_aloxalate={0,170000,0,170000}; // PPF_8_1_10
Parameter_properties_literal PP_aloxalate_r        = {"aloxalate_r"     ,"aloxalate_r","xxxx","Oxalate Al - Representative Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};
Parameter_properties_literal PP_aloxalate_h        = {"aloxalate_h"     ,"aloxalate_h","xxxx","Oxalate Al - High Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};
Parameter_properties_literal PP_pbray1_l           = {"pbray1_l"        ,"pbray1_l","pbray1_l","Bray 1 Phos - Low Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};  Parameter_range PPR_pbray1={0,500,0,500}; // PPF_8_1_10
Parameter_properties_literal PP_pbray1_r           = {"pbray1_r"        ,"pbray1_r","pbray1_r","Bray 1 Phos - Representative Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};
Parameter_properties_literal PP_pbray1_h           = {"pbray1_h"        ,"pbray1_h","pbray1_h","Bray 1 Phos - High Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};
Parameter_properties_literal PP_poxalate_l         = {"poxalate_l"      ,"poxalate_l","poxalate_l","Oxalate Phos - Low Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};  Parameter_range PPR_poxalate={0,999999,0,999999}; // PPF_8_1_10
Parameter_properties_literal PP_poxalate_r         = {"poxalate_r"      ,"poxalate_r","poxalate_r","Oxalate Phos - Representative Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};   // Note that the documentation does not specify a maximum
Parameter_properties_literal PP_poxalate_h         = {"poxalate_h"      ,"poxalate_h","poxalate_h","Oxalate Phos - High Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};
Parameter_properties_literal PP_ph2osoluble_l      = {"ph2osoluble_l"   ,"ph2osoluble_l","ph2osoluble_l","Water Soluble Phos - Low Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};  Parameter_range PPR_ph2osoluble={0,5000,0,5000}; // PPF_8_1_10
Parameter_properties_literal PP_ph2osoluble_r      = {"ph2osoluble_r"   ,"ph2osoluble_r","ph2osoluble_r","Water Soluble Phos - Representative Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};
Parameter_properties_literal PP_ph2osoluble_h      = {"ph2osoluble_h"   ,"ph2osoluble_h","ph2osoluble_h","Water Soluble Phos - High Value","Hint","HTTP:\\descriptionURL",UC_mg_per_kg};
Parameter_properties_literal PP_ptotal_l           = {"ptotal_l"        ,"ptotal_l","ptotal_l","Total Phos - Low Value","Hint","HTTP:\\descriptionURL",UC_percent}; Parameter_range PPR_ptotal={0,100,0,100}; // PPF_8_2_10
Parameter_properties_literal PP_ptotal_r           = {"ptotal_r"        ,"ptotal_r","ptotal_r","Total Phos - Representative Value","Hint","HTTP:\\descriptionURL",UC_percent};
Parameter_properties_literal PP_ptotal_h           = {"ptotal_h"        ,"ptotal_h","ptotal_h","Total Phos - High Value","Hint","HTTP:\\descriptionURL",UC_percent};
//Parameter_properties_literal PP_excavdifcl = {"excavdifcl","excavdifcl",254,"Excav Diff","Hint","HTTP:\\descriptionURL",UC_unitless};
//Parameter_properties_literal PP_excavdifms = {"Kexcavdifmsey","excavdifms",254,"Excav Diff Moisture","Hint","HTTP:\\descriptionURL",UC_unitless};
// cokey Component Key String Yes 30
// chkey Chorizon Key String Yes 30
//______________________________________________________________________________
CHorizon_record::CHorizon_record()
: CHorizon_struct()
, CORN::Data_record("chorizon")
//,hzname() // Designation String No 12
,p_desgndisc         (desgndisc        ,PP_desgndisc,PPR_desgndisc,PPF_8_0_10) // Disc Integer No 2 99
//,desgnmaster() // Master Choice No 254   Horz_desgn_master
//,desgnmasterprime() // Prime Choice No 254 Horz_desgn_master_prime
,p_desgnvert         (desgnvert        ,PP_desgnvert,PPR_desgnvert,PPF_8_0_10)  // Sub   1
,p_hzdept_l          (hzdept.low       ,PP_hzdept_l,PPR_hzdep,PPF_8_0_10)  // Top Depth 0 9999 centimeters
,p_hzdept_r          (hzdept.repr      ,PP_hzdept_r,PPR_hzdep,PPF_8_0_10)
,p_hzdept_h          (hzdept.high      ,PP_hzdept_h,PPR_hzdep,PPF_8_0_10)
,p_hzdepb_l          (hzdepb.low       ,PP_hzdepb_l,PPR_hzdep,PPF_8_0_10)     // Bottom depth centimeters
,p_hzdepb_r          (hzdepb.repr      ,PP_hzdepb_r,PPR_hzdep,PPF_8_0_10)
,p_hzdepb_h          (hzdepb.high      ,PP_hzdepb_h,PPR_hzdep,PPF_8_0_10)
,p_hzthk_l           (hzthk.low        ,PP_hzthk_l,PPR_hzthk,PPF_8_0_10)      // Thickness - e   0 9999 centimeters
,p_hzthk_r           (hzthk.repr       ,PP_hzthk_r,PPR_hzthk,PPF_8_0_10)
,p_hzthk_h           (hzthk.high       ,PP_hzthk_h,PPR_hzthk,PPF_8_0_10)
,p_fraggt10_l        (fraggt10.low     ,PP_fraggt10_l,PPR_percent_0_100_integer,PPF_8_0_10)   // Rock >10 -   0 100 percent
,p_fraggt10_r        (fraggt10.repr    ,PP_fraggt10_r,PPR_percent_0_100_integer,PPF_8_0_10)
,p_fraggt10_h        (fraggt10.high    ,PP_fraggt10_h,PPR_percent_0_100_integer,PPF_8_0_10)
,p_frag3to10_l       (frag3to10.low    ,PP_frag3to10_l,PPR_percent_0_100_integer,PPF_8_0_10)  // Rock 3-10 - 0 100 percent
,p_frag3to10_r       (frag3to10.repr   ,PP_frag3to10_r,PPR_percent_0_100_integer,PPF_8_0_10)
,p_frag3to10_h       (frag3to10.high   ,PP_frag3to10_h,PPR_percent_0_100_integer,PPF_8_0_10)
,p_sieveno4_l        (sieveno4.low     ,PP_sieveno4_l,PPR_percent_0_100_f1,PPF_8_1_10)   // #4 -  1 0 100 percent
,p_sieveno4_r        (sieveno4.repr    ,PP_sieveno4_r,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sieveno4_h        (sieveno4.high    ,PP_sieveno4_h,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sieveno10_l       (sieveno10.low    ,PP_sieveno10_l,PPR_percent_0_100_f1,PPF_8_1_10)  //  #10 -  1 0 100 percent
,p_sieveno10_r       (sieveno10.repr   ,PP_sieveno10_r,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sieveno10_h       (sieveno10.high   ,PP_sieveno10_h,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sieveno40_l       (sieveno40.low    ,PP_sieveno40_l,PPR_percent_0_100_f1,PPF_8_1_10)  //#40  1 0 100 percent
,p_sieveno40_r       (sieveno40.repr   ,PP_sieveno40_r,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sieveno40_h       (sieveno40.high   ,PP_sieveno40_h,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sieveno200_l      (sieveno200.low   ,PP_sieveno200_l,PPR_percent_0_100_f1,PPF_8_1_10) //  #200  1 0 100 percent
,p_sieveno200_r      (sieveno200.repr  ,PP_sieveno200_r,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sieveno200_h      (sieveno200.high  ,PP_sieveno200_h,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sandtotal_l       (sandtotal.low    ,PP_sandtotal_l,PPR_percent_0_100_f1,PPF_8_1_10)  //  Total Sand 1 0 100 percent
,p_sandtotal_r       (sandtotal.repr   ,PP_sandtotal_r,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sandtotal_h       (sandtotal.high   ,PP_sandtotal_h,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sandvc_l          (sandvc.low       ,PP_sandvc_l,PPR_percent_0_100_f1,PPF_8_1_10)     //  vcos   1 0 100 percent
,p_sandvc_r          (sandvc.repr      ,PP_sandvc_r,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sandvc_h          (sandvc.high      ,PP_sandvc_h,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sandco_l          (sandco.low       ,PP_sandco_l,PPR_percent_0_100_f1,PPF_8_1_10)     //  cos  1 0 100 percent
,p_sandco_r          (sandco.repr      ,PP_sandco_r      ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sandco_h          (sandco.high      ,PP_sandco_h      ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sandmed_l         (sandmed.low      ,PP_sandmed_l     ,PPR_percent_0_100_f1,PPF_8_1_10)    //  ms  1 0 100 percent
,p_sandmed_r         (sandmed.repr     ,PP_sandmed_r     ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sandmed_h         (sandmed.high     ,PP_sandmed_h     ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sandfine_l        (sandfine.low     ,PP_sandfine_l    ,PPR_percent_0_100_f1,PPF_8_1_10)   //  fs  1 0 100 percent
,p_sandfine_r        (sandfine.repr    ,PP_sandfine_r    ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sandfine_h        (sandfine.high    ,PP_sandfine_h    ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sandvf_l          (sandvf.low       ,PP_sandvf_l      ,PPR_percent_0_100_f1,PPF_8_1_10)     //  vfs 1 0 100 percent
,p_sandvf_r          (sandvf.repr      ,PP_sandvf_r      ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_sandvf_h          (sandvf.high      ,PP_sandvf_h      ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_silttotal_l       (silttotal.low    ,PP_silttotal_l   ,PPR_percent_0_100_f1,PPF_8_1_10)  //  Total Silt 1 0 100 percent
,p_silttotal_r       (silttotal.repr   ,PP_silttotal_r   ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_silttotal_h       (silttotal.high   ,PP_silttotal_h   ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_siltco_l          (siltco.low       ,PP_siltco_l      ,PPR_percent_0_100_f1,PPF_8_1_10)     //  Coarse Silt  1 0 100 percent
,p_siltco_r          (siltco.repr      ,PP_siltco_r      ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_siltco_h          (siltco.high      ,PP_siltco_h      ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_siltfine_l        (siltfine.low     ,PP_siltfine_l    ,PPR_percent_0_100_f1,PPF_8_1_10)   //  Fine Silt  1 0 100 percent
,p_siltfine_r        (siltfine.repr    ,PP_siltfine_r    ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_siltfine_h        (siltfine.high    ,PP_siltfine_h    ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_claytotal_l       (claytotal.low    ,PP_claytotal_l   ,PPR_percent_0_100_f1,PPF_8_1_10)   //  Total Clay - Low Value Float No 1 0 100 percent
,p_claytotal_r       (claytotal.repr   ,PP_claytotal_r   ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_claytotal_h       (claytotal.high   ,PP_claytotal_h   ,PPR_percent_0_100_f1,PPF_8_1_10)
,p_claysizedcarb_l   (claysizedcarb.low,PP_claysizedcarb_l,PPR_percent_0_100_f1,PPF_8_1_10)  //  CaCO3 Clay - Low Value Float No 1 0 100 percent
,p_claysizedcarb_r   (claysizedcarb.repr,PP_claysizedcarb_r,PPR_percent_0_100_f1,PPF_8_1_10)
,p_claysizedcarb_h   (claysizedcarb.high,PP_claysizedcarb_h,PPR_percent_0_100_f1,PPF_8_1_10)
,p_om_l              (om.low           ,PP_om_l          ,PPR_percent_0_100_f2,PPF_8_1_10)        //  OM - Low Value Float No 2 0 100 percent
,p_om_r              (om.repr          ,PP_om_r          ,PPR_percent_0_100_f2,PPF_8_1_10)
,p_om_h              (om.high          ,PP_om_h          ,PPR_percent_0_100_f2,PPF_8_1_10)
,p_dbtenthbar_l      (dbtenthbar.low   ,PP_dbtenthbar_l  ,PPR_db         ,PPF_8_2_10) //  Db 0.1 bar H2O - Low Value Float No 2 0.02 2.6 UC_g_per_cm
,p_dbtenthbar_r      (dbtenthbar.repr  ,PP_dbtenthbar_r  ,PPR_db         ,PPF_8_2_10)
,p_dbtenthbar_h      (dbtenthbar.high  ,PP_dbtenthbar_h  ,PPR_db         ,PPF_8_2_10)
,p_dbthirdbar_l      (dbthirdbar.low   ,PP_dbthirdbar_l  ,PPR_db         ,PPF_8_2_10) //  Db 0.33 bar H2O - Low Value Float No 2 0.02 2.6 UC_g_per_cm
,p_dbthirdbar_r      (dbthirdbar.repr  ,PP_dbthirdbar_r  ,PPR_db         ,PPF_8_2_10)
,p_dbthirdbar_h      (dbthirdbar.high  ,PP_dbthirdbar_h  ,PPR_db         ,PPF_8_2_10)
,p_dbfifteenbar_l    (dbfifteenbar.low ,PP_dbfifteenbar_l,PPR_db         ,PPF_8_2_10)//  Db 15 bar H2O - Low Value Float No 2 0.02 2.6 UC_g_per_cm
,p_dbfifteenbar_r    (dbfifteenbar.repr,PP_dbfifteenbar_r,PPR_db         ,PPF_8_2_10)
,p_dbfifteenbar_h    (dbfifteenbar.high,PP_dbfifteenbar_h,PPR_db         ,PPF_8_2_10)
,p_dbovendry_l       (dbovendry.low    ,PP_dbovendry_l   ,PPR_db         ,PPF_8_2_10)//  Db oven dry - Low Value Float No 2 0.02 2.6 UC_g_per_cm
,p_dbovendry_r       (dbovendry.repr   ,PP_dbovendry_r   ,PPR_db         ,PPF_8_2_10)
,p_dbovendry_h       (dbovendry.high   ,PP_dbovendry_h   ,PPR_db         ,PPF_8_2_10)
,p_partdensity       (partdensity      ,PP_partdensity   ,PPR_partdensity         ,PPF_8_2_10) // Dp Float No 2 0.01 5
,p_ksat_l            (ksat.low         ,PP_ksat_l        ,PPR_ksat         ,PPF_8_4_10) //  Ksat - Low Value  4 0 705 micrometers per second
,p_ksat_r            (ksat.repr        ,PP_ksat_r        ,PPR_ksat         ,PPF_8_4_10)
,p_ksat_h            (ksat.high        ,PP_ksat_h        ,PPR_ksat         ,PPF_8_4_10)
,p_awc_l             (awc.low          ,PP_awc_l         ,PPR_awc          ,PPF_8_2_10) //  AWC - Low Value  2 0 0.7 centimeters per centimeter
,p_awc_r             (awc.repr         ,PP_awc_r         ,PPR_awc          ,PPF_8_2_10)
,p_awc_h             (awc.high         ,PP_awc_h         ,PPR_awc          ,PPF_8_2_10)
,p_wtenthbar_l       (wtenthbar.low    ,PP_wtenthbar_l   ,PPR_wtenthbar         ,PPF_8_1_10) // 0.1 bar H2O - Low Value  1 0 2000 percent
,p_wtenthbar_r       (wtenthbar.repr   ,PP_wtenthbar_r   ,PPR_wtenthbar         ,PPF_8_1_10)
,p_wtenthbar_h       (wtenthbar.high   ,PP_wtenthbar_h   ,PPR_wtenthbar         ,PPF_8_1_10)
,p_wthirdbar_l       (wthirdbar.low    ,PP_wthirdbar_l   ,PPR_wthirdbar         ,PPF_8_1_10) // 0.33 bar H2O - Low Value  1 0 2000 percent
,p_wthirdbar_r       (wthirdbar.repr   ,PP_wthirdbar_r   ,PPR_wthirdbar         ,PPF_8_1_10)
,p_wthirdbar_h       (wthirdbar.high   ,PP_wthirdbar_h   ,PPR_wthirdbar         ,PPF_8_1_10)
,p_wfifteenbar_l     (wfifteenbar.low  ,PP_wfifteenbar_l ,PPR_wfifteenbar         ,PPF_8_1_10) // 15 bar H2O - Low Value  1 0 400 percent
,p_wfifteenbar_r     (wfifteenbar.repr ,PP_wfifteenbar_r ,PPR_wfifteenbar         ,PPF_8_1_10)
,p_wfifteenbar_h     (wfifteenbar.high ,PP_wfifteenbar_h ,PPR_wfifteenbar         ,PPF_8_1_10)
,p_wsatiated_l       (wsatiated.low    ,PP_wsatiated_l   ,PPR_wsatiated         ,PPF_8_0_10) // Satiated H2O - Low Value   10 70 percent
,p_wsatiated_r       (wsatiated.repr   ,PP_wsatiated_r   ,PPR_wsatiated         ,PPF_8_0_10)
,p_wsatiated_h       (wsatiated.high   ,PP_wsatiated_h   ,PPR_wsatiated         ,PPF_8_0_10)
,p_lep_l             (lep.low          ,PP_lep_l         ,PPR_lep         ,PPF_8_1_10)  // LEP - Low Value Float No 1 0 30 percent
,p_lep_r             (lep.repr         ,PP_lep_r         ,PPR_lep         ,PPF_8_1_10)
,p_lep_h             (lep.high         ,PP_lep_h         ,PPR_lep         ,PPF_8_1_10)
,p_ll_l              (ll.low           ,PP_ll_l          ,PPR_ll         ,PPF_8_1_10)  //LL - Low Value Float No 1 0 400 percent
,p_ll_r              (ll.repr          ,PP_ll_r          ,PPR_ll         ,PPF_8_1_10)
,p_ll_h              (ll.high          ,PP_ll_h          ,PPR_ll         ,PPF_8_1_10)
,p_pi_l              (pi.low           ,PP_pi_l          ,PPR_pi         ,PPF_8_1_10) //PI - Low Value Float No 1 0 130 percent
,p_pi_r              (pi.repr          ,PP_pi_r          ,PPR_pi         ,PPF_8_1_10)
,p_pi_h              (pi.high          ,PP_pi_h          ,PPR_pi         ,PPF_8_1_10)
,p_aashind_l         (aashind.low      ,PP_aashind_l     ,PPR_aashind         ,PPF_8_0_10) // AASHTO Group Index - Low Value Integer No 0 120
,p_aashind_r         (aashind.repr     ,PP_aashind_r     ,PPR_aashind         ,PPF_8_0_10)
,p_aashind_h         (aashind.high     ,PP_aashind_h     ,PPR_aashind         ,PPF_8_0_10)
//,kwfact(xxxx,PP_kwfact,PPR_x) //Kw 254 Soil_erodibility_factor
//,kffact(xxxx,PP_kffact,PPR_x) //Kf  254  Soil_erodibility_factor
,p_caco3_l           (caco3.low        ,PP_caco3_l       ,PPR_caco3         ,PPF_8_0_10) // CaCO3 - Low Value Integer No 0 110 percent
,p_caco3_r           (caco3.repr       ,PP_caco3_r       ,PPR_caco3         ,PPF_8_0_10)
,p_caco3_h           (caco3.high       ,PP_caco3_h       ,PPR_caco3         ,PPF_8_0_10)
,p_gypsum_l          (gypsum.low       ,PP_gypsum_l      ,PPR_gypsum         ,PPF_8_0_10)  // Gypsum - Low Value Integer No 0 120 percent
,p_gypsum_r          (gypsum.repr      ,PP_gypsum_r      ,PPR_gypsum         ,PPF_8_0_10)
,p_gypsum_h          (gypsum.high      ,PP_gypsum_h      ,PPR_gypsum         ,PPF_8_0_10)
,p_sar_l             (sar.low          ,PP_sar_l         ,PPR_sar         ,PPF_8_1_10)// SAR - Low Value Float No 1 0 9999
,p_sar_r             (sar.repr         ,PP_sar_r         ,PPR_sar         ,PPF_8_1_10)
,p_sar_h             (sar.high         ,PP_sar_h         ,PPR_sar         ,PPF_8_1_10)
,p_ec_l              (ec.low           ,PP_ec_l          ,PPR_ec         ,PPF_8_1_10) // EC - Low Value Float No 1 0 15000 millimhos per centimeter
,p_ec_r              (ec.repr          ,PP_ec_r          ,PPR_ec         ,PPF_8_1_10)
,p_ec_h              (ec.high          ,PP_ec_h          ,PPR_ec         ,PPF_8_1_10)
,p_cec7_l            (cec7.low         ,PP_cec7_l        ,PPR_cec7         ,PPF_8_1_10)  // CEC-7 - Low Value Float No 1 0 400 milliequivalents per 100grams
,p_cec7_r            (cec7.repr        ,PP_cec7_r        ,PPR_cec7         ,PPF_8_1_10)
,p_cec7_h            (cec7.high        ,PP_cec7_h        ,PPR_cec7         ,PPF_8_1_10)
,p_ecec_l            (ecec.low         ,PP_ecec_l        ,PPR_ecec         ,PPF_8_1_10) // ECEC - Low Value Float No 1 0 400 milliequivalents per 100grams
,p_ecec_r            (ecec.repr        ,PP_ecec_r        ,PPR_ecec         ,PPF_8_1_10)
,p_ecec_h            (ecec.high        ,PP_ecec_h        ,PPR_ecec         ,PPF_8_1_10)
,p_sumbases_l        (sumbases.low     ,PP_sumbases_l    ,PPR_sumbases     ,PPF_8_1_10) // Sum of Bases - Low Value Float No 1 0 300 milliequivalents per 100grams
,p_sumbases_r        (sumbases.repr    ,PP_sumbases_r    ,PPR_sumbases     ,PPF_8_1_10)
,p_sumbases_h        (sumbases.high    ,PP_sumbases_h    ,PPR_sumbases     ,PPF_8_1_10)
,p_ph1to1h2o_l       (ph1to1h2o.low    ,PP_ph1to1h2o_l   ,PPR_ph1to1h2o    ,PPF_8_1_10) // pH H2O - Low Value Float No 1 1.8 11
,p_ph1to1h2o_r       (ph1to1h2o.repr   ,PP_ph1to1h2o_r   ,PPR_ph1to1h2o    ,PPF_8_1_10)
,p_ph1to1h2o_h       (ph1to1h2o.high   ,PP_ph1to1h2o_h   ,PPR_ph1to1h2o    ,PPF_8_1_10)
,p_ph01mcacl2_l      (ph01mcacl2.low   ,PP_ph01mcacl2_l  ,PPR_ph01mcacl2   ,PPF_8_1_10) // pH CaCl2 - Low Value Float No 1 1.8 11
,p_ph01mcacl2_r      (ph01mcacl2.repr  ,PP_ph01mcacl2_r  ,PPR_ph01mcacl2   ,PPF_8_1_10)
,p_ph01mcacl2_h      (ph01mcacl2.high  ,PP_ph01mcacl2_h  ,PPR_ph01mcacl2   ,PPF_8_1_10)
,p_freeiron_l        (freeiron.low     ,PP_freeiron_l    ,PPR_freeiron     ,PPF_8_2_10) // Free Iron - Low Value Float No 2 0 100 percent
,p_freeiron_r        (freeiron.repr    ,PP_freeiron_r    ,PPR_freeiron     ,PPF_8_2_10)
,p_freeiron_h        (freeiron.high    ,PP_freeiron_h    ,PPR_freeiron     ,PPF_8_2_10)
,p_feoxalate_l       (feoxalate.low    ,PP_feoxalate_l   ,PPR_feoxalate    ,PPF_8_2_10) // Oxalate Fe - Low Value Float No 2 0 150000 milligrams per kilogram
,p_feoxalate_r       (feoxalate.repr   ,PP_feoxalate_r   ,PPR_feoxalate    ,PPF_8_2_10)
,p_feoxalate_h       (feoxalate.high   ,PP_feoxalate_h   ,PPR_feoxalate    ,PPF_8_2_10)
,p_extracid_l        (extracid.low     ,PP_extracid_l    ,PPR_extracid     ,PPF_8_1_10)  // Ext Acidity - Low Value Float No 1 0 250 milliequivalents per 100grams
,p_extracid_r        (extracid.repr    ,PP_extracid_r    ,PPR_extracid     ,PPF_8_1_10)
,p_extracid_h        (extracid.high    ,PP_extracid_h    ,PPR_extracid     ,PPF_8_1_10)
,p_extral_l          (extral.low       ,PP_extral_l      ,PPR_extral       ,PPF_8_2_10) // Extract Al - Low Value Float No 2 0 150 milliequivalents per 100 grams
,p_extral_r          (extral.repr      ,PP_extral_r      ,PPR_extral       ,PPF_8_2_10)
,p_extral_h          (extral.high      ,PP_extral_h      ,PPR_extral       ,PPF_8_2_10)
,p_aloxalate_l       (aloxalate.low    ,PP_aloxalate_l   ,PPR_aloxalate    ,PPF_8_1_10) // Oxalate Al - Low Value Float No 1 0 170000 milligrams per kilogram
,p_aloxalate_r       (aloxalate.repr   ,PP_aloxalate_r   ,PPR_aloxalate    ,PPF_8_1_10)
,p_aloxalate_h       (aloxalate.high   ,PP_aloxalate_h   ,PPR_aloxalate    ,PPF_8_1_10)
,p_pbray1_l          (pbray1.low       ,PP_pbray1_l      ,PPR_pbray1       ,PPF_8_1_10) // Bray 1 Phos - Low Value Float No 1 0 500 milligrams per kilogram
,p_pbray1_r          (pbray1.repr      ,PP_pbray1_r      ,PPR_pbray1       ,PPF_8_1_10)
,p_pbray1_h          (pbray1.high      ,PP_pbray1_h      ,PPR_pbray1       ,PPF_8_1_10)
,p_poxalate_l        (poxalate.low     ,PP_poxalate_l    ,PPR_poxalate     ,PPF_8_1_10) // Oxalate Phos - Low Value Float No 1 0 milligrams per kilogram
,p_poxalate_r        (poxalate.repr    ,PP_poxalate_r    ,PPR_poxalate     ,PPF_8_1_10)
,p_poxalate_h        (poxalate.high    ,PP_poxalate_h    ,PPR_poxalate     ,PPF_8_1_10)
,p_ph2osoluble_l     (ph2osoluble.low  ,PP_ph2osoluble_l ,PPR_ph2osoluble  ,PPF_8_1_10)  // Water Soluble Phos - Low Value Float No 1 0 5000 milligrams per kilogram
,p_ph2osoluble_r     (ph2osoluble.repr ,PP_ph2osoluble_r ,PPR_ph2osoluble  ,PPF_8_1_10)
,p_ph2osoluble_h     (ph2osoluble.high ,PP_ph2osoluble_h ,PPR_ph2osoluble  ,PPF_8_1_10)
,p_ptotal_l          (ptotal.low       ,PP_ptotal_l      ,PPR_ptotal       ,PPF_8_2_10)   // Total Phos - Low Value Float No 2 0 percent
,p_ptotal_r          (ptotal.repr      ,PP_ptotal_r      ,PPR_ptotal       ,PPF_8_2_10)
,p_ptotal_h          (ptotal.high      ,PP_ptotal_h      ,PPR_ptotal       ,PPF_8_2_10)
//,excavdifcl() // Excav Diff Choice No 254 //Excavation_difficulty_class
//,excavdifms() // Excav Diff Moisture Choice No 254 //Observed_soil_moisture_status
//,cokey() //  Component Key String Yes 30
//,chkey() // Chorizon Key String Yes 30
{  clear();                                                                      //111212
}
//_CHorizon:constructor________________________________________________________/
bool CHorizon_record::expect_structure(bool for_write)
{
   bool expected = CORN::Data_record::expect_structure(for_write);
   expect_string("hzname",hzname,12); // Designation String No 12
   expect_parameter(p_desgndisc); // Disc Integer No 2 99
   expect_string("desgnmaster",desgnmaster,254); // Master Choice
   expect_string("desgnmasterprime",desgnmasterprime,254); // Prime Choice No 254 Horz_desgn_master_prime
   expect_parameter(p_desgnvert);  // Sub   1
   expect_parameter(p_hzdept_l);  // Top Depth 0 9999 centimeters
   expect_parameter(p_hzdept_r);
   expect_parameter(p_hzdept_h);
   expect_parameter(p_hzdepb_l);     // Bottom depth centimeters
   expect_parameter(p_hzdepb_r);
   expect_parameter(p_hzdepb_h);
   expect_parameter(p_hzthk_l);      // Thickness - e   0 9999 centimeters
   expect_parameter(p_hzthk_r);
   expect_parameter(p_hzthk_h);
   expect_parameter(p_fraggt10_l);   // Rock >10 -   0 100 percent
   expect_parameter(p_fraggt10_r);
   expect_parameter(p_fraggt10_h);
   expect_parameter(p_frag3to10_l);  // Rock 3-10 - 0 100 percent
   expect_parameter(p_frag3to10_r);
   expect_parameter(p_frag3to10_h);
   expect_parameter(p_sieveno4_l);   // #4 -  1 0 100 percent
   expect_parameter(p_sieveno4_r);
   expect_parameter(p_sieveno4_h);
   expect_parameter(p_sieveno10_l);  //  #10 -  1 0 100 percent
   expect_parameter(p_sieveno10_r);
   expect_parameter(p_sieveno10_h);
   expect_parameter(p_sieveno40_l);  //#40  1 0 100 percent
   expect_parameter(p_sieveno40_r);
   expect_parameter(p_sieveno40_h);
   expect_parameter(p_sieveno200_l); //  #200  1 0 100 percent
   expect_parameter(p_sieveno200_r);
   expect_parameter(p_sieveno200_h);
   expect_parameter(p_sandtotal_l);  //  Total Sand 1 0 100 percent
   expect_parameter(p_sandtotal_r);
   expect_parameter(p_sandtotal_h);
   expect_parameter(p_sandvc_l);     //  vcos   1 0 100 percent
   expect_parameter(p_sandvc_r);
   expect_parameter(p_sandvc_h);
   expect_parameter(p_sandco_l);     //  cos  1 0 100 percent
   expect_parameter(p_sandco_r);
   expect_parameter(p_sandco_h);
   expect_parameter(p_sandmed_l);    //  ms  1 0 100 percent
   expect_parameter(p_sandmed_r);
   expect_parameter(p_sandmed_h);
   expect_parameter(p_sandfine_l);   //  fs  1 0 100 percent
   expect_parameter(p_sandfine_r);
   expect_parameter(p_sandfine_h);
   expect_parameter(p_sandvf_l);     //  vfs 1 0 100 percent
   expect_parameter(p_sandvf_r);
   expect_parameter(p_sandvf_h);
   expect_parameter(p_silttotal_l);  //  Total Silt 1 0 100 percent
   expect_parameter(p_silttotal_r);
   expect_parameter(p_silttotal_h);
   expect_parameter(p_siltco_l);     //  Coarse Silt  1 0 100 percent
   expect_parameter(p_siltco_r);
   expect_parameter(p_siltco_h);
   expect_parameter(p_siltfine_l);   //  Fine Silt  1 0 100 percent
   expect_parameter(p_siltfine_r);
   expect_parameter(p_siltfine_h);
   expect_parameter(p_claytotal_l);   //  Total Clay - Low Value Float No 1 0 100 percent
   expect_parameter(p_claytotal_r);
   expect_parameter(p_claytotal_h);
   expect_parameter(p_claysizedcarb_l);  //  CaCO3 Clay - Low Value Float No 1 0 100 percent
   expect_parameter(p_claysizedcarb_r);
   expect_parameter(p_claysizedcarb_h);
   expect_parameter(p_om_l);        //  OM - Low Value Float No 2 0 100 percent
   expect_parameter(p_om_r);
   expect_parameter(p_om_h);
   expect_parameter(p_dbtenthbar_l); //  Db 0.1 bar H2O - Low Value Float No 2 0.02 2.6 grams per cubic centimeter
   expect_parameter(p_dbtenthbar_r);
   expect_parameter(p_dbtenthbar_h);
   expect_parameter(p_dbthirdbar_l); //  Db 0.33 bar H2O - Low Value Float No 2 0.02 2.6 grams per cubic centimeter
   expect_parameter(p_dbthirdbar_r);
   expect_parameter(p_dbthirdbar_h);
   expect_parameter(p_dbfifteenbar_l);//  Db 15 bar H2O - Low Value Float No 2 0.02 2.6 grams per cubic centimeter
   expect_parameter(p_dbfifteenbar_r);
   expect_parameter(p_dbfifteenbar_h);
   expect_parameter(p_dbovendry_l);//  Db oven dry - Low Value Float No 2 0.02 2.6 grams per cubic centimeter
   expect_parameter(p_dbovendry_r);
   expect_parameter(p_dbovendry_h);
   expect_parameter(p_partdensity); // Dp Float No 2 0.01 5 grams per cubic centimeter
   expect_parameter(p_ksat_l); //  Ksat - Low Value  4 0 705 micrometers per second
   expect_parameter(p_ksat_r);
   expect_parameter(p_ksat_h);
   expect_parameter(p_awc_l); //  AWC - Low Value  2 0 0.7 centimeters per centimeter
   expect_parameter(p_awc_r);
   expect_parameter(p_awc_h);
   expect_parameter(p_wtenthbar_l); // 0.1 bar H2O - Low Value  1 0 2000 percent
   expect_parameter(p_wtenthbar_r);
   expect_parameter(p_wtenthbar_h);
   expect_parameter(p_wthirdbar_l); // 0.33 bar H2O - Low Value  1 0 2000 percent
   expect_parameter(p_wthirdbar_r);
   expect_parameter(p_wthirdbar_h);
   expect_parameter(p_wfifteenbar_l); // 15 bar H2O - Low Value  1 0 400 percent
   expect_parameter(p_wfifteenbar_r);
   expect_parameter(p_wfifteenbar_h);
   expect_parameter(p_wsatiated_l); // Satiated H2O - Low Value   10 70 percent
   expect_parameter(p_wsatiated_r);
   expect_parameter(p_wsatiated_h);
   expect_parameter(p_lep_l);  // LEP - Low Value Float No 1 0 30 percent
   expect_parameter(p_lep_r);
   expect_parameter(p_lep_h);
   expect_parameter(p_ll_l);  //LL - Low Value Float No 1 0 400 percent
   expect_parameter(p_ll_r);
   expect_parameter(p_ll_h);
   expect_parameter(p_pi_l); //PI - Low Value Float No 1 0 130 percent
   expect_parameter(p_pi_r);
   expect_parameter(p_pi_h);
   expect_parameter(p_aashind_l); // AASHTO Group Index - Low Value Integer No 0 120
   expect_parameter(p_aashind_r);
   expect_parameter(p_aashind_h);
   expect_string("kwfact",kwfact,254);
   expect_string("kffact",kffact,254);
   expect_parameter(p_caco3_l); // CaCO3 - Low Value Integer No 0 110 percent
   expect_parameter(p_caco3_r);
   expect_parameter(p_caco3_h);
   expect_parameter(p_gypsum_l);  // Gypsum - Low Value Integer No 0 120 percent
   expect_parameter(p_gypsum_r);
   expect_parameter(p_gypsum_h);
   expect_parameter(p_sar_l);// SAR - Low Value Float No 1 0 9999
   expect_parameter(p_sar_r);
   expect_parameter(p_sar_h);
   expect_parameter(p_ec_l); // EC - Low Value Float No 1 0 15000 millimhos per centimeter
   expect_parameter(p_ec_r);
   expect_parameter(p_ec_h);
   expect_parameter(p_cec7_l);  // CEC-7 - Low Value Float No 1 0 400 milliequivalents per 100grams
   expect_parameter(p_cec7_r);
   expect_parameter(p_cec7_h);
   expect_parameter(p_ecec_l); // ECEC - Low Value Float No 1 0 400 milliequivalents per 100grams
   expect_parameter(p_ecec_r);
   expect_parameter(p_ecec_h);
   expect_parameter(p_sumbases_l); // Sum of Bases - Low Value Float No 1 0 300 milliequivalents per 100grams
   expect_parameter(p_sumbases_r);
   expect_parameter(p_sumbases_h);
   expect_parameter(p_ph1to1h2o_l); // pH H2O - Low Value Float No 1 1.8 11
   expect_parameter(p_ph1to1h2o_r);
   expect_parameter(p_ph1to1h2o_h);
   expect_parameter(p_ph01mcacl2_l); // pH CaCl2 - Low Value Float No 1 1.8 11
   expect_parameter(p_ph01mcacl2_r);
   expect_parameter(p_ph01mcacl2_h);
   expect_parameter(p_freeiron_l); // Free Iron - Low Value Float No 2 0 100 percent
   expect_parameter(p_freeiron_r);
   expect_parameter(p_freeiron_h);
   expect_parameter(p_feoxalate_l);// Oxalate Fe - Low Value Float No 2 0 150000 milligrams per kilogram
   expect_parameter(p_feoxalate_r);
   expect_parameter(p_feoxalate_h);
   expect_parameter(p_extracid_l);  // Ext Acidity - Low Value Float No 1 0 250 milliequivalents per 100grams
   expect_parameter(p_extracid_r);
   expect_parameter(p_extracid_h);
   expect_parameter(p_extral_l); // Extract Al - Low Value Float No 2 0 150 milliequivalents per 100 grams
   expect_parameter(p_extral_r);
   expect_parameter(p_extral_h);
   expect_parameter(p_aloxalate_l);// Oxalate Al - Low Value Float No 1 0 170000 milligrams per kilogram
   expect_parameter(p_aloxalate_r);
   expect_parameter(p_aloxalate_h);
   expect_parameter(p_pbray1_l); // Bray 1 Phos - Low Value Float No 1 0 500 milligrams per kilogram
   expect_parameter(p_pbray1_r);
   expect_parameter(p_pbray1_h);
   expect_parameter(p_poxalate_l); // Oxalate Phos - Low Value Float No 1 0 milligrams per kilogram
   expect_parameter(p_poxalate_r);
   expect_parameter(p_poxalate_h);
   expect_parameter(p_ph2osoluble_l);  // Water Soluble Phos - Low Value Float No 1 0 5000 milligrams per kilogram
   expect_parameter(p_ph2osoluble_r);
   expect_parameter(p_ph2osoluble_h);
   expect_parameter(p_ptotal_l);   // Total Phos - Low Value Float No 2 0 percent
   expect_parameter(p_ptotal_r);
   expect_parameter(p_ptotal_h);
   expect_string("excavdifcl",excavdifcl,254);
   expect_string("excavdifms",excavdifms,254);
   expect_string("cokey",cokey,30);
   expect_string("chkey",chkey,30);
   structure_defined = true;                                                     //120314
   return expected;
}
//_expect_structure____________________________________________________________/
}//_namespace USDA_NRCS________________________________________________________/

/*
"H1"||"H"||1||0|||36|||||0|0|0|0|0|0|95|97.5|100|85|92.5|100|70|82.5|95|40|55|70||69.2|||0.1|||3.2|||15.7|||33.2|||17|||16.3||||||||7|14.5|22||||0.5|0.75|1||||1.3|1.45|1.6|||||1.52|||4.2343|23.2887|42.343|0.08|0.12|0.15|||||19.2|||10|||41||0|1.5|2.9|15|22.5|30|0|5|10||||".24"|".24"|0|0|0|0|0|0|0|0|0|0|0|0||||||||||4.5|5.3|6|||||||||||||||||||||||||||||||||"657761:942642"|"657761:2409983"
"H2"||"H"||2||36|||104|||||0|0|0|0|0|0|90|95|100|85|92.5|100|80|87.5|95|36|53|70||38.5|||3.4|||4.9|||6.7|||12.1|||11.4|||36.5||||||||18|25|32||||||||||1.4|1.48|1.55|||||1.55|||4.2343|9.1743|14.1143|0.12|0.15|0.18|||||||||||||0|1.5|2.9|15|25|35|2|8.5|15||||".32"|".32"|0|0|0|0|0|0|0|0|0|0|0|0||||||||||4.5|5|5.5|||||||||||||||||||||||||||||||||"657761:942642"|"657761:2409984"
"H3"||"H"||3||104|||188|||||0|0|0|0|0|0|85|92.5|100|80|90|100|80|87.5|95|40|55|70||37.2|||3.3|||4.7|||6.5|||11.7|||11|||35.3||||||||20|27.5|35||||||||||1.4|1.5|1.6|||||1.57|||4.2343|9.1743|14.1143|0.12|0.15|0.18|||||||||||||0|1.5|2.9|20|30|40|8|13|18||||".32"|".32"|0|0|0|0|0|0|0|0|0|0|0|0||||||||||4.5|5|5.5|||||||||||||||||||||||||||||||||"657761:942642"|"657761:2409985"
*/
