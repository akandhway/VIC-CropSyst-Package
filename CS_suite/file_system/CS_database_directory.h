#ifndef CS_database_directoryH
#define CS_database_directoryH
// Note that this is replacing CropSyst version4 database_dir
#ifndef temp_disabled
#include "corn/OS/file_system_logical.h"
#endif

namespace CS
{
//______________________________________________________________________________
class Database_directory_context
{
private: // eventually may use CORN::OS::File_system::Directory for these instead of Directory names
   provided_ CORN::OS::Directory_name *crop_directory;
   provided_ CORN::OS::Directory_name *output_directory;
   provided_ CORN::OS::Directory_name *soil_directory;
   provided_ CORN::OS::Directory_name *weather_directory;
   provided_ CORN::OS::Directory_name *weather_scenarios_directory;              //140821
   provided_ CORN::OS::Directory_name *management_directory;
   provided_ CORN::OS::Directory_name *biomatter_directory;
   provided_ CORN::OS::Directory_name *rotation_directory;
   provided_ CORN::OS::Directory_name *format_directory;
   provided_ CORN::OS::Directory_name *recalibration_directory;
   provided_ CORN::OS::Directory_name *water_table_directory;
   provided_ CORN::OS::Directory_name *initialization_directory;

   //160919 These component initialization directories will be obsolete
   // now using the new initialization composition mechanism
   provided_ CORN::OS::Directory_name *initialization_H2O_directory;
   provided_ CORN::OS::Directory_name *initialization_NO3_directory;
   provided_ CORN::OS::Directory_name *initialization_NH4_directory;
   provided_ CORN::OS::Directory_name *initialization_OM_directory;

   provided_ CORN::OS::Directory_name *STATSGO_states_directory;
public:
   Database_directory_context();
   virtual void reset()                                              provision_;
   virtual ~Database_directory_context();
protected:
/*NYI
    Smart_directory_name *provide_XXXX_directory                  (const char *subdir_name,bool create_if_doesnt_exist)provision_;
    Smart_directory_name *provide_initialization_XXXX_directory   (const char *subdir_name,bool create_if_doesnt_exist)provision_;
*/
   virtual const CORN::OS::Directory_name &get_dir_DEN()               const=0;  //160902
public:
   // will append drive separator if necessary (may be with or without colon :)
   // Note the following methods are capitalized to be consistent with
   // name of the directory being provided
   const CORN::OS::Directory_name &Crop()                            provision_;
   const CORN::OS::Directory_name &Output()                          provision_;
   const CORN::OS::Directory_name &Soil()                            provision_;
   const CORN::OS::Directory_name &Weather()                         provision_;
   const CORN::OS::Directory_name &Weather_Scenarios()               provision_; //140821
   const CORN::OS::Directory_name &Management()                      provision_;
   const CORN::OS::Directory_name &Biomatter()                       provision_;
   const CORN::OS::Directory_name &Rotation()                        provision_;
   const CORN::OS::Directory_name &Format()                          provision_;
   const CORN::OS::Directory_name &Recalibration()                   provision_;
   const CORN::OS::Directory_name &Water_table()                     provision_;
   const CORN::OS::Directory_name &Initialization()                  provision_;
   #ifdef NYI
   // The following are subdirectories of initialization
   const CORN::OS::Directory_name &Initialization_H2O()              provision_;
   const CORN::OS::Directory_name &Initialization_NO3()              provision_;
   const CORN::OS::Directory_name &Initialization_NH4()              provision_;
   const CORN::OS::Directory_name &Initialization_OM()               provision_;
   #endif
   #if (CS_VERSION == 4)
   const CORN::OS::Directory_name &STATSGO_states()                  provision_;
   #endif
   #if (CS_VERSION == 5)
   const CORN::OS::Directory_name &STATSGO2()                        provision_;
   #endif
};
//_Database_directory_context_______________________________________2016-09-02_/
class Database_directory
: public extends_ Database_directory_context
, public implements_ CORN::Item
{
 public: // should be private with accessor
   const CORN::OS::Directory_name *name; bool name_owned;
 public:
   inline Database_directory(CORN::OS::Directory_name *name_given)
      : Database_directory_context()
      , name(name_given)
      , name_owned(true)
      { }
   inline Database_directory(const CORN::OS::Directory_name *name_known)
      : Database_directory_context()
      , name(name_known)
      , name_owned(false)
      { }
   inline Database_directory(const CORN::OS::Directory_name &name_reference)
      : Database_directory_context()
      , name(&name_reference)
      , name_owned(false)
      { }
   virtual ~Database_directory()
      { if (name_owned) delete name;
      }
 public:
   inline virtual const CORN::OS::Directory_name &get_dir_DEN()            const //160902 get get_dir_name()
      {  return *name; }
};
//_Database_directory_______________________________________________2016-09-02_/
class Database_directory_logical  // deprecated
: public extends_ Database_directory_context
, public extends_ CORN::OS::File_system_logical::Directory
{
   // This is logical file system from of  Database_directory
   // It was previously called simply Database_directory
   // I have decided to drop using the logical filesystem for CS suite
   // because it requires setting up a File_system object
 public:
   inline Database_directory_logical
      (const CORN::OS::File_system::Directory
         &simulation_project_or_scenario_dir_parent_dir
      ,bool create_now_if_needed                                                 //130126
      ,const wchar_t *database_name=L"Database")
      : CORN::OS::File_system_logical::Directory
        (simulation_project_or_scenario_dir_parent_dir
        ,database_name,create_now_if_needed)
      {}
 protected:
   inline virtual const CORN::OS::Directory_name &get_dir_DEN()            const //160902
      {  return *this; }
};
//_Database_directory_logical____________________________2016-09-02_2012-02-05_/
}//_namespace_CS_______________________________________________________________/
#endif
