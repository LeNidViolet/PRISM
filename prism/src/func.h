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
#ifndef PRISM_FUNC_H
#define PRISM_FUNC_H

#include <QByteArray>
#include "ui_config.h"

void setDarkTheme();
void setLightTheme();
void selectFount();

QByteArray configToJson(const ConfigVars &config);
ConfigVars *configFromJson(const QByteArray &bytes);
QString formatBytes(qint64 bytes);
QString genLinkKey(bool isStream, int index);




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

// 保存对其状态
#pragma pack(push)
#pragma pack(1)

typedef struct ethernet_header_	 {	// 以太头 14字节
    unsigned char		dst_mac[MAC_ADDRESS_LEN];		    // 目标MAC
    unsigned char		src_mac[MAC_ADDRESS_LEN];		    // 源MAC
    unsigned short		type;							    // 类型
}ethernet_header;

typedef struct ip_header_ {			// IP头 20 BYTES
    unsigned char		hdr_len : 4;					    // IP数据报首部的双字个数,一般为5即20字节长
    unsigned char		ver : 4;							// 版本信息
    unsigned char		tos;								// 服务类型<一般忽略>
    unsigned short		total_len;							// IP数据报总长度<字节单位>,包括了IP头,这个长度减去HdrLen就是数据实际长度
    unsigned short		id; 								// 封包ID 每发送一个包这里加一
    unsigned short		flags;  							// 标致字段和片偏移 3位标志 13位偏移
    unsigned char		ttl;								// 存活周期
    unsigned char		protocol;							// 协议类型
    unsigned short		checksum;							// 效验和
    unsigned int		src_ip;								// 源IP
    unsigned int		dst_ip;								// 目的IP
}ip_header;

typedef struct ip_pkt_ {			// IP封包
    ethernet_header	    ehhdr;
    ip_header		    iphdr;
}ip_pkt;

typedef struct tcp_header_ {		// TCP头 20 BYTES
    unsigned short		src_port;							// 源端口
    unsigned short		dst_port;							// 目的端口
    unsigned int		seq_num;							// 序号
    unsigned int		ack_num;							// 确认号
    unsigned char       unused : 4;                         //
    unsigned char       hdr_len : 4;                        // TCP头长度
    unsigned char		flags;	    						// 标志
    unsigned short		wnd_size;							// 窗口大小<实现流量控制>
    unsigned short		checksum;							// 效验和
    unsigned short		urg_pointer;						// 紧急指针
}tcp_header;

// 4位首部长度 + 6位保留位 + URG+ACK+PSH+RST+SYN+FIN

typedef struct tcp_pkt_ {           // TCP封包 <当ip中没有多余数据时>
    ethernet_header	    ehhdr;
    ip_header		    iphdr;
    tcp_header		    tcphdr;
}tcp_pkt;


typedef struct udp_header_ {
    unsigned short		src_port;							// 源端口
    unsigned short		dst_port;							// 目的端口
    unsigned short		udp_len;							// 长度 头+数据字节长 最小8字节
    unsigned short		checksum;							// 效验和
}udp_header;

typedef struct udp_pkt_ {			    // UDP封包 <当ip中没有多余数据时>
    ethernet_header	    ehhdr;
    ip_header		    iphdr;
    udp_header		    udphdr;
}udp_pkt;


// 恢复对其状态
#pragma pack(pop)



// =====================

// libcap file format
// global header
typedef struct pcap_hdr_ {
    unsigned int magic_number;          // magic number               // 0xa1b2c3d4
    unsigned short version_major;       // major version number       // 0x0002
    unsigned short version_minor;       // minor version number       // 0x0004
    int thiszone;                       // GMT to local correction    // 0
    unsigned int sigfigs;               // accuracy of timestamps     // 0
    unsigned int snaplen;               // max length of captured packets, in octets      // 0x40000
    unsigned int network;               // data link type             // LINKTYPE_ETHERNET 1
}pcap_hdr;

// packet header
typedef struct pcaprec_hdr_ {
    unsigned int ts_sec;                // timestamp seconds          // timeval.tv_sec   秒
    unsigned int ts_usec;               // timestamp microseconds     // timeval.tv_usec  微秒偏移
    unsigned int incl_len;              // number of octets of packet saved in file   // 实际保存长度
    unsigned int orig_len;              // actual length of packet    // 数据包原始长度 与incl_len相同即可
}pcaprec_hdr;



// IN HOST BYTE ORDER
typedef struct pkt_track_ {
    unsigned int		src_ip;
    unsigned int		dst_ip;
    unsigned short		src_port;
    unsigned short		dst_port;
    int                 protocol;
    unsigned int		bytes_in;   // FOR TCP
    unsigned int		bytes_out;  // FOR TCP
}pkt_track;

QByteArray writeout_tcp_handshake_pkts(pkt_track *track);
QByteArray writeout_tcp_fin_pkts(pkt_track *track, bool sendOut);
QByteArray writeout_tcp_data_pkt(pkt_track *track, const char *data, size_t data_len, bool sendOut);
QByteArray writeout_udp_data_pkt(pkt_track *track, const char *data, size_t data_len, bool sendOut);

#endif //PRISM_FUNC_H
