/**
 *  Copyright 2022, raprepo.
 *  Created by raprepo on 2022/9/1.
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
#ifndef PRISM_UI_STATISTICS_H
#define PRISM_UI_STATISTICS_H

#include <QDialog>
#include <QStandardItemModel>
#include "custom/searchable_treeview.h"


class MainWidget;

class StaticsTreeViewModel : public QStandardItemModel {

Q_OBJECT

public:
    explicit StaticsTreeViewModel(MainWidget *mainWidget, QObject *parent = nullptr)
        : mmainWidget(mainWidget), QStandardItemModel(parent) {}

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

private:
    MainWidget *mmainWidget = nullptr;
};


// 统计对话框窗口
class StatisticsView : public QDialog {

Q_OBJECT

public:
    explicit StatisticsView(MainWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

public slots:
    void updateStatistics();

signals:
    void flushCache();

protected:
    void showEvent(QShowEvent *event) override;

private:
    SearchableTreeView *treeView = nullptr;
    StaticsTreeViewModel *treeModel = nullptr;

    static void explrFile(const QString& filePath);
};





#endif //PRISM_UI_STATISTICS_H
