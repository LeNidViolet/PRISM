//
// Created by raven on 2023/10/26.
//

#include "ui_mainwgt.h"
#include "ui_config.h"
#include "misc.h"
#include <QApplication>
#include <QMenuBar>
#include <QMessageBox>
#include <QIcon>
#include <QFile>
#include <QPushButton>
#include <QVBoxLayout>
#include <QThread>

#include "ss_impl.h"
#include "macros.h"

#include "custom/http_server.h"



typedef enum {
    CREATECONNECTEDNAME(Link, Time),
    CREATECONNECTEDNAME(Link, Type),
    CREATECONNECTEDNAME(Link, Src),
    CREATECONNECTEDNAME(Link, Dst),
    CREATECONNECTEDNAME(Link, Duration),
    CREATECONNECTEDNAME(Link, BytesIn),
    CREATECONNECTEDNAME(Link, BytesOut),
} KEYOPRTCOLUMN;

static NAMEINDEX OprtName[] = {
    CREATECONNECTEDMAP(Link, Time),
    CREATECONNECTEDMAP(Link, Type),
    CREATECONNECTEDMAP(Link, Src),
    CREATECONNECTEDMAP(Link, Dst),
    CREATECONNECTEDMAP(Link, Duration),
    CREATECONNECTEDMAP(Link, BytesIn),
    CREATECONNECTEDMAP(Link, BytesOut),
};



MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {

    QStringList labels;
    for ( const auto &name : OprtName )
        labels << name.name;

    // 数据组件
    this->treeView = new SearchableTreeView(this);
    this->treeModel = new LinkTreeViewModel(this);
    this->treeModel->setHorizontalHeaderLabels(labels);
    this->treeView->setSourceModel(this->treeModel);
    this->treeView->setSortingEnabled(true);

    // 子窗口
    this->statisticsView = new StatisticsView(this);
    this->hostsView = new HostsView(this);

    // 刷新缓存的数据包到文件
    QObject::connect(this->statisticsView, &StatisticsView::flushCache, this, &MainWidget::onFlushCache);

    this->caaddr = new QLabel(this);

    // 关联SS接口
    auto ss = SsImpl::instance();
    QObject::connect(ss, &SsImpl::ssStop, this, &MainWidget::onSsStop);
    QObject::connect(ss, &SsImpl::ssStreamConnectionMade, this, &MainWidget::onSsStreamConnectionMade);
    QObject::connect(ss, &SsImpl::ssStreamTeardown, this, &MainWidget::onSsStreamTeardown);
    QObject::connect(ss, &SsImpl::ssPlainStream, this, &MainWidget::onSsPlainStream);
    QObject::connect(ss, &SsImpl::ssDgramConnectionMade, this, &MainWidget::onSsDgramConnectionMade);
    QObject::connect(ss, &SsImpl::ssDgramTeardown, this, &MainWidget::onSsDgramTeardown);
    QObject::connect(ss, &SsImpl::ssPlainDgram, this, &MainWidget::onSsPlainDgram);

    // 运行SS线程
    auto thread = new QThread(this);
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    ss->moveToThread(thread);
    thread->start();

    auto btnStatis = new QPushButton("STATIS", this);
    auto btnHosts = new QPushButton("HOSTS", this);
    this->btnCapture = new QPushButton(this);
    this->mcapturing = false;
    this->updateStatus();

    QObject::connect(btnStatis, &QPushButton::clicked, this, [=]() { this->statisticsView->show(); });
    QObject::connect(btnHosts, &QPushButton::clicked, this, [=]() { this->hostsView->show(); });
    QObject::connect(this->btnCapture, &QPushButton::clicked, this, &MainWidget::onStartClicked);

    // 主界面右键菜单
    auto ctxMenu = new QMenu(this);
    QObject::connect(this->treeView, &SearchableTreeView::customContextMenuRequested, this, [=]() { ctxMenu->exec(QCursor::pos()); });
    auto actn = ctxMenu->addAction("CLEAR LINKS");
    QObject::connect(actn, &QAction::triggered, this, &MainWidget::onClearClicked);

    auto hlayout1 = new QHBoxLayout();
    hlayout1->addWidget(this->caaddr);
    hlayout1->addStretch();
    hlayout1->addWidget(btnHosts);
    hlayout1->addWidget(btnStatis);
    hlayout1->addWidget(this->btnCapture);

    auto hlayout2 = new QHBoxLayout();
    hlayout2->addWidget(this->treeView);

    auto layout = new QVBoxLayout();
    layout->addLayout(hlayout1);
    layout->addLayout(hlayout2);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    this->setLayout(layout);
}

