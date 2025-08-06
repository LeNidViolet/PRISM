/**
 *  Copyright 2022, LeNidViolet.
 *  Created by LeNidViolet on 2022/8/31.
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
#include "http_server.h"
#include <QFile>
#include <QDateTime>


HttpServer::HttpServer(
    const QHostAddress &address,
    const quint16 port,
    const QString &filePath,
    const QString &reqPath,
    const QString &respFileName,
    QObject *parent)
    : QTcpServer(parent) {

    this->m_filePath = filePath;
    this->m_reqPath = reqPath;
    this->m_respFileName = respFileName;

    if ( this->m_reqPath[0] != QChar('/') )
        this->m_reqPath = QStringLiteral("/") + this->m_reqPath;

    this->listen(address, port);
}

// ReSharper disable once CppParameterMayBeConst
void HttpServer::incomingConnection(qintptr handle) {

    // When a new client connects, the server constructs a QTcpSocket and all
    // communication with the client is done over this QTcpSocket. QTcpSocket
    // works asynchronously, this means that all the communication is done
    // in the two slots readClient() and discardClient().
    // ReSharper disable once CppDFAMemoryLeak
    const auto s = new QTcpSocket(this);
    QObject::connect(s, &QTcpSocket::readyRead, this, &HttpServer::readClient);
    QObject::connect(s, &QTcpSocket::disconnected, this, &HttpServer::discardClient);

    s->setSocketDescriptor(handle);
}

void HttpServer::discardClient() const {

    const auto socket = dynamic_cast<QTcpSocket *>(sender());
    socket->deleteLater();
}

void HttpServer::readClient() const {

    // This slot is called when the client sent data to the server
    const auto socket = dynamic_cast<QTcpSocket *>(sender());
    if ( socket->canReadLine() ) {
        const auto bytes = socket->readAll();
        const auto request = QString(bytes);
        auto lines = request.split(QStringLiteral("\r\n"));
        if ( lines.count() > 3 ) {
            auto parts = lines[0].split(QStringLiteral(" "));
            if ( parts.count() == 3 ) {
                if ( parts[0] == QStringLiteral("GET") && parts[1] == this->m_reqPath ) {
                    QFile file(this->m_filePath);
                    if ( file.open(QIODevice::ReadOnly) ) {
                        const auto filecontent = file.readAll();

                        QTextStream response(socket);
                        response << "HTTP/1.0 200 Ok\r\n";
                        response << QStringLiteral("Content-Disposition: attachment; filename=%1\r\n").arg(this->m_respFileName);
                        response << "Content-Type: application/octet-stream\r\n";
                        response << QStringLiteral("Content-Length: %1\r\n").arg(filecontent.size());
                        response << "\r\n";
                        response << filecontent;

                        socket->close();
                        if ( socket->state() == QTcpSocket::UnconnectedState ) {
                            delete socket;
                        }
                    }
                }
            }
        }
    }
}



