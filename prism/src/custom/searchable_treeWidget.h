/**
 *  Copyright 2020, raprepo.
 *  Created by raprepo on 2020/6/24.
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
#ifndef SEARCHABLE_TREEWIDGET_H
#define SEARCHABLE_TREEWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QComboBox>
#include <QItemDelegate>

class SearchableTreeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchableTreeWidget(const QStringList &headerLabels, QWidget *parent = nullptr, bool dbclickCopy = true);

    void addTopLevelItem(QTreeWidgetItem *item) { this->treeWgt->addTopLevelItem(item); }

    QHeaderView *header() { return this->treeWgt->header(); }
    [[nodiscard]] QTreeWidgetItem *currentItem() const { return this->treeWgt->currentItem(); }

    void setSortingEnabled(bool enable) { this->treeWgt->setSortingEnabled(enable); }
    void setAlternatingRowColors(bool enable) { this->treeWgt->setAlternatingRowColors(enable); }
    int topLevelItemCount() { return this->treeWgt->topLevelItemCount(); }
    QTreeWidgetItem *topLevelItem(int index) { return this->treeWgt->topLevelItem(index); }

    void collapseItem(const QTreeWidgetItem *item) { this->treeWgt->collapseItem(item); }
    void expandItem(const QTreeWidgetItem *item) { this->treeWgt->expandItem(item); }

    void echoText(const QString &text, bool copyecho = false);
    QString copyCurrentItemText();

    void clear() { this->treeWgt->clear(); /*this->lineEdit->clear();*/ this->echo->clear(); }
    void postload();

public slots:
    void expandAll() { this->treeWgt->expandAll(); }
    void collapseAll() { this->treeWgt->collapseAll(); }

signals:
    void itemDoubleClicked(QTreeWidgetItem *item, int column);
    void currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    void searching(const QString &text);
    void hideAllItems(bool hide);
    void hideAllSubItems(QTreeWidgetItem *item, bool hide);
    static void hideItems(QList<QTreeWidgetItem *> &items, bool hide);
    void itemDoubleClickedHandler(QTreeWidgetItem *item, int column);

    QTreeWidget *treeWgt;
    QLineEdit *lineEdit;
    QLabel *echo;
    QComboBox *mode;
};


class SpanTreeDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit SpanTreeDelegate(int height, QObject *parent = nullptr) :
        QItemDelegate(parent), rowHeigth(height) {}

    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    int rowHeigth;
};

#endif
