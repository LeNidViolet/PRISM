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
#include "ui_hosts.h"

#include <QDesktopServices>
#include <QFileInfo>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>
#include <QMenu>
#include "hosts.h"
#include "macros.h"
#include "misc.h"


typedef enum {
    CREATECONNECTEDNAME(Hosts, Address),
    CREATECONNECTEDNAME(Hosts, Domain),
} KEYOPRTCOLUMN;

static NAMEINDEX OprtName[] = {
    CREATECONNECTEDMAP(Hosts, Address),
    CREATECONNECTEDMAP(Hosts, Domain),
};

HostsView::HostsView(QWidget *parent, const Qt::WindowFlags f) : QDialog(parent, f) {

    QStringList labels;
    for ( const auto &[index, name] : OprtName )
        labels << name;

    this->m_treeView = new SearchableTreeView(this);
    this->m_treeModel = new HostsTreeViewModel(this);
    this->m_treeModel->setHorizontalHeaderLabels(labels);
    this->m_treeView->setSourceModel(this->m_treeModel);

    this->m_treeView->setSortingEnabled(true);

    // ReSharper disable once CppDFAMemoryLeak
    const auto timer = new QTimer(this);
    timer->start(5000);
    QObject::connect(timer, &QTimer::timeout, this, &HostsView::onTimeout);

    // ReSharper disable once CppDFAMemoryLeak
    const auto ctxMenu = new QMenu(this);
    QObject::connect(this->m_treeView, &SearchableTreeView::customContextMenuRequested, this, [ctxMenu]() { ctxMenu->exec(QCursor::pos()); });
    auto actn = ctxMenu->addAction(QStringLiteral("LOCATE HOSTS FILE"));
    QObject::connect(actn, &QAction::triggered, this, &HostsView::onExplrClicked);
    actn = ctxMenu->addAction(QStringLiteral("CLEAR HOSTS"));
    QObject::connect(actn, &QAction::triggered, this, &HostsView::onClearClicked);

    // ReSharper disable once CppDFAMemoryLeak
    const auto btnClose = new QPushButton(QStringLiteral("CLOSE"), this);
    QObject::connect(btnClose, &QPushButton::clicked, this, &HostsView::hide);
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
    this->setWindowTitle(QStringLiteral("HOSTS"));
}

void HostsView::showEvent(QShowEvent *event) {
    static bool firstShow = true;
    QDialog::showEvent(event);
    if ( firstShow && this->m_treeModel->rowCount() > 0 ) {
        firstShow = false;
        this->m_treeView->header()->resizeSections(QHeaderView::ResizeToContents);
    }
}

void HostsView::createNewHostsLine(const QSharedPointer<HOSTS_NODE> &newNode) {

    const auto newLine = QSharedPointer<HOSTS_LINE>::create(*newNode);

    const auto key = newNode->address.toString();
    this->m_lastHostsNodes[key] = newLine;

    QVariant var;
    var.setValue(newLine);

    QList<QStandardItem*> items;
    for ( int i = 0; i < G_N_ELEMENTS(OprtName); i++ ) {
        auto *item = new QStandardItem();
        item->setCheckable(false);
        item->setEditable(false);
        item->setData(var);

        if ( 0 == i )
            newLine->item = item;
        items << item;
    }
    this->m_treeModel->appendRow(items);
    this->m_treeView->postload();
}

void HostsView::updateHostsLine(const QSharedPointer<HOSTS_NODE> &node, const QSharedPointer<HOSTS_LINE> &line) const {

    bool dirty = false;

    if (line->last.domains.size() != node->domains.size()) {
        line->last.domains = node->domains;
        dirty = true;
    }

    if (dirty) {
        this->doUpdate(line);
    }
}

void HostsView::doUpdate(const QSharedPointer<HOSTS_LINE> &line) const {

    const auto index = this->m_treeModel->indexFromItem(line->item);
    if (!index.isValid()) return;

    const auto *item = this->m_treeModel->item(index.row());
    if ( !item ) return ;
    const auto var = item->data();
    if ( !var.isValid() ) return ;

    const auto start = this->m_treeModel->index(index.row(), 0);
    const auto end = this->m_treeModel->index(index.row(), G_N_ELEMENTS(OprtName) - 1);
    emit this->m_treeModel->dataChanged(start, end);
}

void HostsView::setHostsPath(const QString &hostsPath) {
    this->m_hostsPath = hostsPath;
    HostsDumper::instance().setHostsPath(hostsPath, true);
}


void HostsView::onTimeout() {
    const auto newNodes = HostsDumper::instance().all();

    // 首先遍历最新的flows, 创建不存在的, 更新需要更新的
    for (auto &node : newNodes) {
        // 是否存在
        const auto key = node->address.toString();
        auto it = this->m_lastHostsNodes.find(key);
        if (it != this->m_lastHostsNodes.end()) {
            this->updateHostsLine(node, it.value());
        } else {
            this->createNewHostsLine(node);
        }
    }

    // 删除已经消失的hosts
    auto it = this->m_lastHostsNodes.begin();
    while (it != this->m_lastHostsNodes.end()) {
        const auto line = it.value();

        bool found = false;
        for (const auto &node : newNodes) {
            if (line->last.address.toString() == node->address.toString()) {
                found = true;
                break;
            }
        }

        bool shouldErase = false;
        if (!found) {
            shouldErase = true;
        }

        if (shouldErase) {
            it = this->m_lastHostsNodes.erase(it);
        } else {
            ++it;
        }
    }

    HostsDumper::instance().writeHostsOut(false);
}

void HostsView::onExplrClicked() const {

    if ( this->m_hostsPath.isEmpty() ) return ;

    const QString folderPath = QFileInfo(this->m_hostsPath).absolutePath();
    const QUrl url = QUrl::fromLocalFile(folderPath);

    QDesktopServices::openUrl(url);
}

void HostsView::onClearClicked() {

    if ( this->m_treeModel->rowCount() > 0 ) {
        const auto ret = QMessageBox::question(this, QStringLiteral("CONFIRM"), QStringLiteral("Remove All Name Resolution?"));
        if ( QMessageBox::Yes == ret ) {

            this->m_treeView->clear();
            this->m_lastHostsNodes.clear();
            HostsDumper::instance().clear();
        }
    }
}

// ReSharper disable once CppParameterMayBeConst
QVariant HostsTreeViewModel::data(const QModelIndex &index, int role) const {
    if ( !index.isValid() )
        return {};

    const auto item = this->item(index.row());
    if ( nullptr == item ) return {};

    const auto var = item->data();
    if ( !var.isValid() ) return {};

    const auto line = var.value<QSharedPointer<HOSTS_LINE>>();
    if ( !line ) return {};

    if ( role == Qt::DisplayRole ) {

        switch ( index.column() ) {
        case Hosts_Domain:    return line->last.domains.join(' ');
        case Hosts_Address:   return line->last.address.toString();
        default: break;
        }
    }

    return {};
}
