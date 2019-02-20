// Copyright (c) 2018 The Dystem Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "DCommission.h"

DCommission::DCommission() 
{
    LOCK(cs);
    state = state::COMMISSION_PRE_ACTIVE;
    dataBlockHeight = -1; //Set default to an impossible block height for validation of an invalid datablock.
    authorName = string("");
    createdAt = string("");
    awardedAt = string("");
    deletedAt = string("");
    updatedAt = string("");
    awardedByAddr = string("");
    awardedToAddr = string("");
    authorAddr = string("");
    title = string("");
    body = string("");
    coin = string("");
    fee = 0.0;
    finalStatus = string("");
    cullFromChainDate = 0;
}

DCommission::DCommission(const DCommission& other)
{
    LOCK(cs);
    state = other.state;
    dataBlockHeight = other.dataBlockHeight;
    authorName = other.authorName;
    createdAt = other.createdAt;
    awardedAt = other.awardedAt;
    deletedAt = other.deletedAt;
    updatedAt = other.updatedAt;
    awardedByAddr = other.awardedByAddr;
    awardedToAddr = other.awardedToAddr;
    authorAddr = other.authorAddr;
    title = other.title;
    body = other.body;
    coin = other.coin;
    fee = other.fee;
    finalStatus = other.finalStatus;
    cullFromChainDate = other.cullFromChainDate;
}

std::string DCommission::ToString() const
{
    std::ostringstream info;
    info << "\nstate: " << state;
    info << "\ndataBlockHeight: " << dataBlockHeight;
    info << "\nauthorName: " << authorName;
    info << "\ncreatedAt: " << createdAt;
    info << "\nawardedAt: " << awardedAt;
    info << "\ndeletedAt: " << deletedAt;
    info << "\nupdatedAt: " << updatedAt;
    info << "\nawardedByAddr: " << awardedByAddr;
    info << "\ntitle: " << title;
    info << "\nbody: " << body;
    info << "\ncoin: " << coin;
    info << "\nfee: " << fee;
    info << "\nfinalStatus: " << finalStatus;
    info << "\ncullFromChainDate: " << cullFromChainDate;


    return info.str();
}
