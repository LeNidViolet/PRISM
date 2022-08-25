/**
 *  Copyright 2022, raprepo.
 *  Created by raprepo on 2022/8/24.
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
#include "ui_main.h"
#include "ui_config.h"
#include "func.h"
#include "custom/searchable_treeview.h"
#include <QApplication>
#include <QFontDialog>
#include <QMenuBar>
#include <QPushButton>
#include <QVBoxLayout>

PrismView::PrismView(QWidget *parent, Qt::WindowFlags f) : QMainWindow(parent, f) {

    // 主菜单
    auto menuBar = this->menuBar();
    auto menu = menuBar->addMenu("Misc");
    auto actionFount = menu->addAction("Font...");
    menu = menu->addMenu("Theme");
    auto actionDark = menu->addAction("Dark");
    auto actionLight = menu->addAction("Light");

    QObject::connect(actionDark, &QAction::triggered, this, [=](){ setDarkTheme(); });
    QObject::connect(actionLight, &QAction::triggered, this, [=](){ setLightTheme(); });
    QObject::connect(actionFount, &QAction::triggered, this, [=](){ selectFount(); });

    // 添加自定义的组件作为状态栏
    this->status = new StatusView(this);
    this->statusBar()->addWidget(this->status);

    auto wgt = new MainWidget(this);
    this->setCentralWidget(wgt);

    auto title = QString("PRISM (%1 %2)").arg(__DATE__, __TIME__);
    this->setWindowTitle(title);
    this->resize(800, 600);
}



MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {


}
