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

#include "flow.h"
#include "misc.h"

void FlowDumper::onStreamConnectionMade(const char *domainLocal, const char *addrLocal, unsigned short portLocal,
                                        const char *domainRemote, const char *addrRemote, unsigned short portRemote, int streamIndex) {

    (void)addrLocal;
    (void)addrRemote;

    const auto key = MiscFuncs::genFlowKey(true, streamIndex);
    Q_ASSERT(!this->m_flows.contains(key));

    const auto node = QSharedPointer<FLOW_NODE>::create(
        streamIndex,
        domainLocal,
        portLocal,
        domainRemote,
        portRemote,
        true
        );
    this->m_flows.set(key, node);
}

// ReSharper disable once CppParameterMayBeConst
void FlowDumper::onStreamTeardown(int streamIndex) {

    const auto key = MiscFuncs::genFlowKey(true, streamIndex);
    Q_ASSERT(this->m_flows.contains(key));
    this->m_flows.remove(key);
}

// ReSharper disable once CppParameterMayBeConst
void FlowDumper::onPlainStream(const char *data, size_t dataLen, bool sendOut, int streamIndex) {

    (void)data;

    const auto key = MiscFuncs::genFlowKey(true, streamIndex);
    Q_ASSERT(this->m_flows.contains(key));

    this->m_flows.update(key, [dataLen, sendOut](const QSharedPointer<FLOW_NODE> &node) {
        if (sendOut) {
            node->txBytes += dataLen;
        } else {
            node->rxBytes += dataLen;
        }
    });
}

void FlowDumper::onDgramConnectionMade(const char *domainLocal, const char *addrLocal, unsigned short portLocal,
    const char *domainRemote, const char *addrRemote, unsigned short portRemote, int dgramIndex) {

    (void)addrLocal;
    (void)addrRemote;

    const auto key = MiscFuncs::genFlowKey(false, dgramIndex);
    Q_ASSERT(!this->m_flows.contains(key));

    const auto node = QSharedPointer<FLOW_NODE>::create(
        dgramIndex,
        domainLocal,
        portLocal,
        domainRemote,
        portRemote,
        false
        );
    this->m_flows.set(key, node);
}

// ReSharper disable once CppParameterMayBeConst
void FlowDumper::onDgramTeardown(int dgramIndex) {

    const auto key = MiscFuncs::genFlowKey(false, dgramIndex);
    Q_ASSERT(this->m_flows.contains(key));
    this->m_flows.remove(key);
}

// ReSharper disable once CppParameterMayBeConst
void FlowDumper::onPlainDgram(const char *data, size_t dataLen, bool sendOut, int dgramIndex) {

    (void)data;

    const auto key = MiscFuncs::genFlowKey(false, dgramIndex);
    Q_ASSERT(this->m_flows.contains(key));

    this->m_flows.update(key, [dataLen, sendOut](const QSharedPointer<FLOW_NODE> &node) {
        if (sendOut) {
            node->txBytes += dataLen;
        } else {
            node->rxBytes += dataLen;
        }
    });
}

QVector<QSharedPointer<FLOW_NODE>> FlowDumper::all() const {
    return this->m_flows.values();
}

