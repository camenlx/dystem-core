// Copyright (c) 2018 The Dystem Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef CURLWRAPPER_H
#define CURLWRAPPER_H

#include <string>
#include <vector>

class CURLWrapper;

extern CURLWrapper HTTPRequest;

class CURLWrapper
{
private:
   

public:
    CURLWrapper();
    //HTTP / HTTPS Get request
    std::string GET(std::string url, struct curl_slist *headers);
};


#endif // CURLWRAPPER_H
