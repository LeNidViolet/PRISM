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
#include <QPushButton>
#include <QHeaderView>
#include <QFileInfo>
#include <QDesktopServices>
#include <QMenu>
#include <QVBoxLayout>
#include <QDialog>

#include "macros.h"
#include "ui_mainwgt.h"


typedef enum {
    TcpLinks,
    TcpBytesIn,
    TcpBytesOut,
    UdpLinks,
    UdpBytesIn,
    UdpBytesOut,
    Address,
    Timeout,
    Method,
    Password,
    Dnssrv,
    CertFile,
    KeyFile,
    PktFile,
    HostsFile,
    CachBytes,
} STATICS_NAME_INDEX;


static const NAMEINDEX Labels[] = {
    CREATESTRMAP(TcpLinks),
    CREATESTRMAP(TcpBytesIn),
    CREATESTRMAP(TcpBytesOut),
    CREATESTRMAP(UdpLinks),
    CREATESTRMAP(UdpBytesIn),
    CREATESTRMAP(UdpBytesOut),

    CREATESTRMAP(Address),
    CREATESTRMAP(Timeout),
    CREATESTRMAP(Method),
    CREATESTRMAP(Password),
    CREATESTRMAP(Dnssrv),

    CREATESTRMAP(CertFile),
    CREATESTRMAP(KeyFile),
    CREATESTRMAP(PktFile),
    CREATESTRMAP(HostsFile),
    CREATESTRMAP(CachBytes),
};


typedef enum {
    CREATECONNECTEDNAME(Statics, Name),
    CREATECONNECTEDNAME(Statics, Value),
} KEYOPRTCOLUMN;

static NAMEINDEX OprtName[] = {
    CREATECONNECTEDMAP(Statics, Name),
    CREATECONNECTEDMAP(Statics, Value),
};

StatisticsView::StatisticsView(MainWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {


    QStringList labels;
    for ( const auto &name : OprtName )
        labels << name.name;

    this->treeView = new SearchableTreeView(this);
    this->treeModel = new StaticsTreeViewModel(parent, this);
    this->treeModel->setHorizontalHeaderLabels(labels);
    this->treeView->setSourceModel(this->treeModel);

    // ReSharper disable once CppDFAMemoryLeak
    auto ctxMenu = new QMenu(this);
    QObject::connect(this->treeView, &SearchableTreeView::customContextMenuRequested, this, [=]() { ctxMenu->exec(QCursor::pos()); });
    auto actn = ctxMenu->addAction(QStringLiteral("EXPLR CERT FILE"));
    QObject::connect(actn, &QAction::triggered, this, [=]() { StatisticsView::explrFile(parent->getConfigVars().crtFile); });
    actn = ctxMenu->addAction(QStringLiteral("EXPLR KEY FILE"));
    QObject::connect(actn, &QAction::triggered, this, [=]() { StatisticsView::explrFile(parent->getConfigVars().keyFile); });
    actn = ctxMenu->addAction(QStringLiteral("EXPLR HOSTS FILE"));
    QObject::connect(actn, &QAction::triggered, this, [=]() { StatisticsView::explrFile(parent->getConfigVars().hostFile); });
    actn = ctxMenu->addAction(QStringLiteral("EXPLR PACKET FILE"));
    QObject::connect(actn, &QAction::triggered, this, [=]() { StatisticsView::explrFile(parent->getConfigVars().pktFile); });
    ctxMenu->addSeparator();
    actn = ctxMenu->addAction(QStringLiteral("FLUSH CACHE"));
    QObject::connect(actn, &QAction::triggered, this, &StatisticsView::flushCache);

    // ReSharper disable once CppDFAMemoryLeak
    auto btnClose = new QPushButton(QStringLiteral("CLOSE"), this);
    QObject::connect(btnClose, &QPushButton::clicked, this, &StatisticsView::hide);

    btnClose->setFocusPolicy(Qt::NoFocus);

    // ReSharper disable once CppDFAMemoryLeak
    auto hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget(btnClose);
    hlayout->setContentsMargins(0,0,0,0);
    hlayout->setSpacing(0);

    // ReSharper disable once CppDFAMemoryLeak
    auto layout = new QVBoxLayout();
    layout->addWidget(this->treeView);
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
            auto item = new QStandardItem();
            item->setCheckable(false);
            item->setEditable(false);

            items << item;
        }
        this->treeModel->appendRow(items);
        items.clear();
    }
}

void StatisticsView::updateStatistics() {

    if ( !this->isVisible() ) return;

    emit this->treeModel->layoutAboutToBeChanged();
    emit this->treeModel->layoutChanged();
}

void StatisticsView::showEvent(QShowEvent *event) {

    QDialog::showEvent(event);
    this->updateStatistics();

    static bool firstShow = true;
    QDialog::showEvent(event);
    if ( firstShow ) {
        firstShow = false;
        this->treeView->header()->resizeSections(QHeaderView::ResizeToContents);
    }
}

void StatisticsView::explrFile(const QString &filePath) {

    if ( filePath.isEmpty() ) return ;
    auto dir = "file://" + QFileInfo(filePath).absolutePath();
    QDesktopServices::openUrl(QUrl(dir));
}

QVariant StaticsTreeViewModel::data(const QModelIndex &index, int role) const {
    if ( !index.isValid() )
        return {};

    if ( role != Qt::DisplayRole )
        return {};

    if ( 0 == index.column() ) {
        return QString(Labels[index.row()].name);
    }

    if ( 1 == index.column() ) {

        auto config = this->mmainWidget->getConfigVars();
        auto statistics = this->mmainWidget->getStatisticsVars();

        switch ( index.row() ) {
        case TcpLinks:      return QStringLiteral("%1/%2").arg(QString::number(statistics.linkActiveTcp), QString::number(statistics.linkTotalTcp));
        case TcpBytesIn:    return QStringLiteral("%1").arg(MiscFuncs::formatBytes(statistics.bytesInTotalTcp));
        case TcpBytesOut:   return QStringLiteral("%1").arg(MiscFuncs::formatBytes(statistics.bytesOutTotalTcp));
        case UdpLinks:      return QStringLiteral("%1/%2").arg(QString::number(statistics.linkActiveUdp), QString::number(statistics.linkTotalUdp));
        case UdpBytesIn:    return QStringLiteral("%1").arg(MiscFuncs::formatBytes(statistics.bytesInTotalUdp));
        case UdpBytesOut:   return QStringLiteral("%1").arg(MiscFuncs::formatBytes(statistics.bytesOutTotalUdp));
        case Address:       return QStringLiteral("%1:%2").arg(config.server.toString(), QString::number(config.port));
        case Timeout:       return QStringLiteral("%1").arg(config.timeout);
        case Method:        return QStringLiteral("%1").arg(config.method);
        case Password:      return QStringLiteral("%1").arg(config.password);
        case Dnssrv:        return QStringLiteral("%1").arg(config.dnssrv);
        case CertFile:      return QStringLiteral("%1").arg(config.crtFile);
        case KeyFile:       return QStringLiteral("%1").arg(config.keyFile);
        case PktFile:       return QStringLiteral("%1").arg(config.pktFile);
        case HostsFile:     return QStringLiteral("%1").arg(config.hostFile);
        case CachBytes:     return QStringLiteral("%1").arg(MiscFuncs::formatBytes(statistics.bytesCaching));
        default: break;
        }
    }

    return {};
}
