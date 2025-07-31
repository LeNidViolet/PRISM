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
#ifndef PRISM_HTTP_SERVER_H
#define PRISM_HTTP_SERVER_H

#include <QTcpServer>
#include <QTcpSocket>

class HttpServer final : public QTcpServer {

    Q_OBJECT

public:
    explicit HttpServer(
        const QHostAddress& address,
        quint16 port,
        const QString& filePath,        // 要响应的文件
        const QString& reqPath,         // 要验证的请求路径
        const QString& respFileName,    // 要在客户端呈现的文件名
        QObject* parent = nullptr
    );
    void incomingConnection(qintptr handle) override;

public slots:
    void terminal() { this->close(); this->deleteLater(); }

private slots:
    void readClient() const;
    void discardClient() const;

private:
    QString m_filePath;
    QString m_reqPath;
    QString m_respFileName;
};

#endif //PRISM_HTTP_SERVER_H