void MainWidget::onStartClicked() {

    if ( this->mcapturing ) {
        auto ret = QMessageBox::question(this, "CONFIRM", "Stop Capture Packet?");
        if ( QMessageBox::Yes == ret ) {
            MainWidget::captureStop();

        } else {
            return;
        }
    } else {
        auto dlg = new ConfigView(this->mconfig, this);
        QObject::connect(dlg, &ConfigView::configConfirm, this, &MainWidget::onConfigConfirm);

        dlg->setModal(true);
        dlg->setAttribute(Qt::WA_DeleteOnClose);
        dlg->show();
    }
}

void MainWidget::updateStatus() {

    if ( this->mcapturing ) {
        this->btnCapture->setIcon(QIcon(":/res/stop.png"));
        this->btnCapture->setText("STOP");
    } else {
        this->btnCapture->setIcon(QIcon(":/res/start.png"));
        this->btnCapture->setText("START");
    }
}

void MainWidget::onConfigConfirm(ConfigVars &config) {

    static int httpport = 7700;

    Q_ASSERT(false == this->mcapturing);

    this->mconfig = config;

    this->hostsView->clear();
    this->hostsView->setHostsPath(this->mconfig.hostFile);
    this->parseHostsFile();

    this->pktBytes.clear();
    this->mstatistics.bytesCaching = 0;

    if ( nullptr != this->httpServer ) {

        this->httpServer->terminal();
        this->httpServer->deleteLater();
        this->httpServer = nullptr;
    }
    this->httpServer = new HttpServer(
            this->mconfig.server,
            httpport,
            this->mconfig.crtFile,
            "ca",
            "ca.crt"
    );

    auto addr = QString("http://%1:%2/ca").arg(this->mconfig.server.toString(), QString::number(httpport++));
    this->caaddr->setText(addr);

    this->captureStart();
    this->mcapturing = !this->mcapturing;
    this->updateStatus();
}

void MainWidget::captureStart() {

    auto ss = SsImpl::instance();
    auto json = MiscFuncs::configToJson(this->mconfig);

    // ss服务运行在另一个线程中 这里进行跨线程调用
    bool bok = QMetaObject::invokeMethod(
            ss,
            "onCommandStart",
            Qt::QueuedConnection,
            Q_ARG(QByteArray, json)
    );
    Q_ASSERT(bok);
}

void MainWidget::captureStop() {

    auto ss = SsImpl::instance();
    ss->onCommandStop();
}



// SS 服务停止了
void MainWidget::onSsStop() {

    Q_ASSERT(true == this->mcapturing);

    this->mcapturing = !this->mcapturing;
    this->updateStatus();

    this->savePkt(true);
}


// TCP 链接建立
void MainWidget::onSsStreamConnectionMade(
        const QString& domain_local,
        const QString& addr_local,
        unsigned short port_local,
        const QString& domain_remote,
        const QString& addr_remote,
        unsigned short port_remote,
        int stream_index) {

    auto key = MiscFuncs::genLinkKey(true, stream_index);
    Q_ASSERT(!this->mlinks.contains(key));

    auto link = new LinkLine(stream_index, domain_local, port_local, domain_remote, port_remote, true);
    this->mlinks[key] = link;

    link->iotrack.src_ip = QHostAddress(addr_local).toIPv4Address();
    link->iotrack.dst_ip = QHostAddress(addr_remote).toIPv4Address();
    link->iotrack.src_port = port_local;
    link->iotrack.dst_port = port_remote;
    link->iotrack.protocol = PROTOCOL_TCP;
    link->iotrack.bytes_in = 0;
    link->iotrack.bytes_out = 0;

    // 创建握手包
    auto bytes = MiscFuncs::buildTcpHandshakePkt(&link->iotrack);
    this->pktBytes.append(bytes);
    this->savePkt(false);

    this->createNewLink(link);

    this->mstatistics.linkTotalTcp++;
    this->mstatistics.linkActiveTcp++;
    this->statisticsView->updateStatistics();

    auto ar = QHostAddress(addr_remote);
    auto dr = QStringList(domain_remote);
    this->hostsView->addHosts(ar, dr);
}

