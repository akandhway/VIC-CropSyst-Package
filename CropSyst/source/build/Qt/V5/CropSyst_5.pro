#-------------------------------------------------
#
# Project created by QtCreator 2012-04-04T06:46:28
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = CropSyst_5
CONFIG   += console
CONFIG   -= app_bundle
DEFINES  += CROPSYST CS_VERSION=5 CROPSYST_VERSION=5 CROPSYST_PROPER=5 USE_CS_SIMULATION_ROTATOR CROP_PARAM_ADJUST OLD_PHENOLOGY_SEQUENCER CO2_V5 OLD_ORCHARD APPTYPE_CONSOLE CS_INSPECTOR_VERSION=5 UED_OUTPUT EXTENDS_LAND_UNIT_OUTPUT
INCLUDEPATH += ../../../../.. ../../.. ../.. ../../../../../common ../../../../../USDA/NRCS
unix:LIBS += -L/home/rnelson/dev/corn/build/Qt/corn_complete_static-build-desktop-Desktop_Qt_4_8_0_for_GCC__Qt_SDK__Release -lcorn_complete_static
QMAKE_CXXFLAGS  += -fpermissive -Wno-unused-parameter -Wno-reorder  -Wno-unused-but-set-variable -Wno-write-strings -Wno-missing-field-initializers -Wno-unknown-pragmas -g
# -Wno-used-but-set-parameter

TEMPLATE = app


