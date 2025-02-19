#ifndef statistics_descriptiveH
#define statistics_descriptiveH
#include <limits>
#ifndef constH
#  include "corn/const.h"
#endif
#include "corn/primitive.h"
#include "corn/math/statistical/statistics_types.hpp"
/*
Four major types of descriptive statistics:
Measures of Frequency: * Count, Percent, Frequency. ...
Measures of Central Tendency. * Mean, Median, and Mode. ...
Measures of Dispersion or Variation. * Range, Variance, Standard Deviation. ...
Measures of Position. * Percentile Ranks, Quartile Ranks.
*/
namespace CORN {
namespace statistical {
//______________________________________________________________________________
interface_ Descriptive // 200701 was Statistics_descriptive_interface
{
   virtual nat32   get_count()                                          const=0;
   virtual float64 get_current()                                        const=0;
   virtual float64 get_min()                                            const=0;
   virtual float64 get_max()                                            const=0;
   virtual float64 get_sum()                                            const=0;
   virtual float64 get_mean()                                           const=0;
   virtual float64 get_sum_of_squares()                                 const=0;
   virtual float64 get_standard_deviation()                             const=0;
   virtual float64 get_coefficient_of_variation()                       const=0;
   virtual float64 get_median()                                            const { return std::numeric_limits<double>::quiet_NaN(); }
   virtual float64 get_mode()                                              const { return std::numeric_limits<double>::quiet_NaN(); }
   virtual float64 get_initial()                                           const { return std::numeric_limits<double>::quiet_NaN(); } //130918
   virtual float64 get_final()                                             const { return std::numeric_limits<double>::quiet_NaN(); } //170319
      // median and mode initial and final are only available with
      // statistical datasets so the defaults are to return NaN
   virtual float64  get_statistic (nat8 stat_index)                        const //170319
      {UNUSED_arg(stat_index);return std::numeric_limits<double>::quiet_NaN();}
   inline virtual ~Descriptive() {}                                              //170216
};
//_Descriptive______________________________________________________2013-08-01_/
template <typename Real>
class Descriptive_data      //200701 was  Statistics_descriptive_data
: public implements_ Descriptive                                                 //200701_181224
{
 public:
   Real element[STAT_COUNT];                                                     //181214
      // currently the element enumeration and bitmask accomodates 16 statistical element
      // but only statistic_count are implemented
   Real &count_as_real;
   Real &summation;
   Real &minimum;
   Real &maximum;
   Real &arithmetic_mean;
   Real &standard_deviation; // actually sample_standard_deviation
   Real &coefficient_of_variation;
   Real &sum_of_squares;

   // count, median, and mode are specifically implemented
   // by derived classes so they are not included here.
 public:
   Descriptive_data()
      : count_as_real                              (element[   count_statistic])
      , summation                                  (element[     sum_statistic])
      , minimum                                    (element[     min_statistic])
      , maximum                                    (element[     max_statistic])
      , arithmetic_mean                            (element[    mean_statistic])
      , standard_deviation                         (element[ std_dev_statistic])
      , coefficient_of_variation                   (element[coef_var_statistic])
      , sum_of_squares                             (element[sum_sqrs_statistic])
      // median and mode are only applicable to datasets
      {  reset();
      }
 public:
   virtual bool reset()
      // NYI specify mask of elements to reset (default to everything)
      {
         for (nat8 e = 0; e < STAT_COUNT; e++)                                   //181224
            element[e] = std::numeric_limits<double>::quiet_NaN();
         // initially min is postive and max is negetative
         // so that one simply cumulatively apply max and min functions.
         // as sample values are appended.
         minimum =  std::numeric_limits<double>::quiet_NaN();
         maximum = -std::numeric_limits<double>::quiet_NaN();
         // Ideally I would like to just use NAN,
         // but it appears that float comparison omits comparing when NAN
         // So I need actual values
         count_as_real              = 0;
         summation                  = 0;
         minimum                    =  999999999.9;
         maximum                    = -999999999.9;
         arithmetic_mean            = 0;
         standard_deviation         = 0;
         coefficient_of_variation   = 0;                                         //120311
         sum_of_squares             = 0; // moved from Statistical_data_set      //020701
         return true;
      }
   virtual bool clear(float32 initial_value)                                     //200529
      {  reset();
         arithmetic_mean = initial_value;
         return true;
      }
   const Real &ref_summation()                                             const { return summation; }
   const Real &ref_minimum()                                               const { return minimum; }
   const Real &ref_maximum()                                               const { return maximum; }
   const Real &ref_arithmetic_mean()                                       const { return arithmetic_mean; }
   const Real &ref_standard_deviation()                                    const { return standard_deviation; }
   const Real &ref_coefficient_of_variation() const { return coefficient_of_variation; }
   const Real &ref_sum_of_squares()                                        const { return sum_of_squares; }
   inline virtual float64 get_statistic(nat8 statistic_index)              const { return (float64)element[statistic_index]; } //181224
   inline virtual Real &mod_statistic (nat8 statistic_index)                     { return          element[statistic_index]; } //181224
   inline virtual const Real &ref_statistic (nat8 statistic_index)         const { return          element[statistic_index]; } //181224
};
//_Descriptive_data_________________________________________________2013-08-01_/
template <typename Real>
class Descriptive_abstract  // 200701 was Statistics_descriptive_abstract
: public extends_ Descriptive_data<Real>
{
 public:
    Descriptive_abstract()                                                       {statistical::Descriptive_data<Real>::reset();}
 public:
//   inline virtual float64 get_current()                                           const { return (float64)xxxx; }
   virtual float64 get_min()                                               const { return (float64)Descriptive_data<Real>::minimum; }
   virtual float64 get_max()                                               const { return (float64)Descriptive_data<Real>::maximum; }
   virtual float64 get_sum()                                               const { return (float64)Descriptive_data<Real>::summation; }
   virtual float64 get_arithmetic_mean()                                   const { return (float64)Descriptive_data<Real>::arithmetic_mean; }
   virtual float64 get_mean() /*alias */                                   const { return (float64)Descriptive_data<Real>::arithmetic_mean; }
   virtual float64 get_sum_of_squares()                                    const { return (float64)Descriptive_data<Real>::sum_of_squares; }
   virtual float64 get_standard_deviation()                                const { return (float64)Descriptive_data<Real>::standard_deviation; }
   virtual float64 get_coefficient_of_variation()                          const { return (float64)Descriptive_data<Real>::coefficient_of_variation; }
//   virtual float64 get_median()                                            const { return (float64)0; }
};
//_Descriptive_abstract_abstract____________________________________2013-08-01_/
}//_namespace_statstical____________________________________________2020-07-01_/
}//_namespace_CORN_____________________________________________________________/
#endif

