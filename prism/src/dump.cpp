/**
 *  Copyright 2025, LeNidViolet
 *  Created by LeNidViolet on 2025/07/29.
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

#include "dump.h"
#include <QDateTime>
#include <QFile>
#include "misc.h"

static QByteArray buildTcpHandshakePkt(const QSharedPointer<FLOW_TRACK> &flow);
static QByteArray buildTcpFinPkt(const QSharedPointer<FLOW_TRACK> &flow, bool sendOut);
static QByteArray buildTcpPayloadPkt(const QSharedPointer<FLOW_TRACK> &flow, const char *data, size_t dataLen, bool sendOut);
static QByteArray buildUdpPayloadPkt(const QSharedPointer<FLOW_TRACK> &flow, const char *data, size_t dataLen, bool sendOut);
static bool writePktsOut(const QString &filePath, const QByteArray &fileContent);

static void createEthernetHeader(ETHERNET_HEADER *ethernet, bool sendOut, bool isIpv6);
static void createIpHeader(IP_HEADER *ipHdr, const QSharedPointer<FLOW_TRACK> &flow, unsigned int payloadLen, bool sendOut);
static void createTcpHeader(TCP_HEADER *tcpHdr, const QSharedPointer<FLOW_TRACK> &flow, unsigned char flags, bool sendOut);
static void createUdpHeader(UDP_HEADER *udpHdr, const QSharedPointer<FLOW_TRACK> &flow, unsigned int payloadLen, bool sendOut);
static void createSynAckPkt(const QSharedPointer<FLOW_TRACK> &flow, TCP_PKT *tcpPkt);
static void createSynPkt(const QSharedPointer<FLOW_TRACK> &flow, TCP_PKT *tcpPkt);
static void createSynDonePkt(const QSharedPointer<FLOW_TRACK> &flow, TCP_PKT *tcpPkt);
static void createTcpFinPkt(const QSharedPointer<FLOW_TRACK> &flow, TCP_PKT *tcpPkt, bool sendOut);
static void createTcpAckPkt(const QSharedPointer<FLOW_TRACK> &flow, TCP_PKT *tcpPkt, bool sendOut);
static void createTcpDataPkt(const QSharedPointer<FLOW_TRACK> &flow, TCP_PKT *tcpPkt, unsigned int payloadLen, bool sendOut);
static void createUdpDataPkt(const QSharedPointer<FLOW_TRACK> &flow, UDP_PKT *udpPkt, unsigned int payloadLen, bool sendOut);





void PacketDumper::onStreamConnectionMade(const char *domainLocal, const char *addrLocal, unsigned short portLocal,
    const char *domainRemote, const char *addrRemote, unsigned short portRemote, const int streamIndex) {

    (void)domainLocal;
    (void)domainRemote;

    const auto key = MiscFuncs::genFlowKey(true, streamIndex);
    Q_ASSERT(!this->m_flows.contains(key));

    const auto flow = QSharedPointer<FLOW_TRACK>::create(
        QHostAddress(addrLocal),
        QHostAddress(addrRemote),
        portLocal,
        portRemote,
        PROTOCOL_TCP,
        0,
        0
        );
    this->m_flows.set(key, flow);

    const auto bytes = buildTcpHandshakePkt(flow);
    this->m_cachingBytes.append(bytes);
    this->savePkts(false);
}

void PacketDumper::onStreamTeardown(const int streamIndex) {

    const auto key = MiscFuncs::genFlowKey(true, streamIndex);
    Q_ASSERT(this->m_flows.contains(key));

    const auto flow = this->m_flows.get(key);

    const auto bytes = buildTcpFinPkt(flow.value(), true);
    this->m_cachingBytes.append(bytes);
    this->savePkts(false);

    this->m_flows.remove(key);
}

void PacketDumper::onPlainStream(const char *data, const size_t dataLen, const bool sendOut, const int streamIndex) {

    const auto key = MiscFuncs::genFlowKey(true, streamIndex);
    Q_ASSERT(this->m_flows.contains(key));

    const auto flow = this->m_flows.get(key);

    const auto bytes = buildTcpPayloadPkt(flow.value(), data, dataLen, sendOut);
    this->m_cachingBytes.append(bytes);
    this->savePkts(false);
}

void PacketDumper::onDgramConnectionMade(const char *domainLocal, const char *addrLocal, unsigned short portLocal,
    const char *domainRemote, const char *addrRemote, unsigned short portRemote, const int dgramIndex) {

    (void)domainLocal;
    (void)domainRemote;

    const auto key = MiscFuncs::genFlowKey(false, dgramIndex);
    Q_ASSERT(!this->m_flows.contains(key));

    const auto flow = QSharedPointer<FLOW_TRACK>::create(
        QHostAddress(addrLocal),
        QHostAddress(addrRemote),
        portLocal,
        portRemote,
        PROTOCOL_UDP,
        0,
        0
        );
    this->m_flows.set(key, flow);
}

void PacketDumper::onDgramTeardown(const int dgramIndex) {

    const auto key = MiscFuncs::genFlowKey(false, dgramIndex);
    Q_ASSERT(this->m_flows.contains(key));

    this->m_flows.remove(key);
}

void PacketDumper::onPlainDgram(const char *data, const size_t dataLen, const bool sendOut, const int dgramIndex) {

    const auto key = MiscFuncs::genFlowKey(false, dgramIndex);
    Q_ASSERT(this->m_flows.contains(key));

    const auto flow = this->m_flows.get(key);

    const auto bytes = buildUdpPayloadPkt(flow.value(), data, dataLen, sendOut);
    this->m_cachingBytes.append(bytes);
    this->savePkts(false);
}


void PacketDumper::savePkts(const bool flush) {

    if ( this->m_cachingBytes.isEmpty() ) {
        this->m_cachingBytesLen = 0;
        return ;
    }

    if ( !this->m_pcapFilePath.isEmpty() ) {
        if ( this->m_cachingBytes.size() >= CACHING_BUFFER_MAX_BYTES || flush || this->timerExpired() ) {

            const bool bok = writePktsOut(this->m_pcapFilePath, this->m_cachingBytes);
            if (bok)
                this->m_cachingBytes.clear();
        }
    }

    this->m_cachingBytesLen = this->m_cachingBytes.size();
}

bool PacketDumper::timerExpired() {
    bool result = false;
    if (this->m_lastRefreshTimer.hasExpired(FILE_FLUSH_INTERVAL_MS)) {
        this->m_lastRefreshTimer.restart();
        result = true;
    }
    return result;
}


static QByteArray buildTcpHandshakePkt(const QSharedPointer<FLOW_TRACK> &flow) {

    PCAPREC_HDR     capHdr = {};
    TCP_PKT         tcpPkt = {};

    const bool isIpv6 = flow->srcIp.protocol() == QAbstractSocket::IPv6Protocol;

    const QDateTime now = QDateTime::currentDateTimeUtc();
    const qint64 msSinceEpoch = now.toMSecsSinceEpoch();

    capHdr.ts_sec      = msSinceEpoch / 1000;
    capHdr.ts_usec     = (msSinceEpoch % 1000) * 1000;
    capHdr.incl_len    = isIpv6 ? sizeof(TCP_PKT_V6) : sizeof(TCP_PKT_V4);
    capHdr.orig_len    = capHdr.incl_len;

    QByteArray result;

    createSynPkt(flow, &tcpPkt);
    result.append(reinterpret_cast<const char *>(&capHdr), sizeof(PCAPREC_HDR));
    result.append(reinterpret_cast<const char *>(&tcpPkt), isIpv6 ? sizeof(TCP_PKT_V6) : sizeof(TCP_PKT_V4));

    createSynAckPkt(flow, &tcpPkt);
    capHdr.ts_usec += 5;
    result.append(reinterpret_cast<const char *>(&capHdr), sizeof(PCAPREC_HDR));
    result.append(reinterpret_cast<const char *>(&tcpPkt), isIpv6 ? sizeof(TCP_PKT_V6) : sizeof(TCP_PKT_V4));

    createSynDonePkt(flow, &tcpPkt);
    capHdr.ts_usec += 5;
    result.append(reinterpret_cast<const char *>(&capHdr), sizeof(PCAPREC_HDR));
    result.append(reinterpret_cast<const char *>(&tcpPkt), isIpv6 ? sizeof(TCP_PKT_V6) : sizeof(TCP_PKT_V4));

    return result;
}


// ReSharper disable once CppDFAConstantParameter
static QByteArray buildTcpFinPkt(const QSharedPointer<FLOW_TRACK> &flow, const bool sendOut) {

    PCAPREC_HDR     capHdr = {};
    TCP_PKT         tcpPkt = {};

    const bool isIpv6 = flow->srcIp.protocol() == QAbstractSocket::IPv6Protocol;

    const QDateTime now = QDateTime::currentDateTimeUtc();
    const qint64 msSinceEpoch = now.toMSecsSinceEpoch();

    capHdr.ts_sec      = msSinceEpoch / 1000;
    capHdr.ts_usec     = (msSinceEpoch % 1000) * 1000;
    capHdr.incl_len    = isIpv6 ? sizeof(TCP_PKT_V6) : sizeof(TCP_PKT_V4);
    capHdr.orig_len    = capHdr.incl_len;

    QByteArray result;

    // 发起方发送FIN
    createTcpFinPkt(flow, &tcpPkt, sendOut);
    result.append(reinterpret_cast<const char *>(&capHdr), sizeof(PCAPREC_HDR));
    result.append(reinterpret_cast<const char *>(&tcpPkt), isIpv6 ? sizeof(TCP_PKT_V6) : sizeof(TCP_PKT_V4));

    // 接收方收到FIN之后ACK
    // ReSharper disable once CppDFAConstantConditions
    createTcpAckPkt(flow, &tcpPkt, !sendOut);
    capHdr.ts_usec += 5;
    result.append(reinterpret_cast<const char *>(&capHdr), sizeof(PCAPREC_HDR));
    result.append(reinterpret_cast<const char *>(&tcpPkt), isIpv6 ? sizeof(TCP_PKT_V6) : sizeof(TCP_PKT_V4));

    // 接收方收到FIN之后FIN
    // ReSharper disable once CppDFAConstantConditions
    createTcpFinPkt(flow, &tcpPkt, !sendOut);
    capHdr.ts_usec += 5;
    result.append(reinterpret_cast<const char *>(&capHdr), sizeof(PCAPREC_HDR));
    result.append(reinterpret_cast<const char *>(&tcpPkt), isIpv6 ? sizeof(TCP_PKT_V6) : sizeof(TCP_PKT_V4));

    // 发起方发送ACK
    createTcpAckPkt(flow, &tcpPkt, sendOut);
    capHdr.ts_usec += 5;
    result.append(reinterpret_cast<const char *>(&capHdr), sizeof(PCAPREC_HDR));
    result.append(reinterpret_cast<const char *>(&tcpPkt), isIpv6 ? sizeof(TCP_PKT_V6) : sizeof(TCP_PKT_V4));

    return result;
}


static QByteArray buildTcpPayloadPkt(const QSharedPointer<FLOW_TRACK> &flow, const char *data, const size_t dataLen, const bool sendOut) {

    PCAPREC_HDR     capHdr = {};
    TCP_PKT         tcpPkt = {};

    const bool isIpv6 = flow->srcIp.protocol() == QAbstractSocket::IPv6Protocol;

    const QDateTime now = QDateTime::currentDateTimeUtc();
    const qint64 msSinceEpoch = now.toMSecsSinceEpoch();

    capHdr.ts_sec      = msSinceEpoch / 1000;
    capHdr.ts_usec     = (msSinceEpoch % 1000) * 1000;
    capHdr.incl_len    = dataLen + (isIpv6 ? sizeof(TCP_PKT_V6) : sizeof(TCP_PKT_V4));
    capHdr.orig_len    = capHdr.incl_len;

    createTcpDataPkt(flow, &tcpPkt, dataLen, sendOut);

    QByteArray result;

    result.append(reinterpret_cast<const char *>(&capHdr), sizeof(PCAPREC_HDR));
    result.append(reinterpret_cast<const char *>(&tcpPkt), isIpv6 ? sizeof(TCP_PKT_V6) : sizeof(TCP_PKT_V4));
    result.append(data, static_cast<int>(dataLen));

    // 对方发送ACK
    createTcpAckPkt(flow, &tcpPkt, !sendOut);
    capHdr.incl_len    = isIpv6 ? sizeof(TCP_PKT_V6) : sizeof(TCP_PKT_V4);
    capHdr.orig_len    = capHdr.incl_len;
    capHdr.ts_usec     += 5;
    result.append(reinterpret_cast<const char *>(&capHdr), sizeof(PCAPREC_HDR));
    result.append(reinterpret_cast<const char *>(&tcpPkt), isIpv6 ? sizeof(TCP_PKT_V6) : sizeof(TCP_PKT_V4));

    return result;
}

static QByteArray buildUdpPayloadPkt(const QSharedPointer<FLOW_TRACK> &flow, const char *data, const size_t dataLen, const bool sendOut) {

    PCAPREC_HDR     capHdr = {};
    UDP_PKT         udpPkt = {};

    const bool isIpv6 = flow->srcIp.protocol() == QAbstractSocket::IPv6Protocol;

    const QDateTime now = QDateTime::currentDateTimeUtc();
    const qint64 msSinceEpoch = now.toMSecsSinceEpoch();

    capHdr.ts_sec      = msSinceEpoch / 1000;
    capHdr.ts_usec     = (msSinceEpoch % 1000) * 1000;
    capHdr.incl_len    = dataLen + (isIpv6 ? sizeof(UDP_PKT_V6) : sizeof(UDP_PKT_V4));
    capHdr.orig_len    = capHdr.incl_len;

    createUdpDataPkt(flow, &udpPkt, dataLen, sendOut);

    QByteArray result;

    result.append(reinterpret_cast<const char *>(&capHdr), sizeof(PCAPREC_HDR));
    result.append(reinterpret_cast<const char *>(&udpPkt), isIpv6 ? sizeof(UDP_PKT_V6) : sizeof(UDP_PKT_V4));
    result.append(data, static_cast<int>(dataLen));

    return result;
}



static void createEthernetHeader(ETHERNET_HEADER *ethernet, const bool sendOut, const bool isIpv6) {

    static unsigned char fake_src_mac[MAC_ADDRESS_LEN] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60};
    static unsigned char fake_dst_mac[MAC_ADDRESS_LEN] = {0xf0, 0xe0, 0xd0, 0xc0, 0xb0, 0xa0};

    if ( sendOut ) {
        memcpy(ethernet->src_mac, fake_src_mac, MAC_ADDRESS_LEN);
        memcpy(ethernet->dst_mac, fake_dst_mac, MAC_ADDRESS_LEN);
    } else {
        memcpy(ethernet->src_mac, fake_dst_mac, MAC_ADDRESS_LEN);
        memcpy(ethernet->dst_mac, fake_src_mac, MAC_ADDRESS_LEN);
    }

    if (isIpv6) {
        ethernet->type = htons_u(ETHERNET_FRAME_IPv6);
    } else {
        ethernet->type = htons_u(ETHERNET_FRAME_IP);
    }
}

// ReSharper disable once CppDFAConstantParameter
static void createIpHeader(IP_HEADER *ipHdr, const QSharedPointer<FLOW_TRACK> &flow, const unsigned int payloadLen, const bool sendOut) {

    static unsigned short fake_src_ip_id = 0x0010;
    static unsigned short fake_dst_ip_id = 0x00a0;

    const bool isIpv6 = flow->srcIp.protocol() == QAbstractSocket::IPv6Protocol;

    if (isIpv6) {
        ipHdr->ipv6.ver_tc_flow     = htonl_u((6 << 28) | (0 << 20) | 0); // version=6, traffic class=0, flow label=0
        ipHdr->ipv6.payload_length  = htons_u(payloadLen);              // 不包括IPv6头部，单位为字节
        ipHdr->ipv6.next_header     = flow->protocol;                    // 通常是 TCP(6) 或 UDP(17)
        ipHdr->ipv6.hop_limit       = 90;                               // 类似TTL

        const auto [s] = sendOut ? flow->srcIp.toIPv6Address() : flow->dstIp.toIPv6Address();
        const auto [d] = sendOut ? flow->dstIp.toIPv6Address() : flow->srcIp.toIPv6Address();

        memcpy(ipHdr->ipv6.src_ip, &s, sizeof(Q_IPV6ADDR));
        memcpy(ipHdr->ipv6.dst_ip, &d, sizeof(Q_IPV6ADDR));
    } else {
        ipHdr->ipv4.hdr_len    = 5;
        ipHdr->ipv4.ver        = 4;
        ipHdr->ipv4.tos        = 0;
        ipHdr->ipv4.flags      = 0x0000;
        ipHdr->ipv4.ttl        = 90;
        ipHdr->ipv4.protocol   = flow->protocol;

        ipHdr->ipv4.id = sendOut ? fake_src_ip_id : fake_dst_ip_id;
        sendOut ? fake_src_ip_id++ : fake_dst_ip_id++;

        ipHdr->ipv4.src_ip = sendOut ? flow->srcIp.toIPv4Address() : flow->dstIp.toIPv4Address();
        ipHdr->ipv4.dst_ip = sendOut ? flow->dstIp.toIPv4Address() : flow->srcIp.toIPv4Address();

        ipHdr->ipv4.total_len = htons_u(payloadLen + sizeof(IP_HEADER_V4));
        ipHdr->ipv4.checksum = 0;
    }
}

static void createTcpHeader(TCP_HEADER *tcpHdr, const QSharedPointer<FLOW_TRACK> &flow, const unsigned char flags, const bool sendOut) {

    tcpHdr->src_port = sendOut ? htons_u(flow->srcPort) : htons_u(flow->dstPort);
    tcpHdr->dst_port = sendOut ? htons_u(flow->dstPort) : htons_u(flow->srcPort);

    tcpHdr->seq_num = 0;
    tcpHdr->ack_num = 0;

    tcpHdr->unused = 0;
    tcpHdr->hdr_len = 5; // 5个双字 标准20字节头

    tcpHdr->flags = flags;
    tcpHdr->wnd_size = htons_u(0x8000u);

    tcpHdr->checksum = 0;
    tcpHdr->urg_pointer = 0;
}

static void createUdpHeader(UDP_HEADER *udpHdr, const QSharedPointer<FLOW_TRACK> &flow, const unsigned int payloadLen, const bool sendOut) {

    udpHdr->src_port = sendOut ? htons_u(flow->srcPort) : htons_u(flow->dstPort);
    udpHdr->dst_port = sendOut ? htons_u(flow->dstPort) : htons_u(flow->srcPort);

    udpHdr->udp_len = htons_u(payloadLen + sizeof(UDP_HEADER));
    udpHdr->checksum = 0;
}

static void createSynPkt(const QSharedPointer<FLOW_TRACK> &flow, TCP_PKT *tcpPkt) {

    constexpr bool send_out = true;
    const bool isIpv6 = flow->srcIp.protocol() == QAbstractSocket::IPv6Protocol;

    if (isIpv6) {
        createEthernetHeader(&tcpPkt->tcpv6.eth_hdr, send_out, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&tcpPkt->tcpv6.ip_hdr), flow, sizeof(TCP_HEADER), send_out);
        createTcpHeader(&tcpPkt->tcpv6.tcp_hdr, flow, TCP_SYN_FLAG, send_out);

        tcpPkt->tcpv6.tcp_hdr.seq_num = 0;
        tcpPkt->tcpv6.tcp_hdr.ack_num = 0;
    } else {
        createEthernetHeader(&tcpPkt->tcpv4.eth_hdr, send_out, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&tcpPkt->tcpv4.ip_hdr), flow, sizeof(TCP_HEADER), send_out);
        createTcpHeader(&tcpPkt->tcpv4.tcp_hdr, flow, TCP_SYN_FLAG, send_out);

        tcpPkt->tcpv4.tcp_hdr.seq_num = 0;
        tcpPkt->tcpv4.tcp_hdr.ack_num = 0;
    }

    flow->rxBytes = 0;
    flow->txBytes = 1;

}

static void createSynAckPkt(const QSharedPointer<FLOW_TRACK> &flow, TCP_PKT *tcpPkt) {

    constexpr bool send_out = false;
    const bool isIpv6 = flow->srcIp.protocol() == QAbstractSocket::IPv6Protocol;

    if (isIpv6) {
        createEthernetHeader(&tcpPkt->tcpv6.eth_hdr, send_out, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&tcpPkt->tcpv6.ip_hdr), flow, sizeof(TCP_HEADER), send_out);
        createTcpHeader(&tcpPkt->tcpv6.tcp_hdr, flow, TCP_SYN_FLAG | TCP_ACK_FLAG, send_out);

        tcpPkt->tcpv6.tcp_hdr.seq_num = htonl_u(flow->rxBytes);
        tcpPkt->tcpv6.tcp_hdr.ack_num = htonl_u(flow->txBytes);
    } else {
        createEthernetHeader(&tcpPkt->tcpv4.eth_hdr, send_out, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&tcpPkt->tcpv4.ip_hdr), flow, sizeof(TCP_HEADER), send_out);
        createTcpHeader(&tcpPkt->tcpv4.tcp_hdr, flow, TCP_SYN_FLAG | TCP_ACK_FLAG, send_out);

        tcpPkt->tcpv4.tcp_hdr.seq_num = htonl_u(flow->rxBytes);
        tcpPkt->tcpv4.tcp_hdr.ack_num = htonl_u(flow->txBytes);
    }
    flow->rxBytes++;
}

static void createSynDonePkt(const QSharedPointer<FLOW_TRACK> &flow, TCP_PKT *tcpPkt) {

    constexpr bool sendOut = true;
    const bool isIpv6 = flow->srcIp.protocol() == QAbstractSocket::IPv6Protocol;

    if (isIpv6) {
        createEthernetHeader(&tcpPkt->tcpv6.eth_hdr, sendOut, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&tcpPkt->tcpv6.ip_hdr), flow, sizeof(TCP_HEADER), sendOut);
        createTcpHeader(&tcpPkt->tcpv6.tcp_hdr, flow, TCP_ACK_FLAG, sendOut);

        tcpPkt->tcpv6.tcp_hdr.seq_num = htonl_u(flow->txBytes);
        tcpPkt->tcpv6.tcp_hdr.ack_num = htonl_u(flow->rxBytes);
    } else {
        createEthernetHeader(&tcpPkt->tcpv4.eth_hdr, sendOut, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&tcpPkt->tcpv4.ip_hdr), flow, sizeof(TCP_HEADER), sendOut);
        createTcpHeader(&tcpPkt->tcpv4.tcp_hdr, flow, TCP_ACK_FLAG, sendOut);

        tcpPkt->tcpv4.tcp_hdr.seq_num = htonl_u(flow->txBytes);
        tcpPkt->tcpv4.tcp_hdr.ack_num = htonl_u(flow->rxBytes);
    }
}

static void createTcpAckPkt(const QSharedPointer<FLOW_TRACK> &flow, TCP_PKT *tcpPkt, const bool sendOut) {

    const bool isIpv6 = flow->srcIp.protocol() == QAbstractSocket::IPv6Protocol;

    if (isIpv6) {
        createEthernetHeader(&tcpPkt->tcpv6.eth_hdr, sendOut, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&tcpPkt->tcpv6.ip_hdr), flow, sizeof(TCP_HEADER), sendOut);
        createTcpHeader(&tcpPkt->tcpv6.tcp_hdr, flow, TCP_ACK_FLAG, sendOut);

        tcpPkt->tcpv6.tcp_hdr.seq_num = sendOut ? htonl_u(flow->txBytes) : htonl_u(flow->rxBytes);
        tcpPkt->tcpv6.tcp_hdr.ack_num = sendOut ? htonl_u(flow->rxBytes) : htonl_u(flow->txBytes);
    } else {
        createEthernetHeader(&tcpPkt->tcpv4.eth_hdr, sendOut, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&tcpPkt->tcpv4.ip_hdr), flow, sizeof(TCP_HEADER), sendOut);
        createTcpHeader(&tcpPkt->tcpv4.tcp_hdr, flow, TCP_ACK_FLAG, sendOut);

        tcpPkt->tcpv4.tcp_hdr.seq_num = sendOut ? htonl_u(flow->txBytes) : htonl_u(flow->rxBytes);
        tcpPkt->tcpv4.tcp_hdr.ack_num = sendOut ? htonl_u(flow->rxBytes) : htonl_u(flow->txBytes);
    }
}

static void createTcpFinPkt(const QSharedPointer<FLOW_TRACK> &flow, TCP_PKT *tcpPkt, const bool sendOut) {

    const bool isIpv6 = flow->srcIp.protocol() == QAbstractSocket::IPv6Protocol;

    if (isIpv6) {
        createEthernetHeader(&tcpPkt->tcpv6.eth_hdr, sendOut, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&tcpPkt->tcpv6.ip_hdr), flow, sizeof(TCP_HEADER), sendOut);
        createTcpHeader(&tcpPkt->tcpv6.tcp_hdr, flow, TCP_FIN_FLAG, sendOut);

        tcpPkt->tcpv6.tcp_hdr.seq_num = sendOut ? htonl_u(flow->txBytes) : htonl_u(flow->rxBytes);
        tcpPkt->tcpv6.tcp_hdr.ack_num = sendOut ? htonl_u(flow->rxBytes) : htonl_u(flow->txBytes);
    } else {
        createEthernetHeader(&tcpPkt->tcpv4.eth_hdr, sendOut, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&tcpPkt->tcpv4.ip_hdr), flow, sizeof(TCP_HEADER), sendOut);
        createTcpHeader(&tcpPkt->tcpv4.tcp_hdr, flow, TCP_FIN_FLAG, sendOut);

        tcpPkt->tcpv4.tcp_hdr.seq_num = sendOut ? htonl_u(flow->txBytes) : htonl_u(flow->rxBytes);
        tcpPkt->tcpv4.tcp_hdr.ack_num = sendOut ? htonl_u(flow->rxBytes) : htonl_u(flow->txBytes);
    }

    sendOut ? flow->txBytes++ : flow->rxBytes++;
}


static void createTcpDataPkt(const QSharedPointer<FLOW_TRACK> &flow, TCP_PKT *tcpPkt, const unsigned int payloadLen, const bool sendOut) {

    const bool isIpv6 = flow->srcIp.protocol() == QAbstractSocket::IPv6Protocol;

    if (isIpv6) {
        createEthernetHeader(&tcpPkt->tcpv6.eth_hdr, sendOut, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&tcpPkt->tcpv6.ip_hdr), flow, sizeof(TCP_HEADER) + payloadLen, sendOut);
        createTcpHeader(&tcpPkt->tcpv6.tcp_hdr, flow, TCP_PSH_FLAG | TCP_ACK_FLAG, sendOut);

        tcpPkt->tcpv6.tcp_hdr.seq_num = sendOut ? htonl_u(flow->txBytes) : htonl_u(flow->rxBytes);
        tcpPkt->tcpv6.tcp_hdr.ack_num = sendOut ? htonl_u(flow->rxBytes) : htonl_u(flow->txBytes);
    } else {
        createEthernetHeader(&tcpPkt->tcpv4.eth_hdr, sendOut, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&tcpPkt->tcpv4.ip_hdr), flow, sizeof(TCP_HEADER) + payloadLen, sendOut);
        createTcpHeader(&tcpPkt->tcpv4.tcp_hdr, flow, TCP_PSH_FLAG | TCP_ACK_FLAG, sendOut);

        tcpPkt->tcpv4.tcp_hdr.seq_num = sendOut ? htonl_u(flow->txBytes) : htonl_u(flow->rxBytes);
        tcpPkt->tcpv4.tcp_hdr.ack_num = sendOut ? htonl_u(flow->rxBytes) : htonl_u(flow->txBytes);
    }

    if ( sendOut ) flow->txBytes += payloadLen;
    else flow->rxBytes += payloadLen;
}

static void createUdpDataPkt(const QSharedPointer<FLOW_TRACK> &flow, UDP_PKT *udpPkt, const unsigned int payloadLen, const bool sendOut) {

    const bool isIpv6 = flow->srcIp.protocol() == QAbstractSocket::IPv6Protocol;

    if (isIpv6) {
        createEthernetHeader(&udpPkt->udpv6.eth_hdr, sendOut, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&udpPkt->udpv6.ip_hdr), flow, sizeof(UDP_HEADER) + payloadLen, sendOut);
        createUdpHeader(&udpPkt->udpv6.udp_hdr, flow, payloadLen, sendOut);
    } else {
        createEthernetHeader(&udpPkt->udpv4.eth_hdr, sendOut, isIpv6);
        createIpHeader(reinterpret_cast<IP_HEADER *>(&udpPkt->udpv4.ip_hdr), flow, sizeof(UDP_HEADER) + payloadLen, sendOut);
        createUdpHeader(&udpPkt->udpv4.udp_hdr, flow, payloadLen, sendOut);
    }
}

static bool writePktsOut(const QString &filePath, const QByteArray &fileContent) {

    auto bs = fileContent;

    if ( !QFile(filePath).exists() ) {
        PCAP_HDR capHdr = {};
        capHdr.magic_number     = 0xa1b2c3d4;
        capHdr.version_major    = 0x02;
        capHdr.version_minor    = 0x04;
        capHdr.thiszone         = 0;
        capHdr.sigfigs          = 0;
        capHdr.snaplen          = 0xA0000000;
        capHdr.network          = 0x01;
        bs.insert(0, reinterpret_cast<const char *>(&capHdr), sizeof(capHdr));
    }

    bool result = false;
    QFile file(filePath);
    if ( file.open(QIODevice::Append) ) {
        file.write(bs);
        result = true;
    }
    return result;
}