// TCP 链接中断
void MainWidget::onSsStreamTeardown(int stream_index) {

    auto key = MiscFuncs::genLinkKey(true, stream_index);
    Q_ASSERT(this->mlinks.contains(key));

    auto link = this->mlinks[key];
    link->teardown = true;
    link->endTime = QTime::currentTime();

    auto bytes = MiscFuncs::buildTcpFinPkt(&link->iotrack, true);
    this->pktBytes.append(bytes);
    this->savePkt(false);

    // 更新当前行
    this->updateLink(link);

    // 更新统计信息
    this->mstatistics.linkActiveTcp--;
    this->statisticsView->updateStatistics();
}

// TCP 数据
void MainWidget::onSsPlainStream(const QByteArray& data, bool send_out, int stream_index) {

    auto key = MiscFuncs::genLinkKey(true, stream_index);
    Q_ASSERT(this->mlinks.contains(key));

    auto link = this->mlinks[key];

    if ( send_out ) {
        this->mstatistics.bytesOutTotalTcp += data.size();
        link->bytesOut += data.size();
    } else {
        this->mstatistics.bytesInTotalTcp += data.size();
        link->bytesIn += data.size();
    }

    auto bytes = MiscFuncs::buildTcpPayloadPkt(&link->iotrack, data.data(), data.size(), send_out);
    this->pktBytes.append(bytes);
    this->savePkt(false);

    // 更新当前行
    this->updateLink(link);

    // 更新统计信息
    this->statisticsView->updateStatistics();
}

// UDP 链接建立
void MainWidget::onSsDgramConnectionMade(
        const QString& domain_local,
        const QString& addr_local,
        unsigned short port_local,
        const QString& domain_remote,
        const QString& addr_remote,
        unsigned short port_remote,
        int dgram_index) {

    // DNS 相关数据隐藏
    if ( 53 == port_remote ) return;

    auto key = MiscFuncs::genLinkKey(false, dgram_index);
    Q_ASSERT(!this->mlinks.contains(key));

    auto link = new LinkLine(dgram_index, domain_local, port_local, domain_remote, port_remote, false);
    this->mlinks[key] = link;

    link->iotrack.src_ip = QHostAddress(addr_local).toIPv4Address();
    link->iotrack.dst_ip = QHostAddress(addr_remote).toIPv4Address();
    link->iotrack.src_port = port_local;
    link->iotrack.dst_port = port_remote;
    link->iotrack.protocol = PROTOCOL_UDP;
    link->iotrack.bytes_in = 0;
    link->iotrack.bytes_out = 0;

    this->createNewLink(link);

    // 更新统计信息
    this->mstatistics.linkTotalUdp++;
    this->mstatistics.linkActiveUdp++;
    this->statisticsView->updateStatistics();

    auto ar = QHostAddress(addr_remote);
    auto dr = QStringList(domain_remote);
    this->hostsView->addHosts(ar, dr);
}

// UDP 链接中断
void MainWidget::onSsDgramTeardown(int dgram_index) {

    auto key = MiscFuncs::genLinkKey(false, dgram_index);

    if ( this->mlinks.contains(key) ) {

        auto link = this->mlinks[key];
        link->teardown = true;
        link->endTime = QTime::currentTime();

        // 更新当前行
        this->updateLink(link);

        // 更新统计信息
        this->mstatistics.linkActiveUdp--;
        this->statisticsView->updateStatistics();
    }
}

// UDP 数据
void MainWidget::onSsPlainDgram(const QByteArray& data, bool send_out, int dgram_index) {

    auto key = MiscFuncs::genLinkKey(false, dgram_index);
    if ( this->mlinks.contains(key) ) {
        auto link = this->mlinks[key];

        if ( send_out ) {
            this->mstatistics.bytesOutTotalUdp += data.size();
            link->bytesOut += data.size();
        } else {
            this->mstatistics.bytesInTotalUdp += data.size();
            link->bytesIn += data.size();
        }

        auto bytes = MiscFuncs::buildUdpPayloadPkt(&link->iotrack, data.data(), data.size(), send_out);
        this->pktBytes.append(bytes);
        this->savePkt(false);

        // 更新当前行
        this->updateLink(link);

        // 更新统计信息
        this->statisticsView->updateStatistics();
    }
}

void MainWidget::createNewLink(LinkLine *link) {

    QVariant var;
    var.setValue(link);

    QList<QStandardItem*> items;
    for ( int i = 0; i < G_N_ELEMENTS(OprtName); i++ ) {
        auto item = new QStandardItem();
        item->setCheckable(false);
        item->setEditable(false);
        item->setData(var);

        if ( 0 == i )
            link->item = item;
        items << item;
    }
    this->treeModel->appendRow(items);
    this->treeView->postload();
}

