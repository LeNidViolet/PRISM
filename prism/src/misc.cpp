/**
 *  Copyright 2022, LeNidViolet.
 *  Created by LeNidViolet on 2022/8/24.
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
#include "misc.h"
#include <QApplication>
#include <QDir>
#include <QFile>

QString MiscFuncs::formatBytes(unsigned int bytes) {

    constexpr auto KB = 1024;
    constexpr auto MB = 1024 * 1024;
    constexpr auto GB = 1024 * 1024 * 1024;

    const auto dbbytes = static_cast<double>(bytes);

    QString result;

    if ( dbbytes >= GB ) {
        auto g = dbbytes / GB;
        result = QString::asprintf("%.2f GB", g);
    } else if ( dbbytes >= MB ) {
        auto m = dbbytes / MB;
        result = QString::asprintf("%.2f MB", m);
    } else if ( dbbytes >= KB ) {
        auto k = dbbytes / KB;
        result = QString::asprintf("%.2f KB", k);
    } else {
        result = QStringLiteral("%1 B").arg(dbbytes);
    }
    return result;
}

QString MiscFuncs::genFlowKey(const bool isStream, const int index) {

    return QStringLiteral("%1[%2]").arg(isStream ? "TCP" : "UDP").arg(index);
}

QString MiscFuncs::getExecutableRootPath() {
#ifdef Q_OS_MAC
    // macOS: 回退到 .app 所在的目录
    const QString path = QApplication::applicationDirPath();
    QDir dir(path);
    // Contents/MacOS -> Contents -> .app 根目录
    if (dir.cdUp()) {
        dir.cdUp();
        dir.cdUp();
    }
    return dir.absolutePath();
#else
    // Windows/Linux: applicationDirPath() 就是 exe 所在目录
    return QApplication::applicationDirPath();
#endif
}

