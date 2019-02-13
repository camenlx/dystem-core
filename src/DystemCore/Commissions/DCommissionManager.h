// Copyright (c) 2018 The Dystem Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COMMISSIONMANAGER_H
#define COMMISSIONMANAGER_H

#include "DCommission.h"

#include "main.h"
#include "net.h"
#include "util.h"

using namespace std;

class DCommissionManager;

extern DCommissionManager commissionsmanager;

/** Access to the Commission database (commissions.dat)
 */
class DCommisionsDB
{
    private:
        boost::filesystem::path commissionDBLoc;
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
    //Bit filthy usign callbacks, but to keep QT seperated cleaniny for non wallet builds this makes some sense.
    //void (commissionslist::*)() updated;

    DCommisionsDB();
    bool Write(const DCommissionManager& commissionsToSave);
    DCommisionsDB::ReadResult Read(DCommissionManager& commissionsToRead, bool fDryRun);
};

class DCommissionManager
{
private:
    // critical section to protect the inner data structures
    mutable CCriticalSection cs;

    // critical section to protect the inner data structures specifically on messaging
    mutable CCriticalSection cs_process_message;

    // map to hold all Commissions
    std::vector<DCommission> vCommissions;

public:

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        LOCK(cs);
        READWRITE(vCommissions);
    }

    DCommissionManager();
    DCommissionManager(DCommissionManager& other);
    
    // Retrive correct state enum for the string value of 
    int getStateFromString(std::string stateStr);

    // Add an new commission to the mempool
    void Add(DCommission& commission);

    std::string ToString() const;
    
    //Return read only list of commissions for display and analysis
    std::vector<DCommission>& getCommissions();

    // Clear the current list of Commissions
    void Clear();
};

#endif //COMMISSIONMANAGER_H
