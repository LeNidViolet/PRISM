/**
 *  Copyright 2025, Song,Tao.
 *  Created by Song,Tao on 2025/07/08.
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

class SearchableTreeWidget final : public QWidget {
    Q_OBJECT

public:
    explicit SearchableTreeWidget(const QStringList &headerLabels, QWidget *parent = nullptr);

    void addTopLevelItem(QTreeWidgetItem *item) const { this->treeWgt->addTopLevelItem(item); }

    [[nodiscard]] QHeaderView *header() const { return this->treeWgt->header(); }
    [[nodiscard]] QTreeWidgetItem *currentItem() const { return this->treeWgt->currentItem(); }

    void setSortingEnabled(const bool enable) const { this->treeWgt->setSortingEnabled(enable); }
    void setAlternatingRowColors(const bool enable) const { this->treeWgt->setAlternatingRowColors(enable); }
    [[nodiscard]] int topLevelItemCount() const { return this->treeWgt->topLevelItemCount(); }
    [[nodiscard]] QTreeWidgetItem *topLevelItem(const int index) const { return this->treeWgt->topLevelItem(index); }

    void collapseItem(const QTreeWidgetItem *item) const { this->treeWgt->collapseItem(item); }
    void expandItem(const QTreeWidgetItem *item) const { this->treeWgt->expandItem(item); }

    void echoText(const QString &text, bool copyecho = false) const;

    QString copyCurrentItemText() const;

    void clear() const {
        this->treeWgt->clear(); /*this->lineEdit->clear();*/
        this->echo->clear();
    }

    void postload() const;

public slots:
    void expandAll() const { this->treeWgt->expandAll(); }
    void collapseAll() const { this->treeWgt->collapseAll(); }

signals:
    void currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    void searching(const QString &text) const;

    void hideAllItems(bool hide) const;

    static void hideAllSubItems(const QTreeWidgetItem *item, bool hide);

    static void hideItems(QList<QTreeWidgetItem *> &items, bool hide);

    QTreeWidget *treeWgt;
    QLineEdit *lineEdit;
    QLabel *echo;
    QComboBox *mode;
};


class SpanTreeDelegate final : public QItemDelegate {
    Q_OBJECT

public:
    explicit SpanTreeDelegate(const int height, QObject *parent = nullptr) : QItemDelegate(parent), rowHeigth(height) {
    }

    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    int rowHeigth;
};

#endif
