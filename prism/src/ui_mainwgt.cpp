/**
 *  Copyright 2022, LeNidViolet.
 *  Created by LeNidViolet on 2022/9/1.
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

#include "ui_mainwgt.h"
#include "ui_config.h"
#include <QApplication>
#include <QMessageBox>
#include <QIcon>
#include <QFile>
#include <QPushButton>
#include <QVBoxLayout>
#include "if_raw.h"
#include "config.hpp"
#include "dump.h"

#include "custom/http_server.h"


MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {

    // 子窗口
    this->m_configView = new ConfigView(this);
    this->m_logView = new LogView(this);
    this->m_flowView = new FlowView(this);
    this->m_statsView = new StatisticsView(this);
    this->m_hostsView = new HostsView(this);

    this->m_configView->setModal(true);
    QObject::connect(this->m_configView, &ConfigView::configConfirm, this, &MainWidget::onConfigConfirm);

    this->m_caaddr = new QLabel(this);

    // ReSharper disable once CppDFAMemoryLeak
    const auto btnStatis = new QPushButton(QStringLiteral("STATIS"), this);
    // ReSharper disable once CppDFAMemoryLeak
    const auto btnHosts = new QPushButton(QStringLiteral("HOSTS"), this);
    // ReSharper disable once CppDFAMemoryLeak
    const auto btnLogs = new QPushButton(QStringLiteral("LOGS"), this);

    this->m_btnCapture = new QPushButton(this);
    this->m_capturing = false;
    this->updateStatus();

    QObject::connect(btnStatis, &QPushButton::clicked, this, [this]() { this->m_statsView->show(); });
    QObject::connect(btnHosts, &QPushButton::clicked, this, [this]() { this->m_hostsView->show(); });
    QObject::connect(btnLogs, &QPushButton::clicked, this, [this]() { this->m_logView->show(); });
    QObject::connect(this->m_btnCapture, &QPushButton::clicked, this, &MainWidget::onStartClicked);


    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayoutBtns = new QHBoxLayout();
    hlayoutBtns->addWidget(this->m_caaddr);
    hlayoutBtns->addStretch();
    hlayoutBtns->addWidget(btnHosts);
    hlayoutBtns->addWidget(btnStatis);
    hlayoutBtns->addWidget(btnLogs);
    hlayoutBtns->addWidget(this->m_btnCapture);

    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayoutFlows = new QHBoxLayout();
    hlayoutFlows->addWidget(this->m_flowView);

    // ReSharper disable once CppDFAMemoryLeak
    const auto layout = new QVBoxLayout();
    layout->addLayout(hlayoutBtns);
    layout->addLayout(hlayoutFlows);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    this->setLayout(layout);
}

void MainWidget::onStartClicked() {

    if ( this->m_capturing ) {
        const auto ret = QMessageBox::question(
            this,
            QStringLiteral("CONFIRM"),
            QStringLiteral("Stop Capture Packet?")
            );
        if ( QMessageBox::Yes == ret ) {
            MainWidget::captureStop();
        }
    } else {
        this->m_configView->show();
    }
}

void MainWidget::updateStatus() const {

    if ( this->m_capturing ) {
        this->m_btnCapture->setIcon(QIcon(":/res/stop.png"));
        this->m_btnCapture->setText(QStringLiteral("STOP"));
    } else {
        this->m_btnCapture->setIcon(QIcon(":/res/start.png"));
        this->m_btnCapture->setText(QStringLiteral("START"));
    }
}

void MainWidget::onConfigConfirm() {

    Q_ASSERT(false == this->m_capturing);

    this->m_hostsView->setHostsPath(ConfigVars::instance().hostFile);
    PacketDumper::instance().setPcapFilePath(ConfigVars::instance().pktFile);

    this->captureStart();
}

void MainWidget::captureStart() {

    if (ConfigVars::instance().runAsSocks5) {
        StartSocks5CryptoServer(
            ConfigVars::instance().listenPort,
            ConfigVars::instance().timeout,
            ConfigVars::instance().crtFile,
            ConfigVars::instance().keyFile
            );
    } else {
        StartShadowsocksCryptoServer(
            ConfigVars::instance().listenPort,
            ConfigVars::instance().timeout,
            ConfigVars::instance().method,
            ConfigVars::instance().password,
            ConfigVars::instance().crtFile,
            ConfigVars::instance().keyFile
            );
    }

    this->m_capturing = true;
    this->updateStatus();
}

void MainWidget::captureStop() {

    StopSocks5CryptoServer();
    StopShadowsocksCryptoServer();

    this->m_capturing = false;
    this->updateStatus();
}




