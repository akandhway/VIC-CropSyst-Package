#ifndef OM_pools_multipleH
#define OM_pools_multipleH

#include "organic_matter/OM_pools_common.h"
#include "organic_matter/multiple_pool/OM_multiple_pool.h"
//_forward declarations_________________________________________________________
class Organic_matter_initialization_parameters;
class Organic_matter_decomposition_parameters;
//______________________________________________________forward declarations___/
class Organic_matter_pools_multiple
: public Organic_matter_pools_common
{public:
   Organic_biomatter_pool_multiple  *microbial_biomass_ref[MAX_soil_layers];     // This is so that fast and slow cycling residue has access for contribute_to
   Organic_biomatter_pool_multiple  *metastable_SOM_ref[MAX_soil_layers];        // This is so lignified residue has access for contribute_to //060524
 public: // structors
   Organic_matter_pools_multiple
      (const Organic_matter_decomposition_parameters &OM_decomposition_params    //120912
      ,Soil::Abiotic_environment_profile         &soil_abiotic_env_profile       //060726
      ,const Soil::Layers_interface              &soil_layers                    //060726
      ,unmodifiable_ Soil::Structure             &soil_structure                 //070725
      ,Soil::Nitrogen                            *soil_N
      ,const Soil::Texture_interface             &soil_texture);                 //070707
 private:
   virtual Organic_biomatter_common *provide_organic_matter_pool                 //070707
      (Organic_matter_type type,nat8 layer)                          provision_; //190215
      // Always returns a valid pointer
   virtual Organic_biomatter_common  *render_OM_pool                             //111001
      (Organic_matter_type     type
      ,Organic_matter_position position
      ,nat8                    layer
      ,Organic_matter_cycling  cycling
      ,RUSLE2_Residue_type     RUSLE2_residue_type                               //060731
      ,float64                 biomass
      ,float64                 carbon_fraction
      ,float64                 carbon_nitrogen_ratio
      ,float64                 decomposition_constant                            //090703
      ,float64                 detrition_constant                                //090413
      ,float64                 area_per_mass      // = 0.0  stubble and surface residue_only?
      ,float64                &contact_fraction_                                 //190327
      ,const Soil::Texture_interface &soil_texture                               //070707
      ,const Soil::Abiotic_environment_layer &soil_abiotic_env_layer)rendition_;
   virtual Organic_biomatter_pool_common *render_OM_pool_for_read()  rendition_; //161116
   inline virtual modifiable_ Organic_biomatter_pool_common *get_metastable_SOM_ref(nat8 layer)    //110929
      { return metastable_SOM_ref[layer]; }
   inline virtual modifiable_ Organic_biomatter_pool_common
      *get_microbial_biomass_ref(nat8 layer)                                     //110929
      { return microbial_biomass_ref[layer]; }
 public:
   inline virtual nat8 SOM_index_last()                                    const //170301
      { return (ORGANIC_MATTER_TYPE_COUNT-1); }
   virtual bool initialize_layer
      (nat8 horizon,nat8 layer
      ,float64 sand_fraction                                                     //060418
      ,float64 initial_organic_matter_carbon_mass
      ,float64 microbial_decomposition_const
      ,const Organic_matter_initialization_parameters &OM_historical_parameters  //060215
      ,const Soil::Abiotic_environment_layer &ref_soil_abiotic_environment_layer);
 protected:
   void calculate_C_contributions
      (float64 sand_fraction
      ,float64 &C_Fraction_From_MB_To_MetastableSOM
      ,float64 &C_Fraction_From_MB_To_LabileSOM
      ,float64 &C_Fraction_From_LabileSOM_To_PassiveSOM
      ,float64 &C_Fraction_From_LabileSOM_To_CO2
      ,float64 &C_Fraction_From_LabileSOM_To_MB
      ,float64 &C_Fraction_From_MetastableSOM_To_PassiveSOM
      ,float64 &C_Fraction_From_MetastableSOM_To_CO2
      ,float64 &C_Fraction_From_MetastableSOM_To_MB
      ,float64 &C_Fraction_From_PassiveSOM_To_CO2
      ,float64 &C_Fraction_From_PassiveSOM_To_MB);
      // this was added so we can store historical equilibrated OM
      // for futher future scenario runs                                         //120912
   virtual nat32 write(std::ostream &)                               stream_IO_; //150728_120912
   virtual bool specialized_reference                                            //170228
      (Organic_biomatter_pool_common *OM_pool_common);
};
//_Organic_matter_pools_multiple_______________________________________________/
#endif

