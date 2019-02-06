// Copyright (c) 2014-2016 The Dash Developers
// Copyright (c) 2018-2019 The Dystem developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

//Dystem includes
#include "identsettings.h"
#include "ui_identsettings.h"
#include "../Dystem/Ident/DIdentManager.h"

//Bitcoin/Dash/PIVX includes
#include "addresstablemodel.h"
#include "guiutil.h"
#include "sendcoinsentry.h"
#include "bitcoinunits.h"
#include "coincontroldialog.h"
#include "clientmodel.h"
#include "optionsmodel.h"
#include "masternode-helpers.h"
#include "coincontrol.h"
#include "init.h"

//STD / Boost includes
#include <QMessageBox>

CCoinControl* IdentSettings::registerCoinControl = new CCoinControl();

IdentSettings::IdentSettings(QWidget* parent) : QDialog(parent),
                                                ui(new Ui::IdentSettings),
                                                clientModel(0),
                                                walletModel(0),
                                                addresses(),
                                                fNewRecipientAllowed(true)
{
    ui->setupUi(this);

    //Add the options available to the user for types of accounts the user may have
    ui->accountLevelComboBox->addItem(QString("None"));
    ui->accountLevelComboBox->addItem(QString("Content creator"));
    ui->accountLevelComboBox->addItem(QString("Commissioner"));

    connect(ui->addressComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(addressSelected(const QString&)));
    connect(ui->accountLevelComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(upgradeOptionSelected(const QString&)));

    //JIM: Leave this in as we will need to test it on Linux. QT doesnt allow for connect SLOTS on QWIDGETS but it does on QOBjects / QDialogs
    //https://stackoverflow.com/questions/25466030/make-qwidget-transparent
    //setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    //setAttribute(Qt::WA_TransparentForMouseEvents);
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

void IdentSettings::addressSelected(const QString& index) {
    if( addresses.size() > 0 && ui->addressComboBox->currentIndex() > -1 ) {
        QString item = ui->addressComboBox->itemData(ui->addressComboBox->currentIndex()).toString();
         std::vector<std::string> rec = addresses.at(ui->addressComboBox->currentIndex());
//TODO: We should be adding this at the point of upgrade
        //identManager.Add(rec.at(1), rec.at(0));
        std::string labelCopy = std::string("The account address you are viewing is ");
        labelCopy += rec.at(0);
        labelCopy += ". You can upgrade your account below. A fee is charged each time you increase the account level.";
        ui->accountInfoLabel->setText(QString(labelCopy.c_str()));

//TODO: Need to read from the ident DB here to see if an entry has already been made for this address and update the upgradeOption accordingly to represent the current status.

    }
}

void IdentSettings::upgradeOptionSelected(const QString& index) {
    if( addresses.size() > 0 && ui->accountLevelComboBox->currentIndex() > -1 ) {
        if( ui->accountLevelComboBox->currentText().toStdString() == "None"){
            ui->upgradeAccountButton->setEnabled(false);
        } else {
            ui->upgradeAccountButton->setEnabled(true);
        }
    }
}
 
void IdentSettings::on_upgradeAccountButton_clicked() {
    if (!walletModel || !walletModel->getOptionsModel())
        return;

    QList<SendCoinsRecipient> recipients;
    recipients.push_back(SendCoinsRecipient());

    //Just to be sure
    registerCoinControl->UnSelectAll();

    //Check all master and super nodes are sunched in the chai before allowing users to upgrade there accounts
    if (!masternodeSync.IsBlockchainSynced()) {
        std::string errorMessage;
        errorMessage += "You must wait until the blockchain has synch'd before upgrading an address.";
        QMessageBox msg;
        msg.setStyleSheet("QLabel{color: #355271;}");
        msg.setText(QString::fromStdString(errorMessage));
        msg.exec();
    } else {

        QString type = ui->accountLevelComboBox->currentText();
        //If none is selected return, the upgrade button should be disabled anyway
        if(type.toStdString() == "None")
            return;

        //Calculate fee based on the block height and select correct signup address
        std::string addressStr;
        CAmount registrationFee = -1;
        if( type.toStdString() == "Content creator" ) {
            addressStr = std::string("D62NsasWcwr6SmBQVq55e9M8cYANLkdUs5");
            registrationFee = identManager.getDeterministicCreatorFee(clientModel->getNumBlocks());
            LogPrintf("NUM OF BLOCKS ARE .... %i FEE %i \n", clientModel->getNumBlocks(),registrationFee);
        } else if ( type.toStdString() == "Commissioner" ) {
            addressStr = std::string("DLwitd7pAH7hqMAed31YFTHRELDonsXwpA");
            registrationFee = identManager.getCommissionerFee(clientModel->getNumBlocks());
        }

        //If the blockheight for when this fee is introduced hasnt passed an error is thrown. Users may only register after the start blockheight.
        if(registrationFee == -1) {
             LogPrintf("NUM OF BLOCKS ARE .... ERROR");
            
            std::string errorMessage("You may not register until the identity system has launched. Please wait until blockheight: ");
            errorMessage += std::to_string(REGISTRATION_START_BLOCKHEIGHT);
            QMessageBox err_block_msg;
            err_block_msg.setStyleSheet("QLabel{color: #355271;}");
            err_block_msg.setText(QString::fromStdString(errorMessage));
            err_block_msg.exec();
            return;
        }

        //Use only this address for content creation. This address signifies the funds locked for content creation.
        CBitcoinAddress address = addressStr;
        recipients[0].address = QString(addressStr.c_str());  

        std::vector<std::string> rec = addresses.at(ui->addressComboBox->currentIndex());
        recipients[0].amount = registrationFee * COIN;  

        //Confifm the address doesnt belong to me first as you shouldnt be sending these payments to yourself. Error check that should never happen.
        if (!walletModel->isMine(address)) {

//TODO: Need to check that the user isn't already in the ident list using this address and they are trying to register twice
            if(getUnspoentUTXOLisForAddress(rec.at(0))) {

            }

//TODO: Need to adjust the payment addresses as we need to send from the currently selectedident address. 
            LogPrintf("SENDING ADDRESS SHOULD BE.... %s \n",rec.at(0));

            QString strFunds = "";
            QString strFee = "";
            recipients[0].inputType = ALL_COINS;
            recipients[0].useSwiftTX = false;

            QStringList formatted;
            // generate bold amount string
            QString amount = "<b>" + BitcoinUnits::formatHtmlWithUnit(walletModel->getOptionsModel()->getDisplayUnit(), recipients[0].amount);
            amount.append("</b> ").append(strFunds);

            // generate monospace address string
            QString address = "<span style='font-family: Open Sans;'>" + recipients[0].address;
            address.append("</span>");

            QString recipientElement;

            if (!recipients[0].paymentRequest.IsInitialized()) // normal payment
            {
                if (recipients[0].label.length() > 0) // label with address
                {
                    recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(recipients[0].label));
                    recipientElement.append(QString(" (%1)").arg(address));
                } else // just address
                {
                    recipientElement = tr("%1 to %2").arg(amount, address);
                }
            } else if (!recipients[0].authenticatedMerchant.isEmpty()) // secure payment request
            {
                recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(recipients[0].authenticatedMerchant));
            } else // insecure payment request
            {
                recipientElement = tr("%1 to %2").arg(amount, address);
            }

            formatted.append(recipientElement);

            fNewRecipientAllowed = false;
                    
            // request unlock only if was locked or unlocked for mixing:
            // this way we let users unlock by walletpassphrase or by menu
            // and make many transactions while unlocking through this dialog
            // will call relock
            WalletModel::EncryptionStatus encStatus = walletModel->getEncryptionStatus();
            if (encStatus == walletModel->Locked || encStatus == walletModel->UnlockedForStakingOnly) {
                WalletModel::UnlockContext ctx(walletModel->requestUnlock(AskPassphraseDialog::Context::Send_DTEM, true));
                if (!ctx.isValid()) {
                    // Unlock wallet was cancelled
                    fNewRecipientAllowed = true;
                    return;
                }
                send(recipients, strFee, formatted);
                return;
            }

            // already unlocked or not encrypted at all
            send(recipients, strFee, formatted);
        } else {
            std::string errorMessage;
            errorMessage += "ERROR: You cannot send registration fees to yourself. You are obviously changing the code trying to hack registration, simply it just wont work.";
            QMessageBox msg;
            msg.setStyleSheet("QLabel{color: #355271;}");
            msg.setText(QString::fromStdString(errorMessage));
            msg.exec();
        }
    }
}

