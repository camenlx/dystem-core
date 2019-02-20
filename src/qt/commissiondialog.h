// Copyright (c) 2019 The Dystem developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DYSTEM_COMMISSION_DIALOG_H
#define DYSTEM_COMMISSION_DIALOG_H

#include "loaderdialog.h"
#include "../DystemCore/Commissions/DCommission.h"

#include <QDialog>

//class DCommission;

namespace Ui
{
class CommissionDialog;
}

class CommissionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommissionDialog(QWidget* parent = nullptr, bool fMultisigEnabled = false);
    ~CommissionDialog();

    void setCommission(const DCommission commission);

private:
    Ui::CommissionDialog* ui;
    LoaderDialog dlg;
    DCommission* currentCommission;

    void showDialogMessage(std::string message);
    void hideDialogMessage();

};

#endif // DYSTEM_COMMISSION_DIALOG_H
