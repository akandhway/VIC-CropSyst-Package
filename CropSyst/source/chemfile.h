#ifndef chemfileH
#define chemfileH
#  include "corn/OS/directory_entry_name_concrete.h"
#include "corn/container/bilist.h"
//______________________________________________________________________________
class chemical_properties
: public CORN::Item
{public:
    float32 half_life;
    float32 linear_sorption_coef;
    float32 Freundlich_N;
    float32 activation_energy;
    float32 soil_moisture_influence; // B_param;                                 //030103
    std::string name;
 public: // constructors
    // Constructor for loading data from chemical pesticide database
    chemical_properties();
    // Constructor for adding new chemicals
    chemical_properties(const char *i_name);
    // Copy constructor
    inline chemical_properties(const chemical_properties &to_copy)
       :CORN::Item()
       ,half_life(to_copy.half_life)
       ,linear_sorption_coef(to_copy.linear_sorption_coef)
       ,Freundlich_N(to_copy.Freundlich_N)
       ,activation_energy(to_copy.activation_energy)
       ,soil_moisture_influence(to_copy.soil_moisture_influence)
       ,name(to_copy.name)
       {}

 public: // virtual methods
   inline virtual bool is_key_string(const std::string &key)        affirmation_  //180820
      { return name == key; }
    inline virtual bool write(std::ostream &strm)  modification_{ return false;} // Not currently needed
    virtual bool read(std::istream &instrm)                       modification_;//990225
};
//_chemical_properties______________________________________________1999-02-25_/
class chemical_file
: public CORN::Bidirectional_list                                                //180101_191205
{private:
    CORN::OS::File_name_concrete file_name;
 public:
    chemical_file(bool add_none_salt);
};
//_chemical_file_______________________________________________________________/
#endif
//chemfile.h

