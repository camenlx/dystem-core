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
    enum roles {
        PREVERIFIED,
        VERIFIED_CREATOR,
        VERIFIED_COMMISSIONER
    };

    int role;
    long verificationBlockHeight;
    std::string address;
    std::string alias;
    bool isActiveLoginAddress;

    DIdent();
    DIdent(const DIdent& other);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        LOCK(cs);
        READWRITE(role);
        READWRITE(verificationBlockHeight);
        READWRITE(address);
        READWRITE(alias);
        READWRITE(isActiveLoginAddress);
    }
};

#endif //DIDENT_H
