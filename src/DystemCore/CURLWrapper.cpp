// Copyright (c) 2018 The Dystem Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "util.h"

#include "CURLWrapper.h"

#include <curl/curl.h>
#include <sstream>
#include <stdexcept>

using namespace std;

CURLWrapper HTTPRequest;

CURLWrapper::CURLWrapper() 
{   

}

static std::string *DownloadedResponse;

static int writer(char *data, size_t size, size_t nmemb, std::string *buffer_in)
{
    // Is there anything in the buffer?  
    if (buffer_in != NULL)  
    {
        // Append the data to the buffer    
        buffer_in->append(data, size * nmemb);

        // How much did we write?   
        DownloadedResponse = buffer_in;

        return size * nmemb;  
    }

    return 0;
}   

std::string CURLWrapper::GET(std::string url, struct curl_slist *headers=NULL)
{   
  CURL *curl;
  CURLcode result;
  
  curl = curl_easy_init();

  if (curl) 
  {
    if(headers){
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET,1); 
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,writer);
    result = curl_easy_perform(curl);

    if(result != CURLE_OK)
        LogPrintf("*** ERROR MAKING REQUEST %s\n", curl_easy_strerror(result));
    else if (CURLE_OK == result) 
    { 
        char *ct;         
        result = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
    }
  }

  curl_slist_free_all(headers);

  return *DownloadedResponse;
}
