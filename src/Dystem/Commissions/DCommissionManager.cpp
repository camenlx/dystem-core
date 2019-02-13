// Copyright (c) 2018 The Dystem Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "DCommissionManager.h"
#include "DCommission.h"
#include "Dystem/CURLWrapper.h"

#include "util.h"
#include "clientversion.h"
#include <boost/filesystem.hpp>
#include <curl/curl.h>
#include <json/json.h>
#include <json/reader.h>
#include <json/writer.h>
#include <json/value.h>

/** Commmissions manager */
DCommissionManager commissionsmanager;

//
// DCommisionsDB
//
DCommisionsDB::DCommisionsDB()
{
    commissionDBLoc = GetDataDir() / "commissions.dat";
    strMagicMessage = "CommissionsCache";
}

bool DCommisionsDB::Write(const DCommissionManager& commissionsToSave)
{
    int64_t nStart = GetTimeMillis();
    // serialize, checksum data up to that point, then append checksum
    CDataStream ssCommissions(SER_DISK, CLIENT_VERSION);
    ssCommissions << strMagicMessage;                   // Commission cache file specific magic message
    ssCommissions << FLATDATA(Params().MessageStart()); // network specific magic number
    ssCommissions << commissionsToSave;
    uint256 hash = Hash(ssCommissions.begin(), ssCommissions.end());
    ssCommissions << hash;

    // open output file, and associate with CAutoFile
    FILE* file = fopen(commissionDBLoc.string().c_str(), "wb");
    CAutoFile fileout(file, SER_DISK, CLIENT_VERSION);
    if (fileout.IsNull())
        return error("%s : Failed to open file %s", __func__, commissionDBLoc.string());

    // Write and commit header, data
    try {
        fileout << ssCommissions;
    } catch (std::exception& e) {
        return error("%s : Serialize or I/O error - %s", __func__, e.what());
    }
    //FileCommit(fileout);
    fileout.fclose();

    LogPrintf("Written info to commissions.dat  %dms\n", GetTimeMillis() - nStart);
    LogPrintf("Commissions saved:  %s\n", commissionsToSave.ToString());

    return true;
}

DCommisionsDB::ReadResult DCommisionsDB::Read(DCommissionManager& commissionsToRead, bool fDryRun)
{
    //Temporary V1 transation. Pull the commissions from the test API. To be replaced with the data chain.
    struct curl_slist *headers=NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "api-key: 07c0faf9-d74a-4164-9003-b45bb8e88963 ");

    std::string commissionsJSON = HTTPRequest.GET(string("https://app.dystem.io/api/commissions"), headers);

    //Make sure to clear the list of commissions firat
    commissionsmanager.Clear();

    if( commissionsJSON.size() > 0 ) {
        //First clear old commissions out ( note only useful when using API, datachain should be rolling and self ammending. )
        //JSON Parsing
        Json::CharReaderBuilder builder;
        Json::CharReader* reader = builder.newCharReader();

        Json::Value json;
        std::string errors;

        bool parsingSuccessful = reader->parse(
            commissionsJSON.c_str(),
            commissionsJSON.c_str() + commissionsJSON.size(),
            &json,
            &errors
        );
        delete reader;

        if (!parsingSuccessful) {
            LogPrintf("*** Failed to parse the JSON\n");
        } else {
            Json::Value response = json.get("response","");
            Json::Value commissions = response.get("commissions","");
            
            for (Json::Value::ArrayIndex i = 0; i != commissions.size(); i++) {
                DCommission* com = new DCommission();
                com->state = commissionsmanager.getStateFromString(commissions[i]["status"].asString());
                //TODO: When linking up the data chain be sure to pull the block height thats relevant to the actuall data block height
                com->dataBlockHeight = -1;
                com->authorName = commissions[i]["author"].asString();
                com->createdAt = commissions[i]["created_at"].asString();
                com->awardedAt = commissions[i]["awarded_at"].asString();
                com->deletedAt = commissions[i]["deleted_at"].asString();
                com->updatedAt = commissions[i]["updated_at"].asString();
                //Thought, surely these should be pub keys that the addresses are derived from on the front end and not the raw address ?
                com->awardedByAddr = commissions[i]["awarded_by"].asString();
                com->awardedToAddr = commissions[i]["awarded_to"].asString();
                //TODO: When the data chain is up replace this with the author id as at some point an author might not be the maintainer of a commission when using multisig
                com->authorAddr = commissions[i]["author_id"].asString();
                com->title = commissions[i]["title"].asString();
                com->body = commissions[i]["body"].asString();
                com->coin = commissions[i]["coin"].asString();
                com->fee = std::stod(commissions[i]["fee"].asString());
                com->finalStatus = commissions[i]["final_status"].asString();
                //TODO: When commissions are writen onto the data chain need to add the culling height
                com->cullFromChainDate = 0;
                commissionsmanager.Add(*com);
            }
            //Temporaty soloution but write the JSON commission model objects to the disk. 
            Write(commissionsmanager);
        }
    } else {
        LogPrintf("WARNING: NO COMMISSIONS FOUND \n");
    }

    return Ok;
}

DCommissionManager::DCommissionManager()
{
}

int DCommissionManager::getStateFromString(std::string stateStr) {    
    if(stateStr == "active") {
        return DCommission::COMMISSION_ACTIVE;
    } else if(stateStr == "awarded") {
        return DCommission::COMMISSION_AWARDED;
    } else if(stateStr == "accepted") {
        return DCommission::COMMISSION_ACCEPTED;
    } else if(stateStr == "completed") {
        return DCommission::COMMISSION_COMPLETED;
    } else if(stateStr == "closed") {
        return DCommission::COMMISSION_CLOSED;
    }

    return DCommission::COMMISSION_PRE_ACTIVE;
}

void DCommissionManager::Add(DCommission& commission)
{
    vCommissions.push_back(commission);
}

void DCommissionManager::Clear()
{
    LOCK(cs);
    vCommissions.clear();
}

std::vector<DCommission>& DCommissionManager::getCommissions() {
    return vCommissions;
}

std::string DCommissionManager::ToString() const
{
    std::ostringstream info;
    info << "Commissions: " << (int)vCommissions.size();
    return info.str();
}