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
#ifndef SAFE_QUEUE_HPP
#define SAFE_QUEUE_HPP

#include <QQueue>
#include <QMutexLocker>
#include <optional>
#include <functional>

template<typename T>
class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue(const size_t maxSize = 0)
        : m_maxSize(maxSize) {}

    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    // 插入元素（拷贝）
    void push(const T& item) {
        QMutexLocker locker(&m_mutex);
        evictIfNeeded();
        m_queue.enqueue(item);
    }

    // 插入元素（移动）
    void push(T&& item) {
        QMutexLocker locker(&m_mutex);
        evictIfNeeded();
        m_queue.enqueue(std::move(item));
    }

    // 尝试弹出，返回 optional
    std::optional<T> pop() {
        QMutexLocker locker(&m_mutex);
        if (m_queue.isEmpty()) {
            return std::nullopt;
        }
        T item = std::move(m_queue.dequeue());
        return item;
    }

    size_t size() const {
        QMutexLocker locker(&m_mutex);
        return static_cast<size_t>(m_queue.size());
    }

    bool empty() const {
        QMutexLocker locker(&m_mutex);
        return m_queue.isEmpty();
    }

    void clear() {
        QMutexLocker locker(&m_mutex);
        m_queue.clear();
    }

    // 清空 + 自定义删除器
    void clear_with_deleter(const std::function<void(T&)>& deleter) {
        QMutexLocker locker(&m_mutex);
        while (!m_queue.isEmpty()) {
            T& front = m_queue.head();
            deleter(front);
            m_queue.dequeue();
        }
    }

private:
    void evictIfNeeded() {
        if (m_maxSize > 0 && m_queue.size() >= static_cast<int>(m_maxSize)) {
            m_queue.dequeue();
        }
    }

    const size_t m_maxSize;
    mutable QMutex m_mutex;
    QQueue<T> m_queue;
};

#endif //SAFE_QUEUE_HPP
