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
#include "ss_impl.h"
#include <QCoreApplication>
#include <QMutex>
#include "misc.h"
extern "C" {
#include "shadowsocks-crypto/shadowsocks-crypto.h"
}




void bridge_on_msg(int level, const char *msg) {

    auto ins = SsImpl::instance();
    emit ins->ssMsgOutput(level, QString(msg));
}

void bridge_on_bind(const char *host, unsigned short port) {

    auto ins = SsImpl::instance();
    emit ins->ssBindDone(QString(host), port);
}

void bridge_on_stream_connection_made(
    const char *domain_local,
    const char *addr_local,
    unsigned short port_local,
    const char *domain_remote,
    const char *addr_remote,
    unsigned short port_remote,
    int stream_index
) {

    auto ins = SsImpl::instance();
    emit ins->ssStreamConnectionMade(
        QString(domain_local),
        QString(addr_local),
        port_local,
        QString(domain_remote),
        QString(addr_remote),
        port_remote,
        stream_index
    );
}

void bridge_on_stream_teardown(int stream_index) {

    auto ins = SsImpl::instance();
    emit ins->ssStreamTeardown(stream_index);
}

void bridge_on_plain_stream(
    const char *data,
    size_t data_len,
    bool send_out,
    int stream_index
) {

    auto ins = SsImpl::instance();
    emit ins->ssPlainStream(
        QByteArray(data, (int)data_len),
        send_out,
        stream_index
    );
}

void bridge_on_dgram_connection_made(
    const char *domain_local,
    const char *addr_local,
    unsigned short port_local,
    const char *domain_remote,
    const char *addr_remote,
    unsigned short port_remote,
    int dgram_index
) {

    auto ins = SsImpl::instance();
    emit ins->ssDgramConnectionMade(
        QString(domain_local),
        QString(addr_local),
        port_local,
        QString(domain_remote),
        QString(addr_remote),
        port_remote,
        dgram_index
    );
}

void bridge_on_dgram_teardown(int dgram_index) {

    auto ins = SsImpl::instance();
    emit ins->ssDgramTeardown(dgram_index);
}

void bridge_on_plain_dgram(
    const char *data,
    size_t data_len,
    bool send_out,
    int dgram_index
) {

    auto ins = SsImpl::instance();
    emit ins->ssPlainDgram(
        QByteArray(data, (int)data_len),
        send_out,
        dgram_index
    );
}










SsImpl *SsImpl::minstance = nullptr;

SsImpl::SsImpl() : QObject(nullptr) {

}

SsImpl *SsImpl::instance() {
    static QMutex mutex;
    if ( !minstance ) {
        QMutexLocker locker(&mutex);
        if ( !minstance )
            minstance = new SsImpl;
    }

    return minstance;
}

void SsImpl::onCommandStart(const QByteArray& bytes) {

    this->instance();
    auto config = MiscFuncs::configFromJson(bytes);

    sscrypto_ctx ctx = {};
    auto bsserver = config->server.toString().toLocal8Bit();
    ctx.config.bind_host = bsserver.data();
    ctx.config.bind_port = config->port;
    ctx.config.idel_timeout = config->timeout;

    auto bspasswd = config->password.toLocal8Bit();
    ctx.config.password = bspasswd.data();

    auto bsmethod = config->method.toLocal8Bit();
    ctx.config.method = bsmethod.data();

    auto bscrt = config->crtFile.toLocal8Bit();
    ctx.config.root_cert = bscrt.data();

    auto bskey = config->keyFile.toLocal8Bit();
    ctx.config.root_key = bskey.data();

    ctx.config.create_dns_srv = config->dnssrv;


    ctx.callbacks.on_bind = bridge_on_bind;
    ctx.callbacks.on_msg = bridge_on_msg;
    ctx.callbacks.on_stream_connection_made = bridge_on_stream_connection_made;
    ctx.callbacks.on_stream_teardown = bridge_on_stream_teardown;
    ctx.callbacks.on_dgram_connection_made = bridge_on_dgram_connection_made;
    ctx.callbacks.on_dgram_teardown = bridge_on_dgram_teardown;
    ctx.callbacks.on_plain_stream = bridge_on_plain_stream;
    ctx.callbacks.on_plain_dgram = bridge_on_plain_dgram;

    sscrypto_launch(&ctx);

    delete config;

    auto ins = SsImpl::instance();
    emit ins->ssStop();
}

void SsImpl::onCommandStop() {

    this->instance();
    sscrypto_stop();
}


