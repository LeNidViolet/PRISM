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
#ifndef FILE_SELECT_DLG_H
#define FILE_SELECT_DLG_H

#include <QDialog>

class SearchableTreeWidget;

class FileSelectDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FileSelectDlg(const QStringList &fileList, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

signals:
    void fileSelected(const QStringList &fileList);

private:
    void okClicked();
    void selectAllStateChangedHandler(int state);
    SearchableTreeWidget *treeWidget = nullptr;
};

#endif
