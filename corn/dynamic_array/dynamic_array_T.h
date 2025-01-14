#ifndef dynamic_array_TH
#define dynamic_array_TH
#include "corn/primitive.h"
#include "corn/const.h"
#include <math.h>
#  ifndef CORN_MATH_MOREMATH_H
#     include "corn/math/moremath.h"
#  endif
#  ifndef compareHPP
#     include "corn/math/compare.hpp"
#  endif
#include <limits>
#include <algorithm>
#include <iostream>
#include "corn/std/std_iomanip.h"
#include "corn/std/std_iostream.h"
#include "corn/dataset/span.h"

namespace CORN {
enum Ordering {UNORDERED,ASCENDING,DESCENDING};                                  //091217
//______________________________________________________________________________
template <typename T>
class Dynamic_array
{
protected: // contributes
   mutable nat32   current_size_plus_overage;                                    //010531
      // This is the size (number of elements) of the current array.
      // It may be larger than the actual number of elements in the array.
   nat32   resize_overage;                                                       //010531
      // When the array is dynamically resized, the new array may be
      // allocated by this much more with the expectation that more elements
      // will be added. This is used to help reduce that number of
      // memory allocation and copying operations needed when a new
      // array needs to be allocated to accomodate more numbers.
   mutable bool      empty_list;                                                 //000707
   mutable int32     terminal_index; // This is the current highest index to the array. (-1 denotes empty list)  //010531
   mutable bool      resized;        // set to true if the array is actually resized   //980310
public: // actually protected, but now sure how to friend a template
   contribute_ bool      changed;  // set to true if the data set changes
      // this can be used by derive classes to determine when to refresh tallies etc.
protected:
   mutable T *array;
   mutable T  default_value;                                                     //000804
   mutable Ordering order;                                                       //091217
private:
   mutable float64 array_sum; // This holds the last known sum of the array.     //070213
      // array_sum was originally only in the float array.
      // It is mostly only relevent for float arrays
      // (specifically the statistical and math functions)
      // but may be useful for integer arrays.
      // We havethe value as float64 incase of overflow.
public:
   inline nat32 get_terminal_index()const              { return terminal_index;} //010531
   inline nat32 get_current_size()  const  { return current_size_plus_overage; }  //010531
   inline bool  was_resized()       affirmation_       { return resized;       } //990427
   inline bool  is_empty()          affirmation_       { return empty_list;    } //000707
   inline bool  is_modified()       affirmation_       { return changed;       } //110828
public:
   //___________________________________________________________________________
   Dynamic_array
       (nat32 initial_size=0          // This is the initial allocation space
       ,nat32 _resize_overage=10 /*= 10*/   // This is the overage to give on reallocation
       ,T      _default_value=0)
      : current_size_plus_overage(initial_size)
      , resize_overage(_resize_overage)
      , empty_list(initial_size == 0)                                            //000707
      , terminal_index(initial_size ? (nat32)(initial_size - 1) : (nat32)0)
      , resized(false)                                                           //990427
      , changed(false)
      , array(new T[((initial_size))])
      , default_value(_default_value)                                            //000804
      , order(ASCENDING)                                                         //091217
         // when all initial values are the same assume ascending order
      , array_sum(0.0)                                                           //070213
      {
         for (nat32 i = 0; i < initial_size; i++)
            array[i] = default_value;
      }
   //_constructor______________________________________________________________/
   Dynamic_array(const Dynamic_array &copy_from)
      : current_size_plus_overage   ( copy_from.current_size_plus_overage)
      , resize_overage              ( copy_from.resize_overage)
      , empty_list                  ( true )
         // we must indicate this is empty otherwise we may try to use
         // array before it has been allocated was: (copy_from.empty_list )       090826
      , terminal_index              ( copy_from.terminal_index)
      , resized                     ( false)
      , changed                     ( false)
      , array(0)
      , default_value               (copy_from.default_value)                    //000804
      , order                       (copy_from.get_order())                      //091217
      , array_sum(0.0)                                                           //070213
      {  nat32 copy_all_elements = terminal_index+1;                             //000707
         terminal_index = 0;                                                     //000517
         if (!copy_from.empty_list)                                              //140227
            set(copy_from,copy_all_elements);
      }
   //_constructor______________________________________________________________/
   virtual ~Dynamic_array()
   {  delete[] array; array = 0;                                                 //170630
   }
   //_destructor_______________________________________________________________/
public:
   virtual void reallocate_array(nat32 bloated_new_size)               mutation_
   {  bloated_new_size = std::max<nat32>(1,bloated_new_size);
         // We must have at least one item in order for new to give an array.
      T *new_array = new T[bloated_new_size];
      if (array)                                                                 //000707
      {  for (nat32 i = 0; i < bloated_new_size; i++)
            new_array[i]=(i<current_size_plus_overage)?array[i]:default_value;
         delete[] array;  array = 0;                                             //100705
      } else
         for (nat32 i = 0; i < bloated_new_size; i++)
            new_array[i] = default_value;
      if ((terminal_index> 0) &&(bloated_new_size < (nat32) terminal_index))     //041111
      {  resized = true;                                                         //041030
         changed = true;                                                         //041030
         terminal_index = bloated_new_size  - 1;                                 //041030
      }                                                                          //041030
      array = new_array;
   }
   //_reallocate_array_________________________________________________________/
   bool if_necessary_resize_for(nat32 index)                           mutation_
   {  if ((index >= current_size_plus_overage) || empty_list)                    //081030
         resize((nat32)(index+1));                                     //200226
      terminal_index = std::max<nat32>(terminal_index,(int32)index);             //111017
      return changed;                                                            //160120
   }
   //_if_necessary_resize_for__________________________________________________/
   void resize(nat32 new_size)                                         mutation_
   {  terminal_index = (int32)(new_size -1);
      nat32  bloated_new_size = (nat32)(new_size + resize_overage);
      reallocate_array(bloated_new_size);
      current_size_plus_overage = bloated_new_size;
      changed = true;
      //180727 array_sum = 0;                                                    //160510
      resized = true;                                                            //990427
      array_sum = sum_first(new_size);                                           //131106
      empty_list =  terminal_index < 0;                                          //180727_070604
         // Even if index is 0 we now have at least 1 element at index 0;
   }
   //_resize___________________________________________________________________/
   inline nat32 get_count()                                                const
      { return empty_list? (nat32)0 : (nat32)(get_terminal_index()+(nat32)1); }
   //_get_count_________________________________________1999-02-14__1998-03-05_/
   inline virtual bool clear()                                     modification_
   {  array_sum = 0;
      terminal_index = 0;
      empty_list = true;
      current_size_plus_overage = 0;
      for (nat32 i = 0; i < current_size_plus_overage; i++)
      {  array[i] = default_value;
         array_sum += default_value;
      }
      // virtual because Statistical dataset needs to reset also
      return true;
   }
   //_clear___________________________________2018-07_27_2004-01-23_1998-03-10_/
   inline void set_resize_overage(nat32 new_resize_overage)
   { resize_overage = new_resize_overage;  }
   //_set_resize_overage____________________________________________2000-08-31_/
   T get(nat32 index,bool allow_resize=false)                          mutation_
   {  if (allow_resize) if_necessary_resize_for(index);                          //090212
      bool index_outof_range = ((index >= current_size_plus_overage)
         || ((int32)index > terminal_index));
      return index_outof_range || !array                                         //090826_990718
      ? default_value                                                            //990408
      : array[index];
   }
   //_get___________________________________________________________1999-04-09_/
   T &ref(nat32 index)                                                 mutation_
   {  if_necessary_resize_for(index);
      return array[index];
   }
   //_ref___________________________________________________________2016-09-15_/
 public:  // Order and sorting
   inline Ordering get_order()                            const { return order;}
   //_get_order_____________________________________________________1909-12-17_/
   inline virtual void require_order(Ordering required_order)      contribution_
      { order = required_order; }
   //_require_order_________________________________________________2010-01-19_/
   virtual void invert_order(nat32 count)                          modification_
   {  if (!count) count = get_count();
      nat32 midpoint = count / 2;
      for (nat32 low_i = 0; low_i <= midpoint; low_i++)                          //120408
      {  nat32 high_i = (count-1) - low_i;
         T low_v = get(low_i,false);
         T high_v = get(high_i,false);
         set(low_i,high_v);
         set(high_i,low_v);
      }
   }
   //_invert_order__________________________________________________2012-02-16_/
   virtual void sort(Ordering ordering = ASCENDING)                modification_
   {  // quick sort  This could be improved to work in place
      if (order == ordering) return; // the set is already ordered.              //091217
      nat32 count = get_count();
      if (count > 1)
      {  if (  ((order == DESCENDING) && (ordering == ASCENDING ))               //120216
             ||((order == ASCENDING ) && (ordering == DESCENDING)))              //120216
            // The array is sorted but in the reverse order
            // in this special case we can simply invert the order.
            invert_order(count);                                                 //120216
         else                                                                    //120216
         {  Dynamic_array<T> lower(0,count,default_value);
            Dynamic_array<T> higher(0,count,default_value);
            T pivot = get(0,false);  /*no_resize */
            for (nat32 i = 1; i < count; i++)
            {  T a_i = get(i,false);     // no_resize
               if (a_i <= pivot)
                    lower.append(a_i);
               else higher.append(a_i);
            }
            if (lower.get_order() != ordering)                                   //100119
               lower.sort(ordering );
            if (higher.get_order() != ordering)                                  //100119
               higher.sort(ordering );
            resize(0);  // We don't want to clear because sort is called by Statics update (calc_median) methods but we don't what to delete what data we have already computed (clear erases everything in derived classes) 050922_
            append_array(ordering == ASCENDING  ? lower : higher);               //040919
            append(pivot);
            append_array(ordering == ASCENDING  ? higher: lower);                //040919
            order = ordering;                                                    //091217
         }
      } // else as single item is already sorted;
   }
   //_sort_____________________________________________________________________/
 public: // virtual or common to other type arrays'
   inline virtual bool   good()                                            const
      { return array != 0; }
   //_good__________________________________________________________1998-03-18_/
   virtual T force(nat32 index,T value)
   {  // I had tried to remove the return value because the float64
      // causes "hides virtual function" compiler warnings.
      // but there are still warnings
      //191130 unused bool resized =
            if_necessary_resize_for(index);                             //160120_000517
      empty_list = false;                                                        //000710
      array[index] = value;
      changed = true;
/*
      T current_value = array[index];                                            //161222
      if (value != current_value)                                                //161222
      {
         if (!resized)                                                           //160120
            array_sum -= (float64)current_value;                                 //090129
         array[index] = value;
         array_sum += (float64)value;                                            //160120_090129
         changed = true;                                                         //070220
      }
*/
      return value;
   }
   //_force___________________________________________________________18-03-08_/
   virtual T set(nat32 index,T value)
   {  // I had tried to remove the return value because the float64
      // causes "hides virtual function" compiler warnings.
      // but there are still warnings
      bool resized = if_necessary_resize_for(index);                             //160120_000517
      empty_list = false;                                                        //000710
      T current_value = array[index];                                            //161222
      if (value != current_value)                                                //161222
      {
         if (!resized)                                                           //160120
            array_sum -= (float64)current_value;                                 //090129
         array[index] = value;
         array_sum += (float64)value;                                            //160120_090129
         changed = true;                                                         //070220
      }
      return value;
   }
   //_set_______________________________________________________________________
   void set(const Dynamic_array &to_copy,nat32 new_size  = 0)
   {  // Sets all entries
      nat32 tocopy_max_index = to_copy.get_terminal_index();
      {  if (get_terminal_index() != tocopy_max_index)                           //990428
            resize(new_size ? new_size : (nat32)(tocopy_max_index+1));           //000225
         for (nat32 i = 0; i <= get_terminal_index(); i++)
            set(i,to_copy.get(i,false));
      }
      changed = true;
   }
   //_set______________________________________________________________________/
   virtual T remove_at(nat32 at_index)
   {  /// Removes the number at the specified index.
      /// All following array elements are moved down one cell and the ?max_index? is decremented
      /// \return the removed value.
      T removed_value = 0;                                                       //100712
      if (!empty_list)                                                           //000707
      {  if ((sint32)at_index < terminal_index)                                  //070213
         {  removed_value = array[at_index];                                     //100712
            array_sum -= (float64)removed_value;                                 //160120_070213
         }
         for (nat32 i = at_index; i < (nat32)terminal_index; i++)
            array[i] = array[i+1];
         if (terminal_index == 0)   empty_list = true;
         else terminal_index = (nat32)(terminal_index - 1);
      }
      return  removed_value;                                                     //100712
   }
   //_remove_at_____________________________________________________1998-07-21_/
   virtual T deduct_at(nat32 at_index, T amount, T limit = 0.0)                  //160719
   {  /// reduces the value in the array by the specified amount
      /// but doesn't allow the amount to go below the limit.
      /// \returns the actual amount of deduction satifying the limit
      T deduction_act = amount;
      T value = array[at_index];
      if (deduction_act > value)
         deduction_act = value;
      value -= deduction_act;
      if (value < limit)
      {
         value = array[at_index];
         deduction_act = value - limit;
         value -= deduction_act;
      }
      set(at_index, value);
      return deduction_act;
   }
   //_deduct_at_____________________________________________________2016-07-19_/
   inline void set_default_value(T default_value_)                    mutation_
   { default_value = default_value_; }
   //_set_default_value_____________________________________________2000-11-03_/
   T pop()
   {  nat32 last_index = get_count();
      T popped = get(last_index);
      resize(last_index);
      return popped;
   }
   //_pop___________________________________________________________2016-02-20_/
   virtual T append(T value)
   {  // Append adds the specified value to the end of the data set.
      // returns the value appended.

      // actually should return true if success

      // This should be replaced with append_where


      nat32 at = is_empty() ? 0 : (nat32)(terminal_index+1);                     //010831
      nat32 curr_count = get_count();  // Note that this ordering can be moved to DataSet, it has general application
      T current = curr_count ? get(terminal_index,false) : 0;
      if (curr_count > 1)
      {  switch (order)
         {  case ASCENDING    : if (value < current)  order = UNORDERED; break; // the added item is out of order, so the whole set is considered out of order
            case DESCENDING   : if (value > current)  order = UNORDERED; break; // the added item is out of order, so the whole set is considered out of order
            default : break; // We have already determined the set is out of order.
         }
      }  else
         order = (curr_count == 1)   // The first two items determine the initial order
            ? (value > current) ? ASCENDING : DESCENDING
         : order;                                                                //100119
      set(at,value);
      return value;
   }
   //_append____________________________________________2009-12-17__2004-10-02_/
   virtual nat32 append_where(T value)
   {  // Append adds the specified value to the end of the data set.
      // returns the index where it occurs .
      // return 0xFFFFFFFF if fails

      nat32 at = is_empty() ? 0 : (nat32)(terminal_index+1);                     //010831
      nat32 curr_count = get_count();  // Note that this ordering can be moved to DataSet, it has general application
      T current = curr_count ? get(terminal_index,false) : 0;
      if (curr_count > 1)
      {  switch (order)
         {  case ASCENDING    : if (value < current)  order = UNORDERED; break; // the added item is out of order, so the whole set is considered out of order
            case DESCENDING   : if (value > current)  order = UNORDERED; break; // the added item is out of order, so the whole set is considered out of order
            default : break; // We have already determined the set is out of order.
         }
      }  else
         order = (curr_count == 1)   // The first two items determine the initial order
            ? (value > current) ? ASCENDING : DESCENDING
         : order;                                                                //100119
      set(at,value);
      return at;
   }
   //_append____________________________________________2009-12-17__2004-10-02_/
   virtual nat32 append_array_null_terminated(const T values[])    modification_
   {  ///< Adds the array of values to this array
      /// Values are appended until values[n] is 0 otherwise the size specified by values_count is copied
      /// \return The new array size is returned.
      /// \note This method is intended only for integer arrays.
      /// Avoid using this with floats because of precision problems 0.0 may not be exactly 0
      nat32 old_size = get_count();
      for (int i = 0; values[i] ; i++)
      {  T append_value = values[i];
         array_sum += (float64)append_value;                                     //160120_070213
         set(old_size+i,append_value);
      }
      return get_count();
   }
   //_append_array_null_terminated__________________________________2003-08-01_/
   nat32 append_array_counted(const T values[], nat32 count_of_values_to_copy)
   {  ///< Adds the arrays of values (with size indicated by values_count) to the array
      /// Adds the arrays of values to the array
      ///     /the size specified by values_count is copied
      /// \return The new array size is returned.
      nat32 old_size = get_count();
      for (int i = 0; i < count_of_values_to_copy; i++)
      {  T append_value = values[i];
         array_sum += (float64)append_value;                                     //160120_070213
         set(old_size+i,values[i]);
      }
      return get_count();
   }
   //_append_array_counted__________________________________________2003-08-01_/
   nat32 append_array
      (const Dynamic_array &values
      ,Span *span = 0)                                                           //200426
   {  ///< appends all the values to this array
      /// \return The new array size is returned.
      nat32 from = span ? span->get_from()   : 0;                                //200426
      nat32 to   = span ? span->get_to()      : values.get_count()-1;            //200426
      for (nat32 i = from; i <= to ; i++)                                        //200426
      //200426 for (nat32 i = 0; i < values.get_count(); i++)
      {  T append_value = values.get(i,false);  // no_resize                     //070213
         array_sum += (float64)append_value;                                     //160120_070213
         append(append_value);                                                   //070213
      }
      return get_count();
   }
   //_append_array__________________________________________________2001-05-23_/
   bool occurs(T value, nat32 &found_index,T precision = 0)         affirmation_
   {  ///< \return true if the specified value is in the array and returns the index of the first occurance in found_index.
      bool found = false;
      /// Avoid using this with floats because of precision problems because a value stored in the array may or may not be exactly the specified value
      for (nat32 i = 0; i < get_count() && !found; i++)
      {  T entry = get(i,false);                                                 //110919
         if ((entry == value) ||                                                 //110919
             (CORN::is_approximately<T>(entry,value,precision)))                 //111017 (entry == value)  // no_resize
         {  found = true;
            found_index = i;
         }
      }
      return found;
   }
   //_is_in_array______________________________________________________________/
   nat32 count_occurances(T value,T precision = 0)                         const
   {  ///< \return true if the specified value is in the array and returns the index of the first occurance in found_index.
      nat32 occurances = false;
      /// Avoid using this with floats because of precision problems because a value stored in the array may or may not be exactly the specified value
      for (nat32 i = 0; i < get_count(); i++)
      {  T entry = get(i,false);
         if ((entry == value) ||
             (CORN::is_approximately<T>(entry,value,precision)))
            occurances += 1;
      }
      return occurances;
   }
   //_count_occurances______________________________________________2013-04-18_/
   nat32 find_nearest_index(T value)                                       const
   {  nat32 closest_index = 0;
       T closest_diff = fabs(get(0) - value);
      for (nat32 i = 0; i < get_count(); i++)
      {  T entry = get(i,false);
         T value_diff = fabs(entry - value);
          if      (value_diff == 0) return i;
          else if (value_diff < closest_diff)
          {
              closest_index = i;
              closest_diff = value_diff;
          }
      }
      return closest_index;
   }
   //_find_nearest_index____________________________________________2012-06-15_
   inline const T *get_array()                           const { return array; }
      ///< \note get_array() functions should only be used if you know what you are doing!!!
   //_get_array_____________________________________________________2008-12-12_/
   inline       T *get_array()                   modification_ { return array; }
      ///< \noteget_array() functions should only be used if you know what you are doing!!!
   //_get_array_____________________________________________________2008-12-12_/
   virtual void set_values(const T values[],nat32 count)           modification_
   {  for (nat32 i = 0; i < count; i++)
        set(i,values[i]);
      changed = true;
   }
   //_set_values_______________________________________________________________/
   virtual void copy(const Dynamic_array<T> &from_copy, nat32 new_size = 0)
   {  nat32 fromcopy_max_index = from_copy.get_terminal_index();
      array_sum = 0.0;                                                           //070213
      {  if (get_terminal_index() != fromcopy_max_index)                         //990428
            resize(new_size ? new_size : (nat32)(fromcopy_max_index+1));         //000225
         if (!from_copy.is_empty())
            for (nat32 i = 0; i <= get_terminal_index(); i++)
            {  T from_value = from_copy.get(i,false);  // no_resize
               set(i,from_value);
            }
      }
      changed = true;
   }
   //_copy_____________________________________________________________________/
   float64 sum_first(nat32 first_count)                                    const
   {  float64 first_sum = 0.0;
      // The array may not have the requested number of items
      // so we some only what is available
      nat32 actual_count = std::min<nat32>(first_count, get_count());
         for (nat32 i = 0; i < actual_count; i++)
            first_sum += (float64)array[i];                                      //160120
      return first_sum;
   }
   //_sum_first_____________________________________________________2013-11-06_/
   float64 sum_last(nat32 last_count)                                      const
   {  float64 last_sum = 0.0;
      // The array may not have the requested number of items
      // so we some only what is available
      nat32 actual_count = std::min<nat32>(last_count, get_count());
      for (nat32 i = get_count() - actual_count ; i < get_count(); i++)
            last_sum += (float64)array[i];                                       //160120
      return last_sum;
   }
   //_sum_last_________________________________________________________________/
   float64 sum()                                                           const
   {  return sum_first(terminal_index+1);
   }
   //_sum______________________________________________________________________/
   const float64 &ref_array_sum()                                          const
   {  return array_sum;
   }
   //_ref_array_sum_________________________________________________2017-05-03_/
   modifiable_ float64 &mod_array_sum()
   {  return array_sum;
   }
   //_mod_array_sum_________________________________________________2018-09-17_/
   T find_minimum(nat32 upto_index )                                       const
   {  T mn =
      #ifdef _MSC_VER
      // Don't know why MSC doesn't like the numeric limits
      // I think there is a max() macro lurking around some where.
      // this really needs to be fixed because the hack here is very problematic
         99999;
      #else
      std::numeric_limits<T>::max();
      #endif
      if (get_count())                                                           //990403
         for (nat32 i = 0; i <= upto_index; i++)
            mn = std::min<T>(mn,array[i]);                                       //111017
      return mn;
   }
   //_find_minimum__________________________________________________2004-09-21_/
   inline T find_minimum()                                                 const
   {   return find_minimum(terminal_index);
   }
   //_find_minimum__________________________________________________2004-09-21_/
   inline T find_maximum()                                                 const
   {   return find_maximum(terminal_index);
   }
   //_find_maximum__________________________________________________2004-09-21_/
   T find_maximum(nat32 upto_index)                                        const
   {  T mx = 
      #ifdef _MSC_VER
      // Don't know why MSC doesn't like the numeric limits
      // I think there is a max() macro lurking around somewhere.
      // this really needs to be fixed because the hack here is very problematic
         -99999;
      #else
         std::numeric_limits<T>::min();
      #endif
      if (get_count())                                                           //990403
         for (nat32 i = 0; i <= upto_index; i++)
            mx = std::max<T>(mx,array[i]);
      return mx;
   }
   //_find_maximum__________________________________________________2004-09-21_/
   nat16  find_minimum_index()                                             const
   {  // Returns the index of the minimum value;
      T mn = 
      #ifdef _MSC_VER
      // Don't know why MSC doesn't like the numeric limits
      // I think there is a max() macro lurking around some where.
      // this really needs to be fixed because the hack here is very problematic
         99999;
      #else
         std::numeric_limits<T>::max();
      #endif
      nat16 min_index(0);
      if (get_count())
         for (sint32 i = 0; i <= terminal_index; i++)
         {  T array_i = array[i];
            if (array_i < mn)
            {  mn = array_i;
               min_index = (nat16)i;
            }
         }
      return min_index;
   }
   //_2005-09-07________________________________________________________________
   inline virtual T get_min()                    const { return find_minimum();}
   //_get_min_______________________________________________________2004-09-21_/
   inline virtual T get_max()                    const { return find_maximum();}
   //_get_max_______________________________________________________2004-09-21_/
   // All the following XXXX_by methods return the updated sum of the array
   virtual float64 increment_by(const Dynamic_array &addends)      modification_
   {  ///< returns float the updated sum of all values in the array
      /// \note This is intended for float arrays so care must be taken with integer arrays because of the possability of overflow
      nat32 cnt = addends.get_count();
      for (nat32 i = 0; i < cnt; i++)
      {  T addend = addends.get(i,false);
         T augend = get(i,false);
         T item_sum = augend + addend;                                           //070213
         set(i,item_sum);
      }
      return sum();                                                              //070213
   }
   //_increment_by________________________________________________________2004_/
   virtual float64 decrement_by(const Dynamic_array &subtrahends)  modification_
   {  ///< \return the updated sum of all values in the array
      /// \note This is intended for float arrays so care must be taken with integer arrays because of the possability of overflow
      for (nat32 i = 0; i < subtrahends.get_count(); i++)
      {  T minuend = get(i,false);
         T subtrahend = subtrahends.get(i,false);
         T subtraction = minuend -subtrahend;
         set(i,subtraction );
      }
      return sum();
   }
   //_decrement_by__________________________________________________2007-02-13_/
   virtual float64 increment_by(nat32 at_index,T addend)           modification_
   {  ///< \return The updated sum of all values in the array is returned.  We return float64 to avoid overflow
      /// \note This is intended for float arrays so care must be taken with integer arrays because of the possability of overflow
      T current_i = get(at_index,false);
      T new_i = current_i + addend;
      //170216 unused T delta = new_i - current_i;
      set(at_index,new_i);
      return sum();
   }
   //_increment_by______________________________________2007-02-13__2009-04-09_/
   virtual float64 decrement_by(nat32 at_index,T subtrahend)       modification_
   {  ///< \return The updated sum of all values in the array is returned. We return float64 to avoid overflow
      /// \note This is intended for float arrays so care must be taken with integer arrays because of the possability of overflow
      // Add the value to the entry at the specified index.
      T minuend = get(at_index,false);
      T new_i = minuend - subtrahend;
      set(at_index,new_i);
      return array_sum;
   }
   //_decrement_by______________________________________2007-02-13__2009-04-09_/
   virtual float64 inc(nat32 at_index,T addend)                    modification_
   {  ///< \return The updated sum of all values in the array is returned.  We return float64 to avoid overflow
      /// \note This is intended for float arrays so care must be taken with integer arrays because of the possability of overflow
      T current_i = get(at_index,false);
      T new_i = current_i + addend;
      //170216 unused  T delta = new_i - current_i;
      return set(at_index,new_i);
   }
   //_inc_______________________________________________2007-02-13__2009-04-09_/
   virtual float64 dec(nat32 at_index,T subtrahend)                modification_
   {  ///< \return The updated sum of all values in the array is returned. We return float64 to avoid overflow
      /// \note This is intended for float arrays so care must be taken with integer arrays because of the possability of overflow
      // Add the value to the entry at the specified index.
      T minuend = get(at_index,false);
      T new_i = minuend - subtrahend;
      return set(at_index,new_i);
   }
   //_dec_______________________________________________2007-02-13__2009-04-09_/

//990409_conceptual float64 increment_by(float64 delta);
      // These would change the value to all elements by the delta amount
      // and return the new sum of the array
   //_2009-04-09______________________________________________________________
//990409_conceptual float64 decrement_by(float64 delta);
      // These would change the value to all elements by the delta amount
      // and return the new sum of the array
   //_______________________________________________________________
   virtual float64 multiply_by(float64 multiplier)                 modification_
   {
      ///< Multiply each entry by value and set the result backinto the entry.
      /// \note This is intended for float arrays so care must be taken with integer arrays because of the possability of overflow
      ///< \return The updated sum of all values in the array is returned.
      for (nat32 i = 0; i < get_count(); i++)
      {  T multiplicand = get(i,false);
         float64 product = (float64)multiplicand * multiplier;
         set(i,(T)product);
      }
      return sum();
   }
   //_multiply_by_______________________________________2007-02-13__2009-04-09_/
   virtual float64 divide_by(float64 divisor)                      modification_
   {
      ///< Divide each entry by value and set the result backinto the entry.
      /// If value is 0, the entries are set to an infiniately large number
      /// with sign matching the original entry value.
      /// \note This is intended for float arrays so care must be taken with integer arrays because of the possability of overflow
      ///< \return The updated sum of all values in the array is returned.
      nat32 count = get_count();
      for (nat32 i = 0; i < count; i++)
      {  float64 dividend = get(i,false);
         float64 result = (divisor != 0.0) ? dividend/divisor : (float64)9999999.99; // avoid div by zero
         set(i,(T)result);
      }
      return sum();
   }
   //_divide_by__________________________________________2007-02-13_1999-04-09_/
   virtual float64 get_summation()                                    provision_
   {  return changed ? sum() : array_sum;
   }
   //_get_summation_________________________________________________2015-06-25_/
   virtual bool clear_changed()                                    contribution_
   {  return changed = false;                                                    //170215
         // After initially loading the array one may consider the array unchanged.
   }
   //_clear_changed____________________________________________________________/
 public: // Output

#ifdef __BCPLUSPLUS__
void Dynamic_array::dump(std::ostream &strm,nat8 precision, bool indexed, char delimiter,  const char *variable, const char *section)
{
	//		This outputs the dataset values to the specified stream.
	//		If indexed is false the format format is dataset=9.999,9.999,....
	//		If indexed is true the format is dataset=1=9.999,2=9.999 with
	//		If section is specified  [section] will be output where section is the specified character string
	//		I.e. dump(cout,3, true,'\n',0,"my_section")
 //    Will give:
	//		[my_section]
	//		1=9.999
	//		2=9.999
	//		:
	//		I.e. dump(cout,2, false,',',"my_var",0)
 //    Will give:
	//		my_var=9.99,9.99,9.99 . . .
	if (section)
		strm << '[' << section << ']' << std::endl;
	if (variable && !indexed)
		strm << variable << '=';

	for (nat32 i = 0; i < get_count(); i++)
	{	if ((!section) && variable)
			strm << variable;
		if (indexed) {strm << i << '=';}
		strm << std::setprecision(precision) << get(i,false);
		strm << delimiter;
	}

}
#endif
   const T &operator[](nat32 index) const
   {  return ref(index);
   }
   T &operator[](nat32 index)
   {  return ref(index);
   }
}; // Dynamic_array
//______________________________________________________________________________
}//_namespace_CORN_____________________________________________________________/
#endif

