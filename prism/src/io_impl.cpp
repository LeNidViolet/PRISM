/**
 *  Copyright 2022, raprepo.
 *  Created by raprepo on 2022/9/1.
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
#include "io_impl.h"
#include <QFile>
#include <QTextStream>


IoImpl *IoImpl::minstance = nullptr;

IoImpl *IoImpl::instance() {
    static QMutex mutex;
    if ( !minstance ) {
        QMutexLocker locker(&mutex);
        if ( !minstance )
            minstance = new IoImpl;
    }

    return minstance;
}

IoImpl::IoImpl() : QObject(nullptr) {

}

__attribute__((unused)) void IoImpl::writeHostsOut(const QString &filePath, const QString &fileContent) {

    this->instance();
    QFile file(filePath);
    if ( file.open(QIODevice::WriteOnly | QIODevice::Text) ) {
        QTextStream out(&file);
        out << fileContent;
    }
}

__attribute__((unused)) void IoImpl::writePktsOut(const QString &filePath, const QByteArray &fileContent) {

    this->instance();
    QFile file(filePath);
    if ( file.open(QIODevice::Append) ) {
        file.write(fileContent);
    }
}
