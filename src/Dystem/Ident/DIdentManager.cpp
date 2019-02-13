// Copyright (c) 2018 The Dystem Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "DIdentManager.h"

#include "main.h"
#include "util.h"
#include "base58.h"
#include "clientversion.h"
#include <boost/filesystem.hpp>
#include "../../rpc/server.h"
#include "script/script.h"
#include "script/script_error.h"
#include "script/sign.h"
#include "script/standard.h"

/** Identity manager */
DIdentManager identManager;

//
// DIdentityDB
//
DIdentDB::DIdentDB()
{
    identDBLoc = GetDataDir() / "idents.dat";
    strMagicMessage = "IdentsCache";
}

bool DIdentDB::Write(const DIdentManager& myIdents)
{
    int64_t nStart = GetTimeMillis();
    // serialize, checksum data up to that point, then append checksum
    CDataStream ssIdents(SER_DISK, CLIENT_VERSION);
    ssIdents << strMagicMessage;                   // Ident cache file specific magic message
    ssIdents << FLATDATA(Params().MessageStart()); // network specific magic number
    ssIdents << myIdents;
    uint256 hash = Hash(ssIdents.begin(), ssIdents.end());
    ssIdents << hash;

    // open output file, and associate with CAutoFile
    FILE* file = fopen(identDBLoc.string().c_str(), "wb");
    CAutoFile fileout(file, SER_DISK, CLIENT_VERSION);
    if (fileout.IsNull())
        return error("%s : Failed to open file %s", __func__, identDBLoc.string());

    // Write and commit header, data
    try {
        fileout << ssIdents;
    } catch (std::exception& e) {
        return error("%s : Serialize or I/O error - %s", __func__, e.what());
    }
    //FileCommit(fileout);
    fileout.fclose();

    LogPrintf("Written info to idents.dat  %dms\n", GetTimeMillis() - nStart);
    LogPrintf("Identities saved:  %s\n", myIdents.ToString());

    return true;
}

DIdentDB::ReadResult DIdentDB::Read(DIdentManager& myIdents, bool fDryRun)
{
    int64_t nStart = GetTimeMillis();
    // open input file, and associate with CAutoFile
    FILE* file = fopen(identDBLoc.string().c_str(), "rb");
    CAutoFile filein(file, SER_DISK, CLIENT_VERSION);
    if (filein.IsNull()) {
        error("%s : Failed to open file %s", __func__, identDBLoc.string());
        return FileError;
    }

    // use file size to size memory buffer
    int fileSize = boost::filesystem::file_size(identDBLoc);
    int dataSize = fileSize - sizeof(uint256);
    // Don't try to resize to a negative number if file is small
    if (dataSize < 0)
        dataSize = 0;
    vector<unsigned char> vchData;
    vchData.resize(dataSize);
    uint256 hashIn;

    // read data and checksum from file
    try {
        filein.read((char*)&vchData[0], dataSize);
        filein >> hashIn;
    } catch (std::exception& e) {
        error("%s : Deserialize or I/O error - %s", __func__, e.what());
        return HashReadError;
    }
    filein.fclose();

    CDataStream ssIdentities(vchData, SER_DISK, CLIENT_VERSION);

    // verify stored checksum matches input data
    uint256 hashTmp = Hash(ssIdentities.begin(), ssIdentities.end());
    if (hashIn != hashTmp) {
        error("%s : Checksum mismatch, data corrupted", __func__);
        return IncorrectHash;
    }

    unsigned char pchMsgTmp[4];
    std::string strMagicMessageTmp;
    try {
        // de-serialize file header (masternode cache file specific magic message) and ..

        ssIdentities >> strMagicMessageTmp;

        // ... verify the message matches predefined one
        if (strMagicMessage != strMagicMessageTmp) {
            error("%s : Invalid identity cache magic message", __func__);
            return IncorrectMagicMessage;
        }

        // de-serialize file header (network specific magic number) and ..
        ssIdentities >> FLATDATA(pchMsgTmp);

        // ... verify the network matches ours
        if (memcmp(pchMsgTmp, Params().MessageStart(), sizeof(pchMsgTmp))) {
            error("%s : Invalid network magic number", __func__);
            return IncorrectMagicNumber;
        }
        // de-serialize data into IdentMan object
        ssIdentities >> myIdents;
    } catch (std::exception& e) {
        myIdents.Clear();
        error("%s : Deserialize or I/O error - %s", __func__, e.what());
        return IncorrectFormat;
    }

    LogPrintf("Loaded info from identity.dat  %dms\n", GetTimeMillis() - nStart);
    LogPrintf(" %s\n", myIdents.ToString());
    if (!fDryRun) {
        LogPrintf("Identity manager - cleaning....\n");
        //myIdents.CheckAndRemove(true);
        LogPrintf("Identity manager - result:\n");
        LogPrintf("  %s\n", myIdents.ToString());
    }
    
    return Ok;
}

