// Copyright (c) 2018-2019 The Dystem developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

//Dystem includes
#include "ui_commissionslist.h"
#include "commissionslist.h"
#include "../Dystem/Commissions/DCommissionManager.h"
#include "../Dystem/Util/adaptive_parser.h"

//Bitcoin/Dash/PIVX includes
#include "clientmodel.h"
#include "guiutil.h"
#include "init.h"
#include "wallet.h"
#include "walletmodel.h"

//STD / Boost includes
#include <sstream>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
  
//QT includes
#include <QMessageBox>

CCriticalSection cs_commissions;

using namespace boost::posix_time;
using namespace mylib::datetime;
using namespace std;

CommissionsList::CommissionsList(QWidget* parent) : QWidget(parent),
                                                  ui(new Ui::CommissionsList),
                                                  clientModel(0),
                                                  walletModel(0)
{
    ui->setupUi(this);

    int columnTitleWidth = 300;
    int columnStartDateWidth = 160;
    int columnCurrencyWidth = 80;
    int columnFeeWidth = 120;
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

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateCommissionList()));
    timer->start(1000);

    updateCommissionList();
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

void CommissionsList::updateCommissionRow(DCommission com)
{
    //TODO: MUST AD VALIDATION TO MAKE TITLES UNIQUE
    LOCK(cs_commissions);
    //If the commission active or being worked on show it in the active list
    QString strTitle = QString::fromStdString(com.title);
     
    //Format the fee correctly
    std::stringstream feeStream;
    feeStream.precision(8);
    feeStream << fixed << com.fee;

    //Format the date correctly
    //TODO: Move this out to a date format helper class. 
    std::string stringDate;
    adaptive_parser parser { adaptive_parser::full_match, { "%Y-%m-%dT%H:%M:%SZ" } };

    try {
        std::stringstream date_str;
        boost::posix_time::ptime pt_1 = boost::posix_time::from_time_t(parser(com.createdAt).count());
        boost::gregorian::date d = pt_1.date();
        date_str << d.year() << "-" << std::setw(2) << std::setfill('0') << d.month().as_number() << "-" << std::setw(2) << std::setfill('0') << d.day();
        stringDate = date_str.str();
    } catch(std::exception const& e) { }

    if(com.state == DCommission::COMMISSION_ACTIVE ||
        com.state == DCommission::COMMISSION_AWARDED ||
        com.state == DCommission::COMMISSION_ACCEPTED) {        
        bool fOldRowFound = false;
        int nNewRow = 0;

        for (int i = 0; i < ui->tableWidgetCommissions->rowCount(); i++) {
            if (ui->tableWidgetCommissions->item(i, 0)->text() == strTitle) {
                fOldRowFound = true;
                nNewRow = i;
                break;
            }
        }

        if (nNewRow == 0 && !fOldRowFound) {
            nNewRow = ui->tableWidgetCommissions->rowCount();
            ui->tableWidgetCommissions->insertRow(nNewRow);
        }

        QTableWidgetItem* titleItem = new QTableWidgetItem(strTitle);
        QTableWidgetItem* startItem = new QTableWidgetItem(QString::fromStdString(stringDate));
        QTableWidgetItem* currencyItem = new QTableWidgetItem(QString::fromStdString(com.coin));
        QTableWidgetItem* feeItem = new QTableWidgetItem(QString::fromStdString(feeStream.str()));
        QTableWidgetItem* commissionerItem = new QTableWidgetItem(QString::fromStdString(com.authorName));

        ui->tableWidgetCommissions->setItem(nNewRow, 0, titleItem);
        ui->tableWidgetCommissions->setItem(nNewRow, 1, startItem);
        ui->tableWidgetCommissions->setItem(nNewRow, 2, currencyItem);
        ui->tableWidgetCommissions->setItem(nNewRow, 3, feeItem);
        ui->tableWidgetCommissions->setItem(nNewRow, 4, commissionerItem);

    } else {
        //If a commission is closed or completed show it in the completed list. 
        //If it has been culled from the data chain it wnot be shown in either list.
        bool fOldRowFound = false;
        int nNewRow = 0;

        for (int i = 0; i < ui->tableWidgetCommissionsCompleted->rowCount(); i++) {
            if (ui->tableWidgetCommissionsCompleted->item(i, 0)->text() == strTitle) {
                fOldRowFound = true;
                nNewRow = i;
                break;
            }
        }

        if (nNewRow == 0 && !fOldRowFound) {
            nNewRow = ui->tableWidgetCommissionsCompleted->rowCount();
            ui->tableWidgetCommissionsCompleted->insertRow(nNewRow);
        }

        QTableWidgetItem* titleItem = new QTableWidgetItem(strTitle);
        QTableWidgetItem* currencyItem = new QTableWidgetItem(QString::fromStdString(com.coin));
        QTableWidgetItem* feeItem = new QTableWidgetItem(QString::fromStdString(feeStream.str()));
        QTableWidgetItem* commissionerItem = new QTableWidgetItem(QString::fromStdString(com.authorName));
        QTableWidgetItem* statusItem = new QTableWidgetItem(QString::fromStdString(com.finalStatus));

        ui->tableWidgetCommissionsCompleted->setItem(nNewRow, 0, titleItem);
        ui->tableWidgetCommissionsCompleted->setItem(nNewRow, 1, currencyItem);
        ui->tableWidgetCommissionsCompleted->setItem(nNewRow, 2, feeItem);
        ui->tableWidgetCommissionsCompleted->setItem(nNewRow, 3, commissionerItem);
        ui->tableWidgetCommissionsCompleted->setItem(nNewRow, 4, statusItem);
    }
}

