// Copyright (c) 2014-2016 The Dash Developers
// Copyright (c) 2018-2019 The Dystem developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef IDENTSETTINGS_H
#define IDENTSETTINGS_H

#include "util.h"

#include <QWidget>

class AddressTableModel;

namespace Ui
{
class IdentSettings;
}

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
QT_END_NAMESPACE

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
    void showEvent(QShowEvent *);

private:
    Ui::IdentSettings* ui;
    ClientModel* clientModel;
    WalletModel* walletModel;
    AddressTableModel* addressModel;
    QSortFilterProxyModel* proxyModel;
    std::vector<std::vector<std::string>> addresses;

    void refreshUserAddresses();

private Q_SLOTS:
        void on_selectAddressButton_clicked();
        void on_upgradeAccountComboBox_clicked();
};
#endif // IDENTSETTINGS_H
