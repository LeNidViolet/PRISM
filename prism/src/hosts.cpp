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

#include "hosts.h"
#include <QFile>
#include <QTextStream>

void HostsDumper::setHostsPath(const QString &path, const bool load) {
    this->m_hostsPath = path;
    if (load) {
        QFile hostFile(path);
        if ( hostFile.open(QIODevice::ReadOnly) ) {

            QTextStream in(&hostFile);
            while ( !in.atEnd() ) {
                auto line = in.readLine();
                line = line.trimmed();
                auto parts = line.split(QStringLiteral(" "));
                if ( parts.size() >= 2 ) {
                    auto address = QHostAddress(parts[0]);
                    parts.removeFirst();
                    auto domains = parts;
                    this->addHostsNode(address, domains);
                }
            }
            hostFile.close();
        }
    }
}

void HostsDumper::addHostsNode(QHostAddress &address, QStringList &domains) {
    if ( address.isNull() ) return;
    if ( domains.empty() ) return;

    // 过滤域名为IP的条目
    while ( true ) {
        bool breakout = true;
        for ( auto& domain : domains  ) {
            if ( !QHostAddress(domain).isNull() ) {
                domains.removeAll(domain);
                breakout = false;
                break;
            }
        }
        if ( breakout ) break;
    }
    if ( domains.empty() ) return;

    if ( this->m_hostsMap.contains(address.toString()) ) {
        this->m_hostsMap.update(address.toString(), [domains, this](const QSharedPointer<HOSTS_NODE_> &node) {
            // 添加可能存在的新域名
            bool append = false;
            for ( auto& domain : domains  ) {
                if ( !node->domains.contains(domain) ) {
                    node->domains.append(domain);
                    append = true;
                }
            }
            if ( append ) {
                this->m_dirty = true;
            }
        });
    } else {

        // 添加新HOSTS条目
        const auto node = QSharedPointer<HOSTS_NODE>::create(address, domains);
        this->m_hostsMap.set(address.toString(), node);
        this->m_dirty = true;
    }
}

bool HostsDumper::writeHostsOut(const bool force) {
    if (this->m_dirty == false && force == false) {
        return true;
    }
    if (this->m_hostsPath.isEmpty()) {
        return false;
    }

    // FORMAT HOSTS LINES
    QString lines;
    this->m_hostsMap.forEach([&lines](const QString& key, const QSharedPointer<HOSTS_NODE>& value) {
        auto line = value->address.toString();
        line += " " + value->domains.join(QStringLiteral(" "));
        lines += line + "\r\n";
    });

    QFile file(this->m_hostsPath);
    if ( file.open(QIODevice::WriteOnly | QIODevice::Text) ) {
        QTextStream out(&file);
        out << lines;
    }

    this->m_dirty = false;
    return true;
}

void HostsDumper::clear() {
    this->m_dirty = false;
    this->m_hostsMap.clear();
}

QVector<QSharedPointer<HOSTS_NODE>> HostsDumper::all() const {
    return this->m_hostsMap.values();
}
