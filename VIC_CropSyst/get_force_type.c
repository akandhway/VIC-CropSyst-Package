#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>
#include <string.h>
#ifdef __BCPLUSPLUS__
#define strcasecmp(a,b) stricmp(a,b)
#endif

static char vcid[] = "$Id: get_force_type.c,v 4.5.2.6 2011/12/23 06:57:55 vicadmin Exp $";

void get_force_type(char   *cmdstr, 
		    int     file_num,
		    int    *field) {
/*************************************************************
  get_force_type.c      Keith Cherkauer     January 20, 2000

  This routine determines the current forcing file data type
  and stores its location in the description of the current 
  forcing file.

  Modifications:
  2005-Mar-24 Modified to accept ALMA forcing variables.	TJB
  2005-May-01 Added the ALMA vars CRainf, CSnowf, LSRainf, and LSSnowf.	TJB
  2005-May-02 Added the ALMA vars Wind_E and Wind_N.			TJB
  2006-Dec-29 Added REL_HUMID to the list of supported met input variables. TJB
  2007-Jan-02 Added ALMA_INPUT option; removed TAIR and PSURF from list
	      of supported met input variables.				TJB
  2007-Jan-05 Bugfix: replaced if(BINARY) with
	      if(param_set.FORCE_FORMAT[file_num]==BINARY).		TJB
  2007-Feb-25 Removed all of the if statements
                if(param_set.FORCE_FORMAT[file_num]==BINARY)
              since this ended up requiring that the FORCE_FORMAT BINARY line
              appear in the global parameter file before the list of forcing
              variables in order to work.  Since the sscanf() performs
              proper parsing regardless of ASCII (which doesn't have SIGNED
              or MULTIPLIER fields) vs. BINARY, I removed the if() statements
              altogether.                                               TJB
  2007-Sep-14 Initialize flgstr to "NULL".				TJB
  2010-Mar-31 Added RUNOFF_IN.						TJB
  2010-Sep-24 Renamed RUNOFF_IN to CHANNEL_IN.				TJB
  2011-Nov-04 Fixed comment describing TSKC.				TJB

*************************************************************/

  extern param_set_struct param_set;

  char optstr[50];
  char flgstr[10];
  char ErrStr[MAXSTRING];
  int  type;

  /** Initialize flgstr **/
  strcpy(flgstr,"NULL");

  if((*field) >= param_set.N_TYPES[file_num]) {
    sprintf(ErrStr,"Too many variables defined for forcing file %i.",file_num);
    nrerror(ErrStr);
  }

  sscanf(cmdstr,"%*s %s",optstr);

  /***************************************
    Get meteorological data forcing info
  ***************************************/
  //171104LML the following "number" might not be right!!!

  /* type 0: air temperature [C] (ALMA_INPUT: [K]) */
  if(strcasecmp("AIR_TEMP",optstr)==0){
    type = AIR_TEMP;
  }

  /* type 1: albedo [fraction] */
  else if(strcasecmp("ALBEDO",optstr)==0){
    type = ALBEDO;
  }

  /* type 2: incoming channel flow [m3] (ALMA_INPUT: [m3/s]) */
  else if(strcasecmp("CHANNEL_IN",optstr)==0){
    type = CHANNEL_IN;
  }

  /* type 3: convective rainfall [mm] (ALMA_INPUT: [mm/s]) */
  else if(strcasecmp("CRAINF",optstr)==0){
    type = CRAINF;
  }

  /* type 4: convective snowfall [mm] (ALMA_INPUT: [mm/s]) */
  else if(strcasecmp("CSNOWF",optstr)==0){
    type = CSNOWF;
  }

  /* type 5: air density [kg/m3] */
  else if(strcasecmp("DENSITY",optstr)==0){
    type = DENSITY;
  }

  /* type 6: incoming longwave radiation [W/m2] */
  else if(strcasecmp("LONGWAVE",optstr)==0 || strcasecmp("LWDOWN",optstr)==0){
    type = LONGWAVE;
  }

  /* type 7: large-scale rainfall [mm] (ALMA_INPUT: [mm/s]) */
  else if(strcasecmp("LSRAINF",optstr)==0){
    type = LSRAINF;
  }

  /* type 8: large-scale snowfall [mm] (ALMA_INPUT: [mm/s]) */
  else if(strcasecmp("LSSNOWF",optstr)==0){
    type = LSSNOWF;
  }

  /* type 9: precipitation [mm] (ALMA_INPUT: [mm/s]) */
  else if(strcasecmp("PREC",optstr)==0){
    type = PREC;
  }

  /* type 10: air pressure [kPa] (ALMA_INPUT: [Pa]) */
  else if(strcasecmp("PRESSURE",optstr)==0){
    type = PRESSURE;
  }

  /* type 11: specific humidity [kg/kg] */
  else if(strcasecmp("QAIR",optstr)==0){
    type = QAIR;
  }

  /* type 12: rainfall [mm] (ALMA_INPUT: [mm/s]) */
  else if(strcasecmp("RAINF",optstr)==0){
    type = RAINF;
  }

  /* type 13: relative humidity [fraction] */
  else if(strcasecmp("REL_HUMID",optstr)==0){
    type = REL_HUMID;
  }

  /* type 14: shortwave radiation [W/m2] */
  else if(strcasecmp("SHORTWAVE",optstr)==0 || strcasecmp("SWDOWN",optstr)==0){
    type = SHORTWAVE;
  }

  /* type 15: snowfall [mm] (ALMA_INPUT: [mm/s]) */
  else if(strcasecmp("SNOWF",optstr)==0){
    type = SNOWF;
  }

  /* type 16: maximum daily temperature [C] (ALMA_INPUT: [K]) */
  else if(strcasecmp("TMAX",optstr)==0){
    type = TMAX;
  }

  /* type 17: minimum daily temperature [C] (ALMA_INPUT: [K]) */
  else if(strcasecmp("TMIN",optstr)==0){
    type = TMIN;
  }

  /* type 18: cloud cover fraction */
  else if(strcasecmp("TSKC",optstr)==0){
    type = TSKC;
  }

  /* type 19: vapor pressure [kPa] (ALMA_INPUT: [Pa]) */
  else if(strcasecmp("VP",optstr)==0){
    type = VP;
  }

  /* type 20: wind speed [m/s] */
  else if(strcasecmp("WIND",optstr)==0){
    type = WIND;
  }

  /* type 21: zonal component of wind speed [m/s] */
  else if(strcasecmp("WIND_E",optstr)==0){
    type = WIND_E;
  }

  /* type 22: meridional component of wind speed [m/s] */
  else if(strcasecmp("WIND_N",optstr)==0){
    type = WIND_N;
  }

#ifdef VIC_CROPSYST_VERSION
  else if(strcasecmp("RHUM_MAX",optstr)==0){
    type = RHUM_MAX;
  }

  else if(strcasecmp("RHUM_MIN",optstr)==0){
    type = RHUM_MIN;
  }

#endif
  /* type 23: unused (blank) data */
  else if(strcasecmp("SKIP",optstr)==0){
    type = SKIP;
  }



  /** Undefined variable type **/
  else {
    sprintf(ErrStr,"Undefined forcing variable type %s in file %i.",
	    optstr, file_num);
    nrerror(ErrStr);
  }

  param_set.TYPE[type].SUPPLIED=file_num+1;
  param_set.FORCE_INDEX[file_num][(*field)] = type;
  if (type == SKIP) {
    param_set.TYPE[type].multiplier = 1;
    param_set.TYPE[type].SIGNED=FALSE;
  }
  else {
    sscanf(cmdstr,"%*s %*s %s %lf",flgstr, &param_set.TYPE[type].multiplier);
    if(strcasecmp("SIGNED",flgstr)==0) param_set.TYPE[type].SIGNED=TRUE;
    else param_set.TYPE[type].SIGNED=FALSE;
  }

  (*field)++;

}
