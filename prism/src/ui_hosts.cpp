/**
 *  Copyright 2022, raprepo.
 *  Created by raprepo on 2022/9/1.
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
#include "ui_hosts.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>
#include <QUrl>
#include <QMenu>
#include <QFileInfo>
#include <QDesktopServices>
#include "io_impl.h"
#include "macros.h"

typedef enum {
    CREATECONNECTEDNAME(Hosts, Address),
    CREATECONNECTEDNAME(Hosts, Domain),
} KEYOPRTCOLUMN;

static NAMEINDEX OprtName[] = {
    CREATECONNECTEDMAP(Hosts, Address),
    CREATECONNECTEDMAP(Hosts, Domain),
};

HostsView::HostsView(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

    QStringList labels;
    for ( const auto &name : OprtName )
        labels << name.name;

    this->treeView = new SearchableTreeView(this);
    this->treeModel = new HostsTreeViewModel(this);
    this->treeModel->setHorizontalHeaderLabels(labels);
    this->treeView->setSourceModel(this->treeModel);

    this->treeView->setSortingEnabled(true);

    auto timer = new QTimer(this);
    timer->setSingleShot(false);
    timer->setInterval(5000);
    timer->start(5000);
    QObject::connect(timer, &QTimer::timeout, this, &HostsView::onTimeOutHostsIo);

    auto ctxMenu = new QMenu(this);
    QObject::connect(this->treeView, &SearchableTreeView::customContextMenuRequested, this, [=]() { ctxMenu->exec(QCursor::pos()); });
    auto actn = ctxMenu->addAction("EXPLR HOSTS FILE");
    QObject::connect(actn, &QAction::triggered, this, &HostsView::onExplrClicked);
    actn = ctxMenu->addAction("CLEAR HOSTS");
    QObject::connect(actn, &QAction::triggered, this, &HostsView::onClearClicked);

    auto btnClose = new QPushButton("CLOSE", this);
    QObject::connect(btnClose, &QPushButton::clicked, this, &HostsView::hide);
    btnClose->setFocusPolicy(Qt::NoFocus);

    auto hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget(btnClose);
    hlayout->setContentsMargins(0,0,0,0);
    hlayout->setSpacing(0);

    auto layout = new QVBoxLayout();
    layout->addWidget(this->treeView);
    layout->addLayout(hlayout);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    this->setLayout(layout);
    this->resize(600, 400);
    this->setWindowTitle("HOSTS");
}

void HostsView::addHosts(QHostAddress& address, QStringList& domains) {

    if ( address.isNull() ) return;
    if ( domains.empty() ) return;

    // 过滤域名为IP的条目
    while ( true ) {
        bool breakout = true;
        for ( auto& domain : domains  ) {
            if ( !QHostAddress(domain).isNull() ) {
                domains.removeAll(domain);
                breakout = false;
                break;
            }
        }
        if ( breakout ) break;
    }
    if ( domains.empty() ) return;


    if ( this->hostsMap.contains(address.toString()) ) {
        auto hosts = this->hostsMap[address.toString()];

        // 添加可能存在的新域名
        bool append = false;
        for ( auto& domain : domains  ) {
            if ( !hosts->domains.contains(domain) ) {
                hosts->domains.append(domain);
                append = true;
            }
        }
        if ( append ) {
            this->dirty = true;
            this->updateHosts(hosts);
        }

    } else {

        // 添加新HOSTS条目
        auto hosts = new HostsLine(address, domains);
        this->hostsMap[address.toString()] = hosts;
        this->dirty = true;
        this->createNewHosts(hosts);
    }
}

void HostsView::clear() {

    this->treeView->clear();
    qDeleteAll(this->hostsMap.values());
    this->hostsMap.clear();
    this->dirty = false;
}

__attribute__((unused)) void HostsView::showEvent(QShowEvent *event) {

    static bool firstShow = true;
    QDialog::showEvent(event);
    if ( firstShow && this->treeModel->rowCount() > 0 ) {
        firstShow = false;
        this->treeView->header()->resizeSections(QHeaderView::ResizeToContents);
    }
}

void HostsView::createNewHosts(HostsLine *hosts) {

    QVariant var;
    var.setValue(hosts);

    QList<QStandardItem*> items;
    for ( int i = 0; i < G_N_ELEMENTS(OprtName); i++ ) {
        auto item = new QStandardItem();
        item->setCheckable(false);
        item->setEditable(false);
        item->setData(var);

        items << item;

        if ( 0 == i )
            hosts->item = item;
    }
    this->treeModel->appendRow(items);
    this->treeView->postload();
}

void HostsView::updateHosts(HostsLine *hosts) {

    auto index = this->treeModel->indexFromItem(hosts->item);
    if ( !index.isValid() ) return ;

    auto item = this->treeModel->item(index.row());
    if ( !item ) return ;
    auto var = item->data();
    if ( !var.isValid() ) return ;

    auto start = this->treeModel->index(index.row(), 0);
    auto end = this->treeModel->index(index.row(), G_N_ELEMENTS(OprtName) - 1);
    emit this->treeModel->dataChanged(start, end);
}

void HostsView::onTimeOutHostsIo() {

    // 定时写HOSTS文件
    if ( this->hostsPath.isEmpty() ) return ;
    if ( !this->dirty ) return ;

    // FORMAT HOSTS LINES
    QString lines;
    for ( auto& hosts : this->hostsMap ) {
        auto line = hosts->address.toString();
        line += " " + hosts->domains.join(" ");
        lines += line + "\r\n";
    }

    this->dirty = false;

    auto io = IoImpl::instance();

    bool bok = QMetaObject::invokeMethod(
        io,
        "writeHostsOut",
        Qt::QueuedConnection,
        Q_ARG(QString, this->hostsPath),
        Q_ARG(QString, lines)
    );
    Q_ASSERT(bok);
}

void HostsView::onExplrClicked() {

    if ( this->hostsPath.isEmpty() ) return ;
    auto dir = "file://" + QFileInfo(this->hostsPath).absolutePath();
    QDesktopServices::openUrl(QUrl(dir));
}

void HostsView::onClearClicked() {

    if ( this->hostsPath.isEmpty() ) return ;

    if ( this->treeModel->rowCount() > 0 ) {
        auto ret = QMessageBox::question(this, "CONFIRM", "Remove All Name Resolution?");
        if ( QMessageBox::Yes == ret ) {

            QString lines;
            auto io = IoImpl::instance();

            bool bok = QMetaObject::invokeMethod(
                io,
                "writeHostsOut",
                Qt::QueuedConnection,
                Q_ARG(QString, this->hostsPath),
                Q_ARG(QString, lines)
            );
            Q_ASSERT(bok);

            this->clear();
        }
    }
}

QVariant HostsTreeViewModel::data(const QModelIndex &index, int role) const {
    if ( !index.isValid() )
        return {};

    auto item = this->item(index.row());
    if ( nullptr == item ) return {};

    auto var = item->data();
    if ( !var.isValid() ) return {};

    auto hosts = var.value<HostsLine *>();
    if ( !hosts ) return {};

    if ( role == Qt::DisplayRole ) {

        switch ( index.column() ) {
        case Hosts_Domain:    return hosts->domains.join(' ');
        case Hosts_Address:   return hosts->address.toString();
        default: break;
        }
    }

    return {};
}
