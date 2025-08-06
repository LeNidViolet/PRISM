/**
 *  Copyright 2025, Song,Tao.
 *  Created by Song,Tao on 2025/07/10.
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
#ifndef LOGGING_H
#define LOGGING_H


#include "safe_queue.hpp"
// Define the log record structure

enum LOG_LEVEL {
    LOG_ERROR   = 0,
    LOG_WARN    = 1,
    LOG_KEY     = 2,
    LOG_INFO    = 3,
    LOG_DEBUG   = 4,
};

inline const QMap<LOG_LEVEL, QString> LogLevelNames = {
    { LOG_ERROR, "ERROR" },
    { LOG_WARN,  "WARN" },
    { LOG_KEY,   "KEY" },
    { LOG_INFO,  "INFO" },
    { LOG_DEBUG, "DEBUG" }
};


typedef struct LOG_NODE_ {
    LOG_LEVEL                   level;                  // Log level
    QString                     message;                // Log message
} LOG_NODE;

class Logging {
public:
    Logging(const Logging&) = delete;
    Logging& operator=(const Logging&) = delete;

    // Get the singleton instance
    static Logging& instance() {
        // Guaranteed thread-safe in C++11 and later
        static auto *instance = new Logging;
        return *instance;
    }

    // Add a log entry to the queue
    void push(const LOG_LEVEL Level, const QString& Message) {
        LOG_NODE logLine{Level, Message};
        logQueue.push(std::move(logLine));
    }

    void push(const LOG_LEVEL Level, QString&& Message) {
        LOG_NODE logLine{Level, std::move(Message)};
        logQueue.push(std::move(logLine));
    }

    void push(const LOG_LEVEL Level, const char *Format, ...) {
        va_list ap;
        char msg[1024];

        va_start(ap, Format);
        vsnprintf(msg, sizeof(msg), Format, ap);
        va_end(ap);

        LOG_NODE logLine{Level, QString::fromUtf8(msg)};
        logQueue.push(std::move(logLine));
    }

    // Pop a log entry
    std::optional<LOG_NODE> pop() {
        return logQueue.pop();
    }

private:
    Logging() = default;
    ~Logging() = default;

    ThreadSafeQueue<LOG_NODE> logQueue{50};       // Thread-safe log queue
};


#define LOGGING_PUSH    Logging::instance().push
#define LOGGING_POP     Logging::instance().pop


#endif //LOGGING_H