//
// DIdentManager
//
DIdentManager::DIdentManager()
{
}

int DIdentManager::getRoleFromString(std::string roleStr) {    
    return DIdent::PREVERIFIED;
}

void DIdentManager::Add(std::string& address, std::string& alias)
{
    bool identAlreadyExists = false;

    DIdent* newIdent = new DIdent();
    newIdent->address = address;
    newIdent->alias = alias;
    newIdent->isActiveLoginAddress = true;

    std::vector<DIdent>* copyIdents = new std::vector<DIdent>();
    copyIdents->swap(vMyIdents);

    for (std::vector<DIdent>::iterator it = copyIdents->begin(); it != copyIdents->end(); ++it) {
        DIdent* ident = &(*it);
        if(ident->address == address ) {
            //If the ident already exists don't replace it just update its flag and don't add it to the structure again.
            identAlreadyExists = true;
            ident->isActiveLoginAddress = true;
        } else {
            ident->isActiveLoginAddress = false;
        }
    }

    if(!identAlreadyExists)
        copyIdents->push_back(*newIdent);
    
    vMyIdents.swap(*copyIdents);

    DIdentDB db;
    db.Write(*this);
}

void DIdentManager::saveSettings() {

}

void DIdentManager::Clear()
{
    LOCK(cs);
    vMyIdents.clear();
}

std::vector<DIdent>& DIdentManager::getIdents() {
    return vMyIdents;
}

std::string DIdentManager::ToString() const
{
    std::ostringstream info;
    info << "Idents: " << (int)vMyIdents.size();
    return info.str();
}

double DIdentManager::getDeterministicCreatorFee(int blockHeight) {
    if( blockHeight >= REGISTRATION_START_BLOCKHEIGHT ) {
        return ((170000000000.0 / (blockHeight - REGISTRATION_START_BLOCKHEIGHT)) / 10000.0);
    }
    return -1;
}

double DIdentManager::getCommissionerFee(int blockHeight) {
    if( blockHeight >= REGISTRATION_START_BLOCKHEIGHT ) {
        return 2000;
    }
    return -1;
}

/** Comparison function for sorting the getchaintips heads.  */
struct CompareBlocksByHeight {
    bool operator()(const CBlockIndex* a, const CBlockIndex* b) const
    {
        /* Make sure that unequal blocks with the same height do not compare
           equal. Use the pointers themselves to make a distinction. */

        if (a->nHeight != b->nHeight)
            return (a->nHeight > b->nHeight);

        return a < b;
    }
};

