// Copyright (c) 2018 The Dystem Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "DIdent.h"

DIdent::DIdent() 
{
    LOCK(cs);

    address = string("");
    alias = string("");
    isPresentActiveIdent = false;
    isContentProvider = false;
    contentProviderTXHash = string("");
    verificationContentProviderBlockHeight = -1; //Set default to an impossible block height for validation of an invalid height.
    isCommissioner = false;
    commisionerTXHash = string("");
    verificationCommissionerBlockHeight = -1; //Set default to an impossible block height for validation of an invalid height.
}

DIdent::DIdent(const DIdent& other)
{
    LOCK(cs);
    address = other.address;
    alias = other.alias;
    isPresentActiveIdent = false;
    isContentProvider = other.isContentProvider;
    contentProviderTXHash = other.contentProviderTXHash;
    verificationContentProviderBlockHeight = other.verificationContentProviderBlockHeight;
    isCommissioner = other.isCommissioner;
    commisionerTXHash = other.commisionerTXHash;
    verificationCommissionerBlockHeight = other.verificationCommissionerBlockHeight;
}

std::string DIdent::ToString() const
{
    std::ostringstream info;
    info << "\naddress: " << address;
    info << "\nalias: " << alias;
    info << "\nisPresentActiveIdent: " << isPresentActiveIdent;
    info << "\nisContentProvider: " << isContentProvider;
    info << "\ncontentProviderTXHash: " << contentProviderTXHash;
    info << "\nverificationContentProviderBlockHeight: " << verificationContentProviderBlockHeight;
    info << "\nisCommissioner: " << isCommissioner;
    info << "\ncommisionerTXHash: " << commisionerTXHash;
    info << "\nsverificationCommissionerBlockHeight: " << verificationCommissionerBlockHeight;

    return info.str();
}