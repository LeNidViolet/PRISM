/**
 *  Copyright 2022, raprepo.
 *  Created by raprepo on 2022/8/25.
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
#include "ui_status.h"
#include <QApplication>
#include <QPalette>
#include <QColor>
#include <QHBoxLayout>
#include "macros.h"

StatusView::StatusView(QWidget *parent) : QWidget(parent) {

    this->echo = new StatusLabel(this);
    this->logView = new LogView(this);

    QObject::connect(
        this->echo,
        &StatusLabel::clicked,
        this,
        [=]() { this->logView->show(); }
    );

    auto layout = new QHBoxLayout();
    layout->addWidget(this->echo);
    layout->setMargin(0);
    layout->setSpacing(0);

    this->setLayout(layout);
}

void StatusView::addMessage(int level, QString &msg) {

    auto palette = this->echo->palette();

    auto color = QApplication::palette().color(QPalette::WindowText);

    switch ( level ) {
    case 1: color = COLOR_ERROR; break;
    case 2: color = COLOR_WARN; break;
    default : break;
    }

    palette.setColor(QPalette::WindowText, color);
    this->echo->setPalette(palette);
    this->echo->setText(msg);

    this->logView->addLog(level, msg);
}


