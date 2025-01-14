#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>

#ifndef _LEAPYR
#define LEAPYR(y) (!((y)%400) || (!((y)%4) && ((y)%100)))
#endif
//LML 150309 #include <sys/resource.h>

static char vcid[] = "$Id: make_dmy.c,v 4.4.2.1 2007/05/04 21:30:46 vicadmin Exp $";

dmy_struct *make_dmy()
/**********************************************************************
    make_dmy    Dag Lohmann        January 1996

  This subroutine creates an array of structures that contain
  information about the day, month and year of each time step.

  modifications:
  7-25-96  Added hour count, so that model can run on less than
           a daily time step.                    KAC
  5-17-99  Modified routine to use LEAPYR function, make use of
           simulation ending dates, and to skip over initial
      forcing data records so that the model can be run on
      subsets of more complete data records.            KAC
  8-19-99  Modified routine to estimate the number of records
           that should be skipped before starting to write
       model output, based on the number of years defined
       with the SKIPYEAR global variable.               KAC
  3-14-00  Fixed problem with accounting for number of days in
           February.  If last simulation year was a leap year,
           number of days in February was not reset to 28 after
       working out number of records and before working out
       the number of forcing file records to skip.      KAC
  2006-02-07 Changed indexing of line 63 (if(endday...) by 1 GCT
**********************************************************************/
{
  extern param_set_struct param_set;

  dmy_struct *temp; temp=0;
  int    hr, year, day, month, jday, ii, daymax;
  int    days[12]={31,28,31,30,31,30,31,31,30,31,30,31};
  int    endmonth, endday, endyear, skiprec, i, offset;
  int    tmpmonth, tmpday, tmpyear, tmphr, tmpjday, step;
  char   DONE;
  char   ErrStr[MAXSTRING];

  hr    = global_param.starthour;
  year  = global_param.startyear;
  day   = global_param.startday;
  month = global_param.startmonth;

  /** Check if user defined end date instead of number of records **/
  if(global_param.nrecs < 0) {
    if((global_param.endyear < 0) || (global_param.endmonth < 0)
       || (global_param.endday < 0)) {
      nrerror("The model global file MUST define EITHER the number of records to simulate (NRECS), or the year (ENDYEAR), month (ENDMONTH), and day (ENDDAY) of the last full simulation day");
    }
    endday   = global_param.endday;
    endmonth = global_param.endmonth;
    endyear  = global_param.endyear;
    if(LEAPYR(endyear)) days[1] = 29;
    else days[1] = 28;
    if(endday < days[global_param.endmonth-1]) endday++;
    else {
      endday = 1;
      endmonth++;
      if(endmonth > 12) {
        endmonth = 1;
        endyear++;
      }
    }

    DONE = FALSE;
    ii   = 0;

    tmpyear  = year;
    tmpmonth = month;
    tmpday   = day;
    tmphr    = hr;
    while(!DONE) {
      get_next_time_step(&tmpyear,&tmpmonth,&tmpday,&tmphr,
             &tmpjday,global_param.dt);
      ii++;
      if(tmpyear == endyear)
        if(tmpmonth == endmonth)
          if(tmpday == endday)
            DONE = TRUE;
    }
    global_param.nrecs = ii;

  }
  else {
    offset = 0;
    tmphr  = hr;
    while (tmphr != 0) {
      tmphr += global_param.dt;
      offset++;
      if(tmphr >= 24) tmphr = 0;
    }
    if( ((global_param.dt * (global_param.nrecs - offset)) % 24) != 0 ) {
      sprintf(ErrStr,"Nrecs must be defined such that the model ends after completing a full day.  Currently Nrecs is set to %i, while %i and %i are allowable values.", global_param.nrecs, ((global_param.dt * (global_param.nrecs - offset)) / 24) * 24, ((global_param.dt * (global_param.nrecs - offset)) / 24) * 24 + 24);
      nrerror(ErrStr);
    }
  }

  // allocate dmy struct
  temp = (dmy_struct*) calloc(global_param.nrecs, sizeof(dmy_struct));

  /** Create Date Structure for each Modeled Time Step **/
  jday = day;
  if( LEAPYR(year) ) days[1] = 29;
  else days[1] = 28;
  for ( ii = 0; ii < month-1; ii++ )
    jday += days[ii];

  DONE = FALSE;
  ii   = 0;

  while(!DONE) {
    temp[ii].hour = hr;
    temp[ii].day   = day;
    temp[ii].month = month;
    temp[ii].year  = year;
    temp[ii].day_in_year = jday;

    get_next_time_step(&year,&month,&day,&hr,&jday,global_param.dt);

    ii++;
    if(ii == global_param.nrecs) DONE=TRUE;

  }

  /** Determine number of forcing records to skip before model start time **/
  for ( i = 0; i < 2; i++ ) {
    if(param_set.FORCE_DT[i] != MISSING) {
      if(global_param.forceyear[i] > 0) {
        tmpyear  = global_param.forceyear[i];
        tmpmonth = global_param.forcemonth[i];
        tmpday   = global_param.forceday[i];
        tmphr    = global_param.forcehour[i];
        tmpjday  = tmpday;
        if ( LEAPYR(tmpyear) ) days[1] = 29;
        else days[1] = 28;
        for ( ii = 0; ii < tmpmonth-1; ii++)
          tmpjday += days[ii];

        step     = (int)(1./((float)global_param.dt/24.));
        while(tmpyear < temp[0].year ||
              (tmpyear == temp[0].year && tmpjday < temp[0].day_in_year)) {

          get_next_time_step(&tmpyear,&tmpmonth,&tmpday,&tmphr,
                             &tmpjday,global_param.dt);

          global_param.forceskip[i] ++;

        }
      }
    }
  }

  /** Determine the number of records to skip before starting output files **/
  skiprec = 0;
  for ( i = 0; i < global_param.skipyear; i++ ) {
    if(LEAPYR(temp[skiprec].year)) skiprec += 366 * 24 / global_param.dt;
    else skiprec += 365 * 24 / global_param.dt;
  }
  global_param.skipyear = skiprec;

  return temp;
}

void get_next_time_step(int *year,
            int *month,
            int *day,
            int *hr,
            int *jday,
            int dt) {

  int    days[12]={31,28,31,30,31,30,31,31,30,31,30,31};
  int daymax;

  *hr += dt;
  if(*hr >= 24) {
    *hr    = 0;
    *day  += 1;
    *jday += 1;

    if(LEAPYR(*year)) days[1] = 29;
    else days[1] = 28;

    if(*day > days[*month-1]) {
      *day    = 1;
      *month += 1;
      if(*month == 13){
        *month = 1;
        *jday  = 1;
        *year += 1;
      }
    }
  }
}
/**********************************************************************
    free_dmy    Ted Bohn        April 2007

  This subroutine frees the dmy array.

  modifications:
**********************************************************************/
void free_dmy(dmy_struct **dmy) {
  if (*dmy == NULL)
    return;
  free(*dmy);
}
