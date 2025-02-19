#ifndef N_sourcesH
#define N_sourcesH

#include "corn/enum.h"
#define   LABEL_ammonium_source        "ammonium_source"
#define   LABEL_urea                   "urea"
#define   LABEL_urea_ammonium_nitrate  "urea_ammonium_nitrate"
#define   LABEL_ammonium_sulfate       "ammonium_sulfate"
#define   LABEL_ammonium_nitrate       "ammonium_nitrate"
#define   LABEL_anhydrous_ammonia      "anhydrous_ammonia"

#define LABEL_NH4_appl_method   "NH4_appl_method"
#define LABEL_surface_broadcast    "surface_broadcast"
#define LABEL_surface_dribble      "surface_dribble"
#define LABEL_incorporate          "incorporate"
#define LABEL_inject               "inject"

#define   LABEL_org_N_appl_method               "org_N_appl_method"
#define   LABEL_surface_broadcast_no_incorporation "surface_broadcast_no_incorporation"
#define   LABEL_surface_broadcast_incorporation    "surface_broadcast_incorporation"
#define   LABEL_knifed                             "knifed"
#define   LABEL_sprinkler                          "sprinkler"

// These residue source with be removed in V.5   (the user now creates organic matter / manure description files)
#define   LABEL_org_N_source           "org_N_source"
#define   LABEL_poultry_layers         "poultry_layers"
#define   LABEL_poultry_broilers_fresh "poultry_broilers_fresh"
#define   LABEL_poultry_broilers_aged  "poultry_broilers_aged"
#define   LABEL_swine                  "swine"
#define   LABEL_dairy_fresh            "dairy_fresh"
#define   LABEL_dairy_tank             "dairy_tank"
#define   LABEL_dairy_lagoon           "dairy_lagoon"
#define   LABEL_beef_fresh             "beef_fresh"
#define   LABEL_beef_aged_2_5          "beef_aged_2.5"
#define   LABEL_beef_aged_1_5          "beef_aged_1.5"
#define   LABEL_beef_aged_1_0          "beef_aged_1.0"

//_labels_____________________________________________________2002-03-22_1997?_/
enum  NH4_Appl_method
     {SURFACE_BROADCAST,SURFACE_DRIBBLE,INCORPORATE,INJECT};
extern const char *NH4_appl_method_label_table[];
DECLARE_ENUM_CLAD(NH4_Appl_method_clad,NH4_Appl_method,NH4_appl_method_label_table)
//_NH4_Appl_method____________________________________________2002-03-22_1997?_/
enum  Org_N_appl_method
     {SURFACE_BROADCAST_NO_INCORPORATION,SPRINKLER_IRRIGATED,SURFACE_BROADCAST_INCORPORATION,KNIFED};
extern const char *org_N_appl_method_label_table[];
DECLARE_ENUM_COWL(Org_N_appl_method_cowl,Org_N_appl_method,org_N_appl_method_label_table)
//_Org_N_appl_method__________________________________________2002-03-22_1997?_/
enum  Ammonium_source {UREA,UREA_AMMONIUM_NITRATE,AMMONIUM_SULFATE,
        AMMONIUM_NITRATE,ANHYDROUS_AMMONIA,AMMONIUM_NITRO_SULFATE};
extern const char *ammonium_source_label_table[];
DECLARE_ENUM_CLAD(Ammonium_source_clad,Ammonium_source,ammonium_source_label_table)
//_Ammonium_source_______________________________________2002-03-22_1997-07-09_/
#endif
