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
#ifndef SAFE_VECTOR_HPP
#define SAFE_VECTOR_HPP

#include <QVector>
#include <QMutexLocker>
#include <optional>
#include <functional>

template<typename T>
class ThreadSafeVector {
public:
    explicit ThreadSafeVector(const size_t maxSize = 0)
        : m_maxSize(maxSize) {}

    ThreadSafeVector(const ThreadSafeVector&) = delete;
    ThreadSafeVector& operator=(const ThreadSafeVector&) = delete;

    void push_back(const T& item) {
        QMutexLocker locker(&m_mutex);
        evict_if_needed();
        m_vector.append(item);
    }

    void push_back(T&& item) {
        QMutexLocker locker(&m_mutex);
        evict_if_needed();
        m_vector.append(std::move(item));
    }

    std::optional<T> pop_front() {
        QMutexLocker locker(&m_mutex);
        if (m_vector.isEmpty()) {
            return std::nullopt;
        }
        T front = std::move(m_vector.front());
        m_vector.removeFirst();
        return front;
    }

    QVector<T> snapshot() const {
        QMutexLocker locker(&m_mutex);
        return m_vector;
    }

    void for_each(const std::function<void(const T&)>& func) const {
        QMutexLocker locker(&m_mutex);
        for (const auto& item : m_vector) {
            func(item);
        }
    }

    void clear() {
        QMutexLocker locker(&m_mutex);
        m_vector.clear();
    }

    void clear_with_deleter(const std::function<void(T&)>& deleter) {
        QMutexLocker locker(&m_mutex);
        for (auto& item : m_vector) {
            deleter(item);
        }
        m_vector.clear();
    }

    bool empty() const {
        QMutexLocker locker(&m_mutex);
        return m_vector.isEmpty();
    }

    size_t size() const {
        QMutexLocker locker(&m_mutex);
        return static_cast<size_t>(m_vector.size());
    }

private:
    void evict_if_needed() {
        if (m_maxSize > 0 && m_vector.size() >= static_cast<int>(m_maxSize)) {
            m_vector.removeFirst();
        }
    }

    const size_t m_maxSize;
    mutable QMutex m_mutex;
    QVector<T> m_vector;
};

#endif //SAFE_VECTOR_HPP
