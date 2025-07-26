/**
 *  Copyright 2020, LeNidViolet.
 *  Created by LeNidViolet on 2020/8/5.
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

#include "file_select_dlg.h"
#include "searchable_treeWidget.h"
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QCheckBox>
#include <QHBoxLayout>


FileSelectDlg::FileSelectDlg(const QStringList &fileList, QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f)
{
    this->treeWidget = new SearchableTreeWidget(QStringList("NAME"), this);

    for ( const auto &fileName : fileList ) {
        auto item = new QTreeWidgetItem(QStringList(fileName));
        item->setCheckState(0, Qt::Unchecked);
        this->treeWidget->addTopLevelItem(item);
    }

    auto btnOk = new QPushButton("OK", this);
    btnOk->setAutoDefault(false); btnOk->setDefault(false);
    auto btnClose = new QPushButton("CLOSE", this);
    btnClose->setAutoDefault(false); btnClose->setDefault(false);
    auto chkSelectAll = new QCheckBox("SELECT ALL", this);
    chkSelectAll->setChecked(false);

    QObject::connect(btnOk, &QPushButton::clicked, this, &FileSelectDlg::okClicked);
    QObject::connect(btnClose, &QPushButton::clicked, this, &FileSelectDlg::close);
    QObject::connect(chkSelectAll, &QCheckBox::stateChanged, this, &FileSelectDlg::selectAllStateChangedHandler);

    auto hlayout = new QHBoxLayout();
    hlayout->addWidget(chkSelectAll);
    hlayout->addStretch();
    hlayout->addWidget(btnOk);
    hlayout->addWidget(btnClose);

    auto layout = new QVBoxLayout();
    layout->addWidget(this->treeWidget);
    layout->addLayout(hlayout);
    layout->setMargin(0);
    layout->setSpacing(0);
    this->setLayout(layout);
    this->setWindowTitle("SELECT FILE");
    this->resize(800, 600);
}


void FileSelectDlg::okClicked()
{
    this->close();
    QStringList list;
    for ( int i = 0; i < this->treeWidget->topLevelItemCount(); i++ ) {
        auto item = this->treeWidget->topLevelItem(i);
        if ( item->checkState(0) == Qt::Checked )
            list << item->text(0);
    }
    emit this->fileSelected(list);
}

void FileSelectDlg::selectAllStateChangedHandler(int state)
{
    for ( int i = 0; i < this->treeWidget->topLevelItemCount(); i++ ) {
        auto item = this->treeWidget->topLevelItem(i);
        item->setCheckState(0, (Qt::CheckState)state);
    }
}
