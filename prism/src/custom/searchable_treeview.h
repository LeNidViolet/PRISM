/**
 *  Copyright 2020, LeNidViolet.
 *  Created by LeNidViolet on 2020/7/6.
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

class MyTreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit MyTreeView(QWidget *parent = nullptr) : QTreeView(parent) {}

signals:
    void currentIndexChanged(const QModelIndex &current, const QModelIndex &previous);

protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override
    {
        this->scrollTo(current);
        emit(currentIndexChanged(current, previous));
    }
};

class SearchableTreeView : public QWidget
{
    Q_OBJECT

public:
    explicit SearchableTreeView(QWidget *parent = nullptr, bool dbclickCopy = true);

    [[nodiscard]] QHeaderView *header() const { return this->treeView->header(); }

    void setSortingEnabled(bool enable) { this->treeView->setSortingEnabled(enable); }
    void setAlternatingRowColors(bool enable) { this->treeView->setAlternatingRowColors(enable); }

    //    void setFirstColumnSpanned(int row, const QModelIndex &parent, bool span) { this->treeView->setFirstColumnSpanned(row, parent, span); }
    //    void collapse(const QModelIndex &index) { this->treeView->setExpanded(index, false); }
    //    void expand(const QModelIndex &index) { this->treeView->setExpanded(index, true); }

    void echoText(const QString &text, bool copyecho = false);
    QString copyCurrentText();

    [[nodiscard]] QModelIndex currentIndex() const { return this->treeView->currentIndex(); }
    QSortFilterProxyModel *model() { return this->proxyModel; }

    void setSourceModel(QAbstractItemModel *model) { this->proxyModel->setSourceModel(model); this->checkRowSpan(); }

    void scrollTo(const QModelIndex &index) { this->treeView->scrollTo(index); }

    void clear();
    void postload();
public slots:
    void expandAll() { this->treeView->expandAll(); }
    void collapseAll() { this->treeView->collapseAll(); }

signals:
    void doubleClicked(const QModelIndex &index);
    void currentIndexChanged(const QModelIndex &current, const QModelIndex &previous);

private:
    void searching(const QString &text);
    void checkRowSpan(const QModelIndex &parent = QModelIndex());
    int currentRowCount(const QModelIndex &parent = QModelIndex());
    void doubleClickedHandler(const QModelIndex &index);

    MyTreeView *treeView = nullptr;
    QLineEdit *lineEdit = nullptr;
    QLabel *echo = nullptr;
    QComboBox *mode = nullptr;
    QSortFilterProxyModel *proxyModel = nullptr;
};


class FixHeightTreeDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit FixHeightTreeDelegate(int height, QObject *parent = nullptr) :
        QItemDelegate(parent), rowHeigth(height) {}

    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QSize oSize = QItemDelegate::sizeHint(option, index);
        oSize.setHeight(rowHeigth);

        return oSize;
    }

private:
    int rowHeigth;
};


#endif
