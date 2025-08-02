/**
 *  Copyright 2025, LeNidViolet
 *  Created by LeNidViolet on 2025/07/28.
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

#include <QtConcurrent/QtConcurrentRun>
#include <QString>
#include <QFuture>
extern "C" {
#include "socks5-crypto/socks5-crypto.h"
}
#include "custom/logging.hpp"
#include "if_raw.h"
#include "dump.h"
#include "hosts.h"
#include "flow.h"


static bool Socks5CryptoServerStarted = false;

static FLOW_STATS FlowStats;

static LOG_LEVEL CurrentLogLevel = LOG_KEY;


// ReSharper disable once CppParameterMayBeConst
void on_bind(const char *host, unsigned short port) {

    LOGGING_PUSH(LOG_KEY, "BIND ON %s:%d", host, port);
}

// ReSharper disable once CppParameterMayBeConst
void on_msg(int level, const char *msg) {
    if ( CurrentLogLevel >= level )
        LOGGING_PUSH(static_cast<LOG_LEVEL>(level), "%d %s", level, msg);
}

void on_stream_connection_made(
    const char *domain_local,
    const char *addr_local,
    // ReSharper disable once CppParameterMayBeConst
    unsigned short port_local,
    const char *domain_remote,
    const char *addr_remote,
    // ReSharper disable once CppParameterMayBeConst
    unsigned short port_remote,
    // ReSharper disable once CppParameterMayBeConst
    int stream_index) {

    if ( CurrentLogLevel >= LOG_KEY ) {
        LOGGING_PUSH(
            LOG_KEY,
            "[T] %04d %s:%d -> %s:%d",
            stream_index,
            domain_local ? domain_local : addr_local, port_local,
            domain_remote ? domain_remote : addr_remote, port_remote
            );
    }


    FlowStats.tcpFlows++;
    FlowStats.tcpActiveFlows++;

    PacketDumper::instance().onStreamConnectionMade(
        domain_local,
        addr_local,
        port_local,
        domain_remote,
        addr_remote,
        port_remote,
        stream_index
        );
    FlowDumper::instance().onStreamConnectionMade(
        domain_local,
        addr_local,
        port_local,
        domain_remote,
        addr_remote,
        port_remote,
        stream_index
        );

    if (QString(addr_remote) != QString(domain_remote)) {

        auto address = QHostAddress(QString(addr_remote));
        auto domain = QStringList(domain_remote);

        HostsDumper::instance().addHostsNode(address, domain);
    }
}

// ReSharper disable once CppParameterMayBeConst
void on_stream_teardown(int stream_index) {
    (void)stream_index;

    FlowStats.tcpActiveFlows--;

    PacketDumper::instance().onStreamTeardown(stream_index);
    FlowDumper::instance().onStreamTeardown(stream_index);
}

void on_plain_stream(
    const char *data,
    // ReSharper disable once CppParameterMayBeConst
    size_t data_len,
    // ReSharper disable once CppParameterMayBeConst
    bool send_out,
    // ReSharper disable once CppParameterMayBeConst
    int stream_index) {

    if (send_out) {
        FlowStats.tcpTxBytes += data_len;
    } else {
        FlowStats.tcpRxBytes += data_len;
    }

    PacketDumper::instance().onPlainStream(
        data,
        data_len,
        send_out,
        stream_index
        );
    FlowDumper::instance().onPlainStream(
        data,
        data_len,
        send_out,
        stream_index
        );
}


void on_dgram_connection_made(
    const char *domain_local,
    const char *addr_local,
    // ReSharper disable once CppParameterMayBeConst
    unsigned short port_local,
    const char *domain_remote,
    const char *addr_remote,
    // ReSharper disable once CppParameterMayBeConst
    unsigned short port_remote,
    // ReSharper disable once CppParameterMayBeConst
    int dgram_index)  {

    if ( CurrentLogLevel >= LOG_KEY ) {
        LOGGING_PUSH(
            LOG_KEY,
            "[U] %04d %s:%d -> %s:%d",
            dgram_index,
            domain_local ? domain_local : addr_local, port_local,
            domain_remote ? domain_remote : addr_remote, port_remote
            );
    }


    FlowStats.udpFlows++;
    FlowStats.udpActiveFlows++;

    PacketDumper::instance().onDgramConnectionMade(
        domain_local,
        addr_local,
        port_local,
        domain_remote,
        addr_remote,
        port_remote,
        dgram_index
        );
    FlowDumper::instance().onDgramConnectionMade(
        domain_local,
        addr_local,
        port_local,
        domain_remote,
        addr_remote,
        port_remote,
        dgram_index
        );


    if (QString(addr_remote) != QString(domain_remote)) {

        auto address = QHostAddress(QString(addr_remote));
        auto domain = QStringList(domain_remote);

        HostsDumper::instance().addHostsNode(address, domain);
    }
}

// ReSharper disable once CppParameterMayBeConst
void on_dgram_teardown(int dgram_index) {
    (void)dgram_index;

    FlowStats.udpActiveFlows--;

    PacketDumper::instance().onDgramTeardown(dgram_index);
    FlowDumper::instance().onDgramTeardown(dgram_index);
}


void on_plain_dgram(
    const char *data,
    // ReSharper disable once CppParameterMayBeConst
    size_t data_len,
    // ReSharper disable once CppParameterMayBeConst
    bool send_out,
    // ReSharper disable once CppParameterMayBeConst
    int dgram_index) {


    if (send_out) {
        FlowStats.udpTxBytes += data_len;
    } else {
        FlowStats.udpRxBytes += data_len;
    }

    PacketDumper::instance().onPlainDgram(
        data,
        data_len,
        send_out,
        dgram_index
        );
    FlowDumper::instance().onPlainDgram(
        data,
        data_len,
        send_out,
        dgram_index
        );
}







static void thread_routine(
    const bool asSocks5,
    const unsigned short ListenPort,
    const unsigned int Timeout,
    const QString &Method,
    const QString &Password,
    const QString &CertPath,
    const QString &KeyPath) {

    char certPath[260] = {};
    char keyPath[260] = {};
    char method[64] = {};
    char password[64] = {};

    auto bs = CertPath.toUtf8();
    auto pc = bs.constData();
    strncpy(certPath, pc, sizeof(certPath) - 1);

    bs = KeyPath.toUtf8();
    pc = bs.constData();
    strncpy(keyPath, pc, sizeof(keyPath) - 1);

    bs = Method.toUtf8();
    pc = bs.constData();
    strncpy(method, pc, sizeof(method) - 1);

    bs = Password.toUtf8();
    pc = bs.constData();
    strncpy(password, pc, sizeof(password) - 1);

    socks5_server_config ctx = {};
    ctx.config.asSocks5 = asSocks5;
    ctx.config.bind_port = ListenPort;
    ctx.config.idel_timeout = Timeout;
    ctx.config.root_cert = certPath;
    ctx.config.root_key = keyPath;
    ctx.config.method = method;
    ctx.config.password = password;

    ctx.callbacks.on_bind = on_bind;
    ctx.callbacks.on_msg = on_msg;

    ctx.callbacks.on_stream_connection_made = on_stream_connection_made;
    ctx.callbacks.on_stream_teardown = on_stream_teardown;
    ctx.callbacks.on_plain_stream = on_plain_stream;

    ctx.callbacks.on_dgram_connection_made = on_dgram_connection_made;
    ctx.callbacks.on_dgram_teardown = on_dgram_teardown;
    ctx.callbacks.on_plain_dgram = on_plain_dgram;

    socks5_crypto_launch(&ctx);
}

QFuture<void>& globalSocks5Thread() {
    static QFuture<void> thread;
    return thread;
}

void StartShadowsocksCryptoServer(
    const unsigned short listenPort,
    const unsigned int timeout,
    const QString &method,
    const QString &password,
    const QString &certPath,
    const QString &keyPath) {

    if (Socks5CryptoServerStarted) {
        return ;
    }

    memset(&FlowStats, 0, sizeof(FlowStats));

    globalSocks5Thread() = QtConcurrent::run(
        thread_routine,
        false,
        listenPort,
        timeout,
        method,
        password,
        certPath,
        keyPath
        );

    Socks5CryptoServerStarted = true;
}


void StopShadowsocksCryptoServer() {

    if (!Socks5CryptoServerStarted) {
        return ;
    }

    socks5_crypto_stop();
    globalSocks5Thread().waitForFinished();
    Socks5CryptoServerStarted = false;
}


void StartSocks5CryptoServer(
    const unsigned short listenPort,
    const unsigned int timeout,
    const QString &certPath,
    const QString &keyPath) {

    if (Socks5CryptoServerStarted) {
        return ;
    }

    memset(&FlowStats, 0, sizeof(FlowStats));

    globalSocks5Thread() = QtConcurrent::run(
        thread_routine,
        true,
        listenPort,
        timeout,
        QString(),
        QString(),
        certPath,
        keyPath
        );

    Socks5CryptoServerStarted = true;
}

void StopSocks5CryptoServer() {

    if (!Socks5CryptoServerStarted) {
        return ;
    }

    socks5_crypto_stop();
    globalSocks5Thread().waitForFinished();
    Socks5CryptoServerStarted = false;
}



FLOW_STATS GetFlowStats() {
    return FlowStats;
}

LOG_LEVEL GetLogLevel() {
    return CurrentLogLevel;
}

void SetLogLevel(const LOG_LEVEL level) {
    CurrentLogLevel = level;
}

