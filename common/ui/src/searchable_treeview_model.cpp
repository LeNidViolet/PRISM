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

#include "searchable_treeview_model.h"
#include "define.h"

QVariant TreeModel::headerData(const int section, const Qt::Orientation orientation, const int role) const {

    if ( orientation == Qt::Horizontal ) {
        if ( role == Qt::DisplayRole ) {
            return this->mHeaders.at(section);
        }
    }
    return {};
}

QVariant TreeModel::data(const QModelIndex &index, const int role) const {

    if ( !index.isValid() )
        return {};

    const auto item = static_cast<TreeItem *>(index.internalPointer());
    if ( role == Qt::DisplayRole ) {
        return item->data(index.column());
    }
    if ( role == ITEMROLE ) {
        return QVariant::fromValue(item);
    }
    if ( role == SPANROLE ) {
        return item->columnSpan();
    }
    return {};
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const {

    if ( !index.isValid() )
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QModelIndex TreeModel::index(const int row, const int column, const QModelIndex &parent) const {

    if ( !hasIndex(row, column, parent) )
        return {};

    TreeItem *parentItem;

    if ( !parent.isValid() )
        parentItem = this->mRootItem;
    else
        parentItem = static_cast<TreeItem *>(parent.internalPointer());

    const TreeItem *childItem = parentItem->child(row);
    if ( childItem )
        return createIndex(row, column, childItem);
    else
        return {};
}

QModelIndex TreeModel::parent(const QModelIndex &index) const{

    if ( !index.isValid() )
        return {};

    const auto childItem = static_cast<TreeItem *>(index.internalPointer());
    const TreeItem *parentItem = childItem->parentItem();

    if ( parentItem == this->mRootItem )
        return {};

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const {

    TreeItem *parentItem;
    if ( parent.column() > 0 )
        return 0;

    if ( !parent.isValid() )
        parentItem = this->mRootItem;
    else
        parentItem = static_cast<TreeItem *>(parent.internalPointer());

    const int ret = parentItem->childCount();
    return ret;
}

TreeItem *TreeModel::itemFromIndex(const QModelIndex &index) const {

    if ( !index.isValid() )
        return this->mRootItem;

    const auto item = static_cast<TreeItem *>(index.internalPointer());
    return item;
}

bool TreeModel::insertRows(const int position, const int rows, const QModelIndex &parent) {

    TreeItem *parentItem = this->itemFromIndex(parent);
    bool success;

    beginInsertRows(parent, position, position + rows - 1);
    success = parentItem->insertChild(position, rows);
    endInsertRows();

    return success;
}


bool TreeModel::removeRows(const int position, const int rows, const QModelIndex &parent) {

    TreeItem *parentItem = this->itemFromIndex(parent);

    for ( int i = 0; i < rows; i++ ) {
        QModelIndex thisIndex = this->index(i, 0, parent);
        const auto item = static_cast<TreeItem *>(thisIndex.internalPointer());

        if ( item && item->childCount() ) {
            removeRows(0, item->childCount(), thisIndex);
        }
    }

    beginRemoveRows(parent, position, position + rows - 1);
    const bool success = parentItem->removeChild(position, rows);
    endRemoveRows();

    return success;
}




bool TreeItem::insertChild(const int position, const int count) {

    if ( position < 0 || position > this->mChildItems.size() )
        return false;

    for ( int row = 0; row < count; ++row ) {
        const auto item = this->createItem(this);
        this->mChildItems.insert(position, item);
    }

    return true;
}


bool TreeItem::removeChild(const int position, const int count) {

    if ( position < 0 || position + count > this->mChildItems.size() )
        return false;

    for ( int row = 0; row < count; ++row ) {
        const auto theItem = this->mChildItems.takeAt(position);
        if ( !theItem->mChildItems.isEmpty() )
            theItem->removeChild(0, static_cast<int>(theItem->mChildItems.size()));

        delete(theItem);
    }

    return true;
}

