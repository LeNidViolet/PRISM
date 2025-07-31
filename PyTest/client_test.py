import socket
import ipaddress
from func import load_config
from s5 import udp_socks5_associate, udp_socks5_pack_packet, udp_socks5_parse_packet
from s5 import tcp_socks5_connect


def begin_test_udp_do(proxy_ipv6, proxy_host, proxy_port, target_host, target_port):

    tcp_sock, proxy_addr, proxy_port = udp_socks5_associate(proxy_ipv6, proxy_host, proxy_port, target_host)
    print(f'got proxyaddr[{proxy_addr}:{proxy_port}]')

    ip = ipaddress.ip_address(proxy_addr)
    if isinstance(ip, ipaddress.IPv4Address):
        udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    elif isinstance(ip, ipaddress.IPv6Address):
        udp_sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
    else:
        assert False

    content = b'1234567890'
    for i  in range(2):

        bs = udp_socks5_pack_packet(target_host, target_port, content)
        udp_sock.sendto(bs, (proxy_addr, proxy_port))

        bs, _ = udp_sock.recvfrom(128)
        dst_addr, dst_port, payload = udp_socks5_parse_packet(bs)

        print(f"Received from: {dst_addr}:{dst_port}")
        print(f"Payload: {payload}")

    udp_sock.close()
    tcp_sock.close()


def begin_test_udp():
    proxy_ipv4 = load_config('socks5_proxy_address_v4')
    proxy_ipv6 = load_config('socks5_proxy_address_v6')
    proxy_port = load_config('socks5_proxy_port')
    remote_target_port = load_config('server_port')
    target_ipv4 = load_config('server_address_v4')
    target_ipv6 = load_config('server_address_v6')

    # ipv4 proxy
    begin_test_udp_do(False, proxy_ipv4, proxy_port, target_ipv4, remote_target_port)
    begin_test_udp_do(False, proxy_ipv4, proxy_port, target_ipv6, remote_target_port)

    # ipv6 proxy
    begin_test_udp_do(True, proxy_ipv6, proxy_port, target_ipv4, remote_target_port)
    begin_test_udp_do(True, proxy_ipv6, proxy_port, target_ipv6, remote_target_port)







def begin_test_do(proxy_ipv6, proxy_host, proxy_port, target_host, target_port):
    sock = tcp_socks5_connect(proxy_ipv6, proxy_host, proxy_port, target_host, target_port)

    content = b'1234567890'
    for i in range(2):
        sock.send(content)
        response = sock.recv(4096)
        print(response[:20])

    sock.close()


def begin_test_tcp():
    proxy_ipv4 = load_config('socks5_proxy_address_v4')
    proxy_ipv6 = load_config('socks5_proxy_address_v6')
    proxy_port = load_config('socks5_proxy_port')
    target_port = load_config('server_port')
    target_ipv4 = load_config('server_address_v4')
    target_ipv6 = load_config('server_address_v6')

    # ipv4 proxy
    begin_test_do(False, proxy_ipv4, proxy_port, target_ipv4, target_port)
    begin_test_do(False, proxy_ipv4, proxy_port, target_ipv6, target_port)

    # ipv6 proxy
    begin_test_do(True, proxy_ipv6, proxy_port, target_ipv4, target_port)
    begin_test_do(True, proxy_ipv6, proxy_port, target_ipv6, target_port)


if __name__ == '__main__':
    begin_test_tcp()
    begin_test_udp()


