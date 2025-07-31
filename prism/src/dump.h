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
#ifndef PRISM_UI_DUMP_H
#define PRISM_UI_DUMP_H

#include <QSharedPointer>
#include <QString>
#include <QElapsedTimer>
#include "custom/safe_map.hpp"
#include "ui_mainwgt.h"


// 多久刷新一下到磁盘
#define FILE_FLUSH_INTERVAL_MS      10 * 1000
// 最大缓存字节数
#define CACHING_BUFFER_MAX_BYTES    1 * 1024 * 1024




#define MAC_ADDRESS_LEN			6

// 定义以太网帧类型
#define ETHERNET_FRAME_IP		0x0800u				// IP帧		0x0800
#define ETHERNET_FRAME_IPv6		0x86DDu				// IP6帧	0x86DD

// 定义IP数据报中的协议类型
#define PROTOCOL_TCP			0x06				// TCP 报文
#define PROTOCOL_UDP			0x11				// UDP 报文 17

// IP标志
#define IP_DF_MASK				0x4000				// 如果置位,表示不能被分割
#define IP_MF_MASK				0x2000				// 为1表示还有更多的数据报分片 为0表示已经是最后一个分片

// TCP头标志位掩码
#define TCP_URG_FLAG			0x0020u				// URG标志
#define TCP_ACK_FLAG			0x0010u				// ACK标志 应答1或请求0
#define TCP_PSH_FLAG			0x0008u				// psh标志 以最快速度传输数据
#define TCP_RST_FLAG			0x0004u				// RST标志 先断开连接,再重建连接
#define TCP_SYN_FLAG			0x0002u				// SYN标志 用来建立连接
#define TCP_FIN_FLAG			0x0001u				// FIN标志 发送方完成数据发送


#define htons_u(x)              (unsigned short)( (((x) & 0xffu) << 8u) | (((x) & 0xff00u) >> 8u) )
#define ntohs_u(x)              htons_u(x)

#define ntohl_u(x)              ( (((x) & 0xffu) << 24u) | \
(((x) & 0xff00u) << 8u) | \
(((x) & 0xff0000u) >> 8u) | \
(((x) & 0xff000000) >> 24u) )
#define htonl_u(x)              ntohl_u(x)



// 保存对齐状态
#pragma pack(push)
#pragma pack(1)

#define MAC_ADDRESS_LEN 6

typedef struct ETHERNET_HEADER_ {
    uint8_t             dst_mac[MAC_ADDRESS_LEN];  // 目标MAC
    uint8_t             src_mac[MAC_ADDRESS_LEN];  // 源MAC
    uint16_t            type;                      // 类型
} ETHERNET_HEADER;

typedef struct IP_HEADER_V4_ {
    uint8_t             hdr_len : 4;   // IP头部长度（单位：32位字）
    uint8_t             ver : 4;       // 协议版本（IPv4为4，IPv6为6）
    uint8_t             tos;           // 服务类型
    uint16_t            total_len;     // 总长度
    uint16_t            id;            // 标识
    uint16_t            flags;         // 标志+片偏移
    uint8_t             ttl;           // 生存时间
    uint8_t             protocol;      // 协议类型
    uint16_t            checksum;      // 校验和
    uint32_t            src_ip;        // 源IP地址（IPv4）
    uint32_t            dst_ip;        // 目标IP地址（IPv4）
} IP_HEADER_V4;

typedef struct IP_PKT_V4_ {
    ETHERNET_HEADER     eth_hdr;
    IP_HEADER_V4        ip_hdr;
} IP_PKT_V4;

typedef struct IP_HEADER_V6_ {
    uint32_t            ver_tc_flow;        // 版本 + 流量类 + 流标签（需手动解析）
    uint16_t            payload_length;     // 有效载荷长度
    uint8_t             next_header;        // 下一个头部类型（例如 TCP = 6, UDP = 17）
    uint8_t             hop_limit;          // 跳数限制
    uint8_t             src_ip[16];         // 源地址
    uint8_t             dst_ip[16];         // 目的地址
} IP_HEADER_V6;

typedef struct IP_PKT_V6_ {
    ETHERNET_HEADER     eth_hdr;
    IP_HEADER_V6        ip_hdr;
} IP_PKT_V6;

union IP_HEADER {
    IP_HEADER_V4        ipv4;
    IP_HEADER_V6        ipv6;
};

union IP_PKT {
    IP_PKT_V4           ipv4;
    IP_PKT_V6           ipv6;
};

typedef struct TCP_HEADER_ {
    uint16_t            src_port;      // 源端口
    uint16_t            dst_port;      // 目标端口
    uint32_t            seq_num;       // 序列号
    uint32_t            ack_num;       // 确认号
    uint8_t             unused : 4;
    uint8_t             hdr_len : 4;   // TCP头部长度（单位：32位字）
    uint8_t             flags;         // 标志位
    uint16_t            wnd_size;      // 窗口大小
    uint16_t            checksum;      // 校验和
    uint16_t            urg_pointer;   // 紧急指针
} TCP_HEADER;

