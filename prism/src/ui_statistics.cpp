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
#include "ui_statistics.h"
#include <QHeaderView>
#include <QFileInfo>
#include <QDesktopServices>
#include <QMenu>
#include <QVBoxLayout>
#include <QDialog>
#include "misc.h"
#include "macros.h"
#include "ui_mainwgt.h"
#include "if_raw.h"
#include "config.hpp"
#include "dump.h"


typedef enum {
    Mode,
    TcpLinks,
    TcpRxBytes,
    TcpTxBytes,
    UdpLinks,
    UdpRxBytes,
    UdpTxBytes,
    Address,
    Timeout,
    Method,
    Password,
    CertFile,
    KeyFile,
    PktFile,
    HostsFile,
    BytesCaching
} STATICS_NAME_INDEX;


static const NAMEINDEX Labels[] = {
    CREATESTRMAP(Mode),
    CREATESTRMAP(TcpLinks),
    CREATESTRMAP(TcpRxBytes),
    CREATESTRMAP(TcpTxBytes),
    CREATESTRMAP(UdpLinks),
    CREATESTRMAP(UdpRxBytes),
    CREATESTRMAP(UdpTxBytes),

    CREATESTRMAP(Address),
    CREATESTRMAP(Timeout),
    CREATESTRMAP(Method),
    CREATESTRMAP(Password),

    CREATESTRMAP(CertFile),
    CREATESTRMAP(KeyFile),
    CREATESTRMAP(PktFile),
    CREATESTRMAP(HostsFile),

    CREATESTRMAP(BytesCaching),
};


typedef enum {
    CREATECONNECTEDNAME(Statics, Name),
    CREATECONNECTEDNAME(Statics, Value),
} KEYOPRTCOLUMN;

static NAMEINDEX OprtName[] = {
    CREATECONNECTEDMAP(Statics, Name),
    CREATECONNECTEDMAP(Statics, Value),
};

StatisticsView::StatisticsView(QWidget *parent, const Qt::WindowFlags f) : QDialog(parent, f) {

    QStringList labels;
    for ( const auto &[index, name] : OprtName )
        labels << name;

    this->m_treeView = new SearchableTreeView(this);
    this->m_treeModel = new StaticsTreeViewModel(this);
    this->m_treeModel->setHorizontalHeaderLabels(labels);
    this->m_treeView->setSourceModel(this->m_treeModel);

    // ReSharper disable once CppDFAMemoryLeak
    auto *timer = new QTimer(this);
    QObject::connect(
        timer,
        &QTimer::timeout,
        this,
        [this](){ this->updateStatistics(); }
        );
    timer->start(1000);

    // ReSharper disable once CppDFAMemoryLeak
    const auto ctxMenu = new QMenu(this);
    QObject::connect(this->m_treeView, &SearchableTreeView::customContextMenuRequested, this, [ctxMenu]() { ctxMenu->exec(QCursor::pos()); });
    auto actn = ctxMenu->addAction(QStringLiteral("LOCATE CERT FILE"));
    QObject::connect(actn, &QAction::triggered, this, []() { StatisticsView::explrFile(ConfigVars::instance().crtFile); });
    actn = ctxMenu->addAction(QStringLiteral("LOCATE KEY FILE"));
    QObject::connect(actn, &QAction::triggered, this, []() { StatisticsView::explrFile(ConfigVars::instance().keyFile); });
    actn = ctxMenu->addAction(QStringLiteral("LOCATE HOSTS FILE"));
    QObject::connect(actn, &QAction::triggered, this, []() { StatisticsView::explrFile(ConfigVars::instance().hostFile); });
    actn = ctxMenu->addAction(QStringLiteral("LOCATE PACKET FILE"));
    QObject::connect(actn, &QAction::triggered, this, []() { StatisticsView::explrFile(ConfigVars::instance().pktFile); });

    // ReSharper disable once CppDFAMemoryLeak
    const auto btnClose = new QPushButton(QStringLiteral("CLOSE"), this);
    QObject::connect(btnClose, &QPushButton::clicked, this, &StatisticsView::hide);
    btnClose->setFocusPolicy(Qt::NoFocus);

    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget(btnClose);
    hlayout->setContentsMargins(0,0,0,0);
    hlayout->setSpacing(0);

    // ReSharper disable once CppDFAMemoryLeak
    const auto layout = new QVBoxLayout();
    layout->addWidget(this->m_treeView);
    layout->addLayout(hlayout);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    this->setLayout(layout);
    this->resize(600, 400);
    this->setWindowTitle(QStringLiteral("STATISTICS"));

    QList<QStandardItem*> items;
    for ( int row = 0; row < G_N_ELEMENTS(Labels); row++ ) {
        for ( int column = 0; column < G_N_ELEMENTS(OprtName); ++column ) {
            // ReSharper disable once CppDFAMemoryLeak
            const auto item = new QStandardItem();
            item->setCheckable(false);
            item->setEditable(false);

            items << item;
        }
        this->m_treeModel->appendRow(items);
        items.clear();
    }
}

