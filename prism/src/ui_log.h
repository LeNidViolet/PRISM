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
#ifndef PRISM_UI_LOG_H
#define PRISM_UI_LOG_H

#include <QDialog>
#include <QTime>
#include <QTimer>
#include <QStandardItemModel>
#include <utility>
#include "searchable_treeview.h"
#include "custom/logging.hpp"

struct LOG_LINE {
    unsigned int no;
    LOG_LEVEL level;
    QString msg;
    QTime time;

    LOG_LINE(const LOG_LEVEL Level, QString  Message)
        : no(0), level(Level), msg(std::move(Message)), time(QTime::currentTime()) {}
} ;
Q_DECLARE_METATYPE(QSharedPointer<LOG_LINE>)


class LogTreeViewModel final : public QStandardItemModel {

    Q_OBJECT

    public:
    explicit LogTreeViewModel(QObject *parent = nullptr) : QStandardItemModel(parent) {}

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
};


class LogView final : public QDialog {

    Q_OBJECT

public:
    explicit LogView(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    void addLog(LOG_LEVEL level, const QString &msg);

private:
    SearchableTreeView *m_treeView = nullptr;
    LogTreeViewModel *m_treeModel = nullptr;

    QComboBox *m_logLevelCombo = nullptr;

    QList<QSharedPointer<LOG_LINE>> m_logList;

private slots:
    void onClearClicked();
    void onTimeout();
};



#endif //PRISM_UI_LOG_H
