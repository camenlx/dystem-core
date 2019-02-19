// Copyright (c) 2018 The Dystem Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DIDENT_H
#define DIDENT_H

#include "key.h"
#include "main.h"

#include <iostream>

using namespace std;

class DIdent;

//
// The Identiry Model Class. Used as a model and utility representation of a users identity on the blockchain. 
//
class DIdent
{
private:
    // critical section to protect the inner data structures
    mutable CCriticalSection cs;

public:

    std::string address;
    std::string alias;
    bool isPresentActiveIdent;
    bool isContentProvider;
    std::string contentProviderTXHash;
    long verificationContentProviderBlockHeight;
    long contentProviderLastScanHeightReached;
    bool isCommissioner;
    std::string commisionerTXHash;
    long verificationCommissionerBlockHeight;
    long commissionerProviderLastScanHeightReached;

    DIdent();
    DIdent(const DIdent& other);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        LOCK(cs);
        READWRITE(address);
        READWRITE(alias);
        READWRITE(isPresentActiveIdent);
        READWRITE(isContentProvider);
        READWRITE(contentProviderTXHash);
        READWRITE(verificationContentProviderBlockHeight);
        READWRITE(contentProviderLastScanHeightReached);
        READWRITE(isCommissioner);
        READWRITE(commisionerTXHash);
        READWRITE(verificationCommissionerBlockHeight);
        READWRITE(commissionerProviderLastScanHeightReached);
    }

    std::string ToString() const;
};

#endif //DIDENT_H
