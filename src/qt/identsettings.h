// Copyright (c) 2014-2016 The Dash Developers
// Copyright (c) 2018-2019 The Dystem developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef IDENTSETTINGS_H
#define IDENTSETTINGS_H

#include "masternode.h"
#include "platformstyle.h"
#include "sync.h"
#include "util.h"

#include <QMenu>
#include <QTimer>
#include <QWidget>

#define MY_MASTERNODELIST_UPDATE_SECONDS 60
#define MASTERNODELIST_UPDATE_SECONDS 15
#define MASTERNODELIST_FILTER_COOLDOWN_SECONDS 3

namespace Ui
{
class IdentSettings;
}

class ClientModel;
class WalletModel;

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** User Ident page widget */
class IdentSettings : public QWidget
{
    Q_OBJECT

public:
    explicit IdentSettings(QWidget* parent = 0);
    ~IdentSettings();

    void setClientModel(ClientModel* clientModel);
    void setWalletModel(WalletModel* walletModel);

private:
    QMenu* contextMenu;

public Q_SLOTS:

Q_SIGNALS:

private:
    Ui::IdentSettings* ui;
    ClientModel* clientModel;
    WalletModel* walletModel;

private Q_SLOTS:

};
#endif // IDENTSETTINGS_H
