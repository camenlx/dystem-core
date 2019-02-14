// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2019 The Dystem developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "loaderdialog.h"
#include "ui_loaderdialog.h"

#include "guiutil.h"
#include "wallet.h"

#include <QString>
#include <QPainter>

using namespace std;

LoaderDialog::LoaderDialog(QWidget* parent, bool fMultisigEnabled) : QDialog(parent, Qt::CustomizeWindowHint),
                                                        ui(new Ui::LoaderDialog),
                                                        model(0)
{
    ui->setupUi(this);

    /* Open CSS when configured */
    this->setStyleSheet(GUIUtil::loadStyleSheet());

    //Load spinner assets
    spinnerMap = QPixmap(":/movies/spinner");
    
    update();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(60);
}

LoaderDialog::~LoaderDialog()
{
    delete ui;
}

void LoaderDialog::setModel(WalletModel* model)
{
    this->model = model;
}

void LoaderDialog::update() {

    spinRot += 15;

    QPixmap rotate(spinnerMap.size());
    QPainter p(&rotate);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);   
    p.setRenderHint(QPainter::HighQualityAntialiasing); 
    p.fillRect(rect(), QBrush(QColor(245,245,245)));
    p.translate(rotate.size().width() / 2 ,rotate.size().height() / 2);
    p.rotate(spinRot);
    p.translate(-rotate.size().width() / 2, -rotate.size().height() / 2); 
    p.drawPixmap(0, 0, spinnerMap);

    p.end();

    ui->spinnerIcon->setPixmap(rotate);
}

// ok button
void LoaderDialog::buttonBoxClicked(QAbstractButton* button)
{
    /*
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::AcceptRole)
        done(QDialog::Accepted); // closes the dialog
    */
}