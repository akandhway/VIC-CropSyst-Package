#if (!defined(_WIN32) && ! defined(_WIN64))
#error Windows only
// These functions are only available under the development environments for Windows
// For Linux or other, use request_cURL
#endif
#include "request_WinINet.hpp"
#include <WinInet.h>

namespace CORN     {
namespace Internet {
//______________________________________________________________________________
#include "corn/Internet/HTTP/request_WinINet.c_cpp"
// request_WinINet.c must be included before this because it provides
// the common WinINet_REQUEST macros and we want it under
// CORN::Internet namespace with C++.
//______________________________________________________________________________
bool WinINet_request_URI_to_stream
(int Method
,const CORN::OS::Uniform_resource_identifier &URI
,LPCSTR header, LPSTR data
,std::ostream &to_stream)
{  try{
   std::string url; URI.get_path_query_fragment_string(url);
   const CORN::OS::Uniform_resource_identifier::Authority
          *authority = URI.get_authority();
   WinINet_REQUEST_START
      (authority ? authority->hostname ? authority->hostname->c_str() :0:0
      ,authority ? authority->port     ? authority->port
         : INTERNET_DEFAULT_HTTP_PORT : INTERNET_DEFAULT_HTTP_PORT
      , authority ? authority->username ? authority->username->c_str() : 0:0
      , authority ? authority->password ? authority->password->c_str() : 0:0
      , url.c_str())
   to_stream.write(szBuffer,dwRead);
   WinINet_REQUEST_FINISH
   } catch(...) {}
   return true;
}
//_2016-08-02___________________________________________________________________
bool WinINet_request_URI_to_FILE
(int Method
,const CORN::OS::Uniform_resource_identifier &URI
,LPCSTR header, LPSTR data
,FILE *to_file)
{  try{
   std::string url; URI.get_path_query_fragment_string(url);
   const CORN::OS::Uniform_resource_identifier::Authority
          *authority = URI.get_authority();
   WinINet_REQUEST_START
      (authority ? authority->hostname ? authority->hostname->c_str() :0:0
      ,authority ? authority->port     ? authority->port
         : INTERNET_DEFAULT_HTTP_PORT : INTERNET_DEFAULT_HTTP_PORT
      , authority ? authority->username ? authority->username->c_str() : 0:0
      , authority ? authority->password ? authority->password->c_str() : 0:0
      , url.c_str())

   fwrite(szBuffer,1,dwRead,to_file);
   WinINet_REQUEST_FINISH
   } catch(...) {}
   return true;
}
//_2016-10-18___________________________________________________________________
}}//_namespace_CORN_Internet___________________________________________________/

