/**
 *  Copyright 2022, LeNidViolet.
 *  Created by LeNidViolet on 2022/8/25.
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
#ifndef PRISM_UI_STATUS_H
#define PRISM_UI_STATUS_H

#include <QWidget>
#include <QLabel>
#include "ui_log.h"


class StatusLabel : public QLabel {

    Q_OBJECT

public:
    explicit StatusLabel(const QString &text, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
        : QLabel(text, parent, f) {}
    explicit StatusLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags())
        : StatusLabel("", parent, f) { }

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *ev) override {
        QLabel::mousePressEvent(ev);
        emit this->clicked();
    }
};

class StatusView : public QWidget {

    Q_OBJECT

public:
    explicit StatusView(QWidget *parent = nullptr);
    void addMessage(int level, const QString& msg);

private:
    StatusLabel *echo = nullptr;
    LogView *logView = nullptr;
};



#endif //PRISM_UI_STATUS_H
