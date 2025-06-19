
# PRISM
Q: 这个项目是做什么用的?

A: 基于shadowsocks协议的数据转存程序, 支持同步解密TLS数据(需要使用自己的根证书)

Q: 原理是什么?

A: 程序作为shadowsocks服务端运行, 接收并转发shadowsocks客户端发来的数据, 同时将数据存储到本地(会存储为wireshark可用的pcap文件)

Q: 如何使用它?

A: 在配置好shadowsocks客户端, 将服务器IP设置为程序所在机器, 两边设置同样的解密算法, 端口, 密码

Q: 项目的编译环境?

A: MacOS, Clion, Qt6


