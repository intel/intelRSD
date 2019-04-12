/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @brief Declaration of generic threadsafe queue
 * */


#pragma once

#include <list>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <chrono>

/*! Generic namespace */
namespace generic {

template <typename T>
class ThreadSafeQueue {
public:
    virtual ~ThreadSafeQueue();
    /*!
     * @brief Appends the given element value to the end of the container
     *
     * @param value The value of the element to append
     */
    void push_back(T&& value) {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_data.push_back(std::move(value));
        m_cv.notify_all();
    }

    /*!
     * @brief Try retrieve value from queue
     *
     * @param ret queue value store in ret object
     *
     * @return true on success
     */
    bool try_pop(T& ret) {
        std::lock_guard<std::mutex> lock{m_mutex};
        if (m_data.empty()) {
            return false;
        }
        ret = std::move(m_data.front());
        m_data.pop_front();
        return true;
    }

    /*!
     * @brief Try retrieve queue value as shared_ptr
     *
     * @return shared_ptr queue value
     */
    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lock{m_mutex};
        if (m_data.empty()) {
           return {};
        }
        auto ret = std::make_shared<T>(std::move(m_data.front()));
        m_data.pop_front();
        return ret;
    }

    /*!
     * @brief Wait and pop value from event queue
     *
     * @param ret queue value store in ret object
     */
    void wait_and_pop(T& ret) {
        std::unique_lock<std::mutex> lock{m_mutex};
        m_cv.wait(lock, [this] { return !m_data.empty(); });
        ret = std::move(m_data.front());
        m_data.pop_front();
    }

    /*!
     * @brief Wait and pop value as share_ptr from event queue
     *
     * @return queue value as shared_ptr
     */
    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lock{m_mutex};
        m_cv.wait(lock, [this] { return !m_data.empty(); });
        auto ret = std::make_shared<T>(std::move(m_data.front()));
        m_data.pop_front();
        return ret;
    }

    /*!
     * @brief Wait given period of time [and] pop value from event queue
     *
     * @param ret queue value store in ret object
     * @param wait_time waiting time
     *
     * @return On success true, on timeout false is returned
     */
    bool
    wait_for_and_pop(T& ret, const std::chrono::milliseconds& wait_time) {
        std::unique_lock<std::mutex> lock{m_mutex};

        if (m_cv.wait_for(lock, wait_time, [this] { return !m_data.empty(); })) {
             ret = std::move(m_data.front());
             m_data.pop_front();
             return true;
        }

        return false;
    }

    /*!
     * @brief Wait given period of time [and] pop value as shared_ptr
     * from event queue
     *
     * @param wait_time waiting time
     *
     * @return On success shared_ptr with queue value, on timeout nullptr
     * is returned
     */
    std::shared_ptr<T>
    wait_for_and_pop(const std::chrono::milliseconds& wait_time) {
        std::unique_lock<std::mutex> lock{m_mutex};
        if (m_cv.wait_for(lock, wait_time, [this] { return !m_data.empty(); })) {
            auto ret = std::make_shared<T>(std::move(m_data.front()));
            m_data.pop_front();
            return ret;
        }
        return {};
    }

    void clear() {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_data.clear();
    }

private:
    mutable std::mutex m_mutex{};
    std::list<T> m_data{};
    std::condition_variable m_cv{};
};

template <typename T>
ThreadSafeQueue<T>::~ThreadSafeQueue() {}

}

