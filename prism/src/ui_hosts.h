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
#ifndef PRISM_UI_HOSTS_UI_H
#define PRISM_UI_HOSTS_UI_H

#include <QDialog>
#include <QMap>
#include <QStandardItemModel>
#include "searchable_treeview.h"
#include "hosts.h"

struct HOSTS_LINE {

    explicit HOSTS_LINE(HOSTS_NODE node) : last(std::move(node)) {
        item = nullptr;
    }

    HOSTS_NODE last;
    QStandardItem *item;
};
Q_DECLARE_METATYPE(QSharedPointer<HOSTS_LINE>)

class HostsTreeViewModel final : public QStandardItemModel {

    Q_OBJECT

public:
    explicit HostsTreeViewModel(QObject *parent = nullptr) : QStandardItemModel(parent) {}
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
};


class HostsView final : public QDialog {

    Q_OBJECT

public:
    explicit HostsView(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    void setHostsPath(const QString &hostsPath);

protected:
    void showEvent(QShowEvent *event) override;

private:
    SearchableTreeView *m_treeView = nullptr;
    HostsTreeViewModel *m_treeModel = nullptr;

    QString m_hostsPath;

    QMap<QString, QSharedPointer<HOSTS_LINE>> m_lastHostsNodes;

    void createNewHostsLine(const QSharedPointer<HOSTS_NODE> &newNode);
    void updateHostsLine(const QSharedPointer<HOSTS_NODE> &node, const QSharedPointer<HOSTS_LINE> &line) const;
    void doUpdate(const QSharedPointer<HOSTS_LINE> &line) const;

    void onExplrClicked() const;
    void onClearClicked();

private slots:
    void onTimeout();
};



#endif //PRISM_UI_HOSTS_UI_H
