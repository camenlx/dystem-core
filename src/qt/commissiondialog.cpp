// Copyright (c) 2019 The Dystem developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "commissiondialog.h"
#include "ui_commissiondialog.h"
#include "../DystemCore/Util/adaptive_parser.h"

#include "guiconstants.h"
#include "guiutil.h"

//STD / Boost includes
#include <sstream>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <QTextEdit>

using namespace boost::posix_time;
using namespace mylib::datetime;
using namespace std;

CommissionDialog::CommissionDialog(QWidget* parent, bool fMultisigEnabled) : QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
                                                        ui(new Ui::CommissionDialog)
{
    ui->setupUi(this);

    this->setStyleSheet(GUIUtil::loadStyleSheet());

    ui->commissionDetailsTextEdit->setReadOnly(true);
}

CommissionDialog::~CommissionDialog()
{
    delete ui;
}

void CommissionDialog::setCommission(const DCommission commission) {
    currentCommission = new DCommission(commission);

    ui->createdLabelData->setText(QString::fromStdString(commission.createdAt));

    //Format the fee correctly
    std::stringstream feeStream;
    feeStream.precision(8);
    feeStream << fixed << commission.fee;

    //Format the date correctly
    std::string stringDate;
    adaptive_parser parser { adaptive_parser::full_match, { "%Y-%m-%dT%H:%M:%SZ" } };

    try {
        std::stringstream date_str;
        boost::posix_time::ptime pt_1 = boost::posix_time::from_time_t(parser(commission.createdAt).count());
        boost::gregorian::date d = pt_1.date();
        date_str << d.year() << "-" << std::setw(2) << std::setfill('0') << d.month().as_number() << "-" << std::setw(2) << std::setfill('0') << d.day();
        stringDate = date_str.str();
    } catch(std::exception const& e) { }

    std::string fee = feeStream.str();
    fee += " " + commission.coin;

    ui->createdLabelData->setText(QString::fromStdString(stringDate));

    ui->feeLabelData->setText(QString::fromStdString(fee));
    ui->authorLabelData->setText(QString::fromStdString(commission.authorName));
    std::string info(commission.title);
    info += "\n\n" + commission.body;
    ui->commissionDetailsTextEdit->setText(QString::fromStdString(info));
}

void CommissionDialog::showDialogMessage(std::string message) {

    QTimer* timer = new QTimer();
    timer->moveToThread(qApp->thread());
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, [=]()
    {
        dlg.setLoaderText(message);    
        dlg.show();
    });
    QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
}

void CommissionDialog::hideDialogMessage() {
    QTimer* timer = new QTimer();
    timer->moveToThread(qApp->thread());
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, [=]()
    {
       dlg.hide();
    });
    QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
}