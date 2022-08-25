/**
 *  Copyright 2022, raprepo.
 *  Created by raprepo on 2022/8/25.
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
#ifndef PRISM_UI_LOG_H
#define PRISM_UI_LOG_H

#include <QDialog>
#include <QList>
#include <QTime>
#include "custom/searchable_treeview.h"
#include "custom/searchable_treeview_model.h"


class LogLine {

public:
    explicit LogLine(int level, QString &msg)
    : level(level), msg(msg) { this->time = QTime::currentTime(); };

    int level;
    QString msg;
    QTime time;
};


class LogDetailItem : public TreeItem {

public:
    explicit LogDetailItem(TreeItem *parentItem = nullptr) : TreeItem(parentItem) {}

    QVariant data(int column) const override;
    TreeItem *createItem(TreeItem *parentItem) override { return new LogDetailItem(parentItem); }
};


class LogView : public QDialog {

    Q_OBJECT

public:
    explicit LogView(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    void addLog(int level, QString msg);

private:
    SearchableTreeView *treeView = nullptr;
    TreeModel *sourceModel = nullptr;
    LogDetailItem *rootItem = nullptr;

    QList<const LogLine *> logList;

    void clear();
};



#endif //PRISM_UI_LOG_H
