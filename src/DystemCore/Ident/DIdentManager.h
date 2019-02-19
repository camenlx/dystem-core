// Copyright (c) 2018 The Dystem Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef IDENTMANAGER_H
#define IDENTMANAGER_H

//Dystem includes
#include "DIdent.h"

//Bitcoin/Dash/PIVX includes
#include "main.h"
#include "net.h"
#include "util.h"

using namespace std;

class DIdentManager;
class DIdentHead;

extern DIdentManager identManager;

//TODO: Increase this for the prescribed official launch block height
static const int REGISTRATION_START_BLOCKHEIGHT = 396749;
static const int REGISTRATION_FEE_BLOCK_DRIFT = 3;
static const std::string REGISTRATION_START_HASH("");
static const  std::string REGISTRATION_CONTENT_CREATOR_ADDRESS("D62NsasWcwr6SmBQVq55e9M8cYANLkdUs5");
static const  std::string REGISTRATION_COMMISSIONER_ADDRESS("DLwitd7pAH7hqMAed31YFTHRELDonsXwpA");

/** Access to the Commission database (commissions.dat)
 */
class DIdentDB
{
    private:
        boost::filesystem::path identDBLoc;
        std::string strMagicMessage;

    public:
        enum ReadResult {
            Ok,
            FileError,
            HashReadError,
            IncorrectHash,
            IncorrectMagicMessage,
            IncorrectMagicNumber,
            IncorrectFormat
        };

    DIdentDB();
    bool Write(const DIdentManager& myIdents);
    DIdentDB::ReadResult Read(DIdentManager& myIdents, bool fDryRun);
};

class DIdentManager
{
private:
    // critical section to protect the inner data structures
    mutable CCriticalSection cs;

    // critical section to protect the inner data structures specifically on messaging
    mutable CCriticalSection cs_process_message;

    // map to hold all Commissions
    std::vector<DIdent> vMyIdents;

    //Based on a blocks inputs find an address in its inputs
    bool findAddressInVin(std::string addrress, std::vector<CTxIn> vin);

    //Get the most accurate chain tip
    std::string getTipAddressHash();

public:

    enum IdentType {
        None,
        ContentCreator,
        Commissioner
    };

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        LOCK(cs);
        READWRITE(vMyIdents);
    }

    DIdentManager();
    DIdentManager(DIdentManager& other);

    // Add an new ident to the list
    void Add(DIdent* newIdent);

    // Return an ident from the list
    DIdent Get(std::string& address);

    // Return the current selected and active ident from the list
    DIdent GetActive();

    // Clear the current list of idents
    void Clear();
    
    //Return read only list of commissions for display and analysis
    std::vector<DIdent>& getIdents();

    //Save the users updated settings to the disk
    void saveSettings();

    //Using an asymptote function bound with a high low cap to calculate a determinstic fee for registering new Content providers.
    double getDeterministicCreatorFee(int blockHeight);

    //Using an asymptote function bound with a high low cap to calculate a determinstic fee for registering new Commissioners.
    double getCommissionerFee(int blockHeight);

    //Validate a user of particular role is verified based on a block hash, address and role.
    DIdentHead validateUserAccountByHash(std::string hash, std::string searchAddr, IdentType type, int forcedMinBlockHeight = -1);

    //Validate a user of particular role is verified based on a address and role.
    DIdentHead validateUserAccountByAddress(std::string searchAddr, IdentType type, int forcedMinBlockHeight = -1);

    std::string ToString() const;
};

class DIdentHead
{
    public:
        DIdentManager::IdentType identType;
        std::string TXHash;
        long blockHeight;
        long scanStartBlockHeight;

    DIdentHead(DIdentManager::IdentType type = DIdentManager::None, std::string hash = "", long height = -1, long scanBlockHeight = -1);
};

#endif //IDENTMANAGER_H