typedef struct TCP_PKT_V4_ {
    ETHERNET_HEADER     eth_hdr;
    IP_HEADER_V4        ip_hdr;
    TCP_HEADER          tcp_hdr;
} TCP_PKT_V4;

typedef struct TCP_PKT_V6_ {
    ETHERNET_HEADER     eth_hdr;
    IP_HEADER_V6        ip_hdr;
    TCP_HEADER          tcp_hdr;
} TCP_PKT_V6;

union TCP_PKT{
    TCP_PKT_V4          tcpv4;
    TCP_PKT_V6          tcpv6;
};

typedef struct UDP_HEADER_ {
    uint16_t            src_port;      // 源端口
    uint16_t            dst_port;      // 目标端口
    uint16_t            udp_len;       // UDP长度（含头部和数据）
    uint16_t            checksum;      // 校验和
} UDP_HEADER;

typedef struct UDP_PKT_V4_ {
    ETHERNET_HEADER     eth_hdr;
    IP_HEADER_V4        ip_hdr;
    UDP_HEADER          udp_hdr;
} UDP_PKT_V4;

typedef struct UDP_PKT_V6_ {
    ETHERNET_HEADER     eth_hdr;
    IP_HEADER_V6        ip_hdr;
    UDP_HEADER          udp_hdr;
} UDP_PKT_V6;

union UDP_PKT {
    UDP_PKT_V4          udpv4;
    UDP_PKT_V6          udpv6;
};


// 恢复对齐状态
#pragma pack(pop)


// libcap file format
// global header
typedef struct PCAP_HDR_ {
    unsigned int magic_number;          // magic number               // 0xa1b2c3d4
    unsigned short version_major;       // major version number       // 0x0002
    unsigned short version_minor;       // minor version number       // 0x0004
    int thiszone;                       // GMT to local correction    // 0
    unsigned int sigfigs;               // accuracy of timestamps     // 0
    unsigned int snaplen;               // max length of captured packets, in octets      // 0x40000
    unsigned int network;               // data link type             // LINKTYPE_ETHERNET 1
}PCAP_HDR;

// packet header
typedef struct PCAPREC_HDR_ {
    unsigned int ts_sec;                // timestamp seconds          // timeval.tv_sec   秒
    unsigned int ts_usec;               // timestamp microseconds     // timeval.tv_usec  微秒偏移
    unsigned int incl_len;              // number of octets of packet saved in file   // 实际保存长度
    unsigned int orig_len;              // actual length of packet    // 数据包原始长度 与incl_len相同即可
}PCAPREC_HDR;







// IN HOST BYTE ORDER
typedef struct FLOW_TRACK_ {
    FLOW_TRACK_(
        const QHostAddress& srcIp,
        const QHostAddress& dstIp,
        const unsigned short srcPort,
        const unsigned short dstPort,
        const int protocol,
        const unsigned int rxBytes,
        const unsigned int txBytes)
        : srcIp(srcIp),
          dstIp(dstIp),
          srcPort(srcPort),
          dstPort(dstPort),
          protocol(protocol),
          rxBytes(rxBytes),
          txBytes(txBytes) {

        if (srcIp.protocol() != dstIp.protocol()) {
            if (srcIp.protocol() == QAbstractSocket::IPv6Protocol) {
                const QString ipv6Mapped = "::ffff:" + dstIp.toString();
                this->dstIp = QHostAddress(ipv6Mapped);
            } else {
                const QString ipv6Mapped = "::ffff:" + srcIp.toString();
                this->srcIp = QHostAddress(ipv6Mapped);
            }
        }
    }

    FLOW_TRACK_() = default;

    QHostAddress srcIp;
    QHostAddress dstIp;
    unsigned short srcPort = 0;
    unsigned short dstPort = 0;
    int protocol = 0;
    unsigned int rxBytes = 0;
    unsigned int txBytes = 0;
}FLOW_TRACK;



class PacketDumper {

public:
    PacketDumper(const PacketDumper&) = delete;
    PacketDumper& operator=(const PacketDumper&) = delete;


    // Get the singleton instance
    static PacketDumper& instance() {
        // Guaranteed thread-safe in C++11 and later
        static auto *instance = new PacketDumper;
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

    unsigned int getCachingBytes() const { return m_cachingBytesLen; }
    void setPcapFilePath(const QString &filePath) { this->m_pcapFilePath = filePath; }

private:
    PacketDumper() { this->m_lastRefreshTimer.start(); }
    ~PacketDumper() = default;

    void savePkts(bool flush);
    bool timerExpired();

    ThreadSafeMap<QString, QSharedPointer<FLOW_TRACK>> m_flows{};

    // 保存到的文件路径
    QString m_pcapFilePath{};
    // 还没有保存到本地的字节
    QByteArray m_cachingBytes{};
    unsigned int m_cachingBytesLen{0};

    QElapsedTimer m_lastRefreshTimer{};
};

#endif //PRISM_UI_DUMP_H