SOURCES += \
    ../../../arguments_CropSyst.cpp \
    ../../../static_phrases.cpp \
    ../../../soil.cpp \
    ../../../sim_param.cpp \
    ../../../seeding_param.cpp \
    ../../../rot_param.cpp \
    ../../../project_types.cpp \
    ../../../pond.cpp \
    ../../../perform_op.cpp \
    ../../../output.cpp \
    ../../../organicn.cpp \
    ../../../N_sources.cpp \
    ../../../model_options.cpp \
    ../../../mgmt_types.cpp \
    ../../../land_unit_sim.cpp \
    ../../../land_unit_output.cpp \
    # ../../../land_unit_output_V4.cpp \
    ../../../inorganic_NH4.cpp \
    ../../../friction_velocity.cpp \
    ../../../fastgraph_vars.cpp \
    ../../../watertab.cpp \
    ../../../CropSyst_inspection.cpp \
    ../../../csprofss.cpp \
    ../../../cs_vars_abbrv.cpp \
    ../../../cs_vars.cpp \
    ../../../cs_UED_season.cpp \
    ../../../cs_UED_harvest.cpp \
    ../../../cs_UED_db.cpp \
    ../../../CS_UED_datasrc.cpp \
    ../../../cs_UED_daily.cpp \
    ../../../cs_UED_annual.cpp \
    ../../../cs_till.cpp \
    ../../../cs_scenario_directory.cpp \
    ../../../cs_scenario.cpp \
    ../../../cs_results.cpp \
    ../../../cs_project.cpp \
    ../../../cs_operation_codes.cpp \
    ../../../cs_operation.cpp \
    ../../../cs_mgmt.cpp \
    ../../../cs_events.cpp \
    ../../../cs_event.cpp \
    ../../../cs_ET.cpp \
    ../../../cs_datarec.cpp \
    ../../../cs_chem_pot_mineral_N.cpp \
    ../../../cs_accum.cpp \
    ../../../soil/abiotic_environment.cpp \
    ../../../soil/chemicals_profile.cpp \
    ../../../soil/chemical_profile.cpp \
    ../../../soil/chemical_mass_profile.cpp \
    ../../../soil/chemical_balance.cpp \
    ../../../soil/disturbance.cpp \
    ../../../soil/dynamic_water_entering.cpp \
    ../../../soil/erosion_RUSLE.cpp \
    ../../../soil/evaporation_without_vapor_path.cpp \
    ../../../soil/evaporation_interface.cpp \
    ../../../soil/freezing_numerical.cpp \
    ../../../soil/freezing_jumakis.cpp \
    ../../../soil/infiltration.cpp \
    ../../../soil/infiltration_cascade_hourly.cpp \
    ../../../soil/infiltration_cascade_daily.cpp \
    ../../../soil/infiltration_cascade_common.cpp \
    ../../../soil/infiltration_finite_diff.cpp \
    ../../../soil/hydrology.cpp \
    ../../../soil/hydrology_finite_diff.cpp \
    ../../../soil/hydrology_cascade.cpp \
    ../../../soil/hydraulic_properties.cpp \
    ../../../soil/layers.cpp \
    ../../../soil/nitrogen_profile.cpp \
    ../../../soil/nitrogen_common.cpp \
    ../../../soil/profile_texture.cpp \
    ../../../soil/runoff.cpp \
    ../../../soil/runoff_SCS.cpp \
    ../../../soil/salinity_profile.cpp \
    ../../../soil/soil_param_struct.cpp \
    ../../../soil/soil_param_class.cpp \
    ../../../soil/soil_param.cpp \
    ../../../soil/soil_interface.cpp \
    ../../../soil/soil_base.cpp \
    ../../../soil/structure_interface.cpp \
    ../../../soil/structure_common.cpp \
    ../../../soil/surface_infiltration_green_ampt.cpp \
    ../../../soil/surface_temperature.cpp \
    ../../../soil/sublayers.cpp \
    ../../../soil/temperature_profile.cpp \
    ../../../soil/temperature_interface.cpp \
    ../../../soil/temperature_hourly.cpp \
    ../../../soil/temperature_functions.cpp \
    ../../../soil/texture_interface.cpp \
    ../../../soil/tillage_effect.cpp \
    ../../../crop/canopy_growth.cpp \
    ../../../crop/canopy_growth_portioned.cpp \
    ../../../crop/canopy_growth_LAI_based.cpp \
    ../../../crop/canopy_growth_cover_based.cpp \
    #../../../crop/canopy_growth_LAI_based_V4.cpp \
    ../../../crop/canopy_growth_LAI_based_V5.cpp \
    ../../../crop/canopy_cover_continuum.cpp \
    ../../../crop/canopy_cover_curve.cpp \
    ../../../crop/crop_biomass.cpp \
    ../../../crop/crop_common.cpp \
    ../../../crop/crop_cropsyst.cpp \
    ../../../crop/crop_emergence.cpp \
    ../../../crop/crop_fruit.cpp \
    ../../../crop/crop_interfaced_cropsyst.cpp \
    ../../../crop/crop_interfaced.cpp \
    ../../../crop/crop_orchard_common.cpp \
    ../../../crop/crop_orchard_Kemanian_Stockle.cpp \
    ../../../crop/crop_orchard_Oyarzun_Stockle.cpp \
    ../../../crop/crop_N_balancer.cpp \
    ../../../crop/crop_N_common.cpp \
    ../../../crop/crop_N_V5.cpp \
    # ../../../crop/crop_N_V4.cpp \
    ../../../crop/crop_param_V4.cpp \
    ../../../crop/crop_param_struct.cpp \
    ../../../crop/crop_param_class.cpp \
    ../../../crop/crop_root.cpp \
    # ../../../crop/crop_root_V4.cpp \
    ../../../crop/crop_root_V5.cpp \
    ../../../crop/crop_types.cpp \
    ../../../crop/extreme_temperature_response.cpp \
    ../../../crop/growth_stages.cpp \
    ../../../crop/phenology_interface.cpp \
    ../../../crop/phenology_common.cpp \
    ../../../crop/transpiration.cpp \
    ../../../crop/transpiration_interface.cpp \
    ../../../crop/transpiration_dependent_growth_V5.cpp \
    ../../../crop/thermal_time_common.cpp \
    ../../../crop/thermal_time_daily.cpp \
    ../../../crop/quiescence.cpp \
    ../../../crop/event_scheduler_crop.cpp \
    ../../../crop/yield.cpp \
    ../../../organic_matter/OM_residues_profile_common.cpp \
    ../../../organic_matter/OM_residues_profile_abstract.cpp \
    ../../../organic_matter/OM_pools_common.cpp \
    ../../../organic_matter/OM_types.cpp \
    ../../../organic_matter/OM_params.cpp \
    ../../../organic_matter/OM_common.cpp \
    ../../../organic_matter/multiple_pool/OM_pools_multiple.cpp \
    ../../../organic_matter/multiple_pool/OM_multiple_pool.cpp \
    ../../../organic_matter/simple/OM_simple.cpp \
    ../../../organic_matter/single_pool/OM_pools_single.cpp \
    ../../../organic_matter/single_pool/OM_single_pool.cpp \
    ../../../project/GIS/GIS_table.cpp \
    ../../../project/GIS/pat.cpp \
    ../../../management/management_param_struct.cpp \
    ../../../management/management_param_class.cpp \
    ../../../management/management_param_V4.cpp \
    ../../../../../common/soil/layering.cpp \
    ../../../../../common/residue/residues_cycling.cpp \
    ../../../../../common/residue/residues_common.cpp \
        ../../../../../common/residue/residue_V4_common.cpp \
    ../../../../../common/residue/residue_pools_simple.cpp \
    ../../../../../common/residue/residue_pool_simple.cpp \
    ../../../../../common/residue/residue_decomposition_param.cpp \
    ../../../../../common/residue/residue_decomposition_limitations.cpp \
    ../../../../../common/residue/residue_const.cpp \
    ../../../../../common/residue/residue_biomatter_profile_simple.cpp \
    ../../../../../common/residue/residue_balancer.cpp \
    ../../../../../common/biomatter/manure_form.cpp \
    ../../../../../common/biomatter/biomass_decomposition_param.cpp \
    ../../../../../common/biomatter/organic_biomatter_balance.cpp \
    ../../../../../common/biomatter/biomass_abstract.cpp \
    ../../../../../common/biomatter/biomass.cpp \
    ../../../../../common/environment/environment_base.cpp \
    ../../../../../common/physics/property.cpp \
    ../../../../../common/physics/mass_common.cpp \
    ../../../../../common/physics/mass.cpp \
    ../../../../../common/geodesy/geolocation.cpp \
    ../../../../../common/geodesy/geocoordinate.cpp \
    ../../../../../common/soil/SCS/SCS.cpp \
    ../../../../../common/soil/USDA_texture.cpp \
    ../../../../../common/soil/txtrhydr.cpp \
    ../../../../../common/soil/texttri.cpp \
    ../../../../../common/soil/texture.cpp \
    ../../../../../common/evaporator.cpp \
    ../../../../../common/physics/water_depth.cpp \
    ../../../../../common/weather/temperature_amplitude_phase_calibrator.cpp \
    ../../../../../UED/convert/ED_tabular_format.cpp \
    ../../../../../USDA/NRCS/soil/survey/domains.cpp \
    ../../../../../USDA/NRCS/RUSLE2/SDR_field_ops.cpp \
    ../../../../../corn/format/binary/oldbinrec/oldbinrec_s.cpp \
    ../../../../../common/weather/subdaily.cpp \
    ../../../CropSyst_main_V5.cpp \
    ../../../soil/soil_evaporator.cpp \
    ../../../../../common/weather/weather_types.cpp \
    ../../../CropSyst_engine_V5.cpp \
    ../../../crop/CO2_response.cpp \
    ../../../../../CS_suite/simulation/CS_simulation_rotator.cpp \
    ../../../crop/transpiration_dependent_growth_V4.cpp \
    ../../../crop/transpiration_dependent_growth.cpp \
    ../../../rotation.cpp \
    ../../../../../corn/float16.cpp \
    ../../../../../corn/math/random.cpp \
    ../../../../../corn/application/explaination/explaination_location.cpp \
    ../../../../../corn/application/explaination/explainations.cpp \
    ../../../../../common/simulation/log.cpp \
    ../../../land_unit_with_output.cpp \
    ../../../soil/soil_param_V5.cpp \
    ../../../../../CS_suite/observation/CS_inspection.cpp \
    ../../../../../corn/application/arguments.cpp \
    ../../../../../corn/data_source/datasrc.cpp \
    ../../../../../UED/library/UED_tuple_datasrc.cpp \
    ../../../../../CS_suite/UED/database_file_substitution.cpp \
    ../../../../../common/biometeorology/ET_reference.cpp \
    ../../../../../CS_suite/file_system/CS_context_discovery_common.cpp \
    ../../../../../CS_suite/simulation/CS_simulation_engine.cpp \
    ../../../../../corn/chronometry/time_conversion.cpp \
    ../../../../../common/biometeorology/ET_reference_Priestley_Taylor.cpp


