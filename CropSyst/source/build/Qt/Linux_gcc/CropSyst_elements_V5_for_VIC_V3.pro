#-------------------------------------------------
#
# Project created by QtCreator 2012-04-04T06:46:28
#
#-------------------------------------------------

QT       -= gui

TARGET = CropSyst_element_V5_for_VIC_V3
CONFIG += staticlib
DEFINES  += CS_VERSION=5 \
            CROPSYST_VERSION=5 \
            WEATHER_PROVIDER_VERSION=5 \
            VIC_CROPSYST_VERSION=3 \
            EVENT_SCHEDULER_VERSION=1 \
            CROP_ORCHARD \
            CROPSYST_HANDLE_CANOPY_EVAP \
            LIU_DEBUG
INCLUDEPATH += ../../../../.. ../../.. ../.. ../../../../../common ../../../../../USDA/NRCS  ../../../../../../dev/corn
QMAKE_CXXFLAGS += -fpermissive -O0
QMAKE_CXXFLAGS += -Wno-reorder -Wno-unused-parameter -Wno-unused-but-set-parameter
QMAKE_CFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-write-strings -Wno-sign-compare  -Wno-unused-value
TEMPLATE = lib


SOURCES += \
    ../../../static_phrases.cpp \
    ../../../soil.cpp \
    ../../../seeding_param.cpp \
    ../../../rprtclss.cpp \
    ../../../rot_param.cpp \
    ../../../pond.cpp \
    ../../../perform_op.cpp \
    ../../../N_sources.cpp \
    ../../../model_options.cpp \
    ../../../mgmt_types.cpp \
    ../../../inorganic_NH4.cpp \
    ../../../cs_vars.cpp \
    ../../../cs_scenario.cpp \
    ../../../cs_operation_codes.cpp \
    ../../../cs_operation.cpp \
    ../../../cs_mgmt.cpp \
    ../../../cs_event.cpp \
    ../../../cs_ET.cpp \
    ../../../soil/abiotic_environment.cpp \
    ../../../soil/tillage_effect.cpp \
    ../../../soil/temperature_profile.cpp \
    ../../../soil/temperature_hourly.cpp \
    ../../../soil/temperature_functions.cpp \
    ../../../soil/surface_temperature.cpp \
    ../../../soil/sublayers.cpp \
    ../../../soil/structure_I.cpp \
    ../../../soil/structure_common.cpp \
    ../../../soil/soil_param_struct.cpp \
    ../../../soil/soil_param_class.cpp \
    ../../../soil/soil_param.cpp \
    ../../../soil/soil_I.cpp \
    ../../../soil/runoff.cpp \
    ../../../soil/nitrogen_profile.cpp \
    ../../../soil/nitrogen_common.cpp \
    ../../../soil/hydrology_cascade.cpp \
    ../../../soil/hydrology_A.cpp \
    ../../../soil/hydraulic_properties.cpp \
    ../../../soil/freezing_jumakis.cpp \
    ../../../soil/dynamic_water_entering.cpp \
    ../../../soil/disturbance.cpp \
    ../../../soil/chemicals_profile.cpp \
    ../../../soil/chemical_profile.cpp \
    ../../../soil/chemical_mass_profile.cpp \
    ../../../soil/chemical_balance.cpp \
    ../../../crop/transpiration_interface.cpp \
    ../../../crop/growth_stages.cpp \
    ../../../crop/crop_types.cpp \
    ../../../crop/crop_root.cpp \
    ../../../crop/crop_param_struct.cpp \
    ../../../crop/crop_param_class.cpp \
    ../../../crop/crop_emergence.cpp \
    ../../../crop/crop_cropsyst.cpp \
    ../../../crop/crop_common.cpp \
    ../../../crop/CO2_response.cpp \
    ../../../crop/canopy_growth_portioned.cpp \
    ../../../crop/canopy_growth_LAI_based.cpp \
    ../../../crop/canopy_growth_cover_based.cpp \
    ../../../crop/canopy_cover_continuum.cpp \
    ../../../../../USDA/NRCS/soil/survey/domains.cpp \
    ../../../../../USDA/NRCS/RUSLE2/SDR_field_ops.cpp \
    ../../../../../common/soil/layering.cpp \
    ../../../../../common/residue/residues_cycling.cpp \
    ../../../../../common/residue/residues_common.cpp \
    ../../../../../common/residue/residue_V4_common.cpp \
    ../../../../../common/residue/residue_decomposition_param.cpp \
    ../../../../../common/residue/residue_decomposition_limitations.cpp \
    ../../../../../common/residue/residue_const.cpp \
    ../../../../../common/residue/residue_biomatter_profile_simple.cpp \
    ../../../../../common/residue/residue_balancer.cpp \
    ../../../../../common/biomatter/manure_form.cpp \
    ../../../../../common/biomatter/biomass_decomposition_param.cpp \
    ../../../../../common/biomatter/biomass_abstract.cpp \
    ../../../../../common/biomatter/biomass.cpp \
    ../../../../../common/environment/environment_base.cpp \
    ../../../../../common/physics/property.cpp \
    ../../../../../common/physics/mass_common.cpp \
    ../../../../../common/physics/mass.cpp \
    ../../../../../common/geodesy/geolocation.cpp \
    ../../../../../common/geodesy/geocoordinate.cpp \
    ../../../../../common/soil/SCS/SCS.cpp \
    ../../../watertab.cpp \
    ../../../organic_matter/OM_residues_profile_common.cpp \
    ../../../organic_matter/OM_residues_profile_abstract.cpp \
    ../../../organic_matter/OM_pools_common.cpp \
    ../../../organic_matter/OM_types.cpp \
    ../../../organic_matter/OM_params.cpp \
    ../../../organic_matter/OM_common.cpp \
    ../../../organic_matter/multiple_pool/OM_pools_multiple.cpp \
    ../../../organic_matter/multiple_pool/OM_multiple_pool.cpp \
    ../../../../../common/soil/USDA_texture.cpp \
    ../../../../../common/soil/txtrhydr.cpp \
    ../../../../../common/soil/texttri.cpp \
    ../../../../../common/soil/texture.cpp \
    ../../../organic_matter/single_pool/OM_pools_single.cpp \
    ../../../organic_matter/single_pool/OM_single_pool.cpp \
    ../../../management/management_param_struct.cpp \
    ../../../management/management_param_class.cpp \
    #../../../crop/extreme_temperature_response.cpp \
    ../../../../../common/weather/database/weather_UED_database.cpp \
    ../../../crop/canopy_cover_curve.cpp \
    ../../../crop/canopy_growth_LAI_based_V5.cpp \
    ../../../crop/crop_root_V5.cpp \
    ../../../crop/canopy_growth_LAI_based_V4.cpp \
    ../../../soil/layers.cpp \
    ../../../soil/soil_param_V5.cpp \
    ../../../../../common/physics/property/P_vapor_pressure.cpp \
    ../../../../../common/simulation/synchronization.cpp \
    ../../../../../common/simulation/scenario.cpp \
    ../../../../../common/simulation/operation.cpp \
    ../../../../../common/simulation/log.cpp \
    ../../../../../common/simulation/event_types.cpp \
    ../../../../../common/simulation/event.cpp \
    ../../../../../common/simulation/event_status.cpp \
    ../../../../../common/simulation/balancer.cpp \
    ../../../../../CS_suite/simulation/CS_simulation_unit.cpp \
    ../../../../../CS_suite/simulation/CS_simulation_element.cpp \
    ../../../../../CS_suite/simulation/CS_simulation_control.cpp \
    ../../../../../CS_suite/simulation/CS_event_scheduler.cpp \
    ../../../cs_accum.cpp \
    ../../../cs_vars_abbrv.cpp \
    ../../../friction_velocity.cpp \
    ../../../soil/soil_properties_sublayers.cpp \
    ../../../soil/profile_texture.cpp \
    ../../../land_unit_sim.cpp \
    ../../../cs_scenario_directory.cpp \
    ../../../../../common/simulation/scenario_directory.cpp \
    ../../../soil/soil_base.cpp \
    ../../../crop/transpiration_dependent_growth_V4.cpp \
    ../../../../../CS_suite/observation/CS_emanator.cpp \
    ../../../../../CS_suite/observation/CS_inspector.cpp \
    ../../../crop/yield.cpp \
    ../../../management/management_param_V4.cpp \
    ../../../crop/transpiration.cpp \
    ../../../crop/thermal_time_common.cpp \
    ../../../crop/thermal_time_daily.cpp \
    ../../../crop/crop_N_common.cpp \
    ../../../crop/crop_N_V5.cpp \
    ../../../../../CS_suite/simulation/CS_identification.cpp \
    ../../../../../CS_suite/simulation/CS_land_unit_meteorological.cpp \
    ../../../CropSyst_inspection.cpp \
    ../../../crop/event_scheduler_crop.cpp \
    ../../../crop/transpiration_dependent_growth.cpp \
    ../../../crop/quiescence.cpp \
    #../../../arguments_CropSyst.cpp \
    ../../../crop/crop_param_V4.cpp \
    ../../../../../common/evaporator.cpp \
    ../../../soil/soil_evaporator.cpp \
    ../../../crop/crop_fruit.cpp \
    ../../../crop/crop_orchard_Kemanian_Stockle.cpp \
    ../../../crop/crop_orchard_common.cpp \
    ../../../crop/crop_orchard_Oyarzun_Stockle.cpp \
    ../../../../../corn/data_source/datarec.cpp \
    ../../../../../CS_suite/UED/database_file_substitution.cpp \
    ../../../crop/crop_element.cpp \
    ../../../crop/phenology_A.cpp \
    ../../../crop/yield_temperature_extreme_response.cpp \
    ../../../crop/yield_response.cpp \
    ../../../crop/phenology_2013.cpp \
    ../../../crop/biomass_growth_RUE_TUE.cpp

