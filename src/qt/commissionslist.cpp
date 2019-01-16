// Copyright (c) 2014-2016 The Dash Developers
// Copyright (c) 2018-2019 The Dystem developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "commissionslist.h"
#include "ui_commissionslist.h"

#include "clientmodel.h"
#include "guiutil.h"
#include "init.h"
#include "wallet.h"
#include "walletmodel.h"

#include <QMessageBox>

CommissionsList::CommissionsList(QWidget* parent) : QWidget(parent),
                                                  ui(new Ui::CommissionsList),
                                                  clientModel(0),
                                                  walletModel(0)
{
    ui->setupUi(this);

    int columnTitleWidth = 340;
    int columnStartDateWidth = 160;
    int columnCurrencyWidth = 80;
    int columnFeeWidth = 80;
    int columnCommissionerWidth = 230;
    int columnStatusWidth = 130;

    ui->tableWidgetCommissions->setAlternatingRowColors(true);
    ui->tableWidgetCommissions->setColumnWidth(0, columnTitleWidth);
    ui->tableWidgetCommissions->setColumnWidth(1, columnStartDateWidth);
    ui->tableWidgetCommissions->setColumnWidth(2, columnCurrencyWidth);
    ui->tableWidgetCommissions->setColumnWidth(3, columnFeeWidth);
    ui->tableWidgetCommissions->setColumnWidth(4, columnCommissionerWidth);

    ui->tableWidgetCommissions->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->tableWidgetCommissionsCompleted->setAlternatingRowColors(true);
    ui->tableWidgetCommissionsCompleted->setColumnWidth(0, columnTitleWidth);
    ui->tableWidgetCommissionsCompleted->setColumnWidth(1, columnCurrencyWidth);
    ui->tableWidgetCommissionsCompleted->setColumnWidth(2, columnFeeWidth);
    ui->tableWidgetCommissionsCompleted->setColumnWidth(3, columnCommissionerWidth);
    ui->tableWidgetCommissionsCompleted->setColumnWidth(4, columnStatusWidth );

    ui->tableWidgetCommissionsCompleted->setContextMenuPolicy(Qt::CustomContextMenu);


    updateCommissionList(true);
}

CommissionsList::~CommissionsList()
{
    delete ui;
}

void CommissionsList::setClientModel(ClientModel* model)
{
    this->clientModel = model;
}

void CommissionsList::setWalletModel(WalletModel* model)
{
    this->walletModel = model;
}

/*
//DO NOT REMOVE EXAPLE OF HOW TO SHOW A MESSAGE BOX
void CommissionsList::StartAlias(std::string strAlias)
{
    std::string strStatusHtml;
    strStatusHtml += "<center>Alias: " + strAlias;

    BOOST_FOREACH (CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
        if (mne.getAlias() == strAlias) {
            std::string strError;
            CMasternodeBroadcast mnb;

            bool fSuccess = CMasternodeBroadcast::Create(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), strError, mnb);

            if (fSuccess) {
                strStatusHtml += "<br>Successfully started masternode.";
                mnodeman.UpdateMasternodeList(mnb);
                mnb.Relay();
            } else {
                strStatusHtml += "<br>Failed to start masternode.<br>Error: " + strError;
            }
            break;
        }
    }
    strStatusHtml += "</center>";

    QMessageBox msg;
    msg.setText(QString::fromStdString(strStatusHtml));
    msg.exec();

    updateMyNodeList(true);
}
*/

void CommissionsList::updateCommissionRow(QString strAlias, QString strAddr, CMasternode* pmn)
{
    /*
    LOCK(cs_mnlistupdate);
    bool fOldRowFound = false;
    int nNewRow = 0;

    for (int i = 0; i < ui->tableWidgetCommissions->rowCount(); i++) {
        if (ui->tableWidgetCommissions->item(i, 0)->text() == strAlias) {
            fOldRowFound = true;
            nNewRow = i;
            break;
        }
    }

    if (nNewRow == 0 && !fOldRowFound) {
        nNewRow = ui->tableWidgetCommissions->rowCount();
        ui->tableWidgetCommissions->insertRow(nNewRow);
    }

    QTableWidgetItem* aliasItem = new QTableWidgetItem(strAlias);
    QTableWidgetItem* addrItem = new QTableWidgetItem(pmn ? QString::fromStdString(pmn->addr.ToString()) : strAddr);
    QTableWidgetItem* protocolItem = new QTableWidgetItem(QString::number(pmn ? pmn->protocolVersion : -1));
    QTableWidgetItem* statusItem = new QTableWidgetItem(QString::fromStdString(pmn ? pmn->GetStatus() : "MISSING"));
    GUIUtil::DHMSTableWidgetItem* activeSecondsItem = new GUIUtil::DHMSTableWidgetItem(pmn ? (pmn->lastPing.sigTime - pmn->sigTime) : 0);
    QTableWidgetItem* lastSeenItem = new QTableWidgetItem(QString::fromStdString(DateTimeStrFormat("%Y-%m-%d %H:%M", pmn ? pmn->lastPing.sigTime : 0)));
    QTableWidgetItem* pubkeyItem = new QTableWidgetItem(QString::fromStdString(pmn ? CBitcoinAddress(pmn->pubKeyCollateralAddress.GetID()).ToString() : ""));

    ui->tableWidgetCommissions->setItem(nNewRow, 0, aliasItem);
    ui->tableWidgetCommissions->setItem(nNewRow, 1, addrItem);
    ui->tableWidgetCommissions->setItem(nNewRow, 2, protocolItem);
    ui->tableWidgetCommissions->setItem(nNewRow, 3, statusItem);
    ui->tableWidgetCommissions->setItem(nNewRow, 4, activeSecondsItem);
    ui->tableWidgetCommissions->setItem(nNewRow, 5, lastSeenItem);
    ui->tableWidgetCommissions->setItem(nNewRow, 6, pubkeyItem);
    */
}

void CommissionsList::updateCommissionList(bool fForce)
{
    //static int64_t nTimeMyListUpdated = 0;

    // automatically update my masternode list only once in MY_MASTERNODELIST_UPDATE_SECONDS seconds,
    // this update still can be triggered manually at any time via button click

    /*
    int64_t nSecondsTillUpdate = nTimeMyListUpdated + MY_MASTERNODELIST_UPDATE_SECONDS - GetTime();
    ui->secondsLabel->setText(QString::number(nSecondsTillUpdate));

    if (nSecondsTillUpdate > 0 && !fForce) return;
    nTimeMyListUpdated = GetTime();

    ui->tableWidgetCommissions->setSortingEnabled(false);
    BOOST_FOREACH (CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
        int nIndex;
        if(!mne.castOutputIndex(nIndex))
            continue;

        CTxIn txin = CTxIn(uint256S(mne.getTxHash()), uint32_t(nIndex));
        CMasternode* pmn = mnodeman.Find(txin);
        updateCommissionRow(QString::fromStdString(mne.getAlias()), QString::fromStdString(mne.getIp()), pmn);
    }
    ui->tableWidgetCommissions->setSortingEnabled(true);

    // reset "timer"
    ui->secondsLabel->setText("0");
    */
}

void CommissionsList::on_tableWidgetCommissions_itemSelectionChanged()
{
    /*
    if (ui->tableWidgetCommissions->selectedItems().count() > 0) {
        ui->startButton->setEnabled(true);
    }
    */
}

void CommissionsList::on_RefreshButton_clicked()
{
    //updateCommissionList(true);
}
