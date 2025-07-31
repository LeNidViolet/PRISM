/**
 *  Copyright 2025, LeNidViolet
 *  Created by LeNidViolet on 2025/07/30.
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

#include "ui_flow.h"
#include <QVBoxLayout>
#include <QMenu>
#include <QMessageBox>
#include <QTimer>
#include "define.h"
#include "misc.h"

typedef enum {
    CREATECONNECTEDNAME(Flow, Time),
    CREATECONNECTEDNAME(Flow, Type),
    CREATECONNECTEDNAME(Flow, Src),
    CREATECONNECTEDNAME(Flow, Dst),
    CREATECONNECTEDNAME(Flow, RxRate),
    CREATECONNECTEDNAME(Flow, TxRate),
    CREATECONNECTEDNAME(Flow, RxBytes),
    CREATECONNECTEDNAME(Flow, TxBytes),
    CREATECONNECTEDNAME(Flow, Duration),
} KEYOPRTCOLUMN;

static NAMEINDEX OprtName[] = {
    CREATECONNECTEDMAP(Flow, Time),
    CREATECONNECTEDMAP(Flow, Type),
    CREATECONNECTEDMAP(Flow, Src),
    CREATECONNECTEDMAP(Flow, Dst),
    CREATECONNECTEDMAP(Flow, RxRate),
    CREATECONNECTEDMAP(Flow, TxRate),
    CREATECONNECTEDMAP(Flow, RxBytes),
    CREATECONNECTEDMAP(Flow, TxBytes),
    CREATECONNECTEDMAP(Flow, Duration),
};


FlowView::FlowView(QWidget *parent) : QWidget(parent) {

    QStringList labels;
    for ( const auto &[index, name] : OprtName )
        labels << name;

    // 数据组件
    this->m_treeView = new SearchableTreeView(this);
    this->m_treeModel = new FlowViewListModel(this);
    this->m_treeModel->setHorizontalHeaderLabels(labels);
    this->m_treeView->setSourceModel(this->m_treeModel);
    this->m_treeView->setSortingEnabled(true);

    // 右键菜单
    // ReSharper disable once CppDFAMemoryLeak
    //const auto ctxMenu = new QMenu(this);
    //QObject::connect(
    //    this->m_treeView,
    //    &SearchableTreeView::customContextMenuRequested,
    //    this,
    //    [ctxMenu]() { ctxMenu->exec(QCursor::pos()); }
    //    );
    //const auto actn = ctxMenu->addAction(QStringLiteral("CLEAR LINKS"));
    //QObject::connect(actn, &QAction::triggered, this, &FlowView::onClearClicked);

    // 定时器
    // ReSharper disable once CppDFAMemoryLeak
    const auto timer = new QTimer(this);
    QObject::connect(
        timer,
        &QTimer::timeout,
        this,
        &FlowView::onTimeout
        );
    timer->start(1000);

    // ReSharper disable once CppDFAMemoryLeak
    const auto layout = new QVBoxLayout();
    layout->addWidget(this->m_treeView);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    this->setLayout(layout);
}

void FlowView::createNewFlowLine(const QSharedPointer<FLOW_NODE> &newNode) {

    const auto newLine = QSharedPointer<FLOW_LINE>::create(*newNode);

    const auto key = MiscFuncs::genFlowKey(newNode->isStream, newNode->index);
    this->m_lastFlowNodes[key] = newLine;

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

void FlowView::updateFlowLine(const QSharedPointer<FLOW_NODE> &node, const QSharedPointer<FLOW_LINE> &line) const {

    bool dirty = false;

    if (line->last.rxBytes != node->rxBytes) {
        Q_ASSERT(line->last.rxBytes < node->rxBytes);
        line->rxBytesDelta = node->rxBytes - line->last.rxBytes;
        line->last.rxBytes = node->rxBytes;

        dirty = true;
    } else {
        if (line->rxBytesDelta != 0) {
            line->rxBytesDelta = 0;
            dirty = true;
        }
    }

    if (line->last.txBytes != node->txBytes) {
        Q_ASSERT(line->last.txBytes < node->txBytes);
        line->txBytesDelta = node->txBytes - line->last.txBytes;
        line->last.txBytes = node->txBytes;

        dirty = true;
    } else {
        if (line->txBytesDelta != 0) {
            line->txBytesDelta = 0;
            dirty = true;
        }
    }

    if (line->state == FlowActive) {

        if (dirty) {
            line->timeInState = 0;
        } else {
            if (line->timeInState >= STAY_ACTIVE_STATE_SEC) {
                line->state = FlowQuiet;
                line->timeInState = 0;
                dirty = true;
            } else {
                line->timeInState++;
            }
        }

    } else if (line->state == FlowQuiet) {

        if (dirty) {
            line->state = FlowActive;
            line->timeInState = 0;
        }
    } else if (line->state == FlowNew) {

        if (line->timeInState >= STAY_NEW_STATE_SEC) {
            line->state = FlowQuiet;
            line->timeInState = 0;
            dirty = true;
        } else {
            line->timeInState++;
        }
    }

    if (dirty) {
        this->doUpdate(line);
    }
}

void FlowView::doUpdate(const QSharedPointer<FLOW_LINE> &line) const {

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

void FlowView::deleteFlowLine(const QSharedPointer<FLOW_LINE> &line) const {

    if (line->item) {
        const int row = line->item->row();
        QStandardItem *parent = line->item->parent();

        if (parent) {
            parent->removeRow(row);
        } else {
            this->m_treeModel->removeRow(row);
        }

        line->item = nullptr;
    }
}

void FlowView::onClearClicked() {

    if ( this->m_treeModel->rowCount() > 0 ) {
        const auto ret = QMessageBox::question(this, QStringLiteral("CONFIRM"), QStringLiteral("Remove All Flows?"));
        if ( QMessageBox::Yes == ret ) {

            // TODO 给删除的FLOW做标记
            // for ( int row = 0; row < this->treeModel->rowCount(); row++ ) {
            //
            //     auto item = this->treeModel->item(row);
            //     Q_ASSERT(nullptr != item);
            //     auto var = item->data();
            //     Q_ASSERT(var.isValid());
            //     auto link = var.value<LinkLine *>();
            //     Q_ASSERT(nullptr != link);
            //     link->item = nullptr;
            // }
            //
            // for ( int row = 0; row < this->treeModel->rowCount(); row++ ) {
            //     auto items = this->treeModel->takeRow(row);
            //     for ( auto *item : items ) {
            //         delete item;
            //     }
            // }
            this->m_treeView->clear();
        }
    }
}

void FlowView::onTimeout() {
    const auto newNodes = FlowDumper::instance().all();

    // 首先遍历最新的flows, 创建不存在的, 更新需要更新的
    for (auto &node : newNodes) {
        // 是否存在
        const auto key = MiscFuncs::genFlowKey(node->isStream, node->index);
        auto it = this->m_lastFlowNodes.find(key);
        if (it != this->m_lastFlowNodes.end()) {
            this->updateFlowLine(node, it.value());
        } else {
            this->createNewFlowLine(node);
        }
    }

    // 删除已经消失的flows
    auto it = this->m_lastFlowNodes.begin();
    while (it != this->m_lastFlowNodes.end()) {
        const auto line = it.value();

        bool found = false;
        for (const auto &node : newNodes) {
            if (line->last.index == node->index && line->last.isStream == node->isStream) {
                found = true;
                break;
            }
        }

        bool shouldErase = false;
        if (!found) {
            // 链接已经销毁了
            if (line->state != FlowClosed) {
                line->state = FlowClosed;
                line->timeInState = 0;

                this->doUpdate(line);
            } else {
                line->timeInState++;
                if (line->timeInState >= STAY_CLOSED_STATE_SEC) {
                    this->deleteFlowLine(line);
                    shouldErase = true;
                }
            }
        }

        if (shouldErase) {
            // delete line
            it = this->m_lastFlowNodes.erase(it);
        } else {
            ++it;
        }
    }
}


// ReSharper disable once CppParameterMayBeConst
QVariant FlowViewListModel::data(const QModelIndex &index, int role) const {

    if ( !index.isValid() )
        return {};

    const auto item = this->item(index.row());
    if ( nullptr == item ) return {};

    const auto var = item->data();
    if ( !var.isValid() ) return {};

    const auto line = var.value<QSharedPointer<FLOW_LINE>>();
    if ( !line ) return {};

    if ( role == Qt::DisplayRole ) {

        auto duration = QStringLiteral("-");
        if ( line->teardown ) {
            auto secs = line->last.createTime.secsTo(line->endTime);

            const int seconds = secs % 60;
            secs /= 60;
            const int minutes = secs % 60;

            duration = QStringLiteral("%1m:%2s").arg(QString::number(minutes), QString::number(seconds));
        }

        switch ( index.column() ) {
            case Flow_Time:     return line->last.createTime.toString(QStringLiteral("hh:mm:ss"));
            case Flow_Type:     return line->last.isStream ? "TCP" : "UDP";
            case Flow_Src:      return QStringLiteral("%1:%2").arg(line->last.localAddr, QString::number(line->last.localPort));
            case Flow_Dst:      return QStringLiteral("%1:%2").arg(line->last.remoteAddr, QString::number(line->last.remotePort));
            case Flow_RxRate:   return QStringLiteral("%1/s").arg(MiscFuncs::formatBytes(line->rxBytesDelta));
            case Flow_TxRate:   return QStringLiteral("%1/s").arg(MiscFuncs::formatBytes(line->rxBytesDelta));
            case Flow_RxBytes:  return MiscFuncs::formatBytes(line->last.rxBytes);
            case Flow_TxBytes:  return MiscFuncs::formatBytes(line->last.txBytes);
            case Flow_Duration: return duration;
            default: break;
        }
    }

    if (role == Qt::BackgroundRole) {
        if (FlowNew == line->state) return QBrush(COLOR_BACKGROUND_NEW);
        if (FlowActive == line->state) return QBrush(COLOR_BACKGROUND_ACTIVE);
        if (FlowClosed == line->state) return QBrush(COLOR_BACKGROUND_DELETE);
        if (FlowQuiet == line->state) return {};
    }


    return {};


}


