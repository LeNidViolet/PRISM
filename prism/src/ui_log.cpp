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
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QMenu>
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

    this->treeView = new SearchableTreeView(this);
    this->treeModel = new LogTreeViewModel(this);
    this->treeModel->setHorizontalHeaderLabels(labels);
    this->treeView->setSourceModel(this->treeModel);
    this->treeView->setSortingEnabled(true);
    this->levelBox = new QComboBox(this);

    for ( int i = 1; i <= 5 ; ++i ) {
        this->levelBox->addItem(QString::number(i), i);
    }
    this->levelBox->setCurrentIndex(3);
    auto lbl = new QLabel("LEVEL:", this);

    auto ctxMenu = new QMenu(this);
    QObject::connect(this->treeView, &SearchableTreeView::customContextMenuRequested, this, [=]() { ctxMenu->exec(QCursor::pos()); });
    auto actn = ctxMenu->addAction("CLEAR LOGS");
    QObject::connect(actn, &QAction::triggered, this, &LogView::onClearClicked);


    auto btnClose = new QPushButton("CLOSE", this);
    QObject::connect(btnClose, &QPushButton::clicked, this, &LogView::hide);
    btnClose->setFocusPolicy(Qt::NoFocus);

    auto hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget(lbl);
    hlayout->addWidget(this->levelBox);
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
    this->setWindowTitle("LOG");
}

void LogView::onClearClicked() {

    if ( this->treeModel->rowCount() > 0 ) {
        auto ret = QMessageBox::question(this, "CONFIRM", "Remove All Logs?");
        if ( QMessageBox::Yes == ret ) {
            this->treeView->clear();
            qDeleteAll(this->logList);
            this->logList.clear();
        }
    }
}

void LogView::addLog(int level, QString msg) {

    if ( level > this->levelBox->currentData().toInt() ) return;

    auto log = new LogLine(level, msg);
    this->logList << log;

    QVariant var;
    var.setValue(log);

    QList<QStandardItem*> items;
    for ( int i = 0; i < G_N_ELEMENTS(OprtName); i++ ) {
        auto item = new QStandardItem();
        item->setCheckable(false);
        item->setEditable(false);
        item->setData(var);

        items << item;
    }
    this->treeModel->appendRow(items);
    this->treeView->postload();
}

__attribute__((unused)) void LogView::showEvent(QShowEvent *event) {

    static bool firstShow = true;
    QDialog::showEvent(event);
    if ( firstShow && this->treeModel->rowCount() > 0 ) {
        firstShow = false;
        this->treeView->header()->resizeSections(QHeaderView::ResizeToContents);
    }
}


QVariant LogTreeViewModel::data(const QModelIndex &index, int role) const {
    if ( !index.isValid() )
        return {};

    auto item = this->item(index.row());
    if ( nullptr == item ) return {};

    auto var = item->data();
    if ( !var.isValid() ) return {};

    auto log = var.value<LogLine *>();
    if ( !log ) return {};

    if ( role == Qt::DisplayRole ) {

        switch ( index.column() ) {
        case Log_Time:      return log->time.toString("hh:mm:ss");
        case Log_Level:     return QString::number(log->level);
        case Log_Message:   return log->msg;
        default: break;
        }
    }

    if ( role == Qt::ForegroundRole ) {

        if ( 1 == log->level ) return QBrush(COLOR_ERROR);
        if ( 2 == log->level ) return QBrush(COLOR_WARN);
    }

    return {};
}
