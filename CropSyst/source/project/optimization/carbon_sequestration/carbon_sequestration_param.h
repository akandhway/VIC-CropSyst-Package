#ifndef CARBON_SEQUESTRATION_PARAMETERS_H
#define CARBON_SEQUESTRATION_PARAMETERS_H

#include "corn/validate/validtyp.h"
#include "common/simulation/event_types.h"

namespace CORN {class Data_record; };
//______________________________________________________________________________
class Carbon_sequestration_optimization_parameters
{
public:
   float32  pasture_size_ha;
   int16    max_plot_count;
   int16    step_plot_count;
   int16    min_herd_size;
   int16    max_herd_size;
   int16    step_herd_size;
   int16    begin_grazing_after_emergence_days;
   int16    max_duration; // the maximum number of days to try keeping the animals on a field
   int16    step_duration;
   int16    grazing_season_duration;
   int16    max_recovery_days;   // this it the maximum number of days that the crop needs to recover (used to limit rotations to practical periods)
   float32  dry_matter_requirements_animal_units;
   float32  reserve_biomass_kg_ha;
   int16    biomass_return_to_field;  // actually simply the percent of material
   int16    grazing_percent_N_as_OM;
   int16    grazing_percent_N_as_NH3;
   Contingency_clad contingency_clad;
   bool     trampling;

public:
   CORN::Valid_float32  v_pasture_size;
   CORN::Valid_int16    v_max_plot_count;
   CORN::Valid_int16    v_step_plot_count;
   CORN::Valid_int16    v_min_herd_size;
   CORN::Valid_int16    v_max_herd_size;
   CORN::Valid_int16    v_step_herd_size;
   CORN::Valid_int16    v_begin_grazing_after_emergence_days;
   CORN::Valid_int16    v_max_duration; // the number of days the animals
   CORN::Valid_int16    v_step_duration;
   CORN::Valid_int16    v_grazing_season_duration;
   CORN::Valid_int16    v_max_recovery_days;
   CORN::Valid_float32  v_dry_matter_requirements_animal_units;
   CORN::Valid_float32  v_reserve_biomass;
   CORN::Valid_int16    v_biomass_return_to_field;
   CORN::Valid_int16    v_grazing_percent_N_as_OM;
   CORN::Valid_int16    v_grazing_percent_N_as_NH3;
public:
   Carbon_sequestration_optimization_parameters();
   virtual bool setup_structure(CORN::Data_record &data_rec,bool for_write) modification_;
   int16 calc_scenarios_required()                                         const;
   void compose_scenario_name(std::string &scenario_name,int16 herd_size,int16 plots,int16 days_on_field,int16 plot) const;
};
//______________________________________________________________________________
#endif
