//
// Created by raven on 2023/10/26.
//

#ifndef PRISM_UI_MAINWGT_H
#define PRISM_UI_MAINWGT_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <utility>
#include "ui_status.h"
#include "ui_config.h"
#include "ui_hosts.h"
#include "ui_statistics.h"
#include "misc.h"
#include "custom/http_server.h"
#include "custom/searchable_treeview.h"


// 记录每一个链接信息
class LinkLine {

public:
    explicit LinkLine(
        int index,
        QString localAddr,
        unsigned short localPort,
        QString remoteAddr,
        unsigned short remotePort,
        bool isStream)
        : index(index),
          localAddr(std::move(localAddr)), localPort(localPort),
          remoteAddr(std::move(remoteAddr)), remotePort(remotePort),
          isStream(isStream)
    {
        this->createTime = QTime::currentTime();
        this->teardown = false;
        this->bytesIn = 0;
        this->bytesOut = 0;
    };

    QTime createTime;
    QTime endTime;
    bool isStream;
    int index;

    QString localAddr;
    unsigned short localPort;
    QString remoteAddr;
    unsigned short remotePort;

    bool teardown;
    qint64 bytesIn;
    qint64 bytesOut;

    QStandardItem *item = nullptr;

    pkt_track iotrack = {};
};
Q_DECLARE_METATYPE(LinkLine *)






class LinkTreeViewModel : public QStandardItemModel {

    Q_OBJECT

public:
    explicit LinkTreeViewModel(QObject *parent = nullptr) : QStandardItemModel(parent) {}

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
};


// 统计信息
class StatisticsVars {

public:
    qint64     linkTotalTcp;
    qint64     linkActiveTcp;
    qint64     bytesInTotalTcp;
    qint64     bytesOutTotalTcp;

    qint64     linkTotalUdp;
    qint64     linkActiveUdp;
    qint64     bytesInTotalUdp;
    qint64     bytesOutTotalUdp;

    qint64     bytesCaching;
};


class MainWidget : public QWidget {

    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent);

    ConfigVars &getConfigVars() { return this->mconfig; }
    StatisticsVars &getStatisticsVars() { return this->mstatistics; }


private slots:
    void onSsStop();
    void onSsStreamConnectionMade(
        const QString& domain_local,
        const QString& addr_local,
        unsigned short port_local,
        const QString& domain_remote,
        const QString& addr_remote,
        unsigned short port_remote,
        int stream_index
    );
    void onSsStreamTeardown(int stream_index);
    void onSsPlainStream(
        const QByteArray& data,
        bool send_out,
        int stream_index
    );
    void onSsDgramConnectionMade(
        const QString& domain_local,
        const QString& addr_local,
        unsigned short port_local,
        const QString& domain_remote,
        const QString& addr_remote,
        unsigned short port_remote,
        int dgram_index
    );
    void onSsDgramTeardown(int dgram_index);
    void onSsPlainDgram(
        const QByteArray& data,
        bool send_out,
        int dgram_index
    );
    void onFlushCache();

private:
    bool mcapturing = false;
    QMap<QString, LinkLine*> mlinks;
    ConfigVars mconfig{};
    StatisticsVars mstatistics{};

    QPushButton *btnCapture = nullptr;
    QLabel *caaddr = nullptr;

    SearchableTreeView *treeView = nullptr;
    LinkTreeViewModel *treeModel = nullptr;
    StatisticsView *statisticsView = nullptr;
    HostsView *hostsView = nullptr;

    HttpServer *httpServer = nullptr;

    QByteArray pktBytes;
    void savePkt(bool now);

    void onStartClicked();
    void onClearClicked();

    void captureStart();
    static void captureStop();

    void updateStatus();
    void onConfigConfirm(ConfigVars &config);

    void createNewLink(LinkLine *link);
    void updateLink(LinkLine *link);

    void parseHostsFile();
};



#endif //PRISM_UI_MAINWGT_H
