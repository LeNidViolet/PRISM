/**
 *  Copyright 2022, raprepo.
 *  Created by raprepo on 2022/8/26.
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
#ifndef PRISM_SS_IMPL_H
#define PRISM_SS_IMPL_H

#include "ui_main.h"
#include "ui_config.h"

class SsImpl : public QObject {

    Q_OBJECT

public:
    static SsImpl *instance();

    void onCommandStop();

public slots:
    // 需要运行在单独线程中
    void onCommandStart(const QByteArray& bytes);

signals:

    // 服务停止了
    void ssStop();

    // 日志输出
    void ssMsgOutput(int level, const QString& msg);

    // 成功绑定地址
    void ssBindDone(const QString& host, unsigned short port);

    // TCP 链接建立
    void ssStreamConnectionMade(
        const QString& domain_local,
        const QString& addr_local,
        unsigned short port_local,
        const QString& domain_remote,
        const QString& addr_remote,
        unsigned short port_remote,
        int stream_index
    );
    // TCP 链接中断
    void ssStreamTeardown(int stream_index);
    // TCP 数据
    void ssPlainStream(
        const QByteArray& data,
        bool send_out,
        int stream_index
    );

    // UDP 链接建立
    void ssDgramConnectionMade(
        const QString& domain_local,
        const QString& addr_local,
        unsigned short port_local,
        const QString& domain_remote,
        const QString& addr_remote,
        unsigned short port_remote,
        int dgram_index
    );
    // UDP 链接中断
    void ssDgramTeardown(int dgram_index);
    // UDP 数据
    void ssPlainDgram(
        const QByteArray& data,
        bool send_out,
        int dgram_index
    );

private:
    SsImpl();
    static SsImpl *minstance;
};

#endif //PRISM_SS_IMPL_H