HEADERS += \
    ../../../../../corn/measure/common_units.h \
    ../../../soil/abiotic_environment.h \
    ../../../soil/chemical_balance.h \
    ../../../soil/chemical_mass_profile.h \
    ../../../soil/chemical_profile.h \
    ../../../soil/chemicals_profile.h \
    ../../../soil/disturbance.h \
    ../../../soil/dynamic_water_entering.h \
    ../../../soil/erosion_RUSLE.h \
    ../../../soil/evaporation_abstract.h \
    ../../../soil/evaporation_interface.h \
    ../../../soil/evaporation_vapor_path.h \
    ../../../soil/evaporation_without_vapor_path.h \
    ../../../soil/freezing_interface.h \
    ../../../soil/freezing_jumakis.h \
    ../../../soil/freezing_numerical.h \
    ../../../soil/hydraulic_properties.h \
    ../../../soil/hydrologic_param.h \
    ../../../soil/hydrology.h \
    ../../../soil/hydrology_cascade.h \
    ../../../soil/hydrology_finite_diff.h \
    ../../../soil/hydrology_interface.h \
    ../../../soil/infiltration.h \
    ../../../soil/infiltration_cascade_common.h \
    ../../../soil/infiltration_cascade_daily.h \
    ../../../soil/infiltration_cascade_hourly.h \
    ../../../soil/infiltration_darcian.h \
    ../../../soil/infiltration_finite_diff.h \
    ../../../soil/layers.h \
    ../../../soil/layers_interface.h \
    ../../../soil/nitrogen_common.h \
    ../../../soil/nitrogen_interface.h \
    ../../../soil/nitrogen_profile.h \
    ../../../soil/profile_texture.h \
    ../../../soil/runoff.h \
    ../../../soil/runoff_SCS.h \
    ../../../soil/salinity_profile.h \
    ../../../soil/salt_interface.h \
    ../../../soil/soil_abstract.h \
    ../../../soil/soil_base.h \
    ../../../soil/soil_interface.h \
    ../../../soil/soil_param.h \
    ../../../soil/soil_param_class.h \
    ../../../soil/soil_param_struct.h \
    ../../../soil/soil_param_V4.h \
    ../../../soil/soil_param_V5.h \
    ../../../soil/soil_param_with_STATSGO.h \
    ../../../soil/soil_properties_sublayers.h \
    ../../../soil/structure_common.h \
    ../../../soil/structure_interface.h \
    ../../../soil/sublayers.h \
    ../../../soil/surface_infiltration_green_ampt.h \
    ../../../soil/surface_temperature.h \
    ../../../soil/temperature_functions.h \
    ../../../soil/temperature_hourly.h \
    ../../../soil/temperature_hourly_interface.h \
    ../../../soil/temperature_interface.h \
    ../../../soil/temperature_profile.h \
    ../../../soil/texture_interface.h \
    ../../../soil/tillage_effect.h \
    ../../../../../common/simulation/synchronization.h
