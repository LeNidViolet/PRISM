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
#ifndef SAFE_SET_H
#define SAFE_SET_H

#include <QSet>
#include <QMutexLocker>
#include <vector>
#include <functional>

template<typename K>
class ThreadSafeSet {
public:
    ThreadSafeSet() = default;

    ThreadSafeSet(const ThreadSafeSet&) = delete;
    ThreadSafeSet& operator=(const ThreadSafeSet&) = delete;

    void insert(const K& key) {
        QMutexLocker lock(&mutex_);
        set_.insert(key);
    }

    void remove(const K& key) {
        QMutexLocker lock(&mutex_);
        set_.remove(key);
    }

    void remove_with_deleter(const K& key, const std::function<void(const K&)>& deleter) {
        QMutexLocker lock(&mutex_);
        if (set_.contains(key)) {
            deleter(key);
            set_.remove(key);
        }
    }

    bool contains(const K& key) const {
        QMutexLocker lock(&mutex_);
        return set_.contains(key);
    }

    void clear() {
        QMutexLocker lock(&mutex_);
        set_.clear();
    }

    void clear_with_deleter(const std::function<void(const K&)>& deleter) {
        QMutexLocker lock(&mutex_);
        for (const auto& val : std::as_const(set_)) {
            deleter(val);
        }
        set_.clear();
    }

    size_t size() const {
        QMutexLocker lock(&mutex_);
        return static_cast<size_t>(set_.size());
    }

    std::vector<K> values() const {
        QMutexLocker lock(&mutex_);
        return std::vector<K>(set_.begin(), set_.end());
    }

    void for_each(const std::function<void(const K&)>& func) const {
        QMutexLocker lock(&mutex_);
        for (const auto& val : std::as_const(set_)) {
            func(val);
        }
    }

private:
    mutable QMutex mutex_;
    QSet<K> set_;
};

#endif //SAFE_SET_H
