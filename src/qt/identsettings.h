// Copyright (c) 2014-2016 The Dash Developers
// Copyright (c) 2018-2019 The Dystem developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef IDENTSETTINGS_H
#define IDENTSETTINGS_H

#include "walletmodel.h"

#include <QWidget>
#include <QObject>
#include <QDialog>

class AddressTableModel;
class SendCoinsEntry;
class ClientModel;
class OptionsModel;
class WalletModel;

namespace Ui
{
class IdentSettings;
}

QT_BEGIN_NAMESPACE
class QSortFilterProxyModel;
class QModelIndex;
QT_END_NAMESPACE

/** User Ident page widget */
class IdentSettings : public QDialog
{
    Q_OBJECT

public:
    explicit IdentSettings(QWidget* parent = 0);
    ~IdentSettings();

    enum UTXORegistrationState {
            None,
            NotFound,
            NotEnoughFunds,
            Complete
    };

    void setClientModel(ClientModel* clientModel);
    void setWalletModel(WalletModel* walletModel);
    void showEvent(QShowEvent *);

    static CCoinControl* registerCoinControl;
    

private:
    Ui::IdentSettings* ui;
    ClientModel* clientModel;
    WalletModel* walletModel;
    AddressTableModel* addressModel;
    QSortFilterProxyModel* proxyModel;
    std::vector<std::vector<std::string>> addresses;
    bool fNewRecipientAllowed;
    void send(QList<SendCoinsRecipient> recipients, QString strFee, QStringList formatted);
    void processSendCoinsReturn(const WalletModel::SendCoinsReturn& sendCoinsReturn, const QString& msgArg = QString(), bool fPrepare = false);
    void refreshUserAddresses();
    IdentSettings::UTXORegistrationState createUnpsentUTXOListForAddress(std::string address, double ammount);

private slots:
    void addressSelected(const QString& index);
    void upgradeOptionSelected(const QString& index);

signals:
    // Fired when a message should be reported to the user
    void message(const QString& title, const QString& message, unsigned int style);

private Q_SLOTS:
        void on_upgradeAccountButton_clicked();
};
#endif // IDENTSETTINGS_H
