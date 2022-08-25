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

#include "searchable_treeWidget.h"

#include <QApplication>
#include <QClipboard>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QShortcut>

#include "define.h"

SearchableTreeWidget::SearchableTreeWidget(const QStringList &headerLabels, QWidget *parent, bool dbclickCopy)
    : QWidget(parent)
{
    this->treeWgt = new QTreeWidget(this);
    this->lineEdit = new QLineEdit(this);
    this->echo = new QLabel(this);
    this->mode = new QComboBox(this);

    this->treeWgt->setHeaderLabels(headerLabels);
    this->treeWgt->setContextMenuPolicy(Qt::CustomContextMenu);
    //    this->treeWgt->setSortingEnabled(true);
    this->treeWgt->setAlternatingRowColors(true);
    this->treeWgt->setUniformRowHeights(true);

    QObject::connect(
        this->treeWgt,
        &QTreeWidget::customContextMenuRequested,
        this,
        &SearchableTreeWidget::customContextMenuRequested
    );
    QObject::connect(
        this->treeWgt,
        &QTreeWidget::currentItemChanged,
        this,
        &SearchableTreeWidget::currentItemChanged
    );
    QObject::connect(
        this->treeWgt,
        &QTreeWidget::itemDoubleClicked,
        this,
        &SearchableTreeWidget::itemDoubleClicked
    );
    if ( dbclickCopy ) {
        QObject::connect(
            this->treeWgt,
            &QTreeWidget::itemDoubleClicked,
            this,
            &SearchableTreeWidget::itemDoubleClickedHandler
        );
    }

    QObject::connect(
        this->lineEdit,
        &QLineEdit::textEdited,
        this,
        &SearchableTreeWidget::searching);
    QObject::connect(
        this->mode,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        [=](int index) { searching(this->lineEdit->text()); }
    );


    auto label = new QLabel("SEARCH:", this);
    label->setBuddy(this->lineEdit);



    auto delegate = new SpanTreeDelegate(TREE_WIDGET_ROW_HEIGHT, this->treeWgt);
    this->treeWgt->setItemDelegate(delegate);


    auto shortCut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_F), this);
    QObject::connect(
        shortCut,
        &QShortcut::activated,
        this,
        [=]() {this->lineEdit->setFocus(); }
    );


    this->mode->addItem("CONTAINS", Qt::MatchContains + Qt::MatchRecursive);
    this->mode->addItem("WILDCARD", Qt::MatchWildcard + Qt::MatchRecursive);
    this->mode->addItem("REGEXP", Qt::MatchRegExp + Qt::MatchRecursive);
    this->mode->setCurrentIndex(0);

    this->echo->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto hLayout = new QHBoxLayout();
    hLayout->addWidget(label);
    hLayout->addWidget(this->mode);
    hLayout->addWidget(this->lineEdit, 2);
    hLayout->addStretch();
    hLayout->addWidget(this->echo, 1);

    auto layout = new QVBoxLayout();
    layout->addWidget(this->treeWgt);
    layout->addLayout(hLayout);

    layout->setSpacing(0);
    layout->setMargin(0);
    this->setLayout(layout);
}

void SearchableTreeWidget::searching(const QString &text)
{
    QString pattern = text.simplified();

    if ( pattern.isEmpty() ) {
        this->echo->clear();
        // make all items visible
        hideAllItems(false);
    } else {
        QFlags<Qt::MatchFlag> flag = (QFlags<Qt::MatchFlag>)this->mode->currentData().toInt();

        QList<QTreeWidgetItem *> items;
        for ( int i = 0; i < this->treeWgt->columnCount(); i++ ) {
            items += this->treeWgt->findItems(pattern, flag, i);
        }
        auto itemSet = QSet<QTreeWidgetItem *>(items.begin(), items.end());
        items = itemSet.values();
        this->echoText(QString("MATCH %1 ITEMS").arg(items.size()));

        // make not matched items invisible
        hideAllItems(true);
        hideItems(items, false);
    }
}

void SearchableTreeWidget::hideAllItems(bool hide)
{
    for ( int i = 0; i < this->treeWgt->topLevelItemCount(); i++ ) {
        auto topItem = this->treeWgt->topLevelItem(i);
        topItem->setHidden(hide);

        hideAllSubItems(topItem, hide);
    }
}

void SearchableTreeWidget::hideAllSubItems(QTreeWidgetItem *item, bool hide)
{
    for ( int j = 0; j < item->childCount(); j++ ) {
        auto childItem = item->child(j);
        childItem->setHidden(hide);

        hideAllSubItems(childItem, hide);
    }
}

void SearchableTreeWidget::hideItems(QList<QTreeWidgetItem *> &items, bool hide)
{
    for ( QTreeWidgetItem *item : items ) {
        item->setHidden(hide);

        for ( QTreeWidgetItem *parent = item->parent(); nullptr != parent; parent = parent->parent() )
            parent->setHidden(hide);
    }
}

void SearchableTreeWidget::itemDoubleClickedHandler(QTreeWidgetItem *item, int column)
{
    column = column;

    // skip visible item Collapsed/Expanded
    for ( int i = 0; i < item->childCount(); i++ ) {
        auto subitem = item->child(i);

        if ( !subitem->isHidden() ) goto exit;
    }

    this->copyCurrentItemText();

exit:

    return;
}


QString SearchableTreeWidget::copyCurrentItemText()
{
    QString result;
    auto item = this->treeWgt->currentItem();
    if ( !item ) return {};

    result = item->text(this->treeWgt->currentColumn());
    result = result.simplified();
    if ( result.isEmpty() ) return {};

    QApplication::clipboard()->setText(result);
    this->echoText(QString("%1").arg(result), true);

    return result;
}

void SearchableTreeWidget::echoText(const QString &text, bool copyecho)
{
    QFontMetrics fontWidth(this->echo->font());
    QString elideNote = fontWidth.elidedText(text, Qt::ElideMiddle, MAX_LABEL_ECHO_LENGTH);

    if ( copyecho ) {
        elideNote = QString("%1 %2").arg(elideNote, "[COPIED]");
    }
    this->echo->setText(elideNote);
}

void SearchableTreeWidget::postload()
{
    if ( !this->lineEdit->text().isEmpty() ) {
        this->expandAll();
        this->searching(this->lineEdit->text());
    }
}


QSize SpanTreeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize oSize = QItemDelegate::sizeHint(option, index);
    oSize.setHeight(rowHeigth);

    return oSize;
}
