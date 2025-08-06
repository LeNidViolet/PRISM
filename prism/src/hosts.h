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

#ifndef PRISM_UI_HOSTS_H
#define PRISM_UI_HOSTS_H

#include <QHostAddress>
#include <QSharedPointer>
#include <utility>
#include "custom/safe_map.hpp"

typedef struct HOSTS_NODE_ {
    HOSTS_NODE_(
        QHostAddress address,
        QStringList domains)
        :address(std::move(address)), domains(std::move(domains)) {}
    QHostAddress address;
    QStringList domains;
} HOSTS_NODE;



class HostsDumper {

public:
    HostsDumper(const HostsDumper&) = delete;
    HostsDumper& operator=(const HostsDumper&) = delete;

    // Get the singleton instance
    static HostsDumper& instance() {
        // Guaranteed thread-safe in C++11 and later
        static auto *instance = new HostsDumper;
        return *instance;
    }

    void setHostsPath(const QString& path, bool load=true);
    void addHostsNode(QHostAddress& address, QStringList& domains);
    bool writeHostsOut(bool force);
    void clear();

    QVector<QSharedPointer<HOSTS_NODE>> all() const;

private:
    HostsDumper() = default;
    ~HostsDumper() = default;

    bool m_dirty = false;

    // hosts 文件路径
    QString m_hostsPath{};

    // ip -> 域名列表
    ThreadSafeMap<QString, QSharedPointer<HOSTS_NODE>> m_hostsMap{};
};


#endif // PRISM_UI_HOSTS_H
