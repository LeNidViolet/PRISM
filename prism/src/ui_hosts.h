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
#ifndef PRISM_UI_HOSTS_H
#define PRISM_UI_HOSTS_H

#include <QDialog>
#include <QMap>
#include <QTime>
#include <QHostAddress>
#include <QStandardItemModel>
#include "custom/searchable_treeview.h"


class HostsLine {

public:
    explicit HostsLine(QHostAddress& address, QStringList& domains)
        : address(address), domains(domains) { }

    QHostAddress address;
    QStringList domains;
    QStandardItem *item = nullptr;
};
Q_DECLARE_METATYPE(HostsLine *)


class HostsTreeViewModel : public QStandardItemModel {

Q_OBJECT

public:
    explicit HostsTreeViewModel(QObject *parent = nullptr) : QStandardItemModel(parent) {}

    QVariant data(const QModelIndex &index, int role) const override;
};


class HostsView : public QDialog {

    Q_OBJECT

public:
    explicit HostsView(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    void addHosts(QHostAddress& address, QStringList& domains);
    void setHostsPath(QString& path) { this->hostsPath = path; }
    void clear();

protected:
    void showEvent(QShowEvent *event) override;

private:
    SearchableTreeView *treeView = nullptr;
    HostsTreeViewModel *treeModel = nullptr;

    QMap<QString, HostsLine*> hostsMap;
    bool dirty = false;
    QString hostsPath;

    void createNewHosts(HostsLine *hosts) const;
    void updateHosts(const HostsLine *hosts) const;

    void onExplrClicked() const;
    void onClearClicked();

private slots:
    void onTimeOutHostsIo();
};



#endif //PRISM_UI_HOSTS_H
