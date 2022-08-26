/**
 *  Copyright 2022, raprepo.
 *  Created by raprepo on 2022/8/24.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include "ui_config.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileInfo>
#include <QDir>
#include <QLabel>
#include <QProcessEnvironment>
#include <QNetworkInterface>


ConfigView::ConfigView(ConfigVars &config, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

    this->crtFileLine = new QLineEdit(this);
    this->keyFileLine = new QLineEdit(this);
    this->pktFileLine = new QLineEdit(this);
    this->hostFileLine = new QLineEdit(this);
    this->serverCombo = new QComboBox(this);
    this->portSpin = new QSpinBox(this);
    this->methodCombo = new QComboBox(this);
    this->passwordLine = new QLineEdit(this);

    this->crtFileLine->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->keyFileLine->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->pktFileLine->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    this->hostFileLine->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    if ( config.crtFile.isEmpty() ) {
        auto s = QString("%1/res/root.crt").arg(QApplication::applicationDirPath());
        s = QDir::toNativeSeparators(s);
        config.crtFile = s;
    }
    this->crtFileLine->setText(config.crtFile);

    if ( config.keyFile.isEmpty() ) {
        auto s = QString("%1/res/root.key").arg(QApplication::applicationDirPath());
        s = QDir::toNativeSeparators(s);
        config.keyFile = s;
    }
    this->keyFileLine->setText(config.keyFile);

    if ( config.pktFile.isEmpty() ) {
        auto s = QString("%1/pkts.pcap").arg(QApplication::applicationDirPath());
        s = QDir::toNativeSeparators(s);
        config.pktFile = s;
    }
    this->pktFileLine->setText(config.pktFile);

    if ( config.hostFile.isEmpty() ) {
        auto path = QProcessEnvironment::systemEnvironment().value("HOME");
        auto s = QString("%1/.config/wireshark/hosts").arg(path);
        config.hostFile = s;
    }
    this->hostFileLine->setText(config.hostFile);

    if ( config.method.isEmpty() ) {
        auto s = "AES-256-CFB";
        config.method = s;
    }
    this->methodCombo->addItem(config.method);

    if ( config.password.isEmpty() ) {
        auto s = "123456";
        config.password = s;
    }
    this->passwordLine->setText(config.password);

    this->portSpin->setMinimum(0);
    this->portSpin->setMaximum(65535);
    if ( 0 == config.port )
        config.port = 24500;
    this->portSpin->setValue(config.port);

    auto iplist = QNetworkInterface::allAddresses();
    foreach ( auto &addr, iplist ) {
        if ( addr.protocol() == QAbstractSocket::IPv4Protocol ) {
            this->serverCombo->addItem(addr.toString());
        }
    }
    if ( !config.server.isNull() ) {
        this->serverCombo->addItem(config.server.toString());
    }

    auto btnSelectCrt = new QPushButton("SELECT", this);
    QObject::connect(
        btnSelectCrt,
        &QPushButton::clicked,
        this,
        [=]() { this->selectClicked(SELECT_CRT); }
    );
    auto btnSelectKey = new QPushButton("SELECT", this);
    QObject::connect(
        btnSelectKey,
        &QPushButton::clicked,
        this,
        [=]() { this->selectClicked(SELECT_KEY); }
    );
    auto btnSelectPkt = new QPushButton("SELECT", this);
    QObject::connect(
        btnSelectPkt,
        &QPushButton::clicked,
        this,
        [=]() { this->selectClicked(SELECT_PKT); }
    );
    auto btnSelectHost = new QPushButton("SELECT", this);
    QObject::connect(
        btnSelectHost,
        &QPushButton::clicked,
        this,
        [=]() { this->selectClicked(SELECT_HOST); }
    );

    auto labelAddr = new QLabel("ADDR: ", this);
    auto labelMthd = new QLabel("MTHD: ", this);
    auto labelPswd = new QLabel("PSWD: ", this);
    auto labelCert = new QLabel("CERT: ", this);
    auto labelSKey = new QLabel("SKEY: ", this);
    auto labelPkts = new QLabel("PKTS: ", this);
    auto labelHost = new QLabel("HOST: ", this);

    auto hlayout1 = new QHBoxLayout();
    hlayout1->addWidget(labelAddr);
    hlayout1->addWidget(this->serverCombo, 1);
    hlayout1->addWidget(this->portSpin);
    hlayout1->setSpacing(0);
    hlayout1->setMargin(0);

    auto hlayout2 = new QHBoxLayout();
    hlayout2->addWidget(labelMthd);
    hlayout2->addWidget(this->methodCombo, 1);
    hlayout2->setSpacing(0);
    hlayout2->setMargin(0);

    auto hlayout3 = new QHBoxLayout();
    hlayout3->addWidget(labelPswd);
    hlayout3->addWidget(this->passwordLine);
    hlayout3->setSpacing(0);
    hlayout3->setMargin(0);

    auto hlayout4 = new QHBoxLayout();
    hlayout4->addWidget(labelCert);
    hlayout4->addWidget(this->crtFileLine);
    hlayout4->addWidget(btnSelectCrt);
    hlayout4->setSpacing(0);
    hlayout4->setMargin(0);

    auto hlayout5 = new QHBoxLayout();
    hlayout5->addWidget(labelSKey);
    hlayout5->addWidget(this->keyFileLine);
    hlayout5->addWidget(btnSelectKey);
    hlayout5->setSpacing(0);
    hlayout5->setMargin(0);

    auto hlayout6 = new QHBoxLayout();
    hlayout6->addWidget(labelPkts);
    hlayout6->addWidget(this->pktFileLine);
    hlayout6->addWidget(btnSelectPkt);
    hlayout6->setSpacing(0);
    hlayout6->setMargin(0);

    auto hlayout7 = new QHBoxLayout();
    hlayout7->addWidget(labelHost);
    hlayout7->addWidget(this->hostFileLine);
    hlayout7->addWidget(btnSelectHost);
    hlayout7->setSpacing(0);
    hlayout7->setMargin(0);

    auto btnConfirm = new QPushButton("CONFIRM", this);
    QObject::connect(
        btnConfirm,
        &QPushButton::clicked,
        this,
        &ConfigView::confirmClicked
    );
    auto btnClose = new QPushButton("CLOSE", this);
    QObject::connect(
        btnClose,
        &QPushButton::clicked,
        this,
        &ConfigView::close
    );

    auto hlayout8 = new QHBoxLayout();
    hlayout8->addStretch();
    hlayout8->addWidget(btnClose);
    hlayout8->addWidget(btnConfirm);

    auto gb1 = new QGroupBox("SS", this);
    auto layoutgb1 = new QVBoxLayout();
    layoutgb1->addLayout(hlayout1);
    layoutgb1->addLayout(hlayout2);
    layoutgb1->addLayout(hlayout3);
    layoutgb1->setMargin(0);
    layoutgb1->setSpacing(0);
    gb1->setLayout(layoutgb1);

    auto gb2 = new QGroupBox("TLS", this);
    auto layoutgb2 = new QVBoxLayout();
    layoutgb2->addLayout(hlayout4);
    layoutgb2->addLayout(hlayout5);
    layoutgb2->addLayout(hlayout6);
    layoutgb2->addLayout(hlayout7);
    layoutgb2->setMargin(0);
    layoutgb2->setSpacing(0);
    gb2->setLayout(layoutgb2);

    auto layout = new QVBoxLayout();
    layout->addWidget(gb1);
    layout->addWidget(gb2);
    layout->addStretch();
    layout->addLayout(hlayout8);
    layout->setMargin(0);
    this->setLayout(layout);

    this->resize(600, 300);

    this->mconfig = config;
}

void ConfigView::confirmClicked() {

    this->mconfig.crtFile = this->crtFileLine->text();
    this->mconfig.keyFile = this->keyFileLine->text();
    this->mconfig.hostFile = this->hostFileLine->text();
    this->mconfig.pktFile = this->pktFileLine->text();

    this->mconfig.port = this->portSpin->value();
    this->mconfig.server = QHostAddress(this->serverCombo->currentText());
    this->mconfig.password = this->passwordLine->text();

    if ( this->mconfig.server.isNull() || QAbstractSocket::IPv4Protocol != this->mconfig.server.protocol() ) {
        QMessageBox::information(this, "INFO", "Select a Valid IP Address");
        return;
    }
    if ( this->mconfig.port <= 0 || this->mconfig.port >= 65535 ) {
        QMessageBox::information(this, "INFO", "Select a Valid Port");
        return;
    }
    if ( this->mconfig.password.isEmpty() || this->mconfig.password.size() >= 64 ) {
        QMessageBox::information(this, "INFO", "Input a Valid Password");
        return;
    }
    if ( !QFileInfo::exists(this->mconfig.crtFile) ) {
        QMessageBox::information(this, "INFO", "Cert File Not Exists");
        return;
    }
    if ( !QFileInfo::exists(this->mconfig.keyFile) ) {
        QMessageBox::information(this, "INFO", "Key File Not Exists");
        return;
    }
    if ( !QDir().mkpath(QFileInfo(this->mconfig.pktFile).absolutePath()) ) {
        QMessageBox::information(this, "INFO", "Create Folder For Pkt File Failed");
        return;
    }
    if ( !QDir().mkpath(QFileInfo(this->mconfig.hostFile).absolutePath()) ) {
        QMessageBox::information(this, "INFO", "Create Folder For Host File Failed");
        return;
    }

    emit this->configConfirm(this->mconfig);

    this->close();
}

void ConfigView::selectClicked(int type) {

    QString filter;
    QString dir;
    bool save = false;

    switch ( type ) {
    case SELECT_CRT:
        filter = "Cert File (*.crt *.cer)";
        dir = QFileInfo(this->mconfig.crtFile).absolutePath();
        break;
    case SELECT_KEY:
        filter = "Key File (*.key)";
        dir = QFileInfo(this->mconfig.keyFile).absolutePath();
        break;
    case SELECT_HOST:
        save = true;
        dir = QFileInfo(this->mconfig.hostFile).absolutePath();
        break;
    case SELECT_PKT:
        save = true;
        filter = "Pkt File (*.pcap)";
        dir = QFileInfo(this->mconfig.pktFile).absolutePath();
        break;
    default:
        break;
    }

    QString fileName;

    if ( !save ) {
        fileName = QFileDialog::getOpenFileName(
            this,
            "Open File",
            dir,
            filter
        );
    } else {
        fileName = QFileDialog::getSaveFileName(
            this,
            "Save File",
            dir,
            filter
        );
    }

    if ( fileName.isEmpty() ) return;

    switch ( type ) {
    case SELECT_CRT:
        this->crtFileLine->setText(fileName);
        break;
    case SELECT_KEY:
        this->keyFileLine->setText(fileName);
        break;
    case SELECT_HOST:
        this->hostFileLine->setText(fileName);
        break;
    case SELECT_PKT:
        this->pktFileLine->setText(fileName);
        break;
    default:
        break;
    }
}
