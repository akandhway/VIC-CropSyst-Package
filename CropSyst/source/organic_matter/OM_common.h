#ifndef OM_commonH
#define OM_commonH
#include "organic_matter/OM_types.h"
#include "common/biomatter/biomass_abstract.h"
#include "common/residue/residue_const.h"
#include "corn/math/compare.hpp"
#include <iomanip>
//______________________________________________________________________________
namespace Soil
{  class Abiotic_environment_layer;
   class Texture_interface;
}
//_forward_declarations________________________________________________________/
struct C_Balance_OM
{
   float64 initial ;   // including initial
   float64 CO2_emission;
   float64 humification_from_residue;
   float64 current;
   mutable float64 balanceOM;
   inline float64 balance_daily() const { return balanceOM = initial + humification_from_residue - CO2_emission - current; }
   inline float64 balance() const { return balanceOM = initial + humification_from_residue - CO2_emission - current; }
   inline friend std::ostream &operator<<(std::ostream &strm, C_Balance_OM C_bal_OM)
      {  strm<< std::setprecision(10)  << '\t' << C_bal_OM.initial << '\t'
            << C_bal_OM.CO2_emission  << '\t' << C_bal_OM.humification_from_residue << '\t' << C_bal_OM.current
            << '\t' << C_bal_OM.balance();
         return strm;
      }
   inline std::ostream &write_column_headers(std::ostream &strm)
      {  strm << "\tOM_initial\tOM_CO2_emission\tOM_humification_from_residue\tOM_current\tOM_balance";
         return strm;
      }
   inline C_Balance_OM()
      :initial(0)
      ,CO2_emission(0)
      ,humification_from_residue(0)
      ,current(0)
      {}
   inline void advance()
      {  initial = current;
         CO2_emission = 0;
         humification_from_residue = 0;
         current = 0;
      }
};
//_C_Balance_OM________________________________________________________________/
struct C_Balance_residue
{  float64 initial;
   float64 additional;
   float64 CO2_emission;
   float64 humification_to_OM;
   float64 current;
   mutable float64 balanceR;
   inline float64 balance()                                                const
      { return balanceR = initial + additional - CO2_emission - humification_to_OM - current ; }
   inline friend std::ostream &operator<<(std::ostream &strm, C_Balance_residue C_bal_residue)
      {  strm << std::setprecision(10) << '\t' << C_bal_residue.initial << '\t' << C_bal_residue.additional << '\t' << C_bal_residue.CO2_emission << '\t' << C_bal_residue.humification_to_OM << '\t' << C_bal_residue.current
         << '\t' << C_bal_residue.balance();
         return strm;
      }
   inline std::ostream &write_column_headers(std::ostream &strm)
      {  strm << "\tR_initial\tR_addition\tR_CO2_emission\tR_humification_from_residue\tR_current\tR_balance";
         return strm;
      }
   inline C_Balance_residue()
      :initial(0)
      ,additional(0)
      ,CO2_emission(0)
      ,humification_to_OM(0)
      ,current(0)
      {}
   inline void advance()
   {  initial = current;
      additional = 0;
      CO2_emission = 0;
      humification_to_OM = 0;
      current = 0;                                                               //120511
   }
};
//_C_Balance_residue___________________________________________________________/
struct C_Balance
{  bool initializing;
   C_Balance_OM         OM;
   C_Balance_residue    residue;
   inline float64 balance() const { return OM.balance() + residue.balance(); }
   inline bool has_error()  const { return !CORN::is_zero<float64>(balance(),0.00001); }
   inline C_Balance()
      :initializing(true)
      ,  OM()
      , residue()
      {}
   inline friend std::ostream &operator<<(std::ostream &strm, C_Balance C_bal)
      {  strm<< std::setprecision(10)  << '\t' << C_bal.balance() <<C_bal.OM << C_bal.residue;
         return strm;
      }
   inline std::ostream &write_column_headers(std::ostream &strm)
      {  strm << "\tbalance";
         OM.write_column_headers(strm);
         residue.write_column_headers(strm);
         return strm;
      }
   inline void add_residue(float64 added_mass)
      {  if (initializing)  residue.initial     += added_mass;
         else               residue.additional  += added_mass;
      }
   inline  void advance()
      {  OM.advance();
         residue.advance();
      }
};
//_C_Balance________________________________________________________2011-10-05_/
class Organic_biomatter_common    // Rename this to Soil_organic_biomatter_common
: public Biomass_abstract
{
 public:  /// properties
   Organic_matter_type     type;
   Organic_matter_position position;
   nat8                    layer; // 0 if position is flat surface or standing stubble  otherwise subsurface soil layer (sublayer)
 public:  /// parameters
   float64                 detrition_constant;                                   //090702
      ///< Non-microbial (abiotic) decomposition
      ///< Currently applied only to surface plant residues (otherwise it is 0.0)
      /// Typically about -ln(0.5)/720.0  //090702
 private:  // external ref
   contribute_ Soil::Abiotic_environment_layer *soil_abiotic_env;
      // Must be a mutable pointer because this value is set when creating detrition.
      // But it otherwise could be considered a const reference because
      // it is always set and the attributes are not modified.

 protected:   /// external
   const Soil::Texture_interface &soil_texture;
 public: /// structors
   Organic_biomatter_common(const Organic_biomatter_common &from_copy);
   Organic_biomatter_common         // For SOM redistribution
      (Organic_matter_type     type
      ,nat8                    layer
      ,Organic_matter_position position
      ,float64                 biomass
      ,float64                 carbon_fraction
      ,float64                 carbon_nitrogen_ratio
      ,float64                 decomposition_constant
      ,float64                 detrition_constant         // only applies to residues (otherwise should be ????) 090702
      ,const Soil::Texture_interface &soil_texture                               //070707
      ,const Soil::Abiotic_environment_layer &soil_abiotic_environment_layer);
   virtual Organic_biomatter_common *clone()                            const=0;
   // Although pools take a (point) reference to the contact fraction
   // detritis added to the soil needs have the contact fraction changed to subsurface
   inline virtual float64 &know_contact_fraction(float64 &contact_fraction_) cognition_
      { return contact_fraction_; }                                             //190327
      // Do nothing, derived classes that have contact fraction will override
   /*190327
   inline virtual float64 set_contact_fraction(float64 &_contact_fraction) modification_
      { return _contact_fraction;}
   */
   inline virtual float64 get_contact_fraction()           const { return 1.0; } //070707

   inline bool know_soil_abiotic_environment                                     //190218
      (const Soil::Abiotic_environment_layer &soil_abiotic_env_)     cognition_
      {
         soil_abiotic_env = const_cast<Soil::Abiotic_environment_layer *>
            (&soil_abiotic_env_);
         return true;
      }

   inline const Soil::Abiotic_environment_layer &ref_soil_abiotic_env()    const //190218
      { return (*soil_abiotic_env); }
 public: // state accessors
   virtual float64 get_area_per_mass()                                  const=0; //070707
   virtual float64 set_area_per_mass(float64 new_area_per_mass) modification_=0; //070707
   virtual RUSLE2_Residue_type     get_RUSLE2_residue_type()            const=0; //070707
   virtual RUSLE2_Residue_type     set_RUSLE2_residue_type(RUSLE2_Residue_type new_RUSLE2_residue_type) = 0; //070707
   virtual Organic_matter_cycling  get_carbon_cycling()                 const=0; //070707
   inline virtual Organic_matter_position set_position(Organic_matter_position new_position) modification_ { return position = new_position; }     //090413
   inline virtual float64 get_C_decomposed_for_output_only() const {return 0.0;}
      // derived classes will override if they implement carbon decomposition.   //090520
 public: // type accessors
   bool is_straw_or_manure_residue()                               affirmation_;
   bool is_manure()                                                affirmation_;
   bool is_standing_stubble()                                      affirmation_;
   bool is_flat_surface()                                          affirmation_; //060728
   bool is_above_ground()                                          affirmation_;
   inline bool is_subsurface()                                     affirmation_
      { return position == OM_subsurface_position; }
   bool is_comparable(const Organic_biomatter_common &to_other)    affirmation_; //060605
 public: //
   float64 add_different_mass                                                     //060727
      (const Organic_biomatter_common &different_but_similar_mass);
   virtual void clear_decomposition()                           modification_=0;
   virtual float64 get_N_mineralization_mass()                         const =0; //061116
   virtual float64 get_N_immobilization_demand_mass()                  const =0; //061116
   modifiable_ Organic_biomatter_common *detrition()              modification_; //090702
   //120912 this was added so we can store historical equilibrated OM for futher future scenario runs
   virtual void  write_header(std::ostream &stream)                  stream_IO_; //120912
   virtual bool  write(std::ostream &stream)                         stream_IO_; //150728_120912
   virtual std::istream &read(std::istream &streamstream)            stream_IO_; //120912
};
//_Organic_biomatter_common_________________________________________2007-07-07_/
namespace Soil { class Nitrogen; }                                               //181206
//______________________________________________________________________________
class Organic_biomatter_pool_common
: public Organic_biomatter_common
{
 private:
   cognate_ float64 *contact_fraction;
      // Must be a pointer/reference because flat surface residues
      // changes based on the current residue load.
 public:  // properties
   Organic_matter_cycling  carbon_cycling;    // Currently applies only to residue
   // Reference to either
   // Organic_matter_pools_profile_V4_4::subsurface_contact_fraction (which should always be 0)
   // Residues_multiple::stubble_contact_fraction
   // Residues_multiple::flat_surface_contact_fraction
   // Residues_multiple::subsurface_contact_fraction (which should always be 0)
   // Canopy_biomass_GAI::decomposing_canopy_contact_fraction

   // Will be 1.0 for subsurface residue and all SOM
   // Currently (from V.B. version) there is a single contact fraction value for all
   // surface and stubble residues.
   // All SOM and subsurface residues have contact fraction 1.0
 public: // properties that apply only to straw residues and manure
   RUSLE2_Residue_type     RUSLE2_residue_type;                                  //060731
 private:
   float64                 area_per_mass;       // applies only to surface residue?
 public: // processing
   float64             C_fraction_transfered_to_CO2;
   float64             C_fraction_transferred_to[4];
 protected:
   Organic_biomatter_pool_common    *receiver[4];  // owned by organic_matter_pools do not delete
 protected:
   float64 C_decomposed_for_output_only;                                         //090520
 public:
   Organic_biomatter_pool_common
      (Organic_matter_type     type
      ,uint8                   layer
      ,Organic_matter_position position
      ,Organic_matter_cycling  cycling
      ,RUSLE2_Residue_type     RUSLE2_residue_type                               //060721
      ,float64                 biomass
      ,float64                 carbon_fraction
      ,float64                 carbon_nitrogen_ratio
      ,float64                 decomposition_constant
      ,float64                 detrition_constant                                //090702
      ,float64                 area_per_mass
         // = 0.0  stubble and surface residue_only?
      ,float64                &contact_fraction
      ,const Soil::Texture_interface    &soil_texture                            //070707
      ,const Soil::Abiotic_environment_layer  &soil_abiotic_environment_layer);
         // Constructor used for For SOM redistribution
   Organic_biomatter_pool_common(const Organic_biomatter_pool_common &from_copy);
   virtual Organic_biomatter_pool_common *clone()                       const=0;
 public:
   float64 deduct_carbon(float64 removed_carbon_mass);
   virtual float64 receive_carbon(float64 added_carbon_mass);                    //190219
   virtual void receive_carbon_pending(float64 added_carbon_mass) = 0;
   void contribute_to
      (float64                 _C_fraction_transfered_to_CO2
      ,Organic_biomatter_pool_common *_receiver_1     ,float64 _C_fraction_transferred_to_1 = 1.0
      ,Organic_biomatter_pool_common *_receiver_2 = 0 ,float64 _C_fraction_transferred_to_2 = 0.0
      ,Organic_biomatter_pool_common *_receiver_3 = 0 ,float64 _C_fraction_transferred_to_3 = 0.0);
 public: //accessors
   inline virtual float64 get_area_per_mass()     const {return area_per_mass; }
   inline virtual float64 set_area_per_mass(float64 new_area_per_mass)
      { area_per_mass = new_area_per_mass; return area_per_mass; }               //070707
 public:
   bool update_net_N_mineralization();
   virtual float64 actual_C_decomposition
      (float64 &N_deficit_for_immobilization
      ,float64 &decomposition_reduction_factor);
 public:
   class Decomposition
   {
    protected:
      const Organic_biomatter_pool_common &pool;                                 //110930 do same multiple
    private:
      float minimized_temperature_moisture_function;  // VB variable was Term  Does not apply to residue
    public: // was protected:
      bool     is_a_residue;
      //This is the daily decomposition
      float64 pot_carbon;    // mass
      float64 act_carbon;    //
      //190311 now permanent #ifdef CORRECTED_SOM_DECOMP
      float64 free_N;                                                            //190212
      float64 assimilate_N_pot;                                                  //190212
      //190311 now permanent       #else
      //190311 now permanent       #endif
      float64 net_N_mineralization;
    protected:
      float64 tillage_decomposition_rate_adj;          // VB variable was Tillage_Decomposition_Rate_Adjustment_Factor
    public: // I should make accessors for these
      float64 N_immobilization_demand;
      float64 N_mineralization;
    public:
      Decomposition
         (const Organic_biomatter_pool_common &_pool                             //110930
         ,float64 _tillage_decomposition_rate_adj
         ,float64 _minimized_temperature_moisture_function
         ,bool    _is_a_residue);
      inline virtual ~Decomposition() {}                                         //170217
      virtual float64 potential();
      virtual float64 actual(float64 N_deficit_for_immobilization
                            ,float64 decomposition_reduction_factor);
      virtual float64 N_mineralization_or_immobilization_demand
         (float64 carbon_nitrogen_ratio)                                     =0; //110930
      // Returns the N immobilization demand
    #ifdef KEMANIAN_HUMIFICATION
    public:
      inline virtual float64 get_humified_carbon_nitrogen_ratio() const { return 1.0; }
         //111003 not applicable to SOM
    #endif
   };
   //___________________________________________________________Decomposition__/
 public: //accessors
   virtual       Decomposition *mod_decomposition()                         = 0;
   virtual const Decomposition *ref_decomposition()                     const=0;
   virtual Decomposition *provide_decomposition_residue                            //111003
      (float64 moisture_function
      ,float64 temperature_function)                               provision_=0;
   virtual Decomposition *provide_decomposition_SOM
      (float64 tillage_decomposition_rate_adj
      ,float64 minimized_temperature_moisture_function)            provision_=0;
   virtual float64 potential_C_decomposition
      (float64 &N_mineralization_sum
      ,float64 &N_immobilization_demand_sum
      ,float64 tillage_decomposition_rate_adj);                                  //060807
   inline virtual float64 get_C_decomposed_for_output_only()               const { return C_decomposed_for_output_only;}  //090520
   inline virtual Organic_matter_cycling  get_carbon_cycling()             const { return carbon_cycling; }      //070707
   inline virtual float64 get_contact_fraction()                           const { return *contact_fraction; }   //070707
   inline virtual RUSLE2_Residue_type     get_RUSLE2_residue_type()        const { return RUSLE2_residue_type; } //070707
   inline virtual RUSLE2_Residue_type     set_RUSLE2_residue_type(RUSLE2_Residue_type new_RUSLE2_residue_type)           //070707
      {RUSLE2_residue_type=new_RUSLE2_residue_type;return RUSLE2_residue_type;}
   void set_standing_stubble();                                                  //060530
   // set_standing_stubble is used by decomposing canopy                         //060530
   virtual Organic_matter_position set_position(Organic_matter_position new_position) modification_;  //090413
   //190327 virtual float64 set_contact_fraction(float64 &_contact_fraction) modification_;
   virtual float64 &know_contact_fraction(float64 &_contact_fraction) cognition_;
   virtual float64 reset_mass(float64 new_mass)                     modification_;//070707
   #ifdef KEMANIAN_HUMIFICATION
   virtual float64 get_humified_carbon_nitrogen_ratio(nat8 receiver_index) const { return 1.0; }
   #endif
   virtual float64 get_N_mineralization_mass()                            const; //111004_061116
      // Returns the actual N mass mineralized today
   virtual float64 get_N_immobilization_demand_mass()                     const; //111004_061116
      // Returns the actual N mass immobilized today
 public: //process
   virtual float64 transfer_carbon_and_nitrogen()                            =0; //111004
   virtual float64 post_carbon_transfer()                       modification_=0; //111003
      // Returns the (actual) net carbon transfer.
      // (the return value is informational, not currently used anywhere).

 public: // other
   // this was added so we can store historical equilibrated OM for futher future scenario runs //120912
   virtual void write_header (std::ostream &stream)                  stream_IO_; //120912
   virtual bool write        (std::ostream &stream)                  stream_IO_; //120912
   virtual std::istream &read(std::istream &stream)                  stream_IO_; //120912
};
//_Organic_biomatter_pool_common____________________________________2011-09-29_/
#endif


