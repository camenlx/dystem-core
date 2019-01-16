// Copyright (c) 2014-2016 The Dash Developers
// Copyright (c) 2018-2019 The Dystem developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COMMISSIONLIST_H
#define COMMISSIONLIST_H

#include "masternode.h"
#include "platformstyle.h"
#include "sync.h"
#include "util.h"

#include <QMenu>
#include <QTimer>
#include <QWidget>

namespace Ui
{
class CommissionsList;
}

class ClientModel;
class WalletModel;

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Commission list page widget */
class CommissionsList : public QWidget
{
    Q_OBJECT

public:
    explicit CommissionsList(QWidget* parent = 0);
    ~CommissionsList();

    void setClientModel(ClientModel* clientModel);
    void setWalletModel(WalletModel* walletModel);

private:

public Q_SLOTS:
    void updateCommissionRow(QString strAlias, QString strAddr, CMasternode* pmn);
    void updateCommissionList(bool fForce = false);

Q_SIGNALS:

private:
    Ui::CommissionsList* ui;
    ClientModel* clientModel;
    WalletModel* walletModel;

private Q_SLOTS:
    void on_tableWidgetCommissions_itemSelectionChanged();
    void on_RefreshButton_clicked();
};
#endif // COMMISSIONLIST_H
