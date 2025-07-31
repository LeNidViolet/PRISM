/**
 *  Copyright 2022, LeNidViolet.
 *  Created by LeNidViolet on 2022/9/1.
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

#ifndef PRISM_UI_MAINWGT_H
#define PRISM_UI_MAINWGT_H

#include <QDialog>
#include <QWidget>
#include <QPushButton>
#include <utility>
#include "ui_config.h"
#include "ui_hosts.h"
#include "ui_statistics.h"
#include "ui_log.h"
#include "ui_flow.h"
#include "custom/http_server.h"

class MainWidget final : public QWidget {

    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);

private:
    bool m_capturing = false;

    QPushButton *m_btnCapture = nullptr;
    QLabel *m_caaddr = nullptr;

    StatisticsView *m_statsView = nullptr;
    HostsView *m_hostsView = nullptr;
    LogView *m_logView = nullptr;
    FlowView *m_flowView = nullptr;
    ConfigView *m_configView = nullptr;

    HttpServer *m_httpServer = nullptr;

    void onStartClicked();

    void captureStart();
    void captureStop();

    void updateStatus() const;
    void onConfigConfirm();
};



#endif //PRISM_UI_MAINWGT_H
