/**
 *  Copyright 2025, Song,Tao.
 *  Created by Song,Tao on 2025/07/13.
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
#include "ui_main_frame.h"

#include <QApplication>
#include <QMenuBar>
#include <QFontDialog>
#include <QStatusBar>

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


UIMainFrame::UIMainFrame(QWidget *CentralWidget, QString &Title, int Width, int Height, QWidget *Parent)
: QMainWindow(Parent) {

    // Create Menu
    const auto menuBar = this->menuBar();
    auto menu = menuBar->addMenu(QStringLiteral("Misc"));
    const auto actionFount = menu->addAction(QStringLiteral("Font..."));
    menu = menu->addMenu(QStringLiteral("Theme"));
    const auto actionDark = menu->addAction(QStringLiteral("Dark"));
    const auto actionLight = menu->addAction(QStringLiteral("Light"));

    QObject::connect(actionDark, &QAction::triggered, this, [=](){ UIMainFrame::setDarkTheme(); });
    QObject::connect(actionLight, &QAction::triggered, this, [=](){ UIMainFrame::setLightTheme(); });
    QObject::connect(actionFount, &QAction::triggered, this, [=](){ UIMainFrame::selectFount(); });

    this->setCentralWidget(CentralWidget);

    const auto title = QStringLiteral("%1 (%2 %3)").arg(Title, __DATE__, __TIME__);
    this->setWindowTitle(title);
    this->resize(Width, Height);
    UIMainFrame::setDarkTheme();
}

void UIMainFrame::setDarkTheme() {

    QPalette palette;

    int groupIndex = 0;
    for ( auto &row : darkRgb ) {
        int roleIndex = 0;
        for (const auto &rgb : row ) {

            palette.setColor(colorGroups[groupIndex], colorRoles[roleIndex], QColor::fromRgb(rgb));
            roleIndex++;
        }

        groupIndex++;
    }

    QApplication::setPalette(palette);
}

void UIMainFrame::setLightTheme() {

    QPalette palette;

    int groupIndex = 0;
    for ( auto &row : lightRgb ) {
        int roleIndex = 0;
        for (const auto &rgb : row ) {

            palette.setColor(colorGroups[groupIndex], colorRoles[roleIndex], QColor::fromRgb(rgb));
            roleIndex++;
        }

        groupIndex++;
    }

    QApplication::setPalette(palette);
}

void UIMainFrame::selectFount() {

    const auto current = QApplication::font();
    bool bok;
    const QFont font = QFontDialog::getFont(&bok, current);
    if ( bok ) {
        QApplication::setFont(font);
    }
}
