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

#include <QFileDialog>
#include <QToolTip>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QProcessEnvironment>
#include <QButtonGroup>
#include <QStandardPaths>
#include "config.hpp"
#include "misc.h"


ConfigView::ConfigView(QWidget *parent, const Qt::WindowFlags f) : QDialog(parent, f) {

    this->crtFileLine = new QLineEdit(this);
    this->keyFileLine = new QLineEdit(this);
    this->pktFileLine = new QLineEdit(this);
    this->hostFileLine = new QLineEdit(this);
    this->listenAddressLine = new QLineEdit(this);
    this->listenPortSpin = new QSpinBox(this);
    this->timeoutSpin = new QSpinBox(this);
    this->methodLine = new QLineEdit(this);
    this->passwordLine = new QLineEdit(this);
    this->runAsShadowsocks = new QRadioButton(QStringLiteral("SHADOWSOCKS"), this);
    this->runAsSocks5 = new QRadioButton(QStringLiteral("SOCKS5"), this);


    auto path = QStringLiteral("%1/res/root.crt").arg(MiscFuncs::getExecutableRootPath());
    path = QDir::toNativeSeparators(path);
    if (QFile::exists(path)) {
        this->crtFileLine->setText(path);
    }

    path = QStringLiteral("%1/res/root.key").arg(MiscFuncs::getExecutableRootPath());
    path = QDir::toNativeSeparators(path);
    if (QFile::exists(path)) {
        this->keyFileLine->setText(path);
    }

    path = QStringLiteral("%1/pkts.pcap").arg(MiscFuncs::getExecutableRootPath());
    path = QDir::toNativeSeparators(path);
    this->pktFileLine->setText(path);

    path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    path = QStringLiteral("%1/.config/wireshark/hosts").arg(path);
    path = QDir::toNativeSeparators(path);
    this->hostFileLine->setText(path);

    this->methodLine->setText(QStringLiteral("AES-256-CFB"));
    this->methodLine->setEnabled(false);

    this->passwordLine->setText(QStringLiteral("123456"));

    this->listenPortSpin->setMinimum(0);
    this->listenPortSpin->setMaximum(65535);
    this->listenPortSpin->setValue(7110);

    this->timeoutSpin->setMinimum(0);
    this->timeoutSpin->setMaximum(24 * 60 * 60);
    this->timeoutSpin->setValue(120);

    this->listenAddressLine->setText(QStringLiteral("all"));
    this->listenAddressLine->setEnabled(false);

    // ReSharper disable once CppDFAMemoryLeak
    auto group = new QButtonGroup(this);
    group->setExclusive(true);
    group->addButton(this->runAsShadowsocks);
    group->addButton(this->runAsSocks5);

    this->runAsSocks5->setChecked(true);
    this->passwordLine->setEnabled(false);

    // ReSharper disable once CppDFAMemoryLeak
    const auto btnSelectCrt = new QPushButton(QStringLiteral("SELECT"), this);
    QObject::connect(
        btnSelectCrt,
        &QPushButton::clicked,
        this,
        [this]() { this->onSelectClicked(SELECT_CRT); }
    );
    // ReSharper disable once CppDFAMemoryLeak
    const auto btnSelectKey = new QPushButton(QStringLiteral("SELECT"), this);
    QObject::connect(
        btnSelectKey,
        &QPushButton::clicked,
        this,
        [this]() { this->onSelectClicked(SELECT_KEY); }
    );
    // ReSharper disable once CppDFAMemoryLeak
    const auto btnSelectPcap = new QPushButton(QStringLiteral("SELECT"), this);
    QObject::connect(
        btnSelectPcap,
        &QPushButton::clicked,
        this,
        [this]() { this->onSelectClicked(SELECT_PKT); }
    );
    // ReSharper disable once CppDFAMemoryLeak
    const auto btnSelectHost = new QPushButton(QStringLiteral("SELECT"), this);
    QObject::connect(
        btnSelectHost,
        &QPushButton::clicked,
        this,
        [this]() { this->onSelectClicked(SELECT_HOST); }
    );

    QObject::connect(
        this->runAsShadowsocks,
        &QRadioButton::clicked,
        this,
        [this]() { this->passwordLine->setEnabled(true); }
        );
    QObject::connect(
        this->runAsSocks5,
        &QRadioButton::clicked,
        this,
        [this]() { this->passwordLine->setEnabled(false); }
        );

    // ReSharper disable once CppDFAMemoryLeak
    const auto labelAddr = new QLabel(QStringLiteral("ADDR: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    const auto labelMthd = new QLabel(QStringLiteral("MTHD: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    const auto labelPswd = new QLabel(QStringLiteral("PSWD: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    const auto labelTimt = new QLabel(QStringLiteral("TIMT: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    const auto labelCert = new QLabel(QStringLiteral("CERT: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    const auto labelSKey = new QLabel(QStringLiteral("SKEY: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    const auto labelPcap = new QLabel(QStringLiteral("PKTS: "), this);
    // ReSharper disable once CppDFAMemoryLeak
    const auto labelHost = new QLabel(QStringLiteral("HOST: "), this);

    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayoutAddr = new QHBoxLayout();
    hlayoutAddr->addWidget(labelAddr);
    hlayoutAddr->addWidget(this->listenAddressLine, 1);
    hlayoutAddr->addWidget(this->listenPortSpin);

    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayoutMethod = new QHBoxLayout();
    hlayoutMethod->addWidget(labelMthd);
    hlayoutMethod->addWidget(this->methodLine, 1);

    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayoutPassword = new QHBoxLayout();
    hlayoutPassword->addWidget(labelPswd);
    hlayoutPassword->addWidget(this->passwordLine);

    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayoutTimeout = new QHBoxLayout();
    hlayoutTimeout->addWidget(labelTimt);
    hlayoutTimeout->addWidget(this->timeoutSpin, 1);

    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayoutCert = new QHBoxLayout();
    hlayoutCert->addWidget(labelCert);
    hlayoutCert->addWidget(this->crtFileLine);
    hlayoutCert->addWidget(btnSelectCrt);

    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayoutKey = new QHBoxLayout();
    hlayoutKey->addWidget(labelSKey);
    hlayoutKey->addWidget(this->keyFileLine);
    hlayoutKey->addWidget(btnSelectKey);

    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayoutPcap = new QHBoxLayout();
    hlayoutPcap->addWidget(labelPcap);
    hlayoutPcap->addWidget(this->pktFileLine);
    hlayoutPcap->addWidget(btnSelectPcap);

    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayoutHosts = new QHBoxLayout();
    hlayoutHosts->addWidget(labelHost);
    hlayoutHosts->addWidget(this->hostFileLine);
    hlayoutHosts->addWidget(btnSelectHost);

    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayoutMode = new QHBoxLayout();
    hlayoutMode->addWidget(this->runAsShadowsocks);
    hlayoutMode->addWidget(this->runAsSocks5);
    hlayoutMode->addStretch();

    // ReSharper disable once CppDFAMemoryLeak
    const auto btnConfirm = new QPushButton(QStringLiteral("CONFIRM"), this);
    QObject::connect(
        btnConfirm,
        &QPushButton::clicked,
        this,
        &ConfigView::onConfirmClicked
    );
    // ReSharper disable once CppDFAMemoryLeak
    const auto btnClose = new QPushButton(QStringLiteral("CLOSE"), this);
    QObject::connect(
        btnClose,
        &QPushButton::clicked,
        this,
        &ConfigView::close
    );

    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayoutAction = new QHBoxLayout();
    hlayoutAction->addStretch();
    hlayoutAction->addWidget(btnClose);
    hlayoutAction->addWidget(btnConfirm);

    // ReSharper disable once CppDFAMemoryLeak
    const auto gb1 = new QGroupBox(QStringLiteral("Shadowsocks/Socks5"), this);
    // ReSharper disable once CppDFAMemoryLeak
    const auto layoutgb1 = new QVBoxLayout();
    layoutgb1->addLayout(hlayoutAddr);
    layoutgb1->addLayout(hlayoutMethod);
    layoutgb1->addLayout(hlayoutPassword);
    layoutgb1->addLayout(hlayoutTimeout);
    gb1->setLayout(layoutgb1);

    // ReSharper disable once CppDFAMemoryLeak
    const auto gb2 = new QGroupBox(QStringLiteral("TLS"), this);
    // ReSharper disable once CppDFAMemoryLeak
    const auto layoutgb2 = new QVBoxLayout();
    layoutgb2->addLayout(hlayoutCert);
    layoutgb2->addLayout(hlayoutKey);
    layoutgb2->addLayout(hlayoutPcap);
    layoutgb2->addLayout(hlayoutHosts);
    gb2->setLayout(layoutgb2);

    // ReSharper disable once CppDFAMemoryLeak
    const auto layout = new QVBoxLayout();
    layout->addLayout(hlayoutMode);
    layout->addWidget(gb1);
    layout->addWidget(gb2);
    layout->addStretch();
    layout->addLayout(hlayoutAction);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    this->setLayout(layout);

    this->resize(600, 400);
}

void ConfigView::onConfirmClicked() {

    auto value = this->listenPortSpin->value();
    if ( value <= 0 || value > 65535 ) {
        QToolTip::showText(QCursor::pos(), QStringLiteral("Select a Valid Port"));
        return;
    }
    ConfigVars::instance().listenPort = value;

    value = this->timeoutSpin->value();
    if ( value <= 0 || value > 24 * 60 * 60 ) {
        QToolTip::showText(QCursor::pos(), QStringLiteral("Select a Valid Timeout"));
        return;
    }
    ConfigVars::instance().timeout = value;

    QString str;
    if (this->runAsShadowsocks->isChecked()) {
        str = this->passwordLine->text().trimmed();
        if ( str.isEmpty() || str.size() >= 64 ) {
            QToolTip::showText(QCursor::pos(), QStringLiteral("Select a Valid Password"));
            return;
        }
        ConfigVars::instance().password = str;
    }

    str = this->crtFileLine->text().trimmed();
    if ( !QFileInfo::exists(str) ) {
        QToolTip::showText(QCursor::pos(), QStringLiteral("Cert File Not Exists"));
        return;
    }
    ConfigVars::instance().crtFile = str;

    str = this->keyFileLine->text().trimmed();
    if ( !QFileInfo::exists(str) ) {
        QToolTip::showText(QCursor::pos(), QStringLiteral("Key File Not Exists"));
        return;
    }
    ConfigVars::instance().keyFile = str;

    str = this->pktFileLine->text().trimmed();
    if ( !QDir().mkpath(QFileInfo(str).absolutePath()) ) {
        QToolTip::showText(QCursor::pos(), QStringLiteral("Create Folder For Pkt File Failed"));
        return;
    }
    ConfigVars::instance().pktFile = str;

    str = this->hostFileLine->text().trimmed();
    if ( !QDir().mkpath(QFileInfo(str).absolutePath()) ) {
        QToolTip::showText(QCursor::pos(), QStringLiteral("Create Folder For Host File Failed"));
        return;
    }
    ConfigVars::instance().hostFile = str;

    str = this->methodLine->text().trimmed();
    ConfigVars::instance().method = str;
    ConfigVars::instance().runAsSocks5 = this->runAsSocks5->isChecked();

    emit this->configConfirm();
    this->close();
}

void ConfigView::onSelectClicked(const int reason) {

    QString filter;
    bool save = false;

    switch ( reason ) {
    case SELECT_CRT:
        filter = QStringLiteral("Cert File (*.crt *.cer);;All Files (*)");
        break;
    case SELECT_KEY:
        filter = QStringLiteral("Key File (*.key);;All Files (*)");
        break;
    case SELECT_HOST:
        save = true;
        break;
    case SELECT_PKT:
        save = true;
        filter = QStringLiteral("Pkt File (*.pcap)");
        break;
    default:
        break;
    }

    QString fileName;

    if ( !save ) {
        fileName = QFileDialog::getOpenFileName(
            this,
            QStringLiteral("Open File"),
            MiscFuncs::getExecutableRootPath(),
            filter
        );
    } else {
        fileName = QFileDialog::getSaveFileName(
            this,
            QStringLiteral("Save File"),
            MiscFuncs::getExecutableRootPath(),
            filter
        );
    }

    if ( fileName.isEmpty() ) return;
    fileName = QDir::toNativeSeparators(fileName);

    switch ( reason ) {
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