void IdentSettings::send(QList<SendCoinsRecipient> recipients, QString strFee, QStringList formatted)
{
    // prepare transaction for getting txFee earlier
    WalletModelTransaction currentTransaction(recipients);
    WalletModel::SendCoinsReturn prepareStatus;
//TODO: Always append IdentSettings::registerCoinControl here as we are selcting a list of UTXO's from an address
    if (walletModel->getOptionsModel()->getCoinControlFeatures()) // coin control enabled
        prepareStatus = walletModel->prepareTransaction(currentTransaction, CoinControlDialog::coinControl);
    else
        prepareStatus = walletModel->prepareTransaction(currentTransaction);

    // process prepareStatus and on error generate message shown to user
    processSendCoinsReturn(prepareStatus,
        BitcoinUnits::formatWithUnit(walletModel->getOptionsModel()->getDisplayUnit(), currentTransaction.getTransactionFee()), true);

    if (prepareStatus.status != WalletModel::OK) {
        fNewRecipientAllowed = true;
        return;
    }

    CAmount txFee = currentTransaction.getTransactionFee();
    QString questionString = tr("Are you sure you want to send?");
    questionString.append("<br /><br />%1");

    if (txFee > 0) {

        // append fee string if a fee is required
        questionString.append("<hr /><span style='color:green;'>");
        questionString.append(BitcoinUnits::formatHtmlWithUnit(walletModel->getOptionsModel()->getDisplayUnit(), txFee));
        questionString.append("</span> ");
        questionString.append(tr("are added as transaction fee"));
        questionString.append(" ");
        questionString.append(strFee);

        // append transaction size
        questionString.append(" (" + QString::number((double)currentTransaction.getTransactionSize() / 1000) + " kB)");
    }

    // add total amount in all subdivision units
    questionString.append("<hr />");
    CAmount totalAmount = currentTransaction.getTotalTransactionAmount() + txFee;
    QStringList alternativeUnits;
    foreach (BitcoinUnits::Unit u, BitcoinUnits::availableUnits()) {
        if (u != walletModel->getOptionsModel()->getDisplayUnit())
            alternativeUnits.append(BitcoinUnits::formatHtmlWithUnit(u, totalAmount));
    }

    // Show total amount + all alternative units
    questionString.append(tr("Total Amount = <b style='color:orange;'>%1</b><br />= %2")
                              .arg(BitcoinUnits::formatHtmlWithUnit(walletModel->getOptionsModel()->getDisplayUnit(), totalAmount))
                              .arg(alternativeUnits.join("<br />= ")));

    // Limit number of displayed entries to one as we are seding from a single address.
    int messageEntries = formatted.size();
    int displayedEntries = 0;
    for (int i = 0; i < formatted.size(); i++) {
        if (i >= 1) {
            formatted.removeLast();
            i--;
        } else {
            displayedEntries = i + 1;
        }
    }

    questionString.append("<hr />");
    questionString.append(tr("<b style='color:brown;'> (%1 of %2 entries displayed)</b>").arg(displayedEntries).arg(messageEntries));

    // Display message box
    QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Confirm send coins"),
        questionString.arg(formatted.join("<br />")),
        QMessageBox::Yes | QMessageBox::Cancel,
        QMessageBox::Cancel);

    if (retval != QMessageBox::Yes) {
        fNewRecipientAllowed = true;
        return;
    }

    // now send the prepared transaction
    WalletModel::SendCoinsReturn sendStatus = walletModel->sendCoins(currentTransaction);
    // process sendStatus and on error generate message shown to user
    processSendCoinsReturn(sendStatus);

    if (sendStatus.status == WalletModel::OK) {
//TODO: We should update any of the UI or emit any signals here to act upon the 
    }
    fNewRecipientAllowed = true;

    walletModel->setWalletLocked(true);
}

