#ifndef linear_regression_TH
#define linear_regression_TH
#include "corn/dynamic_array/dynamic_array_T.h"
#include <math.h>
namespace CORN {
//______________________________________________________________________________
template <typename T>
class Linear_regression
{
private:
   Dynamic_array<T> &x_set;   // independent (explanatory) variable
   Dynamic_array<T> &y_set;   // dependent variable
protected: // interum variable common to all methods
   T sum_xy ;
   T sum_x2 ;
   T sum_x ;  //observation
   T sum_y ;   //predicted
protected: // Specific to methodB
   T sum_y2;
public:
   inline Linear_regression
      (Dynamic_array<T> &_x_set
      ,Dynamic_array<T> &_y_set)
      : x_set(_x_set)
      , y_set(_y_set)
      {};
public: // Regression methods
   bool get_methodA(T &slope, T &intercept)
   {   slope = 0.0;
       intercept = 0.0;
      nat16 count = tally();
      if (count > 0)
      { //Need to avoid div 0.
         slope     = (count * sum_xy - sum_y * sum_x)
                   / (count * sum_x2 - CORN_sqr(sum_x));  //000701
         intercept = (sum_y - slope * sum_x) /  count ;                                      //000701
      }
      return count > 0;
   }
   //_1998-03-05__________________________________________________get_methodA__/
   bool get_methodB(T &slope, T &intercept,T &r2,bool force_intercept_at_0)
   // The methods return false if the data set count = 0 or division by zero condition
   // One of these methods is what is used by Excel (I need to figure out which).
   // Probably want to use the same method as Excel.
   {   slope = 0.0;
       intercept = 0.0;
       r2 = 0.0;
       nat16 n = tally();
      bool result = true;
        if (n)  // Note, must ensure that the dataset have more than 1 item to  avoid div 0.
        {   T denom       = (sum_x2 - (sum_x * sum_x) / n);
         if (!CORN::is_approximately<T>(denom,0.0,0.0000001))
         {  if (force_intercept_at_0)
            {  intercept = 0;
             if (!CORN::is_approximately<T>(sum_x2,0.0,0.0000001))
                {  slope = sum_xy / sum_x2;
                   // WARNING still need to compute r2 in this case,
                } else
                {  slope = 0;
                   result = false;
                }
            } else
            {  slope       = (sum_xy - (sum_x * sum_y) / n) / denom;
                 intercept    = (sum_y - slope * sum_x) / n;
                 T numerator = sum_xy - (1.0 / n) * sum_x * sum_y ;
                 T x_term    = (1.0 / n) * CORN_sqr(sum_x);
                 T y_term    = (1.0 / n) * CORN_sqr(sum_y);
                 T denominator = sqrt(sum_x2 - x_term) * sqrt(sum_y2 - y_term);
               if (!CORN::is_approximately<T>(denominator,0.0,0.0000001))
                 {  T r   = numerator / denominator;
                    r2    = CORN_sqr(r);
                 }  else result = false;
            }
         } else result =  false;
      } else result = false;
      return result;
   }
   //_2000-11-03__________________________________________________get_methodB__/
 private:
   nat16 tally()
   // Returns the number of points in data set
   {  sum_xy = 0;
      sum_x2 = 0;
      sum_y2 = 0;
      sum_x = 0;  //observation
      sum_y = 0;
      nat16 count =  x_set.get_count();
      for (int i = 0 ; i <count; i++)
      {   T x_i = x_set.get(i,false);    // x[i]
         T y_i = y_set.get(i,false);    // y(i)
         sum_xy += (y_i * x_i);
         sum_x2 += CORN_sqr(x_i);
         sum_y2 += CORN_sqr(y_i); // method B
         sum_y += y_i;
         sum_x += x_i;
      }
      return count;
   }
   //_1998-03-05________________________________________________________tally__/
};
//______________________________________________________________________________
}//_namespace_CORN_____________________________________________________________/
#endif

