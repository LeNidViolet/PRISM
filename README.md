
# PRISM

PRISM is a SOCKS5/Shadowsocks proxy service built on libuv, designed for traffic inspection and TLS decryption.

It utilizes mbedTLS to decrypt TLS-encrypted traffic in real-time. All proxied data is saved to a specified PCAP file, which can be opened with Wireshark for detailed traffic analysis.
Decrypted TLS data will be stored in plaintext within the PCAP file.

⸻

### Supported Protocols
1. [x] TCP (IPv4 & IPv6)
2. [x] UDP (IPv4 & IPv6)

⸻

### Supported Operating Systems
1. [x] macOS
2. [x] Windows(x64 & arm64)

⸻

### How to Build

#### Prerequisites

Install the following tools depending on your platform:

macOS:
* Ninja
* CMake (version 3.25 or higher)
* Qt (version 6 or higher)

Windows:
* Visual Studio 2022
* CMake (3.25+)
* Qt (version 6 or higher)

#### Note: On Windows, you may need to configure the following environment variables:
* CMAKE_PREFIX_PATH = C:\Qt\6.9.1\msvc2022_xxxxx
* PATH += C:\Qt\6.9.1\msvc2022_xxxxx\bin


#### Build Steps (using Terminal/PowerShell)
Navigate to the project root directory.

`cmake --list-presets`

`cmake --preset <your-preset>`

`cmake --build --preset <your-preset>`

⸻

### Usage

To enable TLS decryption, you must install the self-signed certificate on both the proxy server and all client systems using the proxy.