/**
 *  Copyright 2022, raprepo.
 *  Created by raprepo on 2022/8/24.
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
#include "misc.h"
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

#include "ui_config.h"

#ifdef _WIN32
#include <windows.h>
// struct timeval {
//     long tv_sec;   // 秒
//     long tv_usec;  // 微秒
// };
int gettimeofday(struct timeval* tv, void* timezone) {
    if (tv == nullptr) return -1;

    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);  // 或用 GetSystemTimePreciseAsFileTime(&ft) if available

    // Convert FILETIME to 64-bit integer
    uint64_t time = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;

    // Windows epoch (1601) -> Unix epoch (1970) difference in 100-nanosecond units
    const uint64_t EPOCH_DIFF = 11644473600000000ULL;

    // Convert to microseconds
    time = (time - EPOCH_DIFF) / 10;

    tv->tv_sec = (long)(time / 1000000ULL);
    tv->tv_usec = (long)(time % 1000000ULL);

    return 0;
}
#else
#include <sys/time.h>
#endif



QByteArray MiscFuncs::configToJson(const ConfigVars &config) {

    QJsonObject object;
    object.insert("certfile", config.crtFile);
    object.insert("keyfile", config.keyFile);
    object.insert("pktfile", config.pktFile);
    object.insert("hostfile", config.hostFile);

    object.insert("server", config.server.toString());
    object.insert("port", config.port);
    object.insert("timeout", (int)config.timeout);
    object.insert("method", config.method);
    object.insert("password", config.password);
    object.insert("dnssrv", config.dnssrv);

    QJsonDocument document;
    document.setObject(object);

    QByteArray array = document.toJson(QJsonDocument::Compact);
    return array;
}

ConfigVars *MiscFuncs::configFromJson(const QByteArray &bytes) {

    ConfigVars *result = nullptr;

    QJsonParseError jsonError{};
    QJsonDocument document = QJsonDocument::fromJson(bytes, &jsonError);
    if( !document.isNull() && document.isObject() && (jsonError.error == QJsonParseError::NoError))
    {
        QJsonObject object = document.object();

        QString crt;
        if ( object.contains("certfile") ) {
            auto value = object.value("certfile");
            if ( value.isString() ) {
                crt = value.toString();
            }
        }
        if ( crt.isEmpty() ) goto exit;

        QString key;
        if ( object.contains("keyfile") ) {
            auto value = object.value("keyfile");
            if ( value.isString() ) {
                key = value.toString();
            }
        }
        if ( key.isEmpty() ) goto exit;

        QString pkt;
        if ( object.contains("pktfile") ) {
            auto value = object.value("pktfile");
            if ( value.isString() ) {
                pkt = value.toString();
            }
        }
        if ( pkt.isEmpty() ) goto exit;

        QString host;
        if ( object.contains("hostfile") ) {
            auto value = object.value("hostfile");
            if ( value.isString() ) {
                host = value.toString();
            }
        }
        if ( host.isEmpty() ) goto exit;

        QString server;
        if ( object.contains("server") ) {
            auto value = object.value("server");
            if ( value.isString() ) {
                server = value.toString();
            }
        }
        if ( server.isEmpty() ) goto exit;

        int port = -1;
        if ( object.contains("port") ) {
            auto value = object.value("port");
            if ( value.isDouble() ) {
                port = value.toInt(0);
            }
        }
        if ( -1 == port ) goto exit;

        int timeout = -1;
        if ( object.contains("timeout") ) {
            auto value = object.value("timeout");
            if ( value.isDouble() ) {
                timeout = value.toInt(0);
            }
        }
        if ( -1 == timeout ) goto exit;

        QString method;
        if ( object.contains("method") ) {
            auto value = object.value("method");
            if ( value.isString() ) {
                method = value.toString();
            }
        }
        if ( method.isEmpty() ) goto exit;

        QString passwd;
        if ( object.contains("password") ) {
            auto value = object.value("password");
            if ( value.isString() ) {
                passwd = value.toString();
            }
        }
        if ( passwd.isEmpty() ) goto exit;

        bool dnssrv = false;
        if ( object.contains("dnssrv") ) {
            auto value = object.value("dnssrv");
            if ( value.isBool() ) {
                dnssrv = value.toBool();
            }
        }

        result = new ConfigVars;
        result->crtFile = crt;
        result->keyFile = key;
        result->pktFile = pkt;
        result->hostFile = host;
        result->server = QHostAddress(server);
        result->port = port;
        result->timeout = timeout;
        result->method = method;
        result->password = passwd;
        result->dnssrv = dnssrv;
    }

exit:
    return result;
}

QString MiscFuncs::formatBytes(qint64 bytes) {

    const auto KB = 1024;
    const auto MB = 1024 * 1024;
    const auto GB = 1024 * 1024 * 1024;

    auto dbbytes = (double)bytes;

    QString result;

    if ( dbbytes >= GB ) {
        auto g = dbbytes / GB;
        result = QString::asprintf("%.2f GB", g);
    } else if ( dbbytes >= MB ) {
        auto m = dbbytes / MB;
        result = QString::asprintf("%.2f MB", m);
    } else if ( dbbytes >= KB ) {
        auto k = dbbytes / KB;
        result = QString::asprintf("%.2f KB", k);
    } else {
        result = QString("%1 B").arg(dbbytes);
    }
    return result;
}

QString MiscFuncs::genLinkKey(bool isStream, int index) {
    auto prot = isStream ? "TCP" : "UDP";
    return QString("%1%2").arg(prot, QString::number(index));
}











// =====================




static void create_ethernet_header(ethernet_header *hdr, bool send_out) {

    static unsigned char fake_src_mac[MAC_ADDRESS_LEN] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60};
    static unsigned char fake_dst_mac[MAC_ADDRESS_LEN] = {0xf0, 0xe0, 0xd0, 0xc0, 0xb0, 0xa0};

    if ( send_out ) {
        memcpy(hdr->src_mac, fake_src_mac, MAC_ADDRESS_LEN);
        memcpy(hdr->dst_mac, fake_dst_mac, MAC_ADDRESS_LEN);
    } else {
        memcpy(hdr->src_mac, fake_dst_mac, MAC_ADDRESS_LEN);
        memcpy(hdr->dst_mac, fake_src_mac, MAC_ADDRESS_LEN);
    }

    // TODO: ipv4 only
    hdr->type = htons_u(ETHERNET_FRAME_IP);
}

static void create_ip_header(ip_header *hdr, pkt_track* track, unsigned int payload_len, bool send_out) {

    static unsigned short fake_src_ip_id = 0x0010;
    static unsigned short fake_dst_ip_id = 0x00a0;

    hdr->hdr_len    = 5;
    hdr->ver        = 4;
    hdr->tos        = 0;
    hdr->flags      = 0x0000;
    hdr->ttl        = 90;
    hdr->protocol   = track->protocol;

    hdr->id = send_out ? fake_src_ip_id : fake_dst_ip_id;
    send_out ? fake_src_ip_id++ : fake_dst_ip_id++;

    hdr->src_ip = send_out ? htonl_u(track->src_ip) : htonl_u(track->dst_ip);
    hdr->dst_ip = send_out ? htonl_u(track->dst_ip) : htonl_u(track->src_ip);

    hdr->total_len = htons_u(payload_len + sizeof(ip_header));
    hdr->checksum = 0;
}

static void create_tcp_header(tcp_header *hdr, pkt_track *track, unsigned char flags, bool send_out) {

    hdr->src_port = send_out ? htons_u(track->src_port) : htons_u(track->dst_port);
    hdr->dst_port = send_out ? htons_u(track->dst_port) : htons_u(track->src_port);

    hdr->seq_num = 0;
    hdr->ack_num = 0;

    hdr->unused = 0;
    hdr->hdr_len = 5; // 5个双字 标准20字节头

    hdr->flags = flags;
    hdr->wnd_size = htons_u(0x8000u);

    hdr->checksum = 0;
    hdr->urg_pointer = 0;
}

static void create_udp_header(udp_header *hdr, pkt_track *track, unsigned int payload_len, bool send_out) {

    hdr->src_port = send_out ? htons_u(track->src_port) : htons_u(track->dst_port);
    hdr->dst_port = send_out ? htons_u(track->dst_port) : htons_u(track->src_port);

    hdr->udp_len = htons_u(payload_len + sizeof(udp_header));
    hdr->checksum = 0;
}

static void create_syn_pkt(pkt_track *track, tcp_pkt *pkt) {

    bool send_out = true;

    create_ethernet_header(&pkt->ehhdr, send_out);
    create_ip_header(&pkt->iphdr, track, sizeof(tcp_header), send_out);
    create_tcp_header(&pkt->tcphdr, track, TCP_SYN_FLAG, send_out);

    pkt->tcphdr.seq_num = 0;
    pkt->tcphdr.ack_num = 0;

    track->bytes_in = 0;
    track->bytes_out = 1;

}

static void create_syn_ack_pkt(pkt_track *track, tcp_pkt *pkt) {

    bool send_out = false;

    create_ethernet_header(&pkt->ehhdr, send_out);
    create_ip_header(&pkt->iphdr, track, sizeof(tcp_header), send_out);
    create_tcp_header(&pkt->tcphdr, track, TCP_SYN_FLAG | TCP_ACK_FLAG, send_out);

    pkt->tcphdr.seq_num = htonl_u(track->bytes_in);
    pkt->tcphdr.ack_num = htonl_u(track->bytes_out);
    track->bytes_in++;
}

static void create_syn_done_pkt(pkt_track *track, tcp_pkt *pkt) {

    bool send_out = true;

    create_ethernet_header(&pkt->ehhdr, send_out);
    create_ip_header(&pkt->iphdr, track, sizeof(tcp_header), send_out);
    create_tcp_header(&pkt->tcphdr, track, TCP_ACK_FLAG, send_out);

    pkt->tcphdr.seq_num = htonl_u(track->bytes_out);
    pkt->tcphdr.ack_num = htonl_u(track->bytes_in);
}

static void create_tcp_ack_pkt(pkt_track *track, tcp_pkt *pkt, bool send_out) {

    create_ethernet_header(&pkt->ehhdr, send_out);
    create_ip_header(&pkt->iphdr, track, sizeof(tcp_header), send_out);
    create_tcp_header(&pkt->tcphdr, track, TCP_ACK_FLAG, send_out);

    pkt->tcphdr.seq_num = send_out ? htonl_u(track->bytes_out) : htonl_u(track->bytes_in);
    pkt->tcphdr.ack_num = send_out ? htonl_u(track->bytes_in) : htonl_u(track->bytes_out);
}

static void create_tcp_fin_pkt(pkt_track *track, tcp_pkt *pkt, bool send_out) {

    create_ethernet_header(&pkt->ehhdr, send_out);
    create_ip_header(&pkt->iphdr, track, sizeof(tcp_header), send_out);
    create_tcp_header(&pkt->tcphdr, track, TCP_FIN_FLAG, send_out);

    pkt->tcphdr.seq_num = send_out ? htonl_u(track->bytes_out) : htonl_u(track->bytes_in);
    pkt->tcphdr.ack_num = send_out ? htonl_u(track->bytes_in) : htonl_u(track->bytes_out);
    send_out ? track->bytes_out++ : track->bytes_in++;
}

static void create_tcp_data_pkt(pkt_track *track, tcp_pkt *pkt, unsigned int payload_len, bool send_out) {

    create_ethernet_header(&pkt->ehhdr, send_out);
    create_ip_header(&pkt->iphdr, track, payload_len + sizeof(tcp_header), send_out);
    create_tcp_header(&pkt->tcphdr, track, TCP_PSH_FLAG | TCP_ACK_FLAG, send_out);

    pkt->tcphdr.seq_num = send_out ? htonl_u(track->bytes_out) : htonl_u(track->bytes_in);
    pkt->tcphdr.ack_num = send_out ? htonl_u(track->bytes_in) : htonl_u(track->bytes_out);
    if ( send_out ) track->bytes_out += payload_len;
    else track->bytes_in += payload_len;
}

static void create_udp_data_pkt(pkt_track *track, udp_pkt *pkt, unsigned int payload_len, bool send_out) {

    create_ethernet_header(&pkt->ehhdr, send_out);
    create_ip_header(&pkt->iphdr, track, payload_len + sizeof(udp_header), send_out);
    create_udp_header(&pkt->udphdr, track, payload_len, send_out);
}

QByteArray MiscFuncs::buildTcpHandshakePkt(pkt_track *track) {

    pcaprec_hdr     hdr;
    tcp_pkt         pkt;
    struct timeval         tv = {0};

    gettimeofday(&tv, nullptr);

    hdr.ts_sec      = tv.tv_sec;
    hdr.ts_usec     = tv.tv_usec;
    hdr.incl_len    = sizeof(tcp_pkt);
    hdr.orig_len    = sizeof(tcp_pkt);

    QByteArray result;

    create_syn_pkt(track, &pkt);
    result.append((const char*)&hdr, sizeof(pcaprec_hdr));
    result.append((const char*)&pkt, sizeof(tcp_pkt));

    create_syn_ack_pkt(track, &pkt);
    hdr.ts_usec += 5;
    result.append((const char*)&hdr, sizeof(pcaprec_hdr));
    result.append((const char*)&pkt, sizeof(tcp_pkt));

    create_syn_done_pkt(track, &pkt);
    hdr.ts_usec += 5;
    result.append((const char*)&hdr, sizeof(pcaprec_hdr));
    result.append((const char*)&pkt, sizeof(tcp_pkt));

    return result;
}

QByteArray MiscFuncs::buildTcpFinPkt(pkt_track *track, bool sendOut) {

    pcaprec_hdr     hdr;
    tcp_pkt         pkt;
    struct timeval         tv = {0};

    gettimeofday(&tv, nullptr);

    hdr.ts_sec      = tv.tv_sec;
    hdr.ts_usec     = tv.tv_usec;
    hdr.incl_len    = sizeof(tcp_pkt);
    hdr.orig_len    = sizeof(tcp_pkt);

    QByteArray result;

    // 发起方发送FIN
    create_tcp_fin_pkt(track, &pkt, sendOut);
    result.append((const char*)&hdr, sizeof(pcaprec_hdr));
    result.append((const char*)&pkt, sizeof(tcp_pkt));

    // 接收方收到FIN之后ACK
    create_tcp_ack_pkt(track, &pkt, !sendOut);
    hdr.ts_usec += 5;
    result.append((const char*)&hdr, sizeof(pcaprec_hdr));
    result.append((const char*)&pkt, sizeof(tcp_pkt));

    // 接收方收到FIN之后FIN
    create_tcp_fin_pkt(track, &pkt, !sendOut);
    hdr.ts_usec += 5;
    result.append((const char*)&hdr, sizeof(pcaprec_hdr));
    result.append((const char*)&pkt, sizeof(tcp_pkt));

    // 发起方发送ACK
    create_tcp_ack_pkt(track, &pkt, sendOut);
    hdr.ts_usec += 5;
    result.append((const char*)&hdr, sizeof(pcaprec_hdr));
    result.append((const char*)&pkt, sizeof(tcp_pkt));

    return result;
}

QByteArray MiscFuncs::buildTcpPayloadPkt(pkt_track *track, const char *data, size_t data_len, bool sendOut) {

    pcaprec_hdr     hdr;
    tcp_pkt         pkt;
    struct timeval         tv = {0};

    gettimeofday(&tv, nullptr);

    hdr.ts_sec      = tv.tv_sec;
    hdr.ts_usec     = tv.tv_usec;
    hdr.incl_len    = sizeof(tcp_pkt) + data_len;
    hdr.orig_len    = sizeof(tcp_pkt) + data_len;

    create_tcp_data_pkt(track, &pkt, data_len, sendOut);

    QByteArray result;

    result.append((const char*)&hdr, sizeof(pcaprec_hdr));
    result.append((const char*)&pkt, sizeof(tcp_pkt));
    result.append((const char*)data, (int)data_len);

    // 对方发送ACK
    create_tcp_ack_pkt(track, &pkt, !sendOut);
    hdr.incl_len    = sizeof(tcp_pkt);
    hdr.orig_len    = sizeof(tcp_pkt);
    hdr.ts_usec     += 5;
    result.append((const char*)&hdr, sizeof(pcaprec_hdr));
    result.append((const char*)&pkt, sizeof(tcp_pkt));

    return result;
}

QByteArray MiscFuncs::buildUdpPayloadPkt(pkt_track *track, const char *data, size_t data_len, bool sendOut) {

    pcaprec_hdr     hdr;
    udp_pkt         pkt;
    struct timeval         tv = {0};

    gettimeofday(&tv, nullptr);

    hdr.ts_sec      = tv.tv_sec;
    hdr.ts_usec     = tv.tv_usec;
    hdr.incl_len    = sizeof(udp_pkt) + data_len;
    hdr.orig_len    = sizeof(udp_pkt) + data_len;

    create_udp_data_pkt(track, &pkt, data_len, sendOut);

    QByteArray result;

    result.append((const char*)&hdr, sizeof(pcaprec_hdr));
    result.append((const char*)&pkt, sizeof(udp_pkt));
    result.append((const char*)data, (int)data_len);

    return result;
}


void MiscFuncs::writePktsOut(const QString &filePath, const QByteArray &fileContent) {

    auto bs = fileContent;

    if ( !QFile(filePath).exists() ) {
        pcap_hdr caphdr = {};
        caphdr.magic_number = 0xa1b2c3d4;
        caphdr.version_major = 0x02;
        caphdr.version_minor = 0x04;
        caphdr.thiszone = 0;
        caphdr.sigfigs = 0;
        caphdr.snaplen = 0xA0000000;
        caphdr.network = 0x01;
        bs.insert(0, (const char*)&caphdr, sizeof(caphdr));
    }

    QFile file(filePath);
    if ( file.open(QIODevice::Append) ) {
        file.write(bs);
    }
}

void MiscFuncs::writeHostsOut(const QString &filePath, const QString &fileContent) {

    QFile file(filePath);
    if ( file.open(QIODevice::WriteOnly | QIODevice::Text) ) {
        QTextStream out(&file);
        out << fileContent;
    }
}