void IdentSettings::processSendCoinsReturn(const WalletModel::SendCoinsReturn& sendCoinsReturn, const QString& msgArg, bool fPrepare)
{
    bool fAskForUnlock = false;
    
    QPair<QString, CClientUIInterface::MessageBoxFlags> msgParams;
    // Default to a warning message, override if error message is needed
    msgParams.second = CClientUIInterface::MSG_WARNING;

    // This comment is specific to SendCoinsDialog usage of WalletModel::SendCoinsReturn.
    // WalletModel::TransactionCommitFailed is used only in WalletModel::sendCoins()
    // all others are used only in WalletModel::prepareTransaction()
    switch (sendCoinsReturn.status) {
    case WalletModel::InvalidAddress:
        msgParams.first = tr("The recipient address is not valid, please recheck.");
        break;
    case WalletModel::InvalidAmount:
        msgParams.first = tr("The amount to pay must be larger than 0.");
        break;
    case WalletModel::AmountExceedsBalance:
        msgParams.first = tr("The amount exceeds your balance.");
        break;
    case WalletModel::AmountWithFeeExceedsBalance:
        msgParams.first = tr("The total exceeds your balance when the %1 transaction fee is included.").arg(msgArg);
        break;
    case WalletModel::DuplicateAddress:
        msgParams.first = tr("Duplicate address found, can only send to each address once per send operation.");
        break;
    case WalletModel::TransactionCreationFailed:
        msgParams.first = tr("Transaction creation failed!");
        msgParams.second = CClientUIInterface::MSG_ERROR;
        break;
    case WalletModel::TransactionCommitFailed:
        msgParams.first = tr("The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here.");
        msgParams.second = CClientUIInterface::MSG_ERROR;
        break;
    case WalletModel::UnlockForStaking:
        // Unlock is only need when the coins are send
        if(!fPrepare)
            fAskForUnlock = true;
        else
            msgParams.first = tr("Error: The wallet is unlocked only to stake coins.");
        break;

    case WalletModel::InsaneFee:
        msgParams.first = tr("A fee %1 times higher than %2 per kB is considered an insanely high fee.").arg(10000).arg(BitcoinUnits::formatWithUnit(walletModel->getOptionsModel()->getDisplayUnit(), ::minRelayTxFee.GetFeePerK()));
        break;
    // included to prevent a compiler warning.
    case WalletModel::OK:
    default:
        return;
    }

    // Unlock wallet if it wasn't fully unlocked already
    if(fAskForUnlock) {
        walletModel->requestUnlock(AskPassphraseDialog::Context::Unlock_Full, false);
        if(walletModel->getEncryptionStatus () != WalletModel::Unlocked) {
            msgParams.first = tr("Error: The wallet was unlocked only to stake coins. Unlock canceled.");
        }
        else {
            // Wallet unlocked
            return;
        }
    }

    clientModel->emit message(tr("Send Coins"), msgParams.first, msgParams.second);
}

