#include "CS_directory_contexts.hpp"

namespace CS
{

#include "CS_directory_contexts.c"
//______________________________________________________________________________
Directory_context get_directory_context(const CORN::OS::Directory_name &dir_name_fully_qualified)
{  return  get_directory_context_cstr(dir_name_fully_qualified.c_str());
}
//______________________________________________________________________________
}

