// Copyright (c) 2011-2013 The Bitcoin developers
// Copyright (c) 2019 The Dystem developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef LOADERDIALOG_H
#define LOADERDIALOG_H

//STD / Boost / QT includes
#include <QAbstractButton>
#include <QTimer>
#include <QDialog>

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

    void setLoaderText(std::string message);
    void show();
    void hide();
 
private:
    Ui::LoaderDialog* ui;
    QTimer* timer;
    QPixmap spinnerMap;
    int spinRot;
    bool isShown;

private slots:
    void spinWeasel();

};

#endif // LOADERDIALOG_H
