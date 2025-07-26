/**
 *  Copyright 2022, LeNidViolet.
 *  Created by LeNidViolet on 2022/8/24.
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

#include <QApplication>
#include <QMenuBar>
#include <QFontDialog>
#include <QStatusBar>

#include "ui_mainwgt.h"
#include "ss_impl.h"




static QPalette::ColorGroup colorGroups[] = {
    QPalette::Disabled,
    QPalette::Active,
    QPalette::Inactive,
};
static QPalette::ColorRole colorRoles[] = {
    QPalette::WindowText,
    QPalette::Button,
    QPalette::Light,
    QPalette::Midlight,
    QPalette::Dark,
    QPalette::Mid,
    QPalette::Text,
    QPalette::BrightText,
    QPalette::ButtonText,
    QPalette::Base,
    QPalette::Window,
    QPalette::Shadow,
    QPalette::Highlight,
    QPalette::HighlightedText,
    QPalette::Link,
    QPalette::AlternateBase,
    QPalette::ToolTipBase,
    QPalette::ToolTipText,
    QPalette::PlaceholderText,
};
static unsigned int darkRgb[std::size(colorGroups)][std::size(colorRoles)] = {
    {0xFF646464, 0xFF323232, 0xFF373737, 0xFF343434, 0xFFBFBFBF, 0xFF232323, 0xFFFFFFFF, 0xFF373737, 0xFFE2E2E2, 0xFF323232, 0xFF323232, 0xFF000000, 0xFF464646, 0xFFFFFFFF, 0xFF0000FF, 0xFF232323, 0xFFFFFFFF, 0xFF000000, 0xFF8F8F8F, },
    {0xFFE2E2E2, 0xFF323232, 0xFF373737, 0xFF343434, 0xFFBFBFBF, 0xFF232323, 0xFFFFFFFF, 0xFF373737, 0xFFE2E2E2, 0xFF1E1E1E, 0xFF323232, 0xFF000000, 0xFF0F64D6, 0xFFFFFFFF, 0xFF419CFF, 0xFF232323, 0xFFFFFFFF, 0xFF000000, 0xFF8F8F8F, },
    {0xFFE2E2E2, 0xFF323232, 0xFF373737, 0xFF343434, 0xFFBFBFBF, 0xFF232323, 0xFFFFFFFF, 0xFF373737, 0xFFE2E2E2, 0xFF1E1E1E, 0xFF323232, 0xFF000000, 0xFF464646, 0xFFFFFFFF, 0xFF0000FF, 0xFF232323, 0xFFFFFFFF, 0xFF000000, 0xFF8F8F8F, },
};
static unsigned int lightRgb[std::size(colorGroups)][std::size(colorRoles)] = {
    {0xFF787878, 0xFFF0F0F0, 0xFFFFFFFF, 0xFFF7F7F7, 0xFFA0A0A0, 0xFFA0A0A0, 0xFF787878, 0xFFFFFFFF, 0xFF787878, 0xFFF0F0F0, 0xFFF0F0F0, 0xFF000000, 0xFF0078D7, 0xFFFFFFFF, 0xFF0000FF, 0xFFE9E7E3, 0xFFFFFFDC, 0xFF000000, 0xFF7F7F7F, },
    {0xFF000000, 0xFFF0F0F0, 0xFFFFFFFF, 0xFFE3E3E3, 0xFFA0A0A0, 0xFFA0A0A0, 0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFFF0F0F0, 0xFF696969, 0xFF0078D7, 0xFFFFFFFF, 0xFF0000FF, 0xFFE9E7E3, 0xFFFFFFDC, 0xFF000000, 0xFF7F7F7F, },
    {0xFF000000, 0xFFF0F0F0, 0xFFFFFFFF, 0xFFE3E3E3, 0xFFA0A0A0, 0xFFA0A0A0, 0xFF000000, 0xFFFFFFFF, 0xFF000000, 0xFFFFFFFF, 0xFFF0F0F0, 0xFF696969, 0xFFF0F0F0, 0xFF000000, 0xFF0000FF, 0xFFE9E7E3, 0xFFFFFFDC, 0xFF000000, 0xFF7F7F7F, },
};




PrismView::PrismView(QWidget *parent) : QMainWindow(parent) {

    // 主菜单
    auto menuBar = this->menuBar();
    auto menu = menuBar->addMenu(QStringLiteral("Misc"));
    auto actionFount = menu->addAction(QStringLiteral("Font..."));
    menu = menu->addMenu(QStringLiteral("Theme"));
    auto actionDark = menu->addAction(QStringLiteral("Dark"));
    auto actionLight = menu->addAction(QStringLiteral("Light"));

    QObject::connect(actionDark, &QAction::triggered, this, [=](){ PrismView::setDarkTheme(); });
    QObject::connect(actionLight, &QAction::triggered, this, [=](){ PrismView::setLightTheme(); });
    QObject::connect(actionFount, &QAction::triggered, this, [=](){ PrismView::selectFount(); });

    // 创建ss服务实例
    auto ss = SsImpl::instance();
    QObject::connect(ss, &SsImpl::ssMsgOutput, this, &PrismView::onSsMsgOutput);
    QObject::connect(ss, &SsImpl::ssBindDone, this, &PrismView::onSsBindDone);


    // 添加自定义的组件作为状态栏
    this->status = new StatusView(this);
    this->statusBar()->addWidget(this->status);

    auto wgt = new MainWidget(this);
    this->setCentralWidget(wgt);

    auto title = QString("PRISM (%1 %2)").arg(__DATE__, __TIME__);
    this->setWindowTitle(title);
    this->resize(800, 600);
    this->setDarkTheme();
}

void PrismView::onSsMsgOutput(int level, const QString &msg) {

    // 将输出信息打印在自定义的状态栏中
    this->status->addMessage(level, msg);
}

void PrismView::onSsBindDone(const QString& host, unsigned short port) {

    auto s = QString("Listening on %1:%2").arg(host, QString::number(port));
    this->status->addMessage(4, s);
}


// 设置暗色主题
void PrismView::setDarkTheme() {
    QPalette palette;

    int groupIndex = 0;
    for ( auto &row : darkRgb ) {
        int roleIndex = 0;
        for ( auto &rgb : row ) {

            palette.setColor(colorGroups[groupIndex], colorRoles[roleIndex], QColor::fromRgb(rgb));
            roleIndex++;
        }

        groupIndex++;
    }

    QApplication::setPalette(palette);
}

// 设置亮色主题
void PrismView::setLightTheme() {
    QPalette palette;

    int groupIndex = 0;
    for ( auto &row : lightRgb ) {
        int roleIndex = 0;
        for ( auto &rgb : row ) {

            palette.setColor(colorGroups[groupIndex], colorRoles[roleIndex], QColor::fromRgb(rgb));
            roleIndex++;
        }

        groupIndex++;
    }

    QApplication::setPalette(palette);
}

// 选择字体
void PrismView::selectFount() {
    auto current = QApplication::font();
    bool bok;
    QFont font = QFontDialog::getFont(&bok, current);
    if ( bok ) {
        QApplication::setFont(font);
    }
}
