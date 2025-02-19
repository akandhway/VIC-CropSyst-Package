#  include "corn/math/statistical/distribution_normal.h"
#  include "math.h"
#  include "corn/math/moremath.h"
#  include "float.h"
#  include "corn/math/compare.h"
#  include "stdlib.h"
// for rand() function

/*
Algorithm for computing the inverse normal cumulative distribution function

Various source codes for different languages.
// http://home.online.no/~pjacklam/notes/invnorm/#C
*/
/*
This C++ class by Roger Nelson

This class was adapted from code by
Jeremy Lea (Research Engineer, Infrastructure Engineering, Transportek, CSIR, South Africa)
Downloaded from
http://home.online.no/~pjacklam/notes/invnorm/impl/lea/
*/
/*
 * A normally distributed random number generator.  We avoid
 * the uniform rv's being 0.0 since this will result in infinte
 * values, and double count the 0 == 2pi.
 */

namespace CORN {
//______________________________________________________________________________
double Distribution_normal::random_normal() const
{  static int i = 1;
   static double u[2] = {0.0, 0.0};
   double r[2];
   if (i == 1)
   {  r[0] = sqrt(-2.0*log(((double)rand()+1.0)/((double)RAND_MAX+1.0)));
      r[1] = 2.0*M_PI*((double)rand()+1.0)/((double)RAND_MAX+1.0);
      u[0] = r[0]*sin(r[1]);
      u[1] = r[0]*cos(r[1]);
      i = 0;
 } else  i = 1;
 return u[i];
}
//_random_normal_______________________________________________________________/
/*
 * The standard normal PDF, for one random variable.
 */

double Distribution_normal::standard_PDF(double u) const
{ // was stdnormal_pdf
 return exp(-u*u/2)/M_SQRT2PI;
}
//______________________________________________________________________________
/*
 * An implementation of adaptive, recursive Newton-Cotes integration.
 * Based on the MATLAB implementation, but covered in a lot of books...
 *
 * This only does integration over the standard normal PDF.  It's just
 * here to check the error function approximations.
 */
#define LEVMAX 10
//______________________________________________________________________________
double Distribution_normal::quad8_standard_PDF(double a, double b, double Q) const
{ // RLN: was quad8_stdnormal_pdf
 // The magic Newton-Cotes weights
   const int w[9] = {3956, 23552, -3712, 41984, -18160, 41984, -3712, 23552,3956};
   const int dw = 14175;
   static int level = -1;
   static double tol = 1e-30;
   register double h, Q1 = 0.0, Q2 = 0.0;
   register int i;
   level++;
   h = (b-a)/16.0;
   for (i = 0; i < 9; i++)
   {  Q1 += h*w[i]*standard_PDF(a+i*h)/dw;
      Q2 += h*w[i]*standard_PDF(a+(i+8)*h)/dw;
   }
   // This is the adaptive recursive bit.  We only recurse if we can improve...
   if (fabs(Q1+Q2-Q) > tol*fabs(Q1+Q2) && level <= LEVMAX)
   {  tol /= 2.0;
      Q1 = quad8_standard_PDF(a,(a+b)/2.0,Q1);
      Q2 = quad8_standard_PDF((a+b)/2.0,b,Q2);
      tol *= 2.0;
   }
   level--;
   return Q1 + Q2;
}
//__quad8_standard_PDF_________________________________________________________/
/*
 * The standard normal CDF, for one random variable.
 *
 *   Author:  W. J. Cody
 *   URL:   http://www.netlib.org/specfun/erf
 *
 * This is the erfc() routine only, adapted by the
 * transform stdnormal_cdf(u)=(erfc(-u/sqrt(2))/2;
 */
double Distribution_normal::standard_CDF(double u) const
{ //RLN: was  stdnormal_cdf
 const double a[5] = {
  1.161110663653770e-002,3.951404679838207e-001,2.846603853776254e+001,
  1.887426188426510e+002,3.209377589138469e+003
 };
 const double b[5] = {
  1.767766952966369e-001,8.344316438579620e+000,1.725514762600375e+002,
  1.813893686502485e+003,8.044716608901563e+003
 };
 const double c[9] = {
  2.15311535474403846e-8,5.64188496988670089e-1,8.88314979438837594e00,
  6.61191906371416295e01,2.98635138197400131e02,8.81952221241769090e02,
  1.71204761263407058e03,2.05107837782607147e03,1.23033935479799725E03
 };
 const double d[9] = {
  1.00000000000000000e00,1.57449261107098347e01,1.17693950891312499e02,
  5.37181101862009858e02,1.62138957456669019e03,3.29079923573345963e03,
  4.36261909014324716e03,3.43936767414372164e03,1.23033935480374942e03
 };
 const double p[6] = {
  1.63153871373020978e-2,3.05326634961232344e-1,3.60344899949804439e-1,
  1.25781726111229246e-1,1.60837851487422766e-2,6.58749161529837803e-4
 };
 const double q[6] = {
  1.00000000000000000e00,2.56852019228982242e00,1.87295284992346047e00,
  5.27905102951428412e-1,6.05183413124413191e-2,2.33520497626869185e-3
 };
 register double y, z;

#ifdef _MSC_VER_disabled
// RLN: These are only available in MSVC
//050801 Nope MSVS didn't have it either
 if (_isnan(u))
  return _Nan._D;
 if (!_finite(u))
  return (u < 0 ? 0.0 : 1.0);
#endif
 y = fabs(u);
    if (y <= 0.46875*M_SQRT2) {
  /* evaluate erf() for |u| <= sqrt(2)*0.46875 */
  z = y*y;
  y = u*((((a[0]*z+a[1])*z+a[2])*z+a[3])*z+a[4])
       /((((b[0]*z+b[1])*z+b[2])*z+b[3])*z+b[4]);
  return 0.5+y;
 }
 z = exp(-y*y/2)/2;
   if (y <= 4.0)
   {  /* evaluate erfc() for sqrt(2)*0.46875 <= |u| <= sqrt(2)*4.0 */
      y = y/M_SQRT2;
      y = ((((((((c[0]*y+c[1])*y+c[2])*y+c[3])*y+c[4])*y+c[5])*y+c[6])*y+c[7])*y+c[8])
         /((((((((d[0]*y+d[1])*y+d[2])*y+d[3])*y+d[4])*y+d[5])*y+d[6])*y+d[7])*y+d[8]);
      y = z*y;
   } else
   { /* evaluate erfc() for |u| > sqrt(2)*4.0 */
      z = z*M_SQRT2/y;
      y = 2.0 / (y*y);
      y = y*(((((p[0]*y+p[1])*y+p[2])*y+p[3])*y+p[4])*y+p[5])
            /(((((q[0]*y+q[1])*y+q[2])*y+q[3])*y+q[4])*y+q[5]);
      y = z*(M_1_SQRTPI-y);
   }
   return (u < 0.0 ? y : 1-y);
}
//_standard_CDF________________________________________________________________/
/*
 * The inverse standard normal distribution.
 *
 *   Author:      Peter J. Acklam <pjacklam@online.no>
 *   URL:         http://home.online.no/~pjacklam
 *
 * This function is based on the MATLAB code from the address above,
 * translated to C, and adapted for our purposes.
 */

// RLN I am pretty sure this is Cumulative distribution function

double Distribution_normal::inverse_standard_CDF(double p) const
{  //    was  stdnormal_inv_cdf
 const double a[6] = {
  -3.969683028665376e+01,  2.209460984245205e+02,
  -2.759285104469687e+02,  1.383577518672690e+02,
  -3.066479806614716e+01,  2.506628277459239e+00
 };
 const double b[5] = {
  -5.447609879822406e+01,  1.615858368580409e+02,
  -1.556989798598866e+02,  6.680131188771972e+01,
  -1.328068155288572e+01
 };
 const double c[6] = {
  -7.784894002430293e-03, -3.223964580411365e-01,
  -2.400758277161838e+00, -2.549732539343734e+00,
   4.374664141464968e+00,  2.938163982698783e+00
 };
 const double d[4] = {
   7.784695709041462e-03,  3.224671290700398e-01,
   2.445134137142996e+00,  3.754408661907416e+00
 };

 register double q, t, u;

#ifdef _MSC_VER_disabled
// RLN: These are only available in MSVC
//050801 Nope MSVS didn't have it either
 if (_isnan(p) || p > 1.0 || p < 0.0)
  return _Nan._D;
 if (p == 0.0)
  return -_Inf._D;
 if (p == 1.0)
  return  _Inf._D;
#else
 if (/*NYI_isnan(p) ||*/ p > 1.0 || p < 0.0)
  return 0.0; // RLN could return NAN?    _Nan._D;
 if (p == 0.0)
  return FLT_MIN; // _Inf._D;
 if (p == 1.0)
  return  FLT_MAX; // _Inf._D;

#endif
//RLN q = MIN(p,1-p);
 q = CORN_min(p,1-p); // RLN
 if (q > 0.02425) {
  /* Rational approximation for central region. */
  u = q-0.5;
  t = u*u;
  u = u*(((((a[0]*t+a[1])*t+a[2])*t+a[3])*t+a[4])*t+a[5])
    /(((((b[0]*t+b[1])*t+b[2])*t+b[3])*t+b[4])*t+1);
 } else {
  /* Rational approximation for tail region. */
  t = sqrt(-2*log(q));
  u = (((((c[0]*t+c[1])*t+c[2])*t+c[3])*t+c[4])*t+c[5])
   /((((d[0]*t+d[1])*t+d[2])*t+d[3])*t+1);
 }
   /* The relative error of the approximation has absolute value less
    than 1.15e-9.  One iteration of Halley's rational method (third
    order) gives full machine precision... */
   t = standard_CDF(u)-q;    /* error */
   t = t*M_SQRT2PI*exp(u*u/2);   /* f(u)/df(u) */
   u = u-t/(1+u*t/2);     /* Halley's method */
   return (p > 0.5 ? -u : u);
}
//_inverse_standard_CDF________________________________________________________/
}//_namespace_CORN_____________________________________________________________/
