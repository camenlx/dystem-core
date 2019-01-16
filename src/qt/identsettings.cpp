// Copyright (c) 2014-2016 The Dash Developers
// Copyright (c) 2018-2019 The Dystem developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "identsettings.h"
#include "ui_identsettings.h"

#include "clientmodel.h"
#include "guiutil.h"
#include "init.h"
#include "wallet.h"
#include "walletmodel.h"

#include <QMessageBox>

IdentSettings::IdentSettings(QWidget* parent) : QWidget(parent),
                                                  ui(new Ui::IdentSettings),
                                                  clientModel(0),
                                                  walletModel(0)
{
    ui->setupUi(this);
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
}
