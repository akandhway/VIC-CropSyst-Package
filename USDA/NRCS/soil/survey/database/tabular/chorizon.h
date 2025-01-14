//---------------------------------------------------------------------------
#ifndef chorizonH
#define chorizonH
#include "chorizon_struct.h"
#include "corn/parameters/parameter.h"
#include "corn/data_source/datarec.h"
//---------------------------------------------------------------------------
namespace USDA_NRCS
{
//______________________________________________________________________________
class CHorizon_record
: public CHorizon_struct
, public CORN::Data_record
{
 public:
//   CORN::Ustring hzname; // Designation String No 12
   CORN::Parameter_number p_desgndisc; // Disc Integer No 2 99
//   CORN::Ustring desgnmaster; // Master Choice No 254   Horz_desgn_master
//   CORN::Ustring desgnmasterprime; // Prime Choice No 254 Horz_desgn_master_prime
   CORN::Parameter_number p_desgnvert;  // Sub   1
   CORN::Parameter_number p_hzdept_l;  // Top Depth 0 9999 centimeters
   CORN::Parameter_number p_hzdept_r;
   CORN::Parameter_number p_hzdept_h;
   CORN::Parameter_number p_hzdepb_l;     // Bottom depth centimeters
   CORN::Parameter_number p_hzdepb_r;
   CORN::Parameter_number p_hzdepb_h;
   CORN::Parameter_number p_hzthk_l;      // Thickness - e   0 9999 centimeters
   CORN::Parameter_number p_hzthk_r;
   CORN::Parameter_number p_hzthk_h;
   CORN::Parameter_number p_fraggt10_l;   // Rock >10 -   0 100 percent
   CORN::Parameter_number p_fraggt10_r;
   CORN::Parameter_number p_fraggt10_h;
   CORN::Parameter_number p_frag3to10_l;  // Rock 3-10 - 0 100 percent
   CORN::Parameter_number p_frag3to10_r;
   CORN::Parameter_number p_frag3to10_h;
   CORN::Parameter_number p_sieveno4_l;   // #4 -  1 0 100 percent
   CORN::Parameter_number p_sieveno4_r;
   CORN::Parameter_number p_sieveno4_h;
   CORN::Parameter_number p_sieveno10_l;  //  #10 -  1 0 100 percent
   CORN::Parameter_number p_sieveno10_r;
   CORN::Parameter_number p_sieveno10_h;
   CORN::Parameter_number p_sieveno40_l;  //#40  1 0 100 percent
   CORN::Parameter_number p_sieveno40_r;
   CORN::Parameter_number p_sieveno40_h;
   CORN::Parameter_number p_sieveno200_l; //  #200  1 0 100 percent
   CORN::Parameter_number p_sieveno200_r;
   CORN::Parameter_number p_sieveno200_h;
   CORN::Parameter_number p_sandtotal_l;  //  Total Sand 1 0 100 percent
   CORN::Parameter_number p_sandtotal_r;
   CORN::Parameter_number p_sandtotal_h;
   CORN::Parameter_number p_sandvc_l;     //  vcos   1 0 100 percent
   CORN::Parameter_number p_sandvc_r;
   CORN::Parameter_number p_sandvc_h;
   CORN::Parameter_number p_sandco_l;     //  cos  1 0 100 percent
   CORN::Parameter_number p_sandco_r;
   CORN::Parameter_number p_sandco_h;
   CORN::Parameter_number p_sandmed_l;    //  ms  1 0 100 percent
   CORN::Parameter_number p_sandmed_r;
   CORN::Parameter_number p_sandmed_h;
   CORN::Parameter_number p_sandfine_l;   //  fs  1 0 100 percent
   CORN::Parameter_number p_sandfine_r;
   CORN::Parameter_number p_sandfine_h;
   CORN::Parameter_number p_sandvf_l;     //  vfs 1 0 100 percent
   CORN::Parameter_number p_sandvf_r;
   CORN::Parameter_number p_sandvf_h;
   CORN::Parameter_number p_silttotal_l;  //  Total Silt 1 0 100 percent
   CORN::Parameter_number p_silttotal_r;
   CORN::Parameter_number p_silttotal_h;
   CORN::Parameter_number p_siltco_l;     //  Coarse Silt  1 0 100 percent
   CORN::Parameter_number p_siltco_r;
   CORN::Parameter_number p_siltco_h;
   CORN::Parameter_number p_siltfine_l;   //  Fine Silt  1 0 100 percent
   CORN::Parameter_number p_siltfine_r;
   CORN::Parameter_number p_siltfine_h;
   CORN::Parameter_number p_claytotal_l;   //  Total Clay - Low Value Float No 1 0 100 percent
   CORN::Parameter_number p_claytotal_r;
   CORN::Parameter_number p_claytotal_h;
   CORN::Parameter_number p_claysizedcarb_l;  //  CaCO3 Clay - Low Value Float No 1 0 100 percent
   CORN::Parameter_number p_claysizedcarb_r;
   CORN::Parameter_number p_claysizedcarb_h;
   CORN::Parameter_number p_om_l;        //  OM - Low Value Float No 2 0 100 percent
   CORN::Parameter_number p_om_r;
   CORN::Parameter_number p_om_h;
   CORN::Parameter_number p_dbtenthbar_l; //  Db 0.1 bar H2O - Low Value Float No 2 0.02 2.6 grams per cubic centimeter
   CORN::Parameter_number p_dbtenthbar_r;
   CORN::Parameter_number p_dbtenthbar_h;
   CORN::Parameter_number p_dbthirdbar_l; //  Db 0.33 bar H2O - Low Value Float No 2 0.02 2.6 grams per cubic centimeter
   CORN::Parameter_number p_dbthirdbar_r;
   CORN::Parameter_number p_dbthirdbar_h;
   CORN::Parameter_number p_dbfifteenbar_l;//  Db 15 bar H2O - Low Value Float No 2 0.02 2.6 grams per cubic centimeter
   CORN::Parameter_number p_dbfifteenbar_r;
   CORN::Parameter_number p_dbfifteenbar_h;
   CORN::Parameter_number p_dbovendry_l;//  Db oven dry - Low Value Float No 2 0.02 2.6 grams per cubic centimeter
   CORN::Parameter_number p_dbovendry_r;
   CORN::Parameter_number p_dbovendry_h;
   CORN::Parameter_number p_partdensity; // Dp Float No 2 0.01 5 grams per cubic centimeter
   CORN::Parameter_number p_ksat_l; //  Ksat - Low Value  4 0 705 micrometers per second
   CORN::Parameter_number p_ksat_r;
   CORN::Parameter_number p_ksat_h;
   CORN::Parameter_number p_awc_l; //  AWC - Low Value  2 0 0.7 centimeters per centimeter
   CORN::Parameter_number p_awc_r;
   CORN::Parameter_number p_awc_h;
   CORN::Parameter_number p_wtenthbar_l; // 0.1 bar H2O - Low Value  1 0 2000 percent
   CORN::Parameter_number p_wtenthbar_r;
   CORN::Parameter_number p_wtenthbar_h;
   CORN::Parameter_number p_wthirdbar_l; // 0.33 bar H2O - Low Value  1 0 2000 percent
   CORN::Parameter_number p_wthirdbar_r;
   CORN::Parameter_number p_wthirdbar_h;
   CORN::Parameter_number p_wfifteenbar_l; // 15 bar H2O - Low Value  1 0 400 percent
   CORN::Parameter_number p_wfifteenbar_r;
   CORN::Parameter_number p_wfifteenbar_h;
   CORN::Parameter_number p_wsatiated_l; // Satiated H2O - Low Value   10 70 percent
   CORN::Parameter_number p_wsatiated_r;
   CORN::Parameter_number p_wsatiated_h;
   CORN::Parameter_number p_lep_l;  // LEP - Low Value Float No 1 0 30 percent
   CORN::Parameter_number p_lep_r;
   CORN::Parameter_number p_lep_h;
   CORN::Parameter_number p_ll_l;  //LL - Low Value Float No 1 0 400 percent
   CORN::Parameter_number p_ll_r;
   CORN::Parameter_number p_ll_h;
   CORN::Parameter_number p_pi_l; //PI - Low Value Float No 1 0 130 percent
   CORN::Parameter_number p_pi_r;
   CORN::Parameter_number p_pi_h;
   CORN::Parameter_number p_aashind_l; // AASHTO Group Index - Low Value Integer No 0 120
   CORN::Parameter_number p_aashind_r;
   CORN::Parameter_number p_aashind_h;
//   CORN::Ustring kwfact; //Kw 254 Soil_erodibility_factor
//   CORN::Ustring kffact; //Kf  254  Soil_erodibility_factor
   CORN::Parameter_number p_caco3_l; // CaCO3 - Low Value Integer No 0 110 percent
   CORN::Parameter_number p_caco3_r;
   CORN::Parameter_number p_caco3_h;
   CORN::Parameter_number p_gypsum_l;  // Gypsum - Low Value Integer No 0 120 percent
   CORN::Parameter_number p_gypsum_r;
   CORN::Parameter_number p_gypsum_h;
   CORN::Parameter_number p_sar_l;// SAR - Low Value Float No 1 0 9999
   CORN::Parameter_number p_sar_r;
   CORN::Parameter_number p_sar_h;
   CORN::Parameter_number p_ec_l; // EC - Low Value Float No 1 0 15000 millimhos per centimeter
   CORN::Parameter_number p_ec_r;
   CORN::Parameter_number p_ec_h;
   CORN::Parameter_number p_cec7_l;  // CEC-7 - Low Value Float No 1 0 400 milliequivalents per 100grams
   CORN::Parameter_number p_cec7_r;
   CORN::Parameter_number p_cec7_h;
   CORN::Parameter_number p_ecec_l; // ECEC - Low Value Float No 1 0 400 milliequivalents per 100grams
   CORN::Parameter_number p_ecec_r;
   CORN::Parameter_number p_ecec_h;
   CORN::Parameter_number p_sumbases_l; // Sum of Bases - Low Value Float No 1 0 300 milliequivalents per 100grams
   CORN::Parameter_number p_sumbases_r;
   CORN::Parameter_number p_sumbases_h;
   CORN::Parameter_number p_ph1to1h2o_l; // pH H2O - Low Value Float No 1 1.8 11
   CORN::Parameter_number p_ph1to1h2o_r;
   CORN::Parameter_number p_ph1to1h2o_h;
   CORN::Parameter_number p_ph01mcacl2_l; // pH CaCl2 - Low Value Float No 1 1.8 11
   CORN::Parameter_number p_ph01mcacl2_r;
   CORN::Parameter_number p_ph01mcacl2_h;
   CORN::Parameter_number p_freeiron_l; // Free Iron - Low Value Float No 2 0 100 percent
   CORN::Parameter_number p_freeiron_r;
   CORN::Parameter_number p_freeiron_h;
   CORN::Parameter_number p_feoxalate_l;// Oxalate Fe - Low Value Float No 2 0 150000 milligrams per kilogram
   CORN::Parameter_number p_feoxalate_r;
   CORN::Parameter_number p_feoxalate_h;
   CORN::Parameter_number p_extracid_l;  // Ext Acidity - Low Value Float No 1 0 250 milliequivalents per 100grams
   CORN::Parameter_number p_extracid_r;
   CORN::Parameter_number p_extracid_h;
   CORN::Parameter_number p_extral_l; // Extract Al - Low Value Float No 2 0 150 milliequivalents per 100 grams
   CORN::Parameter_number p_extral_r;
   CORN::Parameter_number p_extral_h;
   CORN::Parameter_number p_aloxalate_l;// Oxalate Al - Low Value Float No 1 0 170000 milligrams per kilogram
   CORN::Parameter_number p_aloxalate_r;
   CORN::Parameter_number p_aloxalate_h;
   CORN::Parameter_number p_pbray1_l; // Bray 1 Phos - Low Value Float No 1 0 500 milligrams per kilogram
   CORN::Parameter_number p_pbray1_r;
   CORN::Parameter_number p_pbray1_h;
   CORN::Parameter_number p_poxalate_l; // Oxalate Phos - Low Value Float No 1 0 milligrams per kilogram
   CORN::Parameter_number p_poxalate_r;
   CORN::Parameter_number p_poxalate_h;
   CORN::Parameter_number p_ph2osoluble_l;  // Water Soluble Phos - Low Value Float No 1 0 5000 milligrams per kilogram
   CORN::Parameter_number p_ph2osoluble_r;
   CORN::Parameter_number p_ph2osoluble_h;
   CORN::Parameter_number p_ptotal_l;   // Total Phos - Low Value Float No 2 0 percent
   CORN::Parameter_number p_ptotal_r;
   CORN::Parameter_number p_ptotal_h;
//   CORN::Ustring excavdifcl; // Excav Diff Choice No 254 //Excavation_difficulty_class
//   CORN::Ustring excavdifms; // Excav Diff Moisture Choice No 254 //Observed_soil_moisture_status
//   CORN::Ustring cokey; //  Component Key String Yes 30
//   CORN::Ustring chkey; // Chorizon Key String Yes 30
 public:
   CHorizon_record();
   virtual bool expect_structure(bool for_write) ;
};
//_CHorizon_record_____________________________________________________________/
}//_namespace USDA_NRCS________________________________________________________/
#endif