void StatisticsView::updateStatistics() const {

    if ( !this->isVisible() ) return;

    emit this->m_treeModel->layoutAboutToBeChanged();
    emit this->m_treeModel->layoutChanged();
}

void StatisticsView::showEvent(QShowEvent *event) {

    QDialog::showEvent(event);
    this->updateStatistics();

    static bool firstShow = true;
    QDialog::showEvent(event);
    if ( firstShow ) {
        firstShow = false;
        this->m_treeView->header()->resizeSections(QHeaderView::ResizeToContents);
    }
}

void StatisticsView::explrFile(const QString &filePath) {

    if ( filePath.isEmpty() ) return ;
    const auto dir = "file://" + QFileInfo(filePath).absolutePath();
    QDesktopServices::openUrl(QUrl(dir));
}

// ReSharper disable once CppParameterMayBeConst
QVariant StaticsTreeViewModel::data(const QModelIndex &index, int role) const {
    if ( !index.isValid() )
        return {};

    if ( role != Qt::DisplayRole )
        return {};

    if ( 0 == index.column() ) {
        return QString(Labels[index.row()].name);
    }

    if ( 1 == index.column() ) {

        const auto statistics = GetFlowStats();

        switch ( index.row() ) {
        case Mode:          return QStringLiteral("%1").arg(ConfigVars::instance().runAsSocks5 ? "SOCKS5" : "SHADOWSOCKS");
        case TcpLinks:      return QStringLiteral("%1/%2").arg(QString::number(statistics.tcpActiveFlows), QString::number(statistics.tcpFlows));
        case TcpRxBytes:    return QStringLiteral("%1").arg(MiscFuncs::formatBytes(statistics.tcpRxBytes));
        case TcpTxBytes:    return QStringLiteral("%1").arg(MiscFuncs::formatBytes(statistics.tcpTxBytes));
        case UdpLinks:      return QStringLiteral("%1/%2").arg(QString::number(statistics.udpActiveFlows), QString::number(statistics.udpFlows));
        case UdpRxBytes:    return QStringLiteral("%1").arg(MiscFuncs::formatBytes(statistics.udpRxBytes));
        case UdpTxBytes:    return QStringLiteral("%1").arg(MiscFuncs::formatBytes(statistics.udpTxBytes));
        case Address:       return QStringLiteral("%1:%2").arg("all", QString::number(ConfigVars::instance().listenPort));
        case Timeout:       return QStringLiteral("%1").arg(ConfigVars::instance().timeout);
        case Method:        return QStringLiteral("%1").arg(ConfigVars::instance().method);
        case Password:      return QStringLiteral("%1").arg(ConfigVars::instance().password);
        case CertFile:      return QStringLiteral("%1").arg(ConfigVars::instance().crtFile);
        case KeyFile:       return QStringLiteral("%1").arg(ConfigVars::instance().keyFile);
        case PktFile:       return QStringLiteral("%1").arg(ConfigVars::instance().pktFile);
        case HostsFile:     return QStringLiteral("%1").arg(ConfigVars::instance().hostFile);
        case BytesCaching:  return QStringLiteral("%1").arg(MiscFuncs::formatBytes(PacketDumper::instance().getCachingBytes()));

        default: break;
        }
    }

    return {};
}
