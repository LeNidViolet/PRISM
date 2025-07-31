/**
 *  Copyright 2022, LeNidViolet.
 *  Created by LeNidViolet on 2022/8/25.
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
#include <QVBoxLayout>
#include <QMessageBox>
#include <QMenu>
#include <QPushButton>
#include "define.h"
#include "if_raw.h"

typedef enum {
    CREATECONNECTEDNAME(Log, No),
    CREATECONNECTEDNAME(Log, Time),
    CREATECONNECTEDNAME(Log, Level),
    CREATECONNECTEDNAME(Log, Message),
} KEYOPRTCOLUMN;

static NAMEINDEX OprtName[] = {
    CREATECONNECTEDMAP(Log, No),
    CREATECONNECTEDMAP(Log, Time),
    CREATECONNECTEDMAP(Log, Level),
    CREATECONNECTEDMAP(Log, Message),
};


LogView::LogView(QWidget *parent, const Qt::WindowFlags f) : QDialog(parent, f) {
    QStringList labels;
    for (const auto &[index, name]: OprtName)
        labels << name;

    this->m_logLevelCombo = new QComboBox(this);
    this->m_treeView = new SearchableTreeView(this);
    this->m_treeModel = new LogTreeViewModel(this);
    this->m_treeModel->setHorizontalHeaderLabels(labels);
    this->m_treeView->setSourceModel(this->m_treeModel);
    this->m_treeView->setSortingEnabled(true);

    int index = 0;
    int levelIndex = 0;
    const auto currentLevel = GetLogLevel();
    for (auto it = LogLevelNames.constBegin(); it != LogLevelNames.constEnd(); ++it) {
        const LOG_LEVEL level = it.key();
        const QString &name = it.value();

        this->m_logLevelCombo->addItem(name, level);

        if (currentLevel == level) {
            levelIndex = index;
        }

        index++;
    }
    this->m_logLevelCombo->setCurrentIndex(levelIndex);

    QObject::connect(this->m_logLevelCombo, &QComboBox::currentIndexChanged, this, [this](int) {
        const auto level = static_cast<LOG_LEVEL>(this->m_logLevelCombo->currentData().toInt());
        SetLogLevel(level);
    });


    // ReSharper disable once CppDFAMemoryLeak
    const auto btnClose = new QPushButton(QStringLiteral("CLOSE"), this);
    QObject::connect(btnClose, &QPushButton::clicked, this, &LogView::hide);
    btnClose->setFocusPolicy(Qt::NoFocus);

    // ReSharper disable once CppDFAMemoryLeak
    auto *ctxMenu = new QMenu(this);
    QObject::connect(
        this->m_treeView,
        &SearchableTreeView::customContextMenuRequested,
        this,
        [ctxMenu]() {
            ctxMenu->exec(QCursor::pos());
        });
    const auto *actn = ctxMenu->addAction(QStringLiteral("CLEAR LOGS"));
    QObject::connect(
        actn,
        &QAction::triggered,
        this,
        &LogView::onClearClicked
    );

    // ReSharper disable once CppDFAMemoryLeak
    auto *timer = new QTimer(this);
    QObject::connect(
        timer,
        &QTimer::timeout,
        this,
        &LogView::onTimeout
        );
    timer->start(1000);

    // ReSharper disable once CppDFAMemoryLeak
    const auto lbl = new QLabel(QStringLiteral("LOG LEVEL: "), this);
    lbl->setBuddy(this->m_logLevelCombo);

    // ReSharper disable once CppDFAMemoryLeak
    const auto hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget(lbl);
    hlayout->addWidget(this->m_logLevelCombo);
    hlayout->addWidget(btnClose);

    // ReSharper disable once CppDFAMemoryLeak
    auto *layout = new QVBoxLayout();
    layout->addWidget(this->m_treeView);
    layout->addLayout(hlayout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    this->setLayout(layout);
    this->resize(600, 400);
    this->setWindowTitle(QStringLiteral("LOGS"));
}

void LogView::onClearClicked() {
    if (this->m_treeModel->rowCount() > 0) {
        const auto ret = QMessageBox::question(this, QStringLiteral("CONFIRM"), QStringLiteral("Remove All Logs?"));
        if (QMessageBox::Yes == ret) {
            this->m_treeView->clear();
            this->m_logList.clear();
        }
    }
}

void LogView::onTimeout() {
    while (true) {
        auto log = LOGGING_POP();
        if (!log.has_value()) {
            break; // NO MORE
        }
        const auto &[level, message] = log.value();
        this->addLog(level, message);
    }
}

void LogView::addLog(const LOG_LEVEL level, const QString &msg) {

    static unsigned int no = 0;

    const auto line = QSharedPointer<LOG_LINE>::create(level, msg);
    line->no = no++;

    this->m_logList.push_back(line);

    QVariant var;
    var.setValue(line);

    QList<QStandardItem *> items;
    for (int i = 0; i < G_N_ELEMENTS(OprtName); i++) {
        // ReSharper disable once CppDFAMemoryLeak
        auto *item = new QStandardItem();
        item->setCheckable(false);
        item->setEditable(false);
        item->setData(var);

        items << item;
    }
    this->m_treeModel->appendRow(items);
    this->m_treeView->postload();
}

QVariant LogTreeViewModel::data(const QModelIndex &index, const int role) const {
    if (!index.isValid())
        return {};

    const auto *item = this->item(index.row());
    if (nullptr == item) return {};

    const auto var = item->data();
    if (!var.isValid()) return {};

    const auto log = var.value<QSharedPointer<LOG_LINE>>();
    if (!log) return {};

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case Log_No:        return QStringLiteral("%1").arg(log->no, 5, 10, QChar('0'));
            case Log_Time:      return log->time.toString(QStringLiteral("hh:mm:ss"));
            case Log_Level:     return QString::number(log->level);
            case Log_Message:   return log->msg;
            default: break;
        }
    }

    if (role == Qt::ForegroundRole) {
        if (LOG_ERROR == log->level) return QBrush(COLOR_FOREGROUND_ERROR);
        if (LOG_WARN == log->level) return QBrush(COLOR_FOREGROUND_WARN);
    }

    return {};
}
