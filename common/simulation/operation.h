#ifndef operationH
#define operationH
#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#ifndef constH
#  include "corn/const.h"
#endif
#ifndef parameters_datarecH
#  include "corn/parameters/parameters_datarec.h"
#endif
#define LABEL_name         "name"
#define LABEL_description  "description"
//______________________________________________________________________________
class Common_operation
: public Common_parameters_data_record   // for when operations parameters are stored in separate files
{
 public: // So we can bind_to
   std::string name;                                                             //040909
      /**< The name or ID of the operation
         031023 Note, I thought I had moved this to event, if so this is only a record and should be private!
         Name matches the operation ID (040909)
      **/
#ifdef MICROBASIN_VERSION
   std::string common_operation_filename;                                        //170321LML used to store management file name (without extention) that conduct this operation.
#endif
   int16 start_hour;
   int16 duration_hours;
// NYI     int16 duration_minutes;                                               //071029
      /**< Actually the units are Minutes
            Note that the duration is the actual amount of time
            the operation is applied to a given unit.
            For example in irrigation the actual amount of time water is applied to any given patch of soil.
            NYI run_time this is the actual run time
            For example in irrigation the amount of time the irrigation device
            is run even though it may only cover a patch of soil for a much shorter time.
      **/
 public:
   Common_operation(Associated_directory_mode         _associated_directory_mode
      ,const char *primary_section);
      //180626 ,nat8 i_major,nat8 i_release,nat8 i_minor);
   Common_operation();
   Common_operation(const Common_operation &copy_from);                          //140828
   virtual void        setup_parameters
      (CORN::Data_record &data_rec,bool for_write)                modification_; //970709
           bool        expect_structure(bool for_write)           modification_; //161023_970709
           void        copy(const Common_operation &from);                       //020315
   virtual int32       get_type()                                       const=0; //011207
   //170424 virtual const char *label_cstr(char *buffer)                           const; //030123
   virtual const char *label_string(std::string &buffer)                  const; //170424
   inline virtual bool repeatable()                        const { return true;} //020227
   inline virtual bool can_be_performed_before_simulation_start_date()
                                                          const { return false;} //021107
       // Some derived operations may be performed before the start of the simulation.
   inline const char  *get_name()                  const { return name.c_str();}//030808
       // Name is used by by CANMS for selection of the operation and filename.
   //140616 replaced by inline const char  *get_detailed_description_c_str()                   const { return description.c_str(); } //020306
   inline const std::string &get_detailed_description()                   const
      // eventually rename to get_brief_description                              //150210
      { return description.brief; }                                              //140616
   virtual inline bool is_mode()                   affirmation_ { return false;} //020313
   virtual void        log(std::ostream &log_file)                        const; //020810
   virtual void        log_ID_and_type(std::ostream &log_file)            const; //050110
   virtual bool        parameters_to_be_stored_in_event_file()          const=0; //030808
   // Derived classes may store parameters in either the event file (CropSyst) or separate parameter files (CANMS);
   virtual const char *get_simulation()                                 const=0; //021108
   inline CORN::Hour    get_start_hour() const { return (CORN::Hour)start_hour;} //081212
   inline CORN::Hours   get_duration_hours()    const { return duration_hours; } //071029
};
//_Common_operation_________________________________________________2001-12-07_/
#endif
// COMMON_OPERATION_H