void MainWidget::updateLink(LinkLine *link) {

    if ( nullptr == link->item ) return ;
    auto index = this->treeModel->indexFromItem(link->item);
    if ( !index.isValid() ) return ;

    auto item = this->treeModel->item(index.row());
    if ( !item ) return ;
    auto var = item->data();
    if ( !var.isValid() ) return ;

    auto start = this->treeModel->index(index.row(), 0);
    auto end = this->treeModel->index(index.row(), G_N_ELEMENTS(OprtName) - 1);
    emit this->treeModel->dataChanged(start, end);
}

void MainWidget::parseHostsFile() {

    QFile hostFile(this->mconfig.hostFile);
    if ( hostFile.open(QIODevice::ReadOnly) ) {

        QTextStream in(&hostFile);
        while ( !in.atEnd() ) {
            auto line = in.readLine();
            line = line.trimmed();
            auto parts = line.split(" ");
            if ( parts.size() >= 2 ) {
                auto address = QHostAddress(parts[0]);
                parts.removeFirst();
                auto domains = parts;
                this->hostsView->addHosts(address, domains);
            }
        }
        hostFile.close();
    }
}

void MainWidget::savePkt(bool now) {

    this->mstatistics.bytesCaching = this->pktBytes.size();
    if ( this->pktBytes.isEmpty() ) return;
    if ( this->mconfig.pktFile.isEmpty() ) return;

    if ( this->pktBytes.size() >= 20 * 1024 || now ) {

        MiscFuncs::writePktsOut(this->mconfig.pktFile, this->pktBytes);

        this->pktBytes.clear();
    }
    this->mstatistics.bytesCaching = this->pktBytes.size();
}

void MainWidget::onFlushCache() {

    this->savePkt(true);
    this->statisticsView->updateStatistics();
}

void MainWidget::onClearClicked() {

    if ( this->treeModel->rowCount() > 0 ) {
        auto ret = QMessageBox::question(this, "CONFIRM", "Remove All Links?");
        if ( QMessageBox::Yes == ret ) {

//            for ( int row = 0; row < this->treeModel->rowCount(); row++ ) {
//                for ( int column = 0; column < this->treeModel->columnCount(); column++ ) {
//                    auto item = this->treeModel->takeItem(row, column);
//                    delete item;
//                }
//            }

            for ( int row = 0; row < this->treeModel->rowCount(); row++ ) {

                auto item = this->treeModel->item(row);
                Q_ASSERT(nullptr != item);
                auto var = item->data();
                Q_ASSERT(var.isValid());
                auto link = var.value<LinkLine *>();
                Q_ASSERT(nullptr != link);
                link->item = nullptr;
            }

            for ( int row = 0; row < this->treeModel->rowCount(); row++ ) {
                auto items = this->treeModel->takeRow(row);
                for ( auto *item : items ) {
                    delete item;
                }
            }
            this->treeView->clear();
        }
    }
}


QVariant LinkTreeViewModel::data(const QModelIndex &index, int role) const {
    if ( !index.isValid() )
        return {};

    if ( role == Qt::DisplayRole ) {
        auto item = this->item(index.row());
        if ( nullptr == item ) return {};

        auto var = item->data();
        if ( !var.isValid() ) return {};

        auto link = var.value<LinkLine *>();
        if ( !link ) return {};

        QString duration = "-";
        if ( link->teardown ) {
            auto secs = link->createTime.secsTo(link->endTime);

            int seconds = (int) (secs % 60);
            secs /= 60;
            int minutes = (int) (secs % 60);

            duration = QString("%1m:%2s").arg(QString::number(minutes), QString::number(seconds));
        }

        switch ( index.column() ) {
            case Link_Time:     return link->createTime.toString("hh:mm:ss");
            case Link_Type:     return link->isStream ? "TCP" : "UDP";
            case Link_Src:      return QString("%1:%2").arg(link->localAddr, QString::number(link->localPort));
            case Link_Dst:      return QString("%1:%2").arg(link->remoteAddr, QString::number(link->remotePort));
            case Link_Duration: return duration;
            case Link_BytesIn:  return MiscFuncs::formatBytes(link->bytesIn);
            case Link_BytesOut: return MiscFuncs::formatBytes(link->bytesOut);
            default: break;
        }
    }

    return {};
}
