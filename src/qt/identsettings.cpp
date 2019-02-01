// Copyright (c) 2014-2016 The Dash Developers
// Copyright (c) 2018-2019 The Dystem developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

//Dystem includes
#include "identsettings.h"
#include "ui_identsettings.h"
#include "../Dystem/Ident/DIdentManager.h"

//Bitcoin/Dash/PIVX includes
#include "clientmodel.h"
#include "addresstablemodel.h"
#include "guiutil.h"
#include "init.h"
#include "wallet.h"
#include "walletmodel.h"

//STD / Boost includes
#include <QMessageBox>
#include <QSortFilterProxyModel>

IdentSettings::IdentSettings(QWidget* parent) : QWidget(parent),
                                                  ui(new Ui::IdentSettings),
                                                  clientModel(0),
                                                  walletModel(0)
{
    ui->setupUi(this);

    //Add the options available to the user for types of accounts the user may have
    ui->accountLevelComboBox->addItem(QString("None"));
    ui->accountLevelComboBox->addItem(QString("Content creator"));
    ui->accountLevelComboBox->addItem(QString("Commissioner"));
}

IdentSettings::~IdentSettings()
{
    delete ui;
}

void IdentSettings::setClientModel(ClientModel* model)
{
    this->clientModel = model;
}

void IdentSettings::setWalletModel(WalletModel* model)
{
    this->walletModel = model;
    refreshUserAddresses();
}

void IdentSettings::refreshUserAddresses() {
    if (!this->walletModel) {
        return;
    } else {
        if(!walletModel->getAddressTableModel())
            return;

        //First clear down the list
        ui->addressComboBox->clear();

        //Add the users addresses to the address drop down
        addresses = walletModel->getAddressTableModel()->listMyAddrreses();
        for ( std::vector<std::vector<std::string>>::const_iterator it = addresses.begin(); it != addresses.end(); ++it) {
            std::vector<std::string> rec = *it;
            ui->addressComboBox->addItem(QString::fromStdString(rec.at(1)) + " : " + QString::fromStdString(rec.at(0)));
        }
    }
}

void IdentSettings::showEvent(QShowEvent *)
{
    refreshUserAddresses();
}

void IdentSettings::on_selectAddressButton_clicked() {
    QString item = ui->addressComboBox->itemData(ui->addressComboBox->currentIndex()).toString();
    QString text = ui->addressComboBox->currentText();
    std::vector<std::string> rec = addresses.at(ui->addressComboBox->currentIndex());
    identManager.Add(rec.at(1), rec.at(0));
}
        
void IdentSettings::on_upgradeAccountComboBox_clicked() {
    std::string errorMessage;
    errorMessage += "WARNING: Feature still in development. Idents can't be upgraded yet. This is currently under development.";
    QMessageBox msg;
    msg.setStyleSheet("QLabel{color: #355271;}");
    msg.setText(QString::fromStdString(errorMessage));
    msg.exec();
}