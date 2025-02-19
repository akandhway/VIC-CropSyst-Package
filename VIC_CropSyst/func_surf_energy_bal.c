#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vicNl.h>
#include <iostream>     //LML 150508
#if (VIC_CROPSYST_VERSION>=3)
#include <assert.h>
#endif

static char vcid[] = "$Id: func_surf_energy_bal.c,v 5.8.2.23 2012/02/05 00:15:44 vicadmin Exp $";
//extern double **accessable_aero_resistance; ///121129 keyvan
double func_surf_energy_bal(double Ts, va_list ap)
/**********************************************************************
	func_surf_energy_bal	Keith Cherkauer		January 3, 1996

  This subroutine computes the surface energy balance for bare soil
  and vegetation uncovered by snow.  It computes outgoing longwave,
  sensible heat flux, ground heat flux, and storage of heat in the thin
  upper layer, based on the given surface temperature.

  The Energy Balance Equation used comes from Xu Liang's Paper
  "Insights of the Ground Heat Flux in Land Surface Parameterization
  Schemes."

  Modifications:
  04-14-98 modified to compute evapotranspiration within this routine
           in the hopes of reducing the number of iteration
      needed to find a solution surface temperature.       KAC
  07-13-98 modified to include elevation bands for vegetation
           and snow                                             KAC
  01-20-00 modified to work with the updated radiation estimation
           routines, as well as the simplified frozen soil moisture
           storage                                              KAC
  01-08-01 sensible heat is now set to 0 W/m^2 when the ground is
           fully covered by snow.                                KAC
  04-12-01 fixed error where sensible heat flux for partial bare
           ground was not multiplied by the snow cover fraction. KAC
  04-29-02 moved calculation of sensible heat so that it is computed
           even in water balance mode.  This assures that it is set
           to 0 in water balance mode and does not yield the
           cumulative sum of sensible heat from the snowpack.    KAC
  11-18-02 modified to compute the effects of blowing snow on the
           surface energy balance.                               LCB
  10-May-04 Added check that both FS_ACTIVE and FROZEN_SOIL are true
        before computing *fusion.  This is just a safety measure;
        ice and ice0 should both be 0 if FS_ACTIVE is FALSE.TJB
  16-Jul-04 Renamed VaporMassFlux, BlowingMassFlux, and SurfaceMassFlux
        to vapor_flux, blowing_flux, and surface_flux, respectively,
        to denote fact that their units are m/timestep rather than
        kg/m2s.  Created new variables VaporMassFlux, BlowingMassFlux,
        and SurfaceMassFlux with units of kg/m2s.  The addresses of
        the *MassFlux variables are passed to latent_heat_from_snow()
        where values for the variables are computed.  After these
        values are computed, vapor_flux, blowing_flux and surface_flux
        are derived from them by unit conversion.  vapor_flux,
        blowing_flux, and surface_flux are the variables that are
        passed in/out of this function.        	TJB
  16-Jul-04 Changed the type of the last few variables (lag_one, Nveg,
        etc) in the va_list to be double.  For some reason, passing
        them as float or int caused them to become garbage.  This may
        have to do with the fact that they followed variables of type
        (double *) in va_list, which may have caused memory alignment
        problems.                    	TJB
  05-Aug-04 Removed iveg, LastSnow, dt, SnowDepth, lag_one, sigma_slope,
        fetch, and Nveg from the this function's argument list,
        since these variables were only used in the call to
        latent_heat_from_snow() which no longer needs them.	TJB
  28-Sep-04 Added Ra_used to store the aerodynamic resistance used in
        flux calculations.                	TJB
  2007-Apr-11 Modified to handle grid cell errors by returning to the
              main subroutine, rather than ending the simulation.	GCT
  2007-Apr-24 Removed (1.-snow_coverage) from three equations where it did not
              belong: for calculating LongBareOut in the second two cases and for
              calculating NetBareRad in the third case.        	JCA
  2007-Apr-24 Features included for IMPLICIT frozen soils option.	JCA
              (including passing in nrec, nrecs, and iveg)
              (including passing in bulk_density, soil_density, and quartz)
              (including counting cases when IMPLICIT fails and involes EXPLICIT)
  2007-Apr-24 Features included for EXP_TRANS frozen soils option.	JCA
  2007-Apr-24 Passing in Zsum_node.                	JCA
  2007-Aug-07 Moved Implicit error counting above call for
              solve_T_profile.                    	JCA
  2008-Aug-08 Added option for EXCESS_ICE.            	JCA
              including: passing entire soil_con structure to
              calc_surf_energy_bal
  2007-Aug-24 Modified to use arno_evap rather than canopy_evap if LAI
              is 0, e.g. winter cropland.            	KAC via TJB
  2008-Mar-01 Fixed typo in declaration of ufwc_table_layer.    	TJB
  2009-Feb-09 Modified to remove dz_node.            	KAC via TJB
  2009-May-20 Corrected the deltaH and fusion terms to account for
          surf_atten, as in Liang et al, 1999.
          Added options.GRND_FLUX_TYPE to allow backwards-compatibility
          with versions 4.0.6 and 4.1.0.            	TJB
  2009-Jun-19 Added T flag to indicate whether TFALLBACK occurred.	TJB
  2009-Jul-26 Removed the special logic for the water balance mode, in
          which net longwave is stored in the "longwave" variable.	TJB
  2009-Sep-19 Added T fbcount to count TFALLBACK occurrences.    	TJB
  2009-Nov-15 Changed definitions of D1 and D2 to work for arbitrary
          node spacing.                    	TJB
  2010-Apr-24 Replaced ra_under with Ra_used[0].        	TJB
  2010-Apr-28 Removed net_short, displacement, roughness, and ref_height
          from arg list of arno_evap() as they are no longer used.	TJB
  2011-May-31 Removed options.GRND_FLUX.  Soil temperatures and ground
          flux are now always computed.            	TJB
  2011-Jun-03 Added options.ORGANIC_FRACT.  Soil properties now take
          organic fraction into account.            	TJB
  2011-Aug-09 Now method used for estimating soil temperatures depends only
          on QUICK_FLUX setting.                	TJB
  2012-Jan-16 Removed LINK_DEBUG code		        	BN
  2012-Jan-28 Changed ground flux etc calculations for case of exponential
          node distribution to be over the same control volume as for
          the linear node distribution and quick flux cases.	TJB
  2012-Jan-28 Removed AR_COMBO and GF_FULL.            	TJB
**********************************************************************/
{
  extern option_struct options;
  extern veg_lib_struct *veg_lib;

  /* define routine input variables */
  /* general model terms */
  int i;
  int rec;
  int nrecs;
  int month;
  int VEG;
  int veg_class;
  int iveg;
  int Error;

  //error counting variables for IMPLICIT option
  static int error_cnt0, error_cnt1;

  double delta_t;

  /* soil layer terms */
  double Cs1;
  double Cs2;
  double D1;
  double D2;
  double T1_old;
  double T2;
  double Ts_old;
  double *Told_node;
  double b_infilt;
  double bubble;
  double dp;
  double expt;
  double ice0;
  double kappa1;
  double kappa2;
  double max_infil;
  double max_moist;
  double moist;

  double *Wcr;
  double *Wpwp;
  double *depth;
  double *resid_moist;
  double *bulk_dens_min;
  double *soil_dens_min;
  double *quartz;
  double *bulk_density;
  double *soil_density;
  double *organic;

  float *root;

  /* meteorological forcing terms */
  int UnderStory;
  int overstory;

  double NetShortBare;  // net SW that reaches bare ground
  double NetShortGrnd;  // net SW that penetrates snowpack
  double NetShortSnow;  // net SW that reaches snow surface
  double Tair;          // temperature of canopy air or atmosphere
  double atmos_density;
  double atmos_pressure;
  double elevation;
  double emissivity;
  double LongBareIn; // incoming LW to snow-free surface
  double LongSnowIn; // incoming LW to snow surface - if INCLUDE_SNOW
  double mu;
  double surf_atten;
  double vp;
  double vpd;

  double *Wdew;
  double *displacement;
  double *ra;
  double *Ra_used;
  double *rainfall;
  double *ref_height;
  double *roughness;
  double *wind;

  /* latent heat terms */
  double  Le;

  /* snowpack terms */
  double Advection;
  double OldTSurf;
  double TPack;
  double Tsnow_surf;
  double kappa_snow; // snow conductance / depth
  double melt_energy; // energy consumed in reducing the snowpack coverage
  double snow_coverage; // snowpack coverage fraction
  double snow_density;
/*   double snow_depth; */
  double snow_swq;
  double snow_water;

  double *deltaCC;
  double *refreeze_energy;
  double *vapor_flux;
  double *blowing_flux;
  double *surface_flux;

  /* soil node terms */
  int     Nnodes;

  double *Cs_node;
  double *T_node;
  double *Tnew_node;
  char   *Tnew_fbflag;
  int    *Tnew_fbcount;
  double *alpha;
  double *beta;
  double *bubble_node;
  double *Zsum_node;
  double *expt_node;
  double *gamma;
  double *ice_node;
  double *kappa_node;
  double *max_moist_node;
  double *moist_node;

  /* spatial frost terms */
#if SPATIAL_FROST
  double *frost_fract;
#endif

  /* quick solution frozen soils terms */
#if QUICK_FS
  double ***ufwc_table_layer;
  double ***ufwc_table_node;
#endif

  /* excess ice terms */
#if EXCESS_ICE
  double porosity; //top layer
  double effective_porosity; //top layer
  double *porosity_node;
  double *effective_porosity_node;
#endif

  /* model structures */
  soil_con_struct *soil_con;
  layer_data_struct *layer_wet;
  layer_data_struct *layer_dry;
  veg_var_struct *veg_var_wet;
  veg_var_struct *veg_var_dry;

  /* control flags */
  int INCLUDE_SNOW;
  int FS_ACTIVE;
  int NOFLUX;
  int EXP_TRANS;
  int SNOWING;

  int *FIRST_SOLN;

  /* returned energy balance terms */
  double *NetLongBare; // net LW from snow-free ground
  double *NetLongSnow; // net longwave from snow surface - if INCLUDE_SNOW
  double *T1;
  double *deltaH;
  double *fusion;
  double *grnd_flux;
  double *latent_heat;
  double *latent_heat_sub;
  double *sensible_heat;
  double *snow_flux;
  double *store_error;

  /* Define internal routine variables */
  double Evap;		/** Total evap in m/s **/
  double LongBareOut; // outgoing LW from snow-free ground
  double NetBareRad;
  double TMean;
  double Tmp;
  double error;
  double ice;
/*   double             kappa_snow; */
  double out_long;
  double temp_latent_heat;
  double temp_latent_heat_sub;
  double VaporMassFlux;
  double BlowingMassFlux;
  double SurfaceMassFlux;
  double T11;
  double T1_minus;
  double T1_plus;
  double D1_minus;
  double D1_plus;

  //* meteorological forcing terms *
  #if SPATIAL_FROST
  double *frost_fract;
  #endif

  //* quick solution frozen soils terms *
  #if QUICK_FS
  double ***ufwc_table_layer;
  double ***ufwc_table_node;
  #endif

  //* excess ice terms *
  #if EXCESS_ICE
  double porosity; //top layer
  double effective_porosity; //top layer
  double *porosity_node;
  double *effective_porosity_node;
  #endif

#ifdef VIC_CROPSYST_VERSION
#if (VIC_CROPSYST_VERSION>=3)
  veg_con_struct *veg_con;
  double gross_shortwave;
  double net_longwave;
  int hidx;                                                                      //170410LML
#endif
  atmos_data_struct *atmos;
  dmy_struct *dmy;
  cell_data_struct *cell_wet;
  cell_data_struct *cell_dry;
  int veg_class_code;
  int band;
  double **aero_resist_ET;
#endif //VIC_CROPSYST_VERSION

  //************************************
  //  Read variables from variable list
  //************************************
  //* general model terms *

  //FunctionInputSurfEnergyBal *seb = (FunctionInputSurfEnergyBal *) va_arg(ap, FunctionInputSurfEnergyBal *); //surf_energy_bal

  rec                     = (int) va_arg(ap, int);
  nrecs                   = (int) va_arg(ap, int);
  month                   = (int) va_arg(ap, int);
  VEG                     = (int) va_arg(ap, int);
  veg_class               = (int) va_arg(ap, int);
  iveg                    = (int) va_arg(ap, int);
  delta_t                 = (double) va_arg(ap, double);

  /* soil layer terms */
  Cs1                     = (double) va_arg(ap, double);
  Cs2                     = (double) va_arg(ap, double);
  D1                      = (double) va_arg(ap, double);
  D2                      = (double) va_arg(ap, double);
  T1_old                  = (double) va_arg(ap, double);
  T2                      = (double) va_arg(ap, double);
  Ts_old                  = (double) va_arg(ap, double);
  Told_node               = (double *) va_arg(ap, double *);
  bubble                  = (double) va_arg(ap, double);
  dp                      = (double) va_arg(ap, double);
  expt                    = (double) va_arg(ap, double);
  ice0                    = (double) va_arg(ap, double);
  kappa1                  = (double) va_arg(ap, double);
  kappa2                  = (double) va_arg(ap, double);
  max_moist               = (double) va_arg(ap, double);
  moist                   = (double) va_arg(ap, double);

  root                    = (float  *) va_arg(ap, float  *);

  /* meteorological forcing terms */
  UnderStory              = (int) va_arg(ap, int);
  overstory               = (int) va_arg(ap, int);

  NetShortBare            = (double) va_arg(ap, double);
  NetShortGrnd            = (double) va_arg(ap, double);
  NetShortSnow            = (double) va_arg(ap, double);
  Tair                    = (double) va_arg(ap, double);
  atmos_density           = (double) va_arg(ap, double);
  atmos_pressure          = (double) va_arg(ap, double);
  emissivity              = (double) va_arg(ap, double);
  LongBareIn              = (double) va_arg(ap, double);
  LongSnowIn              = (double) va_arg(ap, double);
  mu                      = (double) va_arg(ap, double);
  surf_atten              = (double) va_arg(ap, double);
  vp                      = (double) va_arg(ap, double);
  vpd                     = (double) va_arg(ap, double);

  Wdew                    = (double *) va_arg(ap, double *);
  displacement            = (double *) va_arg(ap, double *);
  ra                      = (double *) va_arg(ap, double *);
  Ra_used                 = (double *) va_arg(ap, double *);
  rainfall                = (double *) va_arg(ap, double *);
  ref_height              = (double *) va_arg(ap, double *);
  roughness               = (double *) va_arg(ap, double *);
  wind                    = (double *) va_arg(ap, double *);

  /* latent heat terms */
  Le                      = (double) va_arg(ap, double);

  /* snowpack terms */
  Advection               = (double) va_arg(ap, double);
  OldTSurf                = (double) va_arg(ap, double);
  TPack                   = (double) va_arg(ap, double);
  Tsnow_surf              = (double) va_arg(ap, double);
  kappa_snow              = (double) va_arg(ap, double);
  melt_energy             = (double) va_arg(ap, double);
  snow_coverage           = (double) va_arg(ap, double);
  snow_density            = (double) va_arg(ap, double);
  snow_swq                = (double) va_arg(ap, double);
  snow_water              = (double) va_arg(ap, double);

  deltaCC                 = (double *) va_arg(ap, double *);
  refreeze_energy         = (double *) va_arg(ap, double *);
  vapor_flux              = (double *) va_arg(ap, double *);
  blowing_flux            = (double *) va_arg(ap, double *);
  surface_flux            = (double *) va_arg(ap, double *);

  /* soil node terms */
  Nnodes                  = (int) va_arg(ap, int);

  Cs_node                 = (double *) va_arg(ap, double *);
  T_node                  = (double *) va_arg(ap, double *);
  Tnew_node               = (double *) va_arg(ap, double *);
  Tnew_fbflag             = (char *) va_arg(ap, char *);
  Tnew_fbcount            = (int *) va_arg(ap, int *);
  alpha                   = (double *) va_arg(ap, double *);
  beta                    = (double *) va_arg(ap, double *);
  bubble_node             = (double *) va_arg(ap, double *);
  Zsum_node               = (double *) va_arg(ap, double *);
  expt_node               = (double *) va_arg(ap, double *);
  gamma                   = (double *) va_arg(ap, double *);
  ice_node                = (double *) va_arg(ap, double *);
  kappa_node              = (double *) va_arg(ap, double *);
  max_moist_node          = (double *) va_arg(ap, double *);
  moist_node              = (double *) va_arg(ap, double *);

  /* model structures */
  soil_con                = (soil_con_struct *) va_arg(ap, soil_con_struct *);
  layer_wet               = (layer_data_struct *) va_arg(ap, layer_data_struct *);
  layer_dry               = (layer_data_struct *) va_arg(ap, layer_data_struct *);
  veg_var_wet             = (veg_var_struct *) va_arg(ap, veg_var_struct *);
  veg_var_dry             = (veg_var_struct *) va_arg(ap, veg_var_struct *);

  /* control flags */
  INCLUDE_SNOW            = (int) va_arg(ap, int);
  NOFLUX                  = (int) va_arg(ap, int);
  EXP_TRANS               = (int) va_arg(ap, int);
  SNOWING                 = (int) va_arg(ap, int);

  FIRST_SOLN              = (int *) va_arg(ap, int *);

  /* returned energy balance terms */
  NetLongBare             = (double *) va_arg(ap, double *);
  NetLongSnow             = (double *) va_arg(ap, double *);
  T1                      = (double *) va_arg(ap, double *);
  deltaH                  = (double *) va_arg(ap, double *);
  fusion                  = (double *) va_arg(ap, double *);
  grnd_flux               = (double *) va_arg(ap, double *);
  latent_heat             = (double *) va_arg(ap, double *);
  latent_heat_sub         = (double *) va_arg(ap, double *);
  sensible_heat           = (double *) va_arg(ap, double *);
  snow_flux               = (double *) va_arg(ap, double *);
  store_error             = (double *) va_arg(ap, double *);

#ifdef VIC_CROPSYST_VERSION
#if (VIC_CROPSYST_VERSION>=3)
  veg_con                 = (veg_con_struct *) va_arg(ap, veg_con_struct *);
  gross_shortwave         = (double) va_arg(ap, double);
  net_longwave            = (double) va_arg(ap, double);
  hidx                    = (int) va_arg(ap, int);
#endif
  atmos                   = (atmos_data_struct *) va_arg(ap, atmos_data_struct *);
  dmy                     = (dmy_struct *) va_arg(ap, dmy_struct *);
  cell_wet                = (cell_data_struct *) va_arg(ap, cell_data_struct *);
  cell_dry                = (cell_data_struct *) va_arg(ap, cell_data_struct *);
  veg_class_code          = (int) va_arg(ap, int);
  band                    = (int) va_arg(ap, int);
  aero_resist_ET          = (double **) va_arg(ap, double**);
#endif //VIC_CROPSYST_VERSION

  //* take additional variables from soil_con structure *
  #ifdef VIC_CROPSYST_VERSION
  resid_moist           = soil_con->Wpwp;                                   //150629LML confused for using resid_moist
  #else
  resid_moist           = soil_con->resid_moist;
  #endif


  #if SPATIAL_FROST
  frost_fract           = soil_con->frost_fract;
  #endif // SPATIAL_FROST
  #if QUICK_FS
  ufwc_table_layer      = soil_con->ufwc_table_layer;
  ufwc_table_node       = soil_con->ufwc_table_node;
  #endif // QUICK_FS
  #if EXCESS_ICE
  porosity              = soil_con->porosity[0];
  effective_porosity    = soil_con->effective_porosity[0];
  porosity_node         = soil_con->porosity_node;
  effective_porosity_node = soil_con->effective_porosity_node;
  #endif

  //***************
  //  MAIN ROUTINE
  //***************
  Error = 0;
  if(rec == 0){
    error_cnt0=0;
    error_cnt1=0;
  }
  TMean = Ts;
  Tmp = CELSIUS_TO_KELVIN(TMean);
  //**********************************************
  //  Compute Surface Temperature at Half Time Step
  //**********************************************
  if ( snow_coverage > 0 && !INCLUDE_SNOW ) {
    //****************************************
    //  Compute energy flux through snow pack
    //****************************************
    *snow_flux = ( kappa_snow * (Tsnow_surf - TMean) );
  } else if ( INCLUDE_SNOW ) {
    *snow_flux = 0;
    Tsnow_surf = TMean;
  } else {
    *snow_flux = 0;
  }
  //***************************************************************
  //  Estimate soil temperatures for ground heat flux calculations
  //***************************************************************

  if ( options.QUICK_FLUX ) {
    //**************************************************************
    //  Use Liang et al. 1999 Equations to Calculate Ground Heat Flux
    //  NOTE: T2 is not the temperature of layer 2, nor of node 2, nor at depth dp;
    //  T2 is the constant temperature at depths much greater than dp to which
    //  the soil temperature profile is asymptotic.
    //**************************************************************
    *(T1) = estimate_T1(TMean, T1_old, T2, D1, D2, kappa1, kappa2, Cs1, Cs2, dp, delta_t);
    //*****************************************************
    //  Compute the Ground Heat Flux from the Top Soil Layer
    //*****************************************************
    if (options.GRND_FLUX_TYPE == GF_406) {
      *grnd_flux = (snow_coverage + (1. - snow_coverage) * surf_atten) * (kappa1 / D1 * ((*T1) - TMean));
    } else {
      *grnd_flux = (snow_coverage + (1. - snow_coverage) * surf_atten) * (kappa1 / D1 * ((*T1) - TMean)
                       + (kappa2 / D2 * ( 1. - exp( -D1 / dp )) * (T2 - (*T1)))) / 2.;
    }
  } else {
    //*************************************************************
    //  Use Finite Difference Method to Solve Ground Heat
    //  Flux at Soil Thermal Nodes (Cherkauer and Lettenmaier, 1999)
    //*************************************************************
    T_node[0] = TMean;
    //* IMPLICIT Solution *
    if(options.IMPLICIT) {
      Error = solve_T_profile_implicit(Tnew_node, T_node, Zsum_node, kappa_node, Cs_node,
                       moist_node, delta_t, max_moist_node, bubble_node, expt_node,
#if EXCESS_ICE
                       porosity_node, effective_porosity_node,
#endif
                       ice_node, alpha, beta, gamma, dp, Nnodes,
                       FIRST_SOLN, soil_con->FS_ACTIVE, NOFLUX, EXP_TRANS, veg_class,
                       soil_con->bulk_dens_min, soil_con->soil_dens_min, soil_con->quartz, soil_con->bulk_density, soil_con->soil_density, soil_con->organic, soil_con->depth);

      //* print out error information for IMPLICIT solution *
      if(Error==0) error_cnt0++;
      else         error_cnt1++;
      if(FIRST_SOLN[1]){
        FIRST_SOLN[1] = FALSE;
        #if VERBOSE
        if ( iveg == 0 && rec == nrecs - 1)
          fprintf(stderr,"The implicit scheme failed %d instances (%.1f%c of attempts).\n",error_cnt1,100.0*(float)error_cnt1/((float)error_cnt0+(float)error_cnt1),'%');
        #endif
      }
    }
    //* EXPLICIT Solution, or if IMPLICIT Solution Failed *
    if(!options.IMPLICIT || Error == 1) {
      if(options.IMPLICIT)
        FIRST_SOLN[0] = TRUE;
      #if QUICK_FS
      Error = solve_T_profile(Tnew_node, T_node, Tnew_fbflag, Tnew_fbcount, Zsum_node, kappa_node, Cs_node,
                  moist_node, delta_t, max_moist_node, bubble_node,
                  expt_node, ice_node, alpha, beta, gamma, dp,
                  depth, ufwc_table_node, Nnodes, FIRST_SOLN, FS_ACTIVE,
                  NOFLUX, EXP_TRANS, veg_class);
      #else
      Error = solve_T_profile(Tnew_node, T_node, Tnew_fbflag, Tnew_fbcount, Zsum_node, kappa_node, Cs_node,
                  moist_node, delta_t, max_moist_node, bubble_node,
                  expt_node, ice_node, alpha, beta, gamma, dp, soil_con->depth,
      #if EXCESS_ICE
                  porosity_node, effective_porosity_node,
      #endif
                  Nnodes, FIRST_SOLN, soil_con->FS_ACTIVE, NOFLUX, EXP_TRANS, veg_class);
      #endif //QUICK_FS
    }
    if ( (int)Error == ERROR ) {
      fprintf(stderr, "ERROR: func_surf_energy_bal calling solve_T_profile\n");
      return( ERROR );
    }
    //* Compute temperatures for calculations of ground heat flux, delta_H, and fusion *
    if (!options.EXP_TRANS) {
      *T1 = Tnew_node[1];
      T11 = Tnew_node[2];
    }
    else {
      i = 0;
      while (soil_con->Zsum_node[i] < D1) i++;
      D1_minus = soil_con->Zsum_node[i-1];
      D1_plus = soil_con->Zsum_node[i];
      T1_minus = Tnew_node[i-1];
      T1_plus = Tnew_node[i];
      *T1 = T1_minus + (T1_plus - T1_minus)*(D1-D1_minus)/(D1_plus-D1_minus);
    }
    //*****************************************************
    //  Compute the Ground Heat Flux between layers 0 and 1
    //*****************************************************
    if (!options.EXP_TRANS) {
      if (options.GRND_FLUX_TYPE == GF_406) {
        *grnd_flux = (snow_coverage + (1. - snow_coverage) * surf_atten) * (kappa1 / D1 * ((*T1) - TMean));
      } else {
        *grnd_flux = (snow_coverage + (1. - snow_coverage) * surf_atten) * (kappa1 / D1 * ((*T1) - TMean)
               + (kappa2 / D2 * (T11 - (*T1)))) / 2.;
      }
    } else {
        *grnd_flux = (snow_coverage + (1. - snow_coverage) * surf_atten) * (kappa1 / (D1-D1_minus) * ((*T1) - T1_minus)
               + (kappa2 / (D1_plus-D1) * (T1_plus - (*T1)))) / 2.;
    }
  }
  //******************************************************
  //  Compute the change in heat storage in the region between layers 0 and 1
  //******************************************************
  if (!options.EXP_TRANS) {
    *deltaH = (Cs1 * ((Ts_old + T1_old) - (TMean + *T1)) * D1 / delta_t / 2.);
  } else {
    *deltaH = 0;
    i = 0;
    while (soil_con->Zsum_node[i+1] < D1) {
      *deltaH += (Cs1 * ((Told_node[i] + Told_node[i+1]) - (Tnew_node[i] + Tnew_node[i+1])) * (soil_con->Zsum_node[i+1]-soil_con->Zsum_node[i]) / delta_t / 2.);
      i++;
    }
    *deltaH += (Cs1 * ((Told_node[i] + T1_old) - (Tnew_node[i] + *T1)) * (D1-soil_con->Zsum_node[i]) / delta_t / 2.);
  }

  //******************************************************
  //  Compute the change in heat due to solid-liquid phase changes in the region between layers 0 and 1
  //******************************************************
  if (soil_con->FS_ACTIVE && options.FROZEN_SOIL) {
    if (!options.EXP_TRANS) {
      double temperature = (TMean + *T1) / 2.;
      double ice = calc_ice_content(moist,temperature,max_moist,bubble,expt);
      *fusion = (-ice_density * Lf * (ice0 - ice) * D1 / delta_t);
    } else {
      *fusion = 0;
      i = 0;
      while (soil_con->Zsum_node[i+1] < D1) {
        double old_temperature = (Told_node[i] + Told_node[i+1]) / 2.;
        double ice0 = calc_ice_content(moist,old_temperature,max_moist,bubble,expt);
        double new_temperature = (Tnew_node[i] + Tnew_node[i+1]) / 2.;
        double ice = calc_ice_content(moist,new_temperature,max_moist,bubble,expt);
        *fusion += (-ice_density * Lf * (ice0 - ice) * (soil_con->Zsum_node[i+1] - soil_con->Zsum_node[i]) / moist);
        i++;
      }
      double old_temperature = (Told_node[i] + T1_old) / 2.;
      double ice0 = calc_ice_content(moist,old_temperature,max_moist,bubble,expt);
      double new_temperature = (Tnew_node[i] + *T1) / 2.;
      double ice = calc_ice_content(moist,new_temperature,max_moist,bubble,expt);
      *fusion += (-ice_density * Lf * (ice0 - ice) * (D1 - soil_con->Zsum_node[i]) / delta_t);
    }
  }
  //* if thin snowpack, compute the change in energy stored in the pack *
  if ( INCLUDE_SNOW ) {
    if ( TMean > 0 )
      *deltaCC = CH_ICE * (snow_swq - snow_water) * (0 - OldTSurf) / delta_t;
    else
      *deltaCC = CH_ICE * (snow_swq - snow_water) * (TMean - OldTSurf) / delta_t;
    *refreeze_energy = (snow_water * Lf * snow_density) / delta_t;
    *deltaCC *= snow_coverage; // adjust for snow cover fraction
    *refreeze_energy *= snow_coverage; // adjust for snow cover fraction
  }

  //** Compute net surface radiation of snow-free area for evaporation estimates **
  LongBareOut = STEFAN_B * Tmp * Tmp * Tmp * Tmp;                                //160525 emissivity is set as one???
  if ( INCLUDE_SNOW ) { // compute net LW at snow surface
    *NetLongSnow = (LongSnowIn - snow_coverage * LongBareOut);
  }
  (*NetLongBare)   = (LongBareIn - (1. - snow_coverage) * LongBareOut); // net LW snow-free area
  NetBareRad = (NetShortBare + (*NetLongBare) + *grnd_flux + *deltaH + *fusion);



  //** Compute atmospheric stability correction **
  //** CHECK THAT THIS WORKS FOR ALL SUMMER SITUATIONS **
  if ( wind[UnderStory] > 0.0 && overstory && SNOWING )
    Ra_used[0] = ra[UnderStory]
                 / StabilityCorrection(ref_height[UnderStory], 0.f, TMean, Tair,
                                       wind[UnderStory], roughness[UnderStory]);
  else if ( wind[UnderStory] > 0.0 )
    Ra_used[0] = ra[UnderStory]
                 / StabilityCorrection(ref_height[UnderStory], displacement[UnderStory],
                                       TMean, Tair, wind[UnderStory], roughness[UnderStory]);
  else
    Ra_used[0] = param.HUGE_RESIST;
  //*************************************************
  // Compute Evapotranspiration if not snow covered
  //
  // Should evapotranspiration be active when the
  // ground is only partially covered with snow????
  //
  // Use Arno Evap if LAI is set to zero (e.g. no
  // winter crop planted).
  //*************************************************
  // /*************** calling CropSyst 2012-Nov-keyvan**********
  // /*****************************
  // compute evap for just
  // short grass PET
  // 30Nov2012               Keyvan
  // *****************************
  #ifdef VIC_CROPSYST_VERSION
  int iscrop_return = iscrop
          #if (VIC_CROPSYST_VERSION>=3)
          (veg_con[iveg].VCS.veg_class_code);
          #else
          (veg_class_code);
          #endif
  #endif
  double check_LAI =
    #if VIC_CROPSYST_VERSION==2
    iscrop_return ? 1 :                                                            //LML 150414 seems wrong
    #endif
    #ifdef VIC_CROPSYST_VERSION
    veg_con[iveg].VCS.veg_lib[band]->LAI[month-1];                                   //151001LML
    #else
    veg_lib[veg_class].LAI[month-1];
    #endif
  int check_VEG=
  #if VIC_CROPSYST_VERSION==2
  iscrop_return ? 1 :                                                           //LML 150414 for V3, it's already set
  #endif
     VEG;
  int check_SNOWING=
  #if VIC_CROPSYST_VERSION==2
  iscrop_return ? 0 :
  #endif
     SNOWING;

  if ( check_VEG && !check_SNOWING && check_LAI > 0 ) {
    //printf("Canopy_Evap::VEG(%d)\n",veg_con[iveg].veg_class_code);
    const double * const *aero_resist_temp = aero_resist_ET;
    Evap = canopy_evap(*soil_con, TRUE,
                       veg_class, month, mu, delta_t, NetBareRad, vpd,
                       NetShortBare, Tair, Ra_used[1],
                       soil_con->elevation, rainfall
                       #if SPATIAL_FROST
                       ,frost_fract,
                       #endif // SPATIAL_FROST
                       ,root
                       #ifdef VIC_CROPSYST_VERSION
                       #if (VIC_CROPSYST_VERSION>=3)
                       ,iveg, band, veg_con,gross_shortwave,net_longwave, hidx
                       #endif
                       ,rec,atmos,dmy,veg_class_code,aero_resist_temp,cell_wet,cell_dry
                       #endif
                       ,Wdew, layer_wet, layer_dry, veg_var_wet, veg_var_dry
                       );
  } else if(!SNOWING) {
    //printf("!SNOWING::VEG(%d)\n",veg_con[iveg].veg_class_code);
    Evap = arno_evap(*soil_con, NetBareRad, Tair, vpd, Ra_used[0], delta_t, mu
                    #if SPATIAL_FROST
                    ,frost_fract
                    #endif // SPATIAL_FROST
                    , layer_wet, layer_dry
                    );
  } else {
    Evap = 0.;
  }
  //#endif
  //**********************************************************************
  //  Compute the Latent Heat Flux from the Surface and Covering Vegetation
  //**********************************************************************
  *latent_heat  = -RHO_W * Le * Evap;
  *latent_heat_sub = 0.;
  //** Compute the latent heat flux from a thin snowpack if present **
  if (INCLUDE_SNOW) {
    //* Convert sublimation terms from m/timestep to kg/m2s *
    VaporMassFlux = *vapor_flux * ice_density / delta_t;
    BlowingMassFlux = *blowing_flux * ice_density / delta_t;
    SurfaceMassFlux = *surface_flux * ice_density / delta_t;
    latent_heat_from_snow(atmos_density, vp, Le, atmos_pressure,
                          Ra_used[0], TMean, vpd, &temp_latent_heat,
                          &temp_latent_heat_sub, &VaporMassFlux,
                          &BlowingMassFlux, &SurfaceMassFlux);
    *latent_heat += temp_latent_heat * snow_coverage;
    *latent_heat_sub = temp_latent_heat_sub * snow_coverage;
    //* Convert sublimation terms from kg/m2s to m/timestep *
    *vapor_flux = VaporMassFlux * delta_t / ice_density;
    *blowing_flux = BlowingMassFlux * delta_t / ice_density;
    *surface_flux = SurfaceMassFlux * delta_t / ice_density;
  } else {
    *latent_heat *= (1. - snow_coverage);
  }
  //************************************************
  //  Compute the Sensible Heat Flux from the Surface
  //************************************************
  if ( snow_coverage < 1 || INCLUDE_SNOW ) {
    *sensible_heat = atmos_density * Cp * (Tair - (TMean)) / Ra_used[0];
    if ( !INCLUDE_SNOW ) (*sensible_heat) *= (1. - snow_coverage);
  } else {
    *sensible_heat = 0.;
  }
  //*************************************
  //  Compute Surface Energy Balance Error
  //*************************************
  error = (NetBareRad // net radiation on snow-free area
           + NetShortGrnd + NetShortSnow // net surface SW
           + emissivity * (*NetLongSnow)) // net surface LW
           + *sensible_heat // surface sensible heat
           + (*latent_heat + *latent_heat_sub) // surface latent heats
           //* heat flux through snowpack - for snow covered fraction *
           + *snow_flux * snow_coverage
           //* energy used in reducing snow coverage area *
           + melt_energy
           //* snow energy terms - values are 0 unless INCLUDE_SNOW *
           + Advection - *deltaCC;
  if ( INCLUDE_SNOW ) {
    if (Tsnow_surf == 0.0 && error > -(*refreeze_energy)) {
      *refreeze_energy = -error;
      error = 0.0;
    } else {
      error += *refreeze_energy;
    }
  }
  *store_error = error;
  return error;
}
//_func_surf_energy_bal________________________________________________________
double calc_ice_content(const double moist, const double temperature, const double max_moist,
                        const double bubble, const double expt)
{
  double ice = 0;
  if(temperature < 0.) {
    ice = moist - maximum_unfrozen_water(temperature, max_moist, bubble, expt);
    if (ice < 0) ice = 0;
  }
  return ice;
}
//_calc_ice_content____________________________________________________________