void CommissionsList::updateCommissionList()
{
    static int64_t nTimeMyListUpdated = 0;

    // automatically update commissions
    //Note whilst API driven still need to press refersh to get new commissions
    int64_t nSecondsTillUpdate = nTimeMyListUpdated + COMMISSIONS_REFRESH_SECONDS - GetTime();
    if (nSecondsTillUpdate > 0) return;
        nTimeMyListUpdated = GetTime();

    //Loop through the commissions and populate each list respecitvely
    ui->tableWidgetCommissions->setSortingEnabled(false);
    ui->tableWidgetCommissionsCompleted->setSortingEnabled(false);
    BOOST_FOREACH (DCommission com, commissionsmanager.getCommissions()) {
        updateCommissionRow(com);
    }
    ui->tableWidgetCommissions->setSortingEnabled(true);
    ui->tableWidgetCommissionsCompleted->setSortingEnabled(true);
}

void CommissionsList::on_tableWidgetCommissions_itemSelectionChanged()
{
    if (ui->tableWidgetCommissions->selectedItems().count() > 0) {
        QTableWidgetItem* item = ui->tableWidgetCommissions->selectedItems().at(0);

        std::string errorMessage;
        errorMessage += "WARNING: Feature still in devlelopment. The commissions \'" + item->text().toStdString() + "\' can not be applied for presently in the wallet. This is currently under development.";
        
        QMessageBox msg;
        msg.setStyleSheet("QLabel{color: #355271;}");
        msg.setText(QString::fromStdString(errorMessage));
        msg.exec();
    }
}

void CommissionsList::on_tableWidgetCommissionsCompleted_itemSelectionChanged()
{
    if (ui->tableWidgetCommissionsCompleted->selectedItems().count() > 0) {
        QTableWidgetItem* item = ui->tableWidgetCommissionsCompleted->selectedItems().at(0);
        QTableWidgetItem* status = ui->tableWidgetCommissionsCompleted->selectedItems().at(4);

        std::string errorMessage;
        errorMessage += "ERROR: You can not apply for a completed commission. The commissions \'" + item->text().toStdString() + "\' has been " + status->text().toStdString() + ".";
        
        QMessageBox msg;
        msg.setStyleSheet("QLabel{color: #355271;}");
        msg.setText(QString::fromStdString(errorMessage));
        msg.exec();
    }
}

void CommissionsList::on_refreshButton_clicked()
{
    DCommisionsDB commissiondb;
    commissiondb.Read(commissionsmanager, false);
}
