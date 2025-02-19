#include "corn/Internet/HTTP/request_cURL.hpp"
#include <stdio.h>
#include <string.h>
#include "curl/curl.h"
/*
// using libcurl
see simple examples https://curl.haxx.se/libcurl/c/example.html
specically
https://curl.haxx.se/libcurl/c/getinmemory.html
http://stackoverflow.com/questions/1636333/download-file-using-libcurl-in-c-c
*/
namespace CORN     {
namespace Internet {
#include "corn/Internet/HTTP/request_cURL.c_cpp"
//______________________________________________________________________________
size_t write_to_FILE_callback(void *ptr, size_t size, size_t nmemb, FILE *stream)
{  size_t written = fwrite(ptr, size, nmemb, stream);
   return written;
}
//______________________________________________________________________________
bool cURL_request_URI_to_FILE
(int Method
,const CORN::OS::Uniform_resource_identifier &URI
,const char * header, char *data
,FILE *to_file)
{
// I am not sure what would be the the equivelent of header and data in cURL
// these are not currently implemented 161018
   CURL *curl;
   CURLcode result;
   const char *url = URI.c_str();
   curl = curl_easy_init();
   if (curl)
   {
      curl_easy_setopt(curl, CURLOPT_URL, url);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_FILE_callback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, to_file);
      result = curl_easy_perform(curl);
      curl_easy_cleanup(curl);
   }
   return true;
}
//_2016-10-18___________________________________________________________________
std::ostream *cURL_to_stream = 0;
static size_t write_stream_callback
(void *contents, size_t size, size_t nmemb, void *userp)
{
   // I think size is the number of bytes each item in block
   size_t realsize = size * nmemb;
   //struct MemoryStruct *mem = (struct MemoryStruct *)userp;
   //cURL_to_stream->write(mem->memory,mem->size);
   cURL_to_stream->write((const char *)contents,realsize);
   return realsize; // not sure what to return
}
//_2016-10-18___________________________________________________________________
bool cURL_request_URI_to_stream
(int Method
,const CORN::OS::Uniform_resource_identifier &URI
,const char *header, char *data
,std::ostream &to_stream)
{
   cURL_to_stream = &to_stream;
   CURL *curl_handle;
   CURLcode res;
   struct MemoryStruct chunk;
   //161222RLN chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc above */
   //161222RLN chunk.size = 0;    /* no data at this point */
   curl_global_init(CURL_GLOBAL_ALL);
   /* init the curl session */
   curl_handle = curl_easy_init();
   /* specify URL to get */
   curl_easy_setopt(curl_handle, CURLOPT_URL,URI.c_str());
   /* send all data to this function  */
   curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_stream_callback);
   /* we pass our 'chunk' struct to the callback function */
   //161222RLN curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
   /* some servers don't like requests that are made without a user-agent field, so we provide one */
   curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
   /* get it! */
   res = curl_easy_perform(curl_handle);
   /* check for errors */
   if(res != CURLE_OK)
      fprintf(stderr, "internet request failed: %s\n",curl_easy_strerror(res));
   /*
   else
      memcpy(xxx,chunk.memory,chunk.size);
   */
   /* cleanup curl stuff */
   curl_easy_cleanup(curl_handle);
   //161222RLN free(chunk.memory);
   /* we're done with libcurl, so clean it up */
   curl_global_cleanup();
   return true;
}
//_2016-10-18___________________________________________________________________
}}//_namespace CORN::Internet__________________________________________________/

