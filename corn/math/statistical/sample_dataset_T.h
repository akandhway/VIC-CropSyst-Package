#ifndef sample_dataset_TH
#define sample_dataset_TH
#include "corn/math/statistical/sample_T.h"
#include <assert.h>
#ifdef __GNUC__
#define CORN_APPLICABLE statistical::Sample_abstract<Observation_type,Element_type>::applicable
#define CORN_VALID statistical::Sample_abstract<Observation_type,Element_type>::valid
#else
#define CORN_APPLICABLE applicable
#define CORN_VALID valid
#endif
namespace CORN {
namespace statistical {
//______________________________________________________________________________
template
   <typename Observation_type                                                    //140624
   ,typename Element_type
   ,typename Store_type>                                                         //160615
class Sample_dataset_cowl //200701 was Statistical_sample_dataset_cowl
: public extends_ Sample_abstract<Observation_type,Element_type>
{
 public:
   Element_type &median;                                                         //182124
 protected:
   /*const*/ Dynamic_array<Store_type> &dataset_ref;
 public:
   Sample_dataset_cowl<Observation_type,Element_type,Store_type>                 //160616
      (/*const*/ Dynamic_array<Store_type> &_dataset_ref)
      : Sample_abstract<Observation_type,Element_type>()

      , median                                 (this->element[median_statistic]) //181224
      , dataset_ref(_dataset_ref)
      { this->reset(); }                                                         //050705
   //___________________________________________________________________________
   Sample_dataset_cowl<Observation_type,Element_type,Store_type>     //160616
      (const Observation_type & observation_
      ,/*const*/ Dynamic_array<Store_type> &_dataset_ref)
      : Sample_abstract<Observation_type,Element_type>
         (observation_)
      , dataset_ref(_dataset_ref)
      , median                                 (this->element[median_statistic]) //181224
      { this->reset(); }                                                         //050705
   //___________________________________________________________________________
   Dynamic_array<Store_type> &mod_dataset() modification_ { return dataset_ref;} //160616
   virtual float64 get(nat32 index,bool allow_resize=false)            mutation_ { return dataset_ref.get(index,allow_resize); }
   //200323 Store_type get(nat32 index,bool allow_resize=false)                 mutation_ { return dataset_ref.get(index,allow_resize); }
 protected: // statistical base pure overrides
   virtual bool get_changed()                                              const { return dataset_ref.is_modified();; }
 public:
   virtual void clear()                                            modification_ { dataset_ref.clear(); this->reset(); }  //130801_980310
   float64 probability(int at_i)                                       provision_{ return  (float64)(at_i / (get_count() + 1.0) * 100.0); }
      // probability in a 1 based array
   virtual nat32  get_count()                                              const { return (nat32)dataset_ref.get_count(); } //010604
   virtual float64 provide_min()                                      provision_ { return dataset_ref.get_min(); }  //050107
   virtual float64 provide_max()                                      provision_ { return dataset_ref.get_max(); }  //050107
   virtual float64 provide_sum()                                      provision_ { return dataset_ref.sum(); }      //050507
   virtual float64 provide_initial()                                  provision_ { return dataset_ref.get(0,false); } //130919
   virtual float64 get_initial()                                           const { return dataset_ref.get(0,false); } //130919
   virtual float64 provide_final()                                    provision_ //170319
      {  nat32 c = get_count();
         return c
         ? dataset_ref.get(c-1,false)
         : std::numeric_limits<double>::quiet_NaN();
      }
   virtual float64 get_final()                  const { return provide_final();} //170319
   //___________________________________________________________________________
   virtual float64 provide_median()                                   provision_
      {  if (!this->get_valid(STAT_median_bit))                                  //150915
         {  // If we haven't already calculate the median, calculate it now.
            median = calc_median();
            this->set_valid(STAT_median_bit);                                    //150915
         }
         return median;
      }
   //_provide_median________________________________________________2001-04-06_/
   virtual float64 get_median()        const {return (float64)provide_median();}
   //___________________________________________________________________________
   virtual float64 provide_arithmetic_mean()                          provision_
      {  if (dataset_ref.is_modified() || !this->get_valid(STAT_arithmetic_mean_bit))  //150915_150820
         {  Sample_dataset_cowl<Observation_type,Element_type,Store_type>
               *not_const_this = const_cast<Sample_dataset_cowl
                  <Observation_type,Element_type,Store_type>*>(this);            //140917
            not_const_this->recompute(); }
         return Sample_abstract<Observation_type,Element_type>::arithmetic_mean;
      }
   //_provide_arithmetic_mean__________________________________________________/
   virtual float64 provide_coefficient_of_variation()                 provision_
      {  if (dataset_ref.is_modified() || !this->get_valid(STAT_coef_var_bit))   //150915_150820
         {  Sample_dataset_cowl<Observation_type,Element_type,Store_type>
               *not_const_this = const_cast<Sample_dataset_cowl
                  <Observation_type,Element_type,Store_type>*>(this);
            not_const_this->recompute();
         }
         return Sample_abstract<Observation_type,Element_type>::coefficient_of_variation;
      }
   //_provide_coefficient_of_variation_________________________________________/
   virtual float64 provide_standard_deviation()                       provision_
      {  if (dataset_ref.is_modified()  || !this->get_valid(STAT_std_dev_bit))   //150915_150820
         {  Sample_dataset_cowl<Observation_type,Element_type,Store_type>
               *not_const_this = const_cast<Sample_dataset_cowl
               <Observation_type,Element_type,Store_type>*>(this);
            not_const_this->recompute();
         }
         return Sample_abstract<Observation_type,Element_type>::standard_deviation;
      }
   //_provide_standard_deviation_______________________________________________/
   float64 get_probability_value(float64 percentage)                       const
      {  float64 d_prob;
         float64 d_value;
         float64 prob;
         #define       zero_base_correction -1
          Sample_dataset_cowl
            <Observation_type,Element_type,Store_type> sorted(*this);            //160616_061020
         sorted.sort(DESCENDING);                                                //061020
         prob = 0.0;
         for (nat32 i=  1; i < (sorted.get_count()) ; i++)
            // probability loop is 1 based, data set is 0 based
            // Need to check this loop is is i = 1 to n-1  or i to n
         {   float64 prob_i = sorted.probability(i);
             float64 prob_1 = sorted.probability(i+1);
             if (  (prob_i < percentage) &&
                   (prob_1 >= percentage))
             {
                d_prob  = sorted.probability(i+1) - sorted.probability(i);
                d_value = sorted.get(i+1 zero_base_correction)
                  - sorted.get(i zero_base_correction);
                prob    = sorted.get(i zero_base_correction)
                  + (percentage - sorted.probability(i)) * d_value / d_prob;
             }
         }
         //if (prob == 0.0)
         //   cout << "probability not found\n";
         return prob;
      }
   //_get_probability_value____________________________________________________/
   virtual Observation_type append
      (Observation_type value)
      {  dataset_ref.append(value);
         return Sample_abstract<Observation_type,Element_type>::append(value);
      }
   //_append________________________________________________________1898-03-05_/
   float64 get_weighted_value(int at_i)                                    const
      // I think this need to only return float64
      {  float64 sum = dataset_ref.sum();
         float64 value = dataset_ref.get(at_i);
         float64 weighted_value
            = CORN::is_approximately<float64>(sum,0.0,0.0000001)
               ? 0.0 : value/sum;
         return weighted_value;
      }
   //_get_weighted_value____________________________________________2008-10-24_/
 public:
   virtual void recompute()                                        modification_
      {  this->invalidate(STAT_max_bit|STAT_min_bit|STAT_sum_bit
            |STAT_sum_sqrs_bit|STAT_std_dev_bit|STAT_coef_var_bit);              //150915
         dataset_ref.changed = false;                                            //151210
         float64 summation64 = 0;                                                //060118
         float64 sum_of_squares64 = 0;                                           //060118
         nat32 local_count = get_count();
         this->count_as_real = local_count;                                      //181224
         this->minimum =( 999999999.9);                                          //020701
         this->maximum =(-999999999.9);                                          //020701
         bool any_negative_values=false;  // to check for coef.var. validity     //111215
         STAT_mask iterated_applicability = (STAT_max_bit|STAT_min_bit
          |STAT_sum_bit|STAT_sum_sqrs_bit|STAT_std_dev_bit|STAT_coef_var_bit);   //150820
         if (CORN_APPLICABLE & iterated_applicability)                           //150820
         {
            for (nat32 i = 0; i < local_count; i++)
            {  float64 Xi = dataset_ref.get(i);                                  //020702
               if (CORN_APPLICABLE & STAT_min_bit)                               //150820
                  //mod_statistic(min_statistic) = std::min<float64>((float64)ref_statistic(min_statistic),Xi);              //020702
                  this->minimum = std::min<float64>((float64)this->minimum,Xi);  //020702
               if (CORN_APPLICABLE & STAT_max_bit)                               //150820
                 // mod_statistic(max_statistic) = std::max<float64>((float64)ref_statistic(max_statistic),Xi);              //020702
                  this->maximum = std::max<float64>((float64)this->maximum,Xi);  //020702
               summation64 +=  Xi;
               if (CORN_APPLICABLE & STAT_sum_sqrs_bit)                          //150820
                   sum_of_squares64 += CORN_sqr(Xi);
               if (Xi < 0) any_negative_values = true;
            }
            CORN_VALID |= iterated_applicability;                                //150820
         }
         if (local_count == 0)
         {  Sample_abstract<Observation_type,Element_type>::arithmetic_mean = 0;
            Sample_abstract<Observation_type,Element_type>::standard_deviation = 0;
            this->set_valid(STAT_arithmetic_mean_bit | STAT_std_dev_bit);        //150820
         } else
         {
            if (CORN_APPLICABLE & STAT_arithmetic_mean_bit)                      //150820
            {
               Sample_abstract<Observation_type,Element_type>::arithmetic_mean
                  = summation64 / this->count_as_real; //181224 ((float64)local_count)
               this->set_valid(STAT_arithmetic_mean_bit) ;                       //150820
            }
            else this->invalidate(STAT_arithmetic_mean_bit);                     //150915_150820
         }
         //080930 Now using the same method as Excel
         if (CORN_APPLICABLE & (STAT_std_dev_bit | STAT_coef_var_bit))           //150820
         {
         float64 sum_squared_deviations = 0;                                     //080930
         for (nat32 i = 0; i < local_count; i++)                                 //080930
         {  float64  Xi = (float64)get(i);                                       //020702
            float64 deviation = (Xi -
             Sample_abstract<Observation_type,Element_type>::arithmetic_mean);//080930
            float64 deviation_squared = CORN_sqr(deviation);                     //080930
            sum_squared_deviations += deviation_squared;                         //080930
         }                                                                       //080930
         if (   (local_count <= 1)                                               //080930
             || CORN::is_approximately<float64>(sum_squared_deviations,0.0,0.000001) // would cause a div0 error
             || CORN::is_approximately<float64>(Sample_abstract<Observation_type,Element_type>::arithmetic_mean,0.0,0.000001)
             )
         {  // sample size is insufficient to calculate sample_standard_deviation
            Sample_abstract<Observation_type,Element_type>::standard_deviation = 0.0;
            Sample_abstract<Observation_type,Element_type>::coefficient_of_variation = 0.0;
            this->invalidate(STAT_std_dev_bit | STAT_coef_var_bit);              //150915_150820
         } else
         {  float64 degrees_of_freedom = (local_count - 1);                      //080930
            if (CORN_APPLICABLE & STAT_std_dev_bit)                              //150820
            {  Sample_abstract<Observation_type,Element_type>::standard_deviation
                  = sqrt(sum_squared_deviations / degrees_of_freedom);           //080930
               this->set_valid(STAT_std_dev_bit);                                //150915_150820
            }
            if (CORN_APPLICABLE & STAT_coef_var_bit)                             //150820
            {  Sample_abstract<Observation_type,Element_type>::coefficient_of_variation =
                  any_negative_values ? 0.0 :
                   //Coefficient of variation is valid only for data sets in a ratio scale (non negative values)
                   //It is not valid for data sets in interval scale.
                  this->calc_coefficient_of_variation();                         //150915_130801
               this->set_valid(STAT_coef_var_bit);                               //150915_150820
            }
         }                                                                       //080930
         }
         //NYI status_set(STAT_sum_bit|STAT_sum_sqrs_bit|STAT_mean_bit|STAT_coef_var_bit|STAT_std_dev_bit);
         dataset_ref.changed = false;
         Sample_abstract<Observation_type,Element_type>::summation
            = (Observation_type)summation64;                                     //060118
         this->set_valid(STAT_sum_bit);                                          //150915_150820
         Sample_abstract<Observation_type,Element_type>::sum_of_squares
            = (Observation_type)sum_of_squares64;                                //060118
         this->set_valid(STAT_sum_sqrs_bit);                                     //150915_150820
      }
   //_recompute________________________________________________________________/
 public: // The following are overridden so we can track dataset status
   virtual Store_type set(nat32 index,Observation_type value)      modification_
      {  this->invalidate();                                                     //150915
         return dataset_ref.set(index,(Store_type)value);
      }
   //_set___________________________________________________________1999-04-01_/
   virtual Store_type remove_at(nat32 at_index)                    modification_
      {  this->invalidate();                                                     //150915
         return dataset_ref.remove_at(at_index);                                 //141105
      }
   //_remove_at_____________________________________________________1998-07-21_/
 public: // I/O
   virtual nat32 write(std::ostream &strm, const CORN::Item::Format &format,nat16 indent_level)   stream_IO_
      {  nat32 state_elements_written = 0;
         std::string indentation(indent_level,format.indentor);
         const char *key_element_separator = format.key.separator;

         if (this->applicable & STAT_median_bit)
         {  state_elements_written++;
            strm << indentation << statistic_label_table[STAT_median_bit]  << key_element_separator << this->get_median()  << std::endl;
         }
         if (this->applicable & STAT_mode_bit)
         {  state_elements_written++;
            strm << indentation << statistic_label_table[STAT_mode_bit]    << key_element_separator << this->get_mode()    << std::endl;
         }
         if (this->applicable & STAT_value_bit)
         {  state_elements_written++;
            std::string indentation(indent_level,format.indentor);
            strm << indentation << "observation" << key_element_separator
               << format.sequence_inline.initializer;
            for (nat16 a = 0; a < get_count(); a++)
            {  strm << dataset_ref.get(a);
               strm << format.sequence_inline.separator;
            }
            strm << format.sequence_inline.finalizer;
            strm << std::endl; // currently assuming elements are terminated with newline in all formats
         }
         state_elements_written += Sample_abstract<Observation_type,Element_type>
            ::write(strm, format,indent_level);
         return state_elements_written+1;  // currently simply returning the number of statistical elements output
      }
      //_write______________________________________________________2018-12-24_/

 protected:
   float64 calc_median()                                                   const
      {  Dynamic_array<Store_type> sorted(dataset_ref);                                 //061020
         sorted.sort(DESCENDING);
         nat32 cnt = sorted.get_count();
         nat32 midpt = cnt/2;
         float64 return_median = (cnt & 1) // odd
         ?  sorted.get(midpt)
         : (sorted.get(midpt) + sorted.get(midpt+1)) / 2.0;
         return return_median;
      }
   //_calc_median___________________________________________________2001-04-06_/
};
//______________________________________________________________________________
template
   <typename Observation_type                                                    //140624
   ,typename Element_type
   ,typename Store_type>                                                         //160616
class Sample_dataset_clad //200701 was statistical::Sample_dataset_clad
: public extends_ Sample_dataset_cowl                                            //200701
      <Observation_type,Element_type,Store_type>
{
 public:
   Dynamic_array<Store_type> dataset;                                            //160616_141105
 public:
   //200701 statistical::Sample_dataset_clad
   Sample_dataset_clad
      <Observation_type,Element_type,Store_type>()   //150820
      : Sample_dataset_cowl<Observation_type,Element_type,Store_type>//150820
         (dataset)
      ,  dataset(0,30,0)
      { this->reset(); }
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   Sample_dataset_clad<Observation_type,Element_type,Store_type>
      (const Dynamic_array<Store_type> &data_to_copy)                            //150820
      : Sample_dataset_cowl
         <Observation_type,Element_type,Store_type>(*this)                       //150820_141105
      , dataset(data_to_copy)                                                    //141105
      { this->reset(); }                                                         //050705
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   Sample_dataset_clad<Observation_type,Element_type,Store_type>
      (const Observation_type & observation_)                                    //150820
      : Sample_dataset_cowl<Observation_type,Element_type,Store_type>
           (observation_,dataset)                                                //150820
      ,  dataset(0,30,0)
      { this->reset(); }
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   virtual bool reset()                                            modification_ //040123
      {  bool resat = true;                                                      //160721
         dataset.clear();
         resat &= Sample_dataset_cowl
            <Observation_type,Element_type,Store_type>::reset();
         return resat;                                                           //160721
      }
   //_reset_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _2016-05-10_/
 protected:
   virtual bool get_changed()                  const { return dataset.changed; } //141105
};
//_____________________________________________statistical::Sample_dataset_clad_/
}//_namespace_statistical___________________________________________2007-07-01_/
}//_namespace_CORN_____________________________________________________________/
#define Statistical_sample_cowl_dataset statistical::Sample_dataset_cowl
#define Statistical_sample_with_dataset statistical::Sample_dataset_clad
#endif

