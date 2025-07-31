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
#ifndef PRISM_UI_CONFIG_H
#define PRISM_UI_CONFIG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QRadioButton>


#define SELECT_CRT      1
#define SELECT_KEY      2
#define SELECT_PKT      3
#define SELECT_HOST     4

class ConfigView final : public QDialog {

    Q_OBJECT

public:
    explicit ConfigView(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

signals:
    void configConfirm();

private:
    QLineEdit *crtFileLine;
    QLineEdit *keyFileLine;
    QLineEdit *pktFileLine;
    QLineEdit *hostFileLine;

    QLineEdit *listenAddressLine;
    QSpinBox *listenPortSpin;
    QSpinBox *timeoutSpin;
    QLineEdit *methodLine;
    QLineEdit *passwordLine;

    QRadioButton *runAsShadowsocks;
    QRadioButton *runAsSocks5;

    void onConfirmClicked();
    void onSelectClicked(int reason);
};


#endif //PRISM_UI_CONFIG_H
