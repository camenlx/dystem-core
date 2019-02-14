// Copyright (c) 2011-2013 The Bitcoin developers
// Copyright (c) 2019 The Dystem developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef LOADERDIALOG_H
#define LOADERDIALOG_H

#include <QAbstractButton>
#include <QTimer>
#include <QDialog>

class WalletModel;

namespace Ui
{
class LoaderDialog;
}

class LoaderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoaderDialog(QWidget* parent = nullptr, bool fMultisigEnabled = false);
    ~LoaderDialog();

    void setModel(WalletModel* model);
 
private:
    Ui::LoaderDialog* ui;
    WalletModel* model;
    QTimer* timer;
    QPixmap spinnerMap;
    int spinRot;

public Q_SLOTS:
    void update();

private slots:
    void buttonBoxClicked(QAbstractButton*);

};

#endif // LOADERDIALOG_H
