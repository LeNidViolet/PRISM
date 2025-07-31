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
#ifndef PRISM_UI_CONFIG_HPP
#define PRISM_UI_CONFIG_HPP

#include <QString>

class ConfigVars {

public:
    ConfigVars(const ConfigVars&) = delete;
    ConfigVars& operator=(const ConfigVars&) = delete;


    // Get the singleton instance
    static ConfigVars& instance() {
        // Guaranteed thread-safe in C++11 and later
        static auto *instance = new ConfigVars;
        return *instance;
    }

    QString crtFile{};
    QString keyFile{};
    QString pktFile{};
    QString hostFile{};

    unsigned short listenPort{0};
    unsigned int timeout{0};
    QString method{};
    QString password{};

    bool runAsSocks5{true};

private:
    ConfigVars() = default;
    ~ConfigVars() = default;
};



#endif //PRISM_UI_CONFIG_HPP
