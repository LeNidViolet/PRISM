/**
 *  Copyright 2022, LeNidViolet.
 *  Created by LeNidViolet on 2022/8/24.
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
    this->timeoutSpin = new QSpinBox(this);
    this->methodCombo = new QComboBox(this);
    this->passwordLine = new QLineEdit(this);
    this->dnssrvChkbox = new QCheckBox(QStringLiteral("CREATE DNS SERVER"), this);

    if ( config.crtFile.isEmpty() ) {
        auto s = QStringLiteral("%1/res/root.crt").arg(QApplication::applicationDirPath());
        s = QDir::toNativeSeparators(s);
        config.crtFile = s;
    }
    this->crtFileLine->setText(config.crtFile);

    if ( config.keyFile.isEmpty() ) {
        auto s = QStringLiteral("%1/res/root.key").arg(QApplication::applicationDirPath());
        s = QDir::toNativeSeparators(s);
        config.keyFile = s;
    }
    this->keyFileLine->setText(config.keyFile);

    if ( config.pktFile.isEmpty() ) {
        auto s = QStringLiteral("%1/pkts.pcap").arg(QApplication::applicationDirPath());
        s = QDir::toNativeSeparators(s);
        config.pktFile = s;
    }
    this->pktFileLine->setText(config.pktFile);

    if ( config.hostFile.isEmpty() ) {
        auto path = QProcessEnvironment::systemEnvironment().value(QStringLiteral("HOME"));
        auto s = QStringLiteral("%1/.config/wireshark/hosts").arg(path);
        config.hostFile = s;
    }
    this->hostFileLine->setText(config.hostFile);

    if ( config.method.isEmpty() ) {
        auto s = "AES-256-CFB";
        config.method = s;
    } else {
        this->methodCombo->addItem(config.method);
    }
    this->methodCombo->addItem(QStringLiteral("AES-256-CFB"));

    if ( config.password.isEmpty() ) {
        auto s = "123456";
        config.password = s;
    }
    this->passwordLine->setText(config.password);

    this->portSpin->setMinimum(0);
    this->portSpin->setMaximum(65535);
    if ( 0 == config.port )
        config.port = 7110;
    this->portSpin->setValue(config.port);

    this->timeoutSpin->setMinimum(0);
    this->timeoutSpin->setMaximum(24 * 60 * 60);
    if ( 0 == config.timeout )
        config.timeout = 120;
    this->timeoutSpin->setValue(static_cast<int>(config.timeout));

    this->dnssrvChkbox->setChecked(config.dnssrv);

    auto addrlist = QStringList();

    auto hostlist = QNetworkInterface::allAddresses();
    foreach (auto &host, hostlist ) {
        if ( host.protocol() == QAbstractSocket::IPv4Protocol ) {
            addrlist.append(host.toString());
        }
    }
    if ( !config.server.isNull() ) {
        addrlist.append(config.server.toString());
    }
    addrlist.append(QStringLiteral("0.0.0.0"));
    // 去重
    for ( auto i = 0; i < addrlist.count(); ++i ) {
        forever { // remove all occurrences at greater indexes
            auto p = addrlist.lastIndexOf(addrlist[i]);
            if ( p==i ) break;
            addrlist.removeAt(p);
        }
    }
    for ( auto& addr : addrlist ) {
        this->serverCombo->addItem(addr);
    }

    // ReSharper disable once CppDFAMemoryLeak
    auto btnSelectCrt = new QPushButton(QStringLiteral("SELECT"), this);
    QObject::connect(
        btnSelectCrt,
        &QPushButton::clicked,
        this,
        [=]() { this->onSelectClicked(SELECT_CRT); }
    );
    // ReSharper disable once CppDFAMemoryLeak
    auto btnSelectKey = new QPushButton(QStringLiteral("SELECT"), this);
    QObject::connect(
        btnSelectKey,
        &QPushButton::clicked,
        this,
        [=]() { this->onSelectClicked(SELECT_KEY); }
    );
    // ReSharper disable once CppDFAMemoryLeak
    auto btnSelectPkt = new QPushButton(QStringLiteral("SELECT"), this);
    QObject::connect(
        btnSelectPkt,
        &QPushButton::clicked,
        this,
        [=]() { this->onSelectClicked(SELECT_PKT); }
    );
    // ReSharper disable once CppDFAMemoryLeak
    auto btnSelectHost = new QPushButton(QStringLiteral("SELECT"), this);
    QObject::connect(
        btnSelectHost,
        &QPushButton::clicked,
        this,
        [=]() { this->onSelectClicked(SELECT_HOST); }
    );

    // ReSharper disable once CppDFAMemoryLeak
    auto labelAddr = new QLabel(QStringLiteral("ADDR: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    auto labelMthd = new QLabel(QStringLiteral("MTHD: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    auto labelPswd = new QLabel(QStringLiteral("PSWD: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    auto labelTimt = new QLabel(QStringLiteral("TIMT: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    auto labelCert = new QLabel(QStringLiteral("CERT: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    auto labelSKey = new QLabel(QStringLiteral("SKEY: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    auto labelPkts = new QLabel(QStringLiteral("PKTS: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    auto labelHost = new QLabel(QStringLiteral("HOST: "), this);

    // ReSharper disable once CppDFAMemoryLeak
    auto hlayout1 = new QHBoxLayout();
    hlayout1->addWidget(labelAddr);
    hlayout1->addWidget(this->serverCombo, 1);
    hlayout1->addWidget(this->portSpin);

    // ReSharper disable once CppDFAMemoryLeak
    auto hlayout2 = new QHBoxLayout();
    hlayout2->addWidget(labelMthd);
    hlayout2->addWidget(this->methodCombo, 1);

    // ReSharper disable once CppDFAMemoryLeak
    auto hlayout3 = new QHBoxLayout();
    hlayout3->addWidget(labelPswd);
    hlayout3->addWidget(this->passwordLine);

    // ReSharper disable once CppDFAMemoryLeak
    auto hlayout4 = new QHBoxLayout();
    hlayout4->addWidget(labelTimt);
    hlayout4->addWidget(this->timeoutSpin, 1);

    // ReSharper disable once CppDFAMemoryLeak
    auto hlayout5 = new QHBoxLayout();
    hlayout5->addWidget(labelCert);
    hlayout5->addWidget(this->crtFileLine);
    hlayout5->addWidget(btnSelectCrt);

    // ReSharper disable once CppDFAMemoryLeak
    auto hlayout6 = new QHBoxLayout();
    hlayout6->addWidget(labelSKey);
    hlayout6->addWidget(this->keyFileLine);
    hlayout6->addWidget(btnSelectKey);

    // ReSharper disable once CppDFAMemoryLeak
    auto hlayout7 = new QHBoxLayout();
    hlayout7->addWidget(labelPkts);
    hlayout7->addWidget(this->pktFileLine);
    hlayout7->addWidget(btnSelectPkt);

    // ReSharper disable once CppDFAMemoryLeak
    auto hlayout8 = new QHBoxLayout();
    hlayout8->addWidget(labelHost);
    hlayout8->addWidget(this->hostFileLine);
    hlayout8->addWidget(btnSelectHost);

    // ReSharper disable once CppDFAMemoryLeak
    auto hlayout10 = new QHBoxLayout();
    hlayout10->addWidget(this->dnssrvChkbox);

    // ReSharper disable once CppDFAMemoryLeak
    auto btnConfirm = new QPushButton(QStringLiteral("CONFIRM"), this);
    QObject::connect(
        btnConfirm,
        &QPushButton::clicked,
        this,
        &ConfigView::onConfirmClicked
    );
    // ReSharper disable once CppDFAMemoryLeak
    auto btnClose = new QPushButton(QStringLiteral("CLOSE"), this);
    QObject::connect(
        btnClose,
        &QPushButton::clicked,
        this,
        &ConfigView::close
    );

    // ReSharper disable once CppDFAMemoryLeak
    auto hlayout9 = new QHBoxLayout();
    hlayout9->addStretch();
    hlayout9->addWidget(btnClose);
    hlayout9->addWidget(btnConfirm);

    // ReSharper disable once CppDFAMemoryLeak
    auto gb1 = new QGroupBox(QStringLiteral("Shadowsocks"), this);
    // ReSharper disable once CppDFAMemoryLeak
    auto layoutgb1 = new QVBoxLayout();
    layoutgb1->addLayout(hlayout1);
    layoutgb1->addLayout(hlayout2);
    layoutgb1->addLayout(hlayout3);
    layoutgb1->addLayout(hlayout4);
    layoutgb1->addLayout(hlayout10);
    gb1->setLayout(layoutgb1);

    // ReSharper disable once CppDFAMemoryLeak
    auto gb2 = new QGroupBox(QStringLiteral("TLS"), this);
    // ReSharper disable once CppDFAMemoryLeak
    auto layoutgb2 = new QVBoxLayout();
    layoutgb2->addLayout(hlayout5);
    layoutgb2->addLayout(hlayout6);
    layoutgb2->addLayout(hlayout7);
    layoutgb2->addLayout(hlayout8);
    gb2->setLayout(layoutgb2);

    // ReSharper disable once CppDFAMemoryLeak
    auto layout = new QVBoxLayout();
    layout->addWidget(gb1);
    layout->addWidget(gb2);
    layout->addStretch();
    layout->addLayout(hlayout9);
    this->setLayout(layout);

    this->resize(600, 300);

    this->mconfig = config;
}

void ConfigView::onConfirmClicked() {

    this->mconfig.crtFile = this->crtFileLine->text();
    this->mconfig.keyFile = this->keyFileLine->text();
    this->mconfig.hostFile = this->hostFileLine->text();
    this->mconfig.pktFile = this->pktFileLine->text();

    this->mconfig.port = this->portSpin->value();
    this->mconfig.timeout = this->timeoutSpin->value();
    this->mconfig.server = QHostAddress(this->serverCombo->currentText());
    this->mconfig.password = this->passwordLine->text();

    this->mconfig.dnssrv = this->dnssrvChkbox->isChecked();

    if ( this->mconfig.server.isNull() || QAbstractSocket::IPv4Protocol != this->mconfig.server.protocol() ) {
        QMessageBox::information(this, QStringLiteral("INFO"), QStringLiteral("Select a Valid IP Address"));
        return;
    }
    if ( this->mconfig.port <= 0 || this->mconfig.port > 65535 ) {
        QMessageBox::information(this, QStringLiteral("INFO"), QStringLiteral("Select a Valid Port"));
        return;
    }
    if ( this->mconfig.timeout <= 0 || this->mconfig.timeout > 24 * 60 * 60 ) {
        QMessageBox::information(this, QStringLiteral("INFO"), QStringLiteral("Select a Valid Timeout"));
        return;
    }
    if ( this->mconfig.password.isEmpty() || this->mconfig.password.size() >= 64 ) {
        QMessageBox::information(this, QStringLiteral("INFO"), QStringLiteral("Input a Valid Password"));
        return;
    }
    if ( !QFileInfo::exists(this->mconfig.crtFile) ) {
        QMessageBox::information(this, QStringLiteral("INFO"), QStringLiteral("Cert File Not Exists"));
        return;
    }
    if ( !QFileInfo::exists(this->mconfig.keyFile) ) {
        QMessageBox::information(this, QStringLiteral("INFO"), QStringLiteral("Key File Not Exists"));
        return;
    }
    if ( !QDir().mkpath(QFileInfo(this->mconfig.pktFile).absolutePath()) ) {
        QMessageBox::information(this, QStringLiteral("INFO"), QStringLiteral("Create Folder For Pkt File Failed"));
        return;
    }
    if ( !QDir().mkpath(QFileInfo(this->mconfig.hostFile).absolutePath()) ) {
        QMessageBox::information(this, QStringLiteral("INFO"), QStringLiteral("Create Folder For Host File Failed"));
        return;
    }

    emit this->configConfirm(this->mconfig);

    this->close();
}

void ConfigView::onSelectClicked(int type) {

    QString filter;
    QString dir;
    bool save = false;

    switch ( type ) {
    case SELECT_CRT:
        filter = QStringLiteral("Cert File (*.crt *.cer)");
        dir = QFileInfo(this->mconfig.crtFile).absolutePath();
        break;
    case SELECT_KEY:
        filter = QStringLiteral("Key File (*.key)");
        dir = QFileInfo(this->mconfig.keyFile).absolutePath();
        break;
    case SELECT_HOST:
        save = true;
        dir = QFileInfo(this->mconfig.hostFile).absolutePath();
        break;
    case SELECT_PKT:
        save = true;
        filter = QStringLiteral("Pkt File (*.pcap)");
        dir = QFileInfo(this->mconfig.pktFile).absolutePath();
        break;
    default:
        break;
    }

    QString fileName;

    if ( !save ) {
        fileName = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("Open File"),
            dir,
            filter
        );
    } else {
        fileName = QFileDialog::getSaveFileName(
            this,
            QStringLiteral("Save File"),
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
