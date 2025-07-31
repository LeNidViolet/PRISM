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
#ifndef SEARCHABLE_TREEVIEW_H
#define SEARCHABLE_TREEVIEW_H

#include <QTreeView>
#include <QSortFilterProxyModel>
#include <QItemDelegate>
#include <QLabel>
#include <QComboBox>

class MyTreeView final : public QTreeView {
    Q_OBJECT

public:
    explicit MyTreeView(QWidget *parent = nullptr) : QTreeView(parent) {
    }

signals:
    void currentIndexChanged(const QModelIndex &current, const QModelIndex &previous);

protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override {
        this->scrollTo(current);
        emit(currentIndexChanged(current, previous));
    }
};

class SearchableTreeView final : public QWidget {
    Q_OBJECT

public:
    explicit SearchableTreeView(QWidget *parent = nullptr);

    [[nodiscard]] QHeaderView *header() const { return this->treeView->header(); }

    void setSortingEnabled(const bool enable) const { this->treeView->setSortingEnabled(enable); }
    void setAlternatingRowColors(const bool enable) const { this->treeView->setAlternatingRowColors(enable); }

    //    void setFirstColumnSpanned(int row, const QModelIndex &parent, bool span) { this->treeView->setFirstColumnSpanned(row, parent, span); }
    //    void collapse(const QModelIndex &index) { this->treeView->setExpanded(index, false); }
    //    void expand(const QModelIndex &index) { this->treeView->setExpanded(index, true); }

    void echoText(const QString &text, bool copyecho = false) const;

    [[nodiscard]] QModelIndex currentIndex() const { return this->treeView->currentIndex(); }
    [[nodiscard]] QSortFilterProxyModel *model() const { return this->proxyModel; }

    void setSourceModel(QAbstractItemModel *model) {
        this->proxyModel->setSourceModel(model);
        this->checkRowSpan();
    }

    void scrollTo(const QModelIndex &index) const { this->treeView->scrollTo(index); }

    void clear() const;

    void postload();

public slots:
    void expandAll() const { this->treeView->expandAll(); }
    void collapseAll() const { this->treeView->collapseAll(); }

signals:
    void currentIndexChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    void searching(const QString &text);

    void checkRowSpan(const QModelIndex &parent = QModelIndex());

    int currentRowCount(const QModelIndex &parent = QModelIndex());

    MyTreeView *treeView = nullptr;
    QLineEdit *lineEdit = nullptr;
    QLabel *echo = nullptr;
    QComboBox *mode = nullptr;
    QSortFilterProxyModel *proxyModel = nullptr;
};


class FixHeightTreeDelegate final : public QItemDelegate {
    Q_OBJECT

public:
    explicit FixHeightTreeDelegate(const int height, QObject *parent = nullptr) : QItemDelegate(parent),
        rowHeigth(height) {
    }

    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QSize oSize = QItemDelegate::sizeHint(option, index);
        oSize.setHeight(rowHeigth);

        return oSize;
    }

private:
    int rowHeigth;
};


#endif
