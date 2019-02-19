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
    contentProviderLastScanHeightReached = -1;
    isCommissioner = false;
    commisionerTXHash = string("");
    verificationCommissionerBlockHeight = -1; //Set default to an impossible block height for validation of an invalid height.
    commissionerProviderLastScanHeightReached = -1;
}

DIdent::DIdent(const DIdent& other)
{
    LOCK(cs);
    address = other.address;
    alias = other.alias;
    isPresentActiveIdent =  other.isPresentActiveIdent;
    isContentProvider = other.isContentProvider;
    contentProviderTXHash = other.contentProviderTXHash;
    verificationContentProviderBlockHeight = other.verificationContentProviderBlockHeight;
    contentProviderLastScanHeightReached = other.contentProviderLastScanHeightReached;
    isCommissioner = other.isCommissioner;
    commisionerTXHash = other.commisionerTXHash;
    verificationCommissionerBlockHeight = other.verificationCommissionerBlockHeight;
    commissionerProviderLastScanHeightReached = other.commissionerProviderLastScanHeightReached;
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
    info << "\ncontentProviderLastScanHeightReached: " << contentProviderLastScanHeightReached;
    info << "\nisCommissioner: " << isCommissioner;
    info << "\ncommisionerTXHash: " << commisionerTXHash;
    info << "\nverificationCommissionerBlockHeight: " << verificationCommissionerBlockHeight;
    info << "\ncommissionerProviderLastScanHeightReached: " << commissionerProviderLastScanHeightReached;

    return info.str();
}