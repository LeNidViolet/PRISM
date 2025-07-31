/**
 *  Copyright 2025, LeNidViolet
 *  Created by LeNidViolet on 2025/07/30.
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
#ifndef PRISM_UI_FLOW_H
#define PRISM_UI_FLOW_H

#include <QHostAddress>
#include <QString>
#include <QTime>
#include "custom/safe_map.hpp"

typedef struct FLOW_NODE_ {
    explicit FLOW_NODE_(
        const int index,
        QString localAddr,
        const unsigned short localPort,
        QString remoteAddr,
        const unsigned short remotePort,
        const bool isStream)
        : isStream(isStream),
          index(index), localAddr(std::move(localAddr)),
          localPort(localPort), remoteAddr(std::move(remoteAddr)),
          remotePort(remotePort)
    {
        this->createTime = QTime::currentTime();
        this->rxBytes = 0;
        this->txBytes = 0;

        const auto haLocal = QHostAddress(localAddr);
        const auto haRemote = QHostAddress(remoteAddr);
        if (haLocal.protocol() != haRemote.protocol()) {
            if (haLocal.protocol() == QAbstractSocket::IPv6Protocol) {
                const QString ipv6Mapped = "::ffff:" + remoteAddr;
                this->remoteAddr = QHostAddress(ipv6Mapped).toString();
            } else {
                const QString ipv6Mapped = "::ffff:" + localAddr;
                this->localAddr = QHostAddress(ipv6Mapped).toString();
            }
        }

        this->isIpv6 = QHostAddress(localAddr).protocol() == QAbstractSocket::IPv6Protocol;
    };

    QTime createTime;
    bool isStream;
    int index;

    QString localAddr;
    unsigned short localPort;
    QString remoteAddr;
    unsigned short remotePort;

    bool isIpv6;

    unsigned int rxBytes;
    unsigned int txBytes;
} FLOW_NODE;



class FlowDumper {

public:
    FlowDumper(const FlowDumper&) = delete;
    FlowDumper& operator=(const FlowDumper&) = delete;

    // Get the singleton instance
    static FlowDumper& instance() {
        // Guaranteed thread-safe in C++11 and later
        static auto *instance = new FlowDumper;
        return *instance;
    }

    void onStreamConnectionMade(
    const char *domainLocal,
    const char *addrLocal,
    unsigned short portLocal,
    const char *domainRemote,
    const char *addrRemote,
    unsigned short portRemote,
    int streamIndex
    );
    void onStreamTeardown(int streamIndex);
    void onPlainStream(const char *data, size_t dataLen, bool sendOut, int streamIndex);

    void onDgramConnectionMade(
        const char *domainLocal,
        const char *addrLocal,
        unsigned short portLocal,
        const char *domainRemote,
        const char *addrRemote,
        unsigned short portRemote,
        int dgramIndex
        );
    void onDgramTeardown(int dgramIndex);
    void onPlainDgram(const char *data, size_t dataLen, bool sendOut, int dgramIndex);

    QVector<QSharedPointer<FLOW_NODE>> all() const;

private:
    FlowDumper() = default;
    ~FlowDumper() = default;

    ThreadSafeMap<QString, QSharedPointer<FLOW_NODE>> m_flows{};
};




#endif //PRISM_UI_FLOW_H
