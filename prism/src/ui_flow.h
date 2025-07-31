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
#ifndef PRISM_UI_FLOW_UI_H
#define PRISM_UI_FLOW_UI_H

#include <QWidget>
#include <QStandardItemModel>
#include <utility>
#include <QMap>
#include "searchable_treeview.h"
#include "flow.h"

#define STAY_NEW_STATE_SEC          3
#define STAY_ACTIVE_STATE_SEC       2
#define STAY_CLOSED_STATE_SEC       3

enum FlowState {
    FlowNew,
    FlowQuiet,
    FlowActive,
    FlowClosed
};


struct FLOW_LINE {

    explicit FLOW_LINE(FLOW_NODE node) : last(std::move(node)) {

        this->teardown = false;
        this->item = nullptr;
        this->timeInState = 0;
        this->state = FlowNew;

        this->txBytesDelta = 0;
        this->rxBytesDelta = 0;
    };

    QTime endTime;
    bool teardown;
    int timeInState;

    unsigned int rxBytesDelta;
    unsigned int txBytesDelta;

    FlowState state;

    FLOW_NODE last;
    QStandardItem *item;
};
Q_DECLARE_METATYPE(QSharedPointer<FLOW_LINE>)


class FlowViewListModel final : public QStandardItemModel {

    Q_OBJECT

public:
    explicit FlowViewListModel(QObject *parent = nullptr) : QStandardItemModel(parent) {}

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
};


class FlowView final : public QWidget {

    Q_OBJECT

public:
    explicit FlowView(QWidget *parent);

private:
    SearchableTreeView *m_treeView = nullptr;
    FlowViewListModel *m_treeModel = nullptr;

    QMap<QString, QSharedPointer<FLOW_LINE>> m_lastFlowNodes;

    void createNewFlowLine(const QSharedPointer<FLOW_NODE> &newNode);
    void updateFlowLine(const QSharedPointer<FLOW_NODE> &node, const QSharedPointer<FLOW_LINE> &line) const;
    void doUpdate(const QSharedPointer<FLOW_LINE> &line) const;
    void deleteFlowLine(const QSharedPointer<FLOW_LINE> &line) const;

private slots:
    void onClearClicked();
    void onTimeout();
};



#endif //PRISM_UI_FLOW_UI_H