bool DIdentManager::validateUserAccountByHash(std::string hash, std::string searchAddr, IdentType type) {
    LOCK(cs_main);

    //Record the initial tip height for searching 
    std::string tipHashString = getTipAddressHash();

    if(tipHashString == "")
        return false;

    //Get the first block hash point
    UniValue tipHashUniV(tipHashString);
    uint256 blockHash(ParseHashV(tipHashUniV, "parameter 1"));

    //Starting at the tip of the chain search backwards untill we find the hash and find the height
    for( int currBlockHeight = chainActive.Height(); currBlockHeight >= REGISTRATION_START_BLOCKHEIGHT; currBlockHeight--) {

        //Check to see the block is actually found        
        if( mapBlockIndex.count(blockHash) != 0 ) {
            //Block found 
            CBlock block;
            CBlockIndex* pblockindex = mapBlockIndex[blockHash];
            
            if (!ReadBlockFromDisk(block, pblockindex)) {
                return false;
            }

            CDataStream ssBlock(SER_NETWORK, PROTOCOL_VERSION);
            ssBlock << block;

            //Now loop through each transaction in the block and find one that matches the input hash
            BOOST_FOREACH (const CTransaction& tx, block.vtx) {
                if( hash == tx.GetHash().GetHex() ) {
                    uint256 currentHash = ParseHashV(tx.GetHash().GetHex(), "parameter 1");
                    CTransaction transactionx;
                    uint256 hashBlock = 0;
                    if (GetTransaction(currentHash, transactionx, hashBlock, true)) {
                        //Loop through the transactions looking for one that matches the pescribed ident address location
                        for (unsigned int i = 0; i < transactionx.vout.size(); i++) {
                            const CTxOut& txout = transactionx.vout[i];
                            double fee = double(txout.nValue / COIN);

                            txnouttype txType;
                            vector<CTxDestination> addressVector;
                            int nRequired;

                            if (!ExtractDestinations(txout.scriptPubKey, txType, addressVector, nRequired)) {
                                //Could be a non standard TX so loop onwards as we are only parsing
                                continue;
                            }

                            UniValue a(UniValue::VARR);
                            BOOST_FOREACH (const CTxDestination& addr, addressVector){
                                //Now verify the output address matches that of the selected search ident
                                if( type == ContentCreator && CBitcoinAddress(addr).ToString() == REGISTRATION_CONTENT_CREATOR_ADDRESS ) {
                                    //As the actually block the fee was written in can happen after its save to say that the range would be the current block and a fee higher than the block fee.
                                    //As such a preceding block drift allows us to find a range that is suitible to find a gibbon user.
                                    if(fee <= getDeterministicCreatorFee(currBlockHeight-REGISTRATION_FEE_BLOCK_DRIFT) && fee >= getDeterministicCreatorFee(currBlockHeight)) {
                                        //Now finally match up the input address with a TXIN address to prove the sender is who they say they are
                                        if( findAddressInVin(searchAddr, transactionx.vin) ) {
                                            return true;
                                        }
                                    }                                    
                                } else if( type == Commissioner && CBitcoinAddress(addr).ToString() == REGISTRATION_COMMISSIONER_ADDRESS ) {
                                    //As we are checking for an exact fee presently for a commissioner check and confirm
                                     if( fee == getCommissionerFee(currBlockHeight) ) {
                                         if( findAddressInVin(searchAddr, transactionx.vin) ) {
                                            return true;
                                        }
                                     }
                                }
                            }
                        }
                    }
                }
            }

            //If not found move to the previous block, rinse and repeat.
            blockHash = pblockindex->pprev->GetBlockHash();
        }
    }

    return false;    
}

