// Copyright (c) 2011-2014 The Bitcoin developers
// Copyright (c) 2019 The Dystem developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

//Dystem includes
#include "loaderdialog.h"
#include "ui_loaderdialog.h"

//Bitcoin/Dash/PIVX includes
#include "guiutil.h"
#include "wallet.h"

//STD / Boost / QT includes
#include <QString>
#include <QPainter>

using namespace std;

LoaderDialog::LoaderDialog(QWidget* parent, bool fMultisigEnabled) : QDialog(parent, Qt::CustomizeWindowHint),
                                                        ui(new Ui::LoaderDialog),
                                                        isShown(0)
{
    ui->setupUi(this);

    /* Open CSS when configured */
    this->setStyleSheet(GUIUtil::loadStyleSheet());

    //Load spinner assets   
    spinnerMap = QPixmap(":/movies/spinner");
    
    update();

    timer = new QTimer(this);
    timer->moveToThread(qApp->thread());
    connect(timer, SIGNAL(timeout()), this, SLOT(spinWeasel()));
    timer->start(60);
}

LoaderDialog::~LoaderDialog()
{
    delete ui;
}

void LoaderDialog::show() {
    if(!isShown) {
        isShown = true;
        exec();
    }
}

void LoaderDialog::hide() {
    if(isShown) {
        isShown = false;
        close();
    }
}

void LoaderDialog::setLoaderText(std::string message)
{
    ui->loaderMessage->setText(QString::fromStdString(message) + "…");
}

void LoaderDialog::spinWeasel() {

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