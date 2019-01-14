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

/** Masternode Manager page widget */
class IdentSettings : public QWidget
{
    Q_OBJECT

public:
    explicit IdentSettings(QWidget* parent = 0);
    ~IdentSettings();

    void setClientModel(ClientModel* clientModel);
    void setWalletModel(WalletModel* walletModel);
    void StartAlias(std::string strAlias);
    void StartAll(std::string strCommand = "start-all");

private:
    QMenu* contextMenu;
    int64_t nTimeFilterUpdated;
    bool fFilterUpdated;

public Q_SLOTS:
    void updateMyMasternodeInfo(QString strAlias, QString strAddr, CMasternode* pmn);
    void updateMyNodeList(bool fForce = false);

Q_SIGNALS:

private:
    QTimer* timer;
    Ui::IdentSettings* ui;
    ClientModel* clientModel;
    WalletModel* walletModel;
    CCriticalSection cs_mnlistupdate;
    QString strCurrentFilter;

private Q_SLOTS:
    void showContextMenu(const QPoint&);
    void on_startButton_clicked();
    void on_startAllButton_clicked();
    void on_startMissingButton_clicked();
    void on_tableWidgetMyMasternodes_itemSelectionChanged();
    void on_UpdateButton_clicked();
};
#endif // IDENTSETTINGS_H