bool DIdentManager::validateUserAccountByAddress(std::string searchAddr, IdentType type) {
     //Record the initial tip height for searching 
    std::string tipHashString = getTipAddressHash();

    if(tipHashString == "")
        return false;

    //Get the first block hash point
    UniValue tipHashUniV(tipHashString);
    uint256 blockHash(ParseHashV(tipHashUniV, "parameter 1"));

    //Starting at the tip of the chain search backwards untill we find the hash and find the height
    for( int currBlockHeight = chainActive.Height(); currBlockHeight >= REGISTRATION_START_BLOCKHEIGHT; currBlockHeight--) {
        //Check to see the block is actually found        
        if( mapBlockIndex.count(blockHash) != 0 ) {
            //Block found 
            CBlock block;
            CBlockIndex* pblockindex = mapBlockIndex[blockHash];
            
            if (!ReadBlockFromDisk(block, pblockindex)) {
                return false;
            }

            CDataStream ssBlock(SER_NETWORK, PROTOCOL_VERSION);
            ssBlock << block;

            //Now loop through each transaction in the block and find one that matches the input hash
            BOOST_FOREACH (const CTransaction& tx, block.vtx) {
                uint256 currentHash = ParseHashV(tx.GetHash().GetHex(), "parameter 1");
                CTransaction transactionx;
                uint256 hashBlock = 0;
                
                if (GetTransaction(currentHash, transactionx, hashBlock, true)) {
                    //Loop through the transactions looking for one that matches the pescribed ident address location
                    for (unsigned int i = 0; i < transactionx.vout.size(); i++) {
                        const CTxOut& txout = transactionx.vout[i];
                        double fee = double(txout.nValue / COIN);

                        txnouttype txType;
                        vector<CTxDestination> addressVector;
                        int nRequired;

                        if (!ExtractDestinations(txout.scriptPubKey, txType, addressVector, nRequired)) {
                            //Could be a non standard TX so loop onwards as we are only parsing
                            continue;
                        }

                        UniValue a(UniValue::VARR);
                        BOOST_FOREACH (const CTxDestination& addr, addressVector){
                            //Now verify the output address matches that of the selected search ident
                            if( type == ContentCreator && CBitcoinAddress(addr).ToString() == REGISTRATION_CONTENT_CREATOR_ADDRESS ) {
                                //As the actually block the fee was written in can happen after its save to say that the range would be the current block and a fee higher than the block fee.
                                //As such a preceding block drift allows us to find a range that is suitible to find a gibbon user.
                                if(fee <= getDeterministicCreatorFee(currBlockHeight-REGISTRATION_FEE_BLOCK_DRIFT) && fee >= getDeterministicCreatorFee(currBlockHeight)) {
                                    //Now finally match up the input address with a TXIN address to prove the sender is who they say they are
                                    if( findAddressInVin(searchAddr, transactionx.vin) ) {
                                        return true;
                                    }
                                }                                    
                            } 
                            else if( type == Commissioner && CBitcoinAddress(addr).ToString() == REGISTRATION_COMMISSIONER_ADDRESS ) {
                                //As we are checking for an exact fee presently for a commissioner check and confirm
                                if( fee == getCommissionerFee(currBlockHeight) ) {
                                    if( findAddressInVin(searchAddr, transactionx.vin) ) {
                                        return true;
                                    }
                                }
                            }   
                        }
                    }
                }
            }

            //If not found move to the previous block, rinse and repeat.
            blockHash = pblockindex->pprev->GetBlockHash();
        }
    }

    return false;
}

std::string DIdentManager::getTipAddressHash() {
    //bool tipFound = false;

    //Get the hash for the tip of the chain we are currently on
    std::set<const CBlockIndex*, CompareBlocksByHeight> setTips;
    BOOST_FOREACH (const PAIRTYPE(const uint256, CBlockIndex*) & item, mapBlockIndex)
        setTips.insert(item.second);
    BOOST_FOREACH (const PAIRTYPE(const uint256, CBlockIndex*) & item, mapBlockIndex) {
        const CBlockIndex* pprev = item.second->pprev;
        if (pprev)
            setTips.erase(pprev);
    }

    // Always report the currently active tip.
    setTips.insert(chainActive.Tip());

    //Find the active tip relative to the chain we are working on.
    UniValue res(UniValue::VARR);
    BOOST_FOREACH (const CBlockIndex* block, setTips) {
        if( chainActive.Height() == block->nHeight ) {
            return block->phashBlock->GetHex();
        }
    }

    return "";
}

bool DIdentManager::findAddressInVin(std::string addrress, std::vector<CTxIn> vin) {
    for (unsigned int i = 0; i < vin.size(); i++) {
        const CTxIn& txin = vin[i];
        uint256 vinHash = ParseHashV(txin.prevout.hash.GetHex(), "parameter 1");
        CTransaction vinTransactionx;
        uint256 vinHashBlock = 0;

        if (GetTransaction(vinHash, vinTransactionx, vinHashBlock, true)) {
            for (unsigned int i = 0; i < vinTransactionx.vout.size(); i++) {
                const CTxOut& txInOut = vinTransactionx.vout[i];
                txnouttype inTxType;
                vector<CTxDestination> inAddressVector;
                int nInRequired;
                if (!ExtractDestinations(txInOut.scriptPubKey, inTxType, inAddressVector, nInRequired)) {
                    return false;
                }

                BOOST_FOREACH (const CTxDestination& inAddr, inAddressVector) {
                    if( addrress == CBitcoinAddress(inAddr).ToString() ) {
                        return true;
                    }
                }
            }
        }   
    }

    return false;
}
