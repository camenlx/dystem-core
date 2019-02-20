// Copyright (c) 2018 The Dystem Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DCOMMISSION_H
#define DCOMMISSION_H

#include "key.h"
#include "main.h"

#include <iostream>

using namespace std;

class DCommission;

//
// The Commisision Model Class. Used as a model and utility representation of a commission. 
//
class DCommission
{
private:
    // critical section to protect the inner data structures
    mutable CCriticalSection cs;

public:
    enum state {
        COMMISSION_ACTIVE,
        COMMISSION_AWARDED,
        COMMISSION_ACCEPTED,
        COMMISSION_COMPLETED,
        COMMISSION_CLOSED,
        COMMISSION_PRE_ACTIVE
    };
    int state;

    long dataBlockHeight;
    std::string authorName;

    std::string createdAt;
    std::string awardedAt;
    std::string deletedAt;
    std::string updatedAt;
    
    //Addresses associated with the current Commissions
    std::string awardedByAddr;
    std::string awardedToAddr;
    std::string authorAddr;

    std::string title;
    std::string body;
    std::string coin;
    double fee;
    
    std::string finalStatus;
    time_t cullFromChainDate;

    std::string ToString() const;

    DCommission();
    DCommission(const DCommission& other);

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        LOCK(cs);
        READWRITE(state);
        READWRITE(dataBlockHeight);
        READWRITE(authorName);
        READWRITE(createdAt);
        READWRITE(awardedAt);
        READWRITE(deletedAt);
        READWRITE(updatedAt);
        READWRITE(awardedByAddr);
        READWRITE(awardedToAddr);
        READWRITE(authorAddr);
        READWRITE(title);
        READWRITE(body);
        READWRITE(coin);
        READWRITE(fee);
        READWRITE(finalStatus);
        READWRITE(cullFromChainDate);
    }
};

#endif //DCOMMISSION_H
