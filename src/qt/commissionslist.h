// Copyright (c) 2018-2019 The Dystem developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef COMMISSIONLIST_H
#define COMMISSIONLIST_H

#include "../Dystem/Commissions/DCommission.h"

#include "masternode.h"
#include "platformstyle.h"
#include "sync.h"
#include "util.h"

#include <QMenu>
#include <QTimer>
#include <QWidget>

#define COMMISSIONS_REFRESH_SECONDS 10

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
    int64_t nTimeFilterUpdated;

    public Q_SLOTS:
    void updateCommissionRow(DCommission com);
    void updateCommissionList();

    Q_SIGNALS:

    private:
        QTimer* timer;
        Ui::CommissionsList* ui;
        ClientModel* clientModel;
        WalletModel* walletModel;
        CCriticalSection cs_commissions;

    private Q_SLOTS:
        void on_tableWidgetCommissions_itemSelectionChanged();
        void on_tableWidgetCommissionsCompleted_itemSelectionChanged();
        void on_refreshButton_clicked();
};
#endif // COMMISSIONLIST_H