unix|win32: LIBS += -L$$PWD/../../../../../CS_suite/build/Qt/CS_suite_static/bin/Release/ -lCS_suite_static
INCLUDEPATH += $$PWD/../../../../../CS_suite/build/Qt/CS_suite_static/bin/Release
DEPENDPATH += $$PWD/../../../../../CS_suite/build/Qt/CS_suite_static/bin/Release
win32:!win32-g++:    PRE_TARGETDEPS += $$PWD/../../../../../CS_suite/build/Qt/CS_suite_static/bin/Release/CS_suite_static.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../../../../../CS_suite/build/Qt/CS_suite_static/bin/Release/libCS_suite_static.a

unix|win32: LIBS += -L$$PWD/../../../../../common/weather/build/Qt/weather_static/bin/Release/ -lweather_static
INCLUDEPATH += $$PWD/../../../../../common/weather/build/Qt/weather_static/bin/Release
DEPENDPATH += $$PWD/../../../../../common/weather/build/Qt/weather_static/bin/Release
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../../../../common/weather/build/Qt/weather_static/bin/Release/weather_static.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../../../../../common/weather/build/Qt/weather_static/bin/Release/libweather_static.a

unix|win32: LIBS += -L$$PWD/../../../../../common/simulation/build/Qt/common_sim_static/bin/Release/ -lcommon_sim_static
INCLUDEPATH += $$PWD/../../../../../common/simulation/build/Qt/common_sim_static/bin/Release
DEPENDPATH += $$PWD/../../../../../common/simulation/build/Qt/common_sim_static/bin/Release
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../../../../common/simulation/build/Qt/common_sim_static/bin/Release/common_sim_static.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../../../../../common/simulation/build/Qt/common_sim_static/bin/Release/libcommon_sim_static.a

unix|win32: LIBS += -L$$PWD/../../../../../corn/build/Qt/corn_complete_static/bin/Release/ -lcorn_complete_static
INCLUDEPATH += $$PWD/../../../../../corn/build/Qt/corn_complete_static/bin/Release
DEPENDPATH += $$PWD/../../../../../corn/build/Qt/corn_complete_static/bin/Release
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../../../../corn/build/Qt/corn_complete_static/bin/Release/corn_complete_static.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../../../../../corn/build/Qt/corn_complete_static/bin/Release/libcorn_complete_static.a

unix|win32: LIBS += -L$$PWD/../../../../../UED/library/build/Qt/UED_static/bin/Release/ -lUED_static
INCLUDEPATH += $$PWD/../../../../../UED/library/build/Qt/UED_static/bin/Release
DEPENDPATH += $$PWD/../../../../../UED/library/build/Qt/UED_static/bin/Release
win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../../../../UED/library/build/Qt/UED_static/bin/Release/UED_static.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../../../../../UED/library/build/Qt/UED_static/bin/Release/libUED_static.a
