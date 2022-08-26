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
#include "ui_log.h"
#include <QHeaderView>
#include <QMenu>
#include <QVBoxLayout>
#include "macros.h"

typedef enum {
    CREATECONNECTEDNAME(Log, Time),
    CREATECONNECTEDNAME(Log, Level),
    CREATECONNECTEDNAME(Log, Message),
} KEYOPRTCOLUMN;

static NAMEINDEX OprtName[] = {
    CREATECONNECTEDMAP(Log, Time),
    CREATECONNECTEDMAP(Log, Level),
    CREATECONNECTEDMAP(Log, Message),
};


LogView::LogView(QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f) {

    QStringList labels;
    for ( const auto &name : OprtName )
        labels << name.name;

    this->rootItem = new LogDetailItem(nullptr);
    this->treeView = new SearchableTreeView(this);
    this->sourceModel = new TreeModel(this->rootItem, labels, this);
    this->treeView->setSourceModel(this->sourceModel);

    // 右键菜单
    auto ctxMenu = new QMenu(this);
    QObject::connect(
        this->treeView,
        &SearchableTreeView::customContextMenuRequested,
        this,
        [=]() { ctxMenu->exec(QCursor::pos()); }
    );

    // 拷贝选中项内容
    auto action = ctxMenu->addAction("COPY ITEM");
    QObject::connect(
        action,
        &QAction::triggered,
        this->treeView,
        &SearchableTreeView::copyCurrentText
    );

    // 清空数据
    action = ctxMenu->addAction("CLEAR");
    QObject::connect(
        action,
        &QAction::triggered,
        this,
        &LogView::clear
    );


    auto layout = new QVBoxLayout();
    layout->addWidget(this->treeView);
    layout->setMargin(0);
    layout->setSpacing(0);

    this->setLayout(layout);
    this->resize(450, 400);
}

void LogView::clear() {

    this->treeView->clear();
    qDeleteAll(this->logList);
    this->logList.clear();
}

void LogView::addLog(int level, QString &msg) {

    this->logList.append(new LogLine(level, msg));

    auto root = this->sourceModel->root();

    this->sourceModel->insertRow(this->logList.size() - 1);
    auto index = this->sourceModel->index(this->logList.size() - 1, 0, QModelIndex());
    auto item = this->sourceModel->itemFromIndex(index);
    item->setPtr((void *)this->logList.last());
    root->appendChild(item);

    this->treeView->postload();
}

QVariant LogDetailItem::data(int column) const {

    auto log = static_cast<const LogLine *>(this->ptr());

    switch ( column ) {
    case Log_Time:      return log->time.toString("hh:mm:ss");
    case Log_Level:     return QString::number(log->level);
    case Log_Message:   return log->msg;
    default: break;
    }

    return {};
}
