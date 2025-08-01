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

#include "searchable_treeview.h"
#include <QApplication>
#include <QClipboard>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QShortcut>
#include "define.h"

SearchableTreeView::SearchableTreeView(QWidget *parent, bool dbclickCopy)
    : QWidget(parent) {

    this->treeView = new MyTreeView(this);
    this->lineEdit = new QLineEdit(this);
    this->echo = new QLabel(this);
    this->mode = new QComboBox(this);

    this->proxyModel = new QSortFilterProxyModel(this);
    this->proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    this->proxyModel->setFilterKeyColumn(-1);
    this->proxyModel->setRecursiveFilteringEnabled(true);

    this->treeView->setModel(this->proxyModel);
    this->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    this->treeView->setAlternatingRowColors(true);
    this->treeView->setUniformRowHeights(true);

    // ReSharper disable once CppDFAMemoryLeak
    auto *delegate = new FixHeightTreeDelegate(TREE_WIDGET_ROW_HEIGHT, this->treeView);
    this->treeView->setItemDelegate(delegate);


    QObject::connect(
        this->treeView,
        &QTreeView::customContextMenuRequested,
        this,
        &SearchableTreeView::customContextMenuRequested
    );
    QObject::connect(
        this->treeView,
        &MyTreeView::currentIndexChanged,
        this,
        &SearchableTreeView::currentIndexChanged
    );
    QObject::connect(
    this->treeView,
        &QTreeView::doubleClicked,
        this,
        &SearchableTreeView::doubleClicked
    );
    if ( dbclickCopy ) {
        QObject::connect(
            this->treeView,
            &QTreeView::doubleClicked,
            this,
            &SearchableTreeView::doubleClickedHandler
        );
    }
    QObject::connect(
        this->lineEdit,
        &QLineEdit::textEdited,
        this,
        &SearchableTreeView::searching);
    QObject::connect(
        this->mode,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        [=](int index) { searching(this->lineEdit->text()); }
    );


    // ReSharper disable once CppDFAMemoryLeak
    const auto label = new QLabel(QStringLiteral("SEARCH:"), this);
    label->setBuddy(this->lineEdit);


    // ReSharper disable once CppDFAMemoryLeak
    const auto shortCut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), this);
    QObject::connect(
        shortCut,
        &QShortcut::activated,
        this,
        [=]() { this->lineEdit->setFocus(); }
    );


    this->mode->addItem(QStringLiteral("CONTAINS"));
    this->mode->setCurrentIndex(0);

    this->echo->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // ReSharper disable once CppDFAMemoryLeak
    const auto hLayout = new QHBoxLayout();
    hLayout->addWidget(label);
    hLayout->addWidget(this->mode);
    hLayout->addWidget(this->lineEdit, 2);
    hLayout->addStretch();
    hLayout->addWidget(this->echo, 1);

    // ReSharper disable once CppDFAMemoryLeak
    const auto layout = new QVBoxLayout();
    layout->addWidget(this->treeView);
    layout->addLayout(hLayout);

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout);
}

void SearchableTreeView::searching(const QString &text) {

    const QString pattern = text.simplified();
    this->proxyModel->setFilterFixedString(pattern);

    if (!pattern.isEmpty()) {
        const auto count = this->currentRowCount();
        this->echoText(QStringLiteral("MATCH %1 ITEMS").arg(count));
    } else {
        this->echo->clear();
    }
    this->checkRowSpan();
}

void SearchableTreeView::checkRowSpan(const QModelIndex &parent) {

    for (int i = 0; i < this->proxyModel->rowCount(parent); i++) {
        auto index = this->proxyModel->index(i, 0, parent);
        auto vect = this->proxyModel->data(index, SPANROLE);
        if (vect.isValid()) {
            const auto span = vect.toBool();
            if (span) {
                this->treeView->setFirstColumnSpanned(i, parent, true);
            }
        }

        this->checkRowSpan(index);
    }
}

int SearchableTreeView::currentRowCount(const QModelIndex &parent) {

    int result = this->proxyModel->rowCount(parent);
    // TODO: MatchExactly
    for (int i = 0; i < this->proxyModel->rowCount(parent); i++) {
        auto index = this->proxyModel->index(i, 0, parent);
        result += this->currentRowCount(index);
    }

    return result;
}

void SearchableTreeView::doubleClickedHandler(const QModelIndex &index) const {
    // skip visible item Collapsed/Expanded
    if ( this->proxyModel->rowCount(index) == 0 )
        this->copyCurrentItemText();
}

void SearchableTreeView::echoText(const QString &text, const bool copyecho) const {

    const QFontMetrics fontWidth(this->echo->font());
    QString elideNote = fontWidth.elidedText(text, Qt::ElideMiddle, MAX_LABEL_ECHO_LENGTH);

    if (copyecho) {
        elideNote = QStringLiteral("%1 %2").arg(elideNote, "[COPIED]");
    }
    this->echo->setText(elideNote);
}


void SearchableTreeView::clear() const {

    this->proxyModel->setFilterFixedString(QLatin1String(""));
    //    this->lineEdit->clear();
    this->proxyModel->removeRows(0, this->proxyModel->rowCount());
    this->echo->clear();
}

void SearchableTreeView::postload() {

    if (!this->lineEdit->text().isEmpty()) {
        this->expandAll();
        this->searching(this->lineEdit->text());
    } else {
        this->checkRowSpan();
    }
}

void SearchableTreeView::copyCurrentItemText() const {
    QString result;
    const auto index = this->treeView->currentIndex();
    if ( !index.isValid() ) return ;

    const auto data = this->proxyModel->data(index);
    if ( !data.isValid() ) return ;

    result = data.toString().simplified();
    if ( result.isEmpty() ) return ;
    QApplication::clipboard()->setText(result);

    this->echoText(QStringLiteral("%1").arg(result), true);
}
