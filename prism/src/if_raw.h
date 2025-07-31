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
#ifndef PRISM_IF_RAW_H
#define PRISM_IF_RAW_H


// 统计信息
struct FLOW_STATS {
    unsigned int        tcpFlows;
    unsigned int        tcpActiveFlows;
    unsigned int        tcpRxBytes;
    unsigned int        tcpTxBytes;

    unsigned int        udpFlows;
    unsigned int        udpActiveFlows;
    unsigned int        udpRxBytes;
    unsigned int        udpTxBytes;
};


void StartSocks5CryptoServer(
    unsigned short listenPort,
    unsigned int timeout,
    const QString &certPath,
    const QString &keyPath);
void StopSocks5CryptoServer();

void StartShadowsocksCryptoServer(
    unsigned short listenPort,
    unsigned int timeout,
    const QString &method,
    const QString &password,
    const QString &certPath,
    const QString &keyPath);
void StopShadowsocksCryptoServer();

FLOW_STATS GetFlowStats();

LOG_LEVEL GetLogLevel();
void SetLogLevel(LOG_LEVEL level);

#endif //PRISM_IF_RAW_H
