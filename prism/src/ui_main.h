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
#ifndef PRISM_UI_MAIN_H
#define PRISM_UI_MAIN_H

#include <QMainWindow>
#include <QDialog>
#include <QStatusBar>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QThread>
#include <QTextEdit>
#include <utility>
#include "ui_status.h"
#include "ui_config.h"
#include "ui_hosts.h"
#include "ui_statistics.h"
#include "func.h"
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

    QVariant data(const QModelIndex &index, int role) const override;
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
    QThread *httpThread = nullptr;
    QThread *ioThread = nullptr;

    QByteArray pktBytes;
    void savePkt(bool now);

    void onStartClicked();

    void captureStart();
    static void captureStop();

    void updateStatus();
    void onConfigConfirm(ConfigVars &config);

    void createNewLink(LinkLine *link);
    void updateLink(LinkLine *link);

    void parseHostsFile();
};



class PrismView : public QMainWindow {

    Q_OBJECT

public:
    explicit PrismView(QWidget *parent = nullptr, Qt::WindowFlags f = nullptr);

private slots:
    void onSsMsgOutput(int level, const QString& msg);
    void onSsBindDone(const QString& host, unsigned short port);

private:
    StatusView *status = nullptr;
};


#endif //PRISM_UI_MAIN_H
