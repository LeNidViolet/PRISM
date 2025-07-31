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
#ifndef SAFE_MAP_HPP
#define SAFE_MAP_HPP

#include <QMap>
#include <QMutexLocker>
#include <QVector>
#include <functional>
#include <optional>

template<typename K, typename V>
class ThreadSafeMap {
public:
    ThreadSafeMap() = default;

    ThreadSafeMap(const ThreadSafeMap&) = delete;
    ThreadSafeMap& operator=(const ThreadSafeMap&) = delete;

    // 插入或更新
    void set(const K& key, const V& value) {
        QMutexLocker locker(&mutex_);
        map_[key] = value;
    }

    // 获取指定键的值
    std::optional<V> get(const K& key) const {
        QMutexLocker locker(&mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            return it.value();
        }
        return std::nullopt;
    }

    // 使用用户提供的函数更新键值
    void update(const K& key, const std::function<void(V&)>& updater) {
        QMutexLocker locker(&mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            updater(it.value());
        }
    }

    // 删除某个键
    void remove(const K& key) {
        QMutexLocker locker(&mutex_);
        map_.remove(key);
    }

    void removeWithDeleter(const K& key, const std::function<void(V&)>& deleter) {
        QMutexLocker locker(&mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            deleter(it.value());
            map_.erase(it);
        }
    }

    // 是否包含键
    bool contains(const K& key) const {
        QMutexLocker locker(&mutex_);
        return map_.contains(key);
    }

    // 当前大小
    int size() const {
        QMutexLocker locker(&mutex_);
        return map_.size();
    }

    // 清除所有元素
    void clear() {
        QMutexLocker locker(&mutex_);
        map_.clear();
    }

    void clearWithDeleter(const std::function<void(const K&, const V&)>& deleter) {
        QMutexLocker locker(&mutex_);
        for (auto it = map_.cbegin(); it != map_.cend(); ++it) {
            deleter(it.key(), it.value());
        }
        map_.clear();
    }

    // 获取所有值
    QVector<V> values() const {
        QMutexLocker locker(&mutex_);
        return map_.values().toVector();
    }

    // 获取快照副本
    QMap<K, V> snapshot() const {
        QMutexLocker locker(&mutex_);
        return map_;
    }

    // 遍历元素（只读）
    void forEach(const std::function<void(const K&, const V&)>& func) const {
        QMutexLocker locker(&mutex_);
        for (auto it = map_.cbegin(); it != map_.cend(); ++it) {
            func(it.key(), it.value());
        }
    }

private:
    mutable QMutex mutex_;
    QMap<K, V> map_;
};

#endif //SAFE_MAP_HPP
