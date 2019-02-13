// Copyright (c) 2018 The Dystem Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "DIdent.h"

DIdent::DIdent() 
{
    LOCK(cs);
    role = roles::PREVERIFIED;
    verificationBlockHeight = -1; //Set default to an impossible block height for validation of an invalid datablock.
    address = string("");
    alias = string("");
    isActiveLoginAddress = false;
}

DIdent::DIdent(const DIdent& other)
{
    LOCK(cs);
    role = other.role;
    verificationBlockHeight = other.verificationBlockHeight;
    address = other.address;
    alias = other.alias;
    isActiveLoginAddress = other.isActiveLoginAddress;
}
