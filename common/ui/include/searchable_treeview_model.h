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

#ifndef SEARCHABLE_TREEVIEW_MODEL_H
#define SEARCHABLE_TREEVIEW_MODEL_H

#include <QAbstractItemModel>
#include <QVariant>


class TreeItem {

public:
    explicit TreeItem(TreeItem *parentItem = nullptr) : mParentItem(parentItem) {}
    virtual ~TreeItem() { this->removeAllChild(); }

    void appendChild(TreeItem *item) { this->mChildItems.append(item); }

    bool removeChild(int position, int count);
    void removeAllChild() { qDeleteAll(this->mChildItems); this->mChildItems.clear(); }

    bool insertChild(int position, int count);

    // Get the row's child node pointer
    [[nodiscard]] TreeItem *child(const int row) const { return this->mChildItems.value(row); }
    // Get the parent node pointer
    [[nodiscard]] TreeItem *parentItem() const { return this->mParentItem; }
    // Child node count
    [[nodiscard]] int childCount() const { return static_cast<int>(this->mChildItems.count()); }
    // Get the number of child nodes of the parent node that the node is
    [[nodiscard]] int row() const { return nullptr != this->mParentItem ? this->mParentItem->indexOf(this) : 0; }

    // Get the data of the column of the node
    [[nodiscard]] virtual QVariant data(int column) const = 0;
    // Create a new item, this function must be implemented by the derived class
    virtual TreeItem *createItem(TreeItem *parentItem) = 0;

    [[nodiscard]] int level() const { return this->mLevel; }
    void setLevel(const int level) { this->mLevel = level; }

    // set/get data pointer stored in the node
    void setPtr(void *p) { this->mPtr = p; }
    [[nodiscard]] void *ptr() const { return this->mPtr; }

    void setPtr2(void *p) { this->mPtr2 = p; }
    [[nodiscard]] void *ptr2() const { return this->mPtr2; }

    void setColumnSpan(const bool bl) { this->mColumnSpan = bl; };
    [[nodiscard]] bool columnSpan() const { return this->mColumnSpan; }

    int indexOf(const TreeItem * childItem) const { return static_cast<int>(this->mChildItems.indexOf(const_cast<TreeItem * const>(childItem))); }

private:
    QList<TreeItem *> mChildItems;
    TreeItem *mParentItem = nullptr;

    void *mPtr = nullptr;
    void *mPtr2 = nullptr;
    int mLevel = 0;
    bool mColumnSpan = false;
};
Q_DECLARE_METATYPE(TreeItem *)




class TreeModel final : public QAbstractItemModel {

    Q_OBJECT

public:
    explicit TreeModel(TreeItem *rootItem, QStringList headers, QObject *parent = nullptr)
        : QAbstractItemModel(parent),
          mHeaders(std::move(headers)),
          mRootItem(rootItem) { }

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override { return static_cast<int>(this->mHeaders.size()); }
    [[nodiscard]] bool hasChildren(const QModelIndex &parent) const override { return this->rowCount(parent) > 0; }

    bool insertRows(int position, int rows, const QModelIndex &parent) override;
    bool removeRows(int position, int rows, const QModelIndex &parent) override;

public:
    [[nodiscard]] TreeItem *itemFromIndex(const QModelIndex &index) const;

    [[nodiscard]] TreeItem *root() const { return this->mRootItem; }

private:
    QStringList mHeaders;
    TreeItem *mRootItem;    // root node
};

#endif
