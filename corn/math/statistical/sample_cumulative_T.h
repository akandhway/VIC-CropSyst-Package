#ifndef sample_cumulative_TH
#define sample_cumulative_TH
#include "corn/math/statistical/sample_T.h"

namespace CORN {
namespace statistical {
//______________________________________________________________________________
template
   <typename Observation_type                                                    //140624
   ,typename Element_type>
class Sample_cumulative // 200701 was Statistical_sample_cumulative

// Progressive

: public Sample_abstract<Observation_type,Element_type>                          //200701
{private:
   nat32  count;
   Observation_type initial_value;  // The first observation                     //140624_130918
   Observation_type final_value;                                                 //170319
 public:
   Observation_type index; // this is special for CropSyst                       //180408
 public:
   Sample_cumulative<Observation_type,Element_type>()
      : Sample_abstract<Observation_type,Element_type>()
      , count(0)
      , initial_value(std::numeric_limits<Observation_type>::quiet_NaN())        //131017_130918
      , final_value  (std::numeric_limits<Observation_type>::quiet_NaN())        //170319
      , index        (std::numeric_limits<Observation_type>::quiet_NaN())        //180408
      { reset();}
   Sample_cumulative<Observation_type,Element_type>
      (const Observation_type & observation_)
      : Sample_abstract<Observation_type,Element_type>(observation_)             //200701
      , count(0)
      , initial_value(std::numeric_limits<Observation_type>::quiet_NaN())        //130918
      , final_value  (std::numeric_limits<Observation_type>::quiet_NaN())        //170319
      , index        (std::numeric_limits<Observation_type>::quiet_NaN())        //180408
      { reset();}
   //______________________________________________________________________________
   virtual bool reset()
   // may want to return bool
      {  count = 0;
         initial_value = std::numeric_limits<Observation_type>::quiet_NaN();     //130918
         final_value   = std::numeric_limits<Observation_type>::quiet_NaN();     //170319
         index         = 0.0;                                                    //180409
         Sample_abstract<Observation_type,Element_type>::reset();
         return true;
      }
   //______________________________________________________________________________
   virtual Observation_type append(Observation_type value)
      {
         Sample_abstract<Observation_type,Element_type>::append(value);          //200701
         count++;
         if (count == 1) initial_value = value;                                  //130918
         final_value = value;                                                    //170319
         index = 1.0 - value;                                                    //180409
         Sample_abstract<Observation_type,Element_type>::set_valid(STAT_count_bit);
         Sample_abstract<Observation_type,Element_type>::summation += value;
         Sample_abstract<Observation_type,Element_type>::arithmetic_mean = Sample_abstract<Observation_type,Element_type>::summation/(Element_type)count;
         Sample_abstract<Observation_type,Element_type>::minimum = std::min<Element_type>(Sample_abstract<Observation_type,Element_type>::minimum,value);
         Sample_abstract<Observation_type,Element_type>::maximum = std::max<Element_type>(Sample_abstract<Observation_type,Element_type>::maximum,value);
         Sample_abstract<Observation_type,Element_type>::sum_of_squares += CORN_sqr(value); //100710
         Sample_abstract<Observation_type,Element_type>::set_valid(STAT_sum_bit|STAT_sum_sqrs_bit|STAT_arithmetic_mean_bit|STAT_min_bit|STAT_max_bit);
         return value;
      }
   //_append_____________________________________________2013-08-01_1998-03-05_/
 protected:
   virtual float64 calc_standard_deviation()                     const
      // Note that the cumulatively calculated standard deviation will be
      // slightly different from a caculation from all values in the sample.
      // I think this might be considered more like an an estimation.
      {  float64 mean = get_arithmetic_mean();
         float64 term = (count) ? Sample_abstract<Observation_type,Element_type>::sum_of_squares/count - CORN_sqr(mean) : 0.0;   //020701
         float64 standard_deviation = (term > 0.0)
         ? sqrt(term)
         : 0.0;
         return standard_deviation;
      }
   //______________________________________________________________________________
 public:
   virtual float64 get_min()                                               const { return (float64)Sample_abstract<Observation_type,Element_type>::minimum; }
   virtual float64 get_max()                                               const { return (float64)Sample_abstract<Observation_type,Element_type>::maximum; }
   virtual float64 get_sum()                                               const { return (float64)Sample_abstract<Observation_type,Element_type>::summation; }
   virtual float64 get_arithmetic_mean()                                   const { return (float64)Sample_abstract<Observation_type,Element_type>::arithmetic_mean; }
   virtual float64 get_mean() /*alias */                                   const { return (float64)Sample_abstract<Observation_type,Element_type>::arithmetic_mean; }
   virtual float64 get_sum_of_squares()                                    const { return (float64)Sample_abstract<Observation_type,Element_type>::sum_of_squares; }
   virtual float64 get_initial()                                           const { return (float64)initial_value; }
   virtual float64 get_final()                                             const { return (float64)  final_value; }

   virtual float64 provide_initial()  provision_{return (float64)initial_value;}
   virtual float64 provide_final()    provision_{return (float64) final_value;}

   virtual float64 get(nat32 index,bool allow_resize=false)            mutation_{ return 9999;}
      // Get is not applicable to cumulative
   //_get______________________________________________________________________/
   virtual float64/*Real*/ provide_standard_deviation()               provision_
      {  if (!Sample_abstract<Observation_type,Element_type>::get_valid(STAT_std_dev_bit))
         {  Sample_abstract<Observation_type,Element_type>::standard_deviation = calc_standard_deviation();
            Sample_abstract<Observation_type,Element_type>::set_valid(STAT_std_dev_bit);
         }
         return Sample_abstract<Observation_type,Element_type>::standard_deviation;
      }
   //_provide_standard_deviation_______________________________________________/
   virtual float64/*Real*/ provide_coefficient_of_variation()         provision_
      {  if (!Sample_abstract<Observation_type,Element_type>::get_valid(STAT_coef_var_bit))
         {  Sample_abstract<Observation_type,Element_type>::coefficient_of_variation
               = Sample_abstract<Observation_type,Element_type>::calc_coefficient_of_variation();
            Sample_abstract<Observation_type,Element_type>::set_valid(STAT_coef_var_bit);
         }
         return Sample_abstract<Observation_type,Element_type>::coefficient_of_variation;
      }
   //_provide_coefficient_of_variation______________________________2009-10-02_/
   //   inline virtual Real get_median()                   provision_{return 0;} //010406
      // This class is not able to calculate a median

   inline virtual nat32 get_count()                      const { return count; } //010604
};
//_Sample_cumulative________________________________________________1999-03-31_/
}//_namespace_statistical___________________________________________2006-07-01_/
}//_namespace_CORN_____________________________________________________________/
#endif
//stats