bool IdentSettings::getUnspoentUTXOLisForAddress(std::string address)
{
    if (!walletModel || !walletModel->getOptionsModel() || !walletModel->getAddressTableModel())
        return false;

    LogPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>> ATTEMPTING UTXO HUNT LOOKING FOR ADDRESS %s \n", address.c_str() );

    map<QString, vector<COutput>> mapCoins;
    walletModel->listCoins(mapCoins);

    BOOST_FOREACH (PAIRTYPE(QString, vector<COutput>) coins, mapCoins) {
        QString sWalletAddress = coins.first;
        QString sWalletLabel = walletModel->getAddressTableModel()->labelForAddress(sWalletAddress);
        if (sWalletLabel.isEmpty())
            sWalletLabel = tr("(no label)");

        CAmount nSum = 0;
        double dPrioritySum = 0;
        int nChildren = 0;
        int nInputSum = 0;

        LogPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>> FOUND INITIAL ADDRESS %s \n",sWalletAddress.toStdString().c_str());
//TODO: This needs to run from the first 
        for(const COutput& out: coins.second) {

            int nInputSize = 0;
            nSum += out.tx->vout[out.i].nValue;
            nChildren++;

            // address
            CTxDestination outputAddress;
            QString sAddress = "";
            if (ExtractDestination(out.tx->vout[out.i].scriptPubKey, outputAddress)) {
                sAddress = QString::fromStdString(CBitcoinAddress(outputAddress).ToString());

                LogPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>> CHEKCING ADDRESS: %s \n",sAddress.toStdString().c_str());
                // if listMode or change => show Dystem address. In tree mode, address is not shown again for direct wallet address outputs
                if (sAddress == sWalletAddress)
                {
                     LogPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>> MATCH\n");                    

                    CPubKey pubkey;
                    CKeyID* keyid = boost::get<CKeyID>(&outputAddress);
                    if (keyid && walletModel->getPubKey(*keyid, pubkey) && !pubkey.IsCompressed())
                        nInputSize = 29; // 29 = 180 - 151 (public key is 180 bytes, priority free area is 151 bytes)

                    uint256 txhash = out.tx->GetHash();

//TODO: Don't try and include UTXO sets that are locked to avoid messing up 
                    //if (walletModel->isLockedCoin(txhash, out.i)) 
                    {
                        QString sLabel = walletModel->getAddressTableModel()->labelForAddress(sAddress);
                        LogPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>> LABEL: %s \n",sLabel.toStdString().c_str() );
                        LogPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>> Ammount: %f \n", out.tx->vout[out.i].nValue );
                        LogPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>> TXTIME: %ld \n", out.tx->GetTxTime() );
                        LogPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>> CONFIRMATIONS: %ld \n", out.nDepth );
                        double dPriority = ((double)out.tx->vout[out.i].nValue / (nInputSize + 78)) * (out.nDepth + 1); // 78 = 2 * 34 + 10
                        LogPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>> PRIORITY: %f \n", dPriority );
                        dPrioritySum += (double)out.tx->vout[out.i].nValue * (out.nDepth + 1);
                        nInputSum += nInputSize;

                        LogPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>> HASH: %s \n", txhash.GetHex().c_str() );
                        LogPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>> VOUT INDEX: %i \n", out.i );

                        /*
                        // transaction hash
                        uint256 txhash = out.tx->GetHash();

                        // vout index
                        itemOutput->setText(COLUMN_VOUT_INDEX, QString::number(out.i));

                        // disable locked coins
                        if (model->isLockedCoin(txhash, out.i)) {
                            COutPoint outpt(txhash, out.i);
                            coinControl->UnSelect(outpt); // just to be sure
                            itemOutput->setDisabled(true);
                            itemOutput->setIcon(COLUMN_CHECKBOX, QIcon(":/icons/lock_closed"));
                        }

                        // set checkbox
                        if (coinControl->IsSelected(txhash, out.i))
                            itemOutput->setCheckState(COLUMN_CHECKBOX, Qt::Checked);
                        */
                    }
                }
            }
        }
    }

    return true;
}