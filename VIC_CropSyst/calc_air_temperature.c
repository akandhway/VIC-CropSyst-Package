#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vicNl.h>

static char vcid[] = "$Id: calc_air_temperature.c,v 4.2.2.2 2012/01/30 22:28:27 vicadmin Exp $";

/****************************************************************************
  Subroutines developed by Bart Nijssen to estimate the daily temperature
  cycle from maximum and minimum daily temperature measurements.  

  Modifications:
  June 23, 1998 by Keith Cherkauer to be run within the VIC-NL model.
*****************************************************************************/

/****************************************************************************/
/*				    hermite                                 */
/****************************************************************************/
/* calculate the coefficients for the Hermite polynomials */
void hermite(const int n,
             const double *x,
             const double *yc1,
             const double *yc2,
             double *yc3,
             double *yc4)
{
  for(int i = 0; i < n-1; i++) {
    double dx = x[i+1] - x[i];
    double divdf1 = (yc1[i+1] - yc1[i]) / dx;
    double divdf3 = yc2[i] + yc2[i+1] - 2 * divdf1;
    yc3[i] = (divdf1 - yc2[i] - divdf3) / dx;
    yc4[i] = divdf3 / (dx * dx);
  }
}
/**************************************************************************/
/*				    hermint                               */
/**************************************************************************/
/* use the Hermite polynomials, to find the interpolation function value at 
   xbar */
double hermint(const double xbar, const int n, const double *x, const double *yc1,
               const double *yc2, const double *yc3, const double *yc4)
{
  int klo = 0;
  int khi = n - 1;
  while ((khi - klo) > 1) {
    int k = (khi + klo) >> 1;                                                        //div 2
    if (x[k] > xbar) khi = k;
    else klo = k;
  }
  double dx = xbar - x[klo];
  return yc1[klo] + dx * (yc2[klo] + dx * (yc3[klo] + dx * yc4[klo]));
}
//______________________________________________________________________________
/****************************************************************************/
/*				    HourlyT                                 */
/****************************************************************************/
void HourlyT(const int Dt,
             const int ndays,
             const int *TmaxHour,
             const double *Tmax,
             const int *TminHour,
             const double *Tmin,
             double *Tair)
{
  int nsteps = HOURSPERDAY/Dt * ndays;
  int n     = ndays * 2 + 2;
  double *x     = (double *) calloc(n, sizeof(double));
  double *Tyc1  = (double *) calloc(n, sizeof(double));
  double *yc2   = (double *) calloc(n, sizeof(double));
  double *yc3   = (double *) calloc(n, sizeof(double));
  double *yc4   = (double *) calloc(n, sizeof(double));
  if (x == NULL || Tyc1 == NULL || yc2 == NULL || yc3 == NULL || yc4 == NULL)
    nrerror("Memory allocation failure in HourlyT()");
  
  /* First fill the x vector with the times for Tmin and Tmax, and fill the 
     Tyc1 with the corresponding temperature and humidity values */
  for (int i = 0, j = 1, hour = 0; i < ndays; i++, hour += HOURSPERDAY) {
    if (TminHour[i] < TmaxHour[i]) {
      x[j]       = TminHour[i] + hour;
      Tyc1[j++]  = Tmin[i];
      x[j]       = TmaxHour[i] + hour;
      Tyc1[j++]  = Tmax[i];
    } else {
      x[j]       = TmaxHour[i] + hour;
      Tyc1[j++]  = Tmax[i];
      x[j]       = TminHour[i] + hour;
      Tyc1[j++]  = Tmin[i];
    } 
  }
  /* To "tie" down the first and last values, repeat those */
  x[0]      = x[2] - HOURSPERDAY;
  Tyc1[0]   = Tyc1[2];
  x[n-1]    = x[n-3] + HOURSPERDAY;
  Tyc1[n-1] = Tyc1[n-3];
  /* we want to preserve maxima and minima, so we require that the first 
     derivative at these points is zero */
  for (int i = 0; i < n; i++) yc2[i] = 0.;
  /* calculate the coefficients for the splines for the temperature */
  hermite(n, x, Tyc1, yc2, yc3, yc4);
  /* interpolate the temperatures */
  for (int i = 0, hour = 0; i < nsteps; i++, hour += Dt)
    Tair[i] = hermint(hour, n, x, Tyc1, yc2, yc3, yc4);
  free(x);   
  free(Tyc1);
  free(yc2);
  free(yc3);
  free(yc4);
  return;
}
//______________________________________________________________________________
void set_max_min_hour(const double *hourlyrad,
              const int ndays,
		      int *tmaxhour,
		      int *tminhour)
/****************************************************************************
                             set_max_min_hour

  This function estimates the times of minimum and maximum temperature for
  each day of the simulation, based on the hourly cycle of incoming solar
  radiation.

  Modifications

  1999-Aug-19 Modified to function in polar regions where daylight or
	      darkness may last for 24 hours.				BN
  2006-Oct-26 Shift tminhour and tmaxhour if necessary to remain within
	      the current day.						TJB
  2011-Nov-04 Changed algorithm because previous algorithm had bugs in
	      identifying the times of sunrise and sunset in some cases.
	      The new algorithm relies on the assumption that the
	      hourlyrad array is referenced to local time, i.e. hour 0 is
	      halfway between the previous day's sunset and the current
	      day's sunrise.  Another assumption is that the hourlyrad
	      array begins on hour 0 of the first day.			TJB
  2012-Jan-28 Added logic to prevent overstepping bounds of hourlyrad
	      array.							TJB
****************************************************************************/
{
  for (int i = 0; i < ndays; i++) {
    int risehour = -999;
    int sethour = -999;
    for (int hour = 0; hour < 12; hour++) {
      if (hourlyrad[i*24+hour] > 0 && (i*24+hour == 0 || hourlyrad[i*24+hour-1] <= 0))
	risehour = hour;
    }
    for (int hour = 12; hour < 24; hour++) {
      if (hourlyrad[i*24+hour] <= 0 && hourlyrad[i*24+hour-1] > 0)
	sethour = hour;
    }
    if (i == ndays-1 && sethour == -999) sethour = 23;
    if (risehour >= 0 && sethour >= 0) {
      tmaxhour[i] = 0.67 * (sethour - risehour) + risehour;
      tminhour[i] = risehour - 1;
    }
    else {
      /* arbitrarily set the min and max times to 2am and 2pm */
      tminhour[i] = 2;
      tmaxhour[i] = 14;
    }
  }
}
