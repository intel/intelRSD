/*!
 * @brief DelayQueue implementation.
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file generic/delay_queue.hpp
 */

#pragma once

#include <chrono>
#include <condition_variable>
#include <queue>
#include <mutex>

namespace generic {

/*!
 * A BlockingQueue of delayed elements.
 * Elements should implement get_delay() method returning remaining delay value.
 */
template<typename E>
class DelayQueue {
public:
    using Timepoint = std::chrono::steady_clock::time_point;
    using Duration = Timepoint::duration;

    /**
     * Inserts the specified element in the queue.
     *
     * @param e the element to add
     */
    void push(const E& e) {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_data.empty()) {
            m_data.push(e);
            m_available.notify_all();
        }
        else {
            const auto& first = m_data.top();
            m_data.push(e);
            if (m_compare(e, first)) {
                m_available.notify_all();
            }
        }
    }

    /**
     * Inserts the specified element in the queue.
     *
     * @param e the element to add
     */
    void push(E&& e) {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_data.empty()) {
            m_data.push(std::move(e));
            m_available.notify_all();
        }
        else {
            const auto& first = m_data.top();
            m_data.push(std::move(e));
            if (m_compare(m_data.top(), first)) {
                m_available.notify_all();
            }
        }
    }

    /**
     * Inserts the specified element in the queue.
     *
     * The inserted element is constructed with E(std::forward<Args>(args)...)
     *
     * @param args arguments
     */
    template<typename... Args>
    void emplace(Args&&... args) {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_data.empty()) {
            m_data.emplace(std::forward<Args>(args)...);
            m_available.notify_all();
        }
        else {
            const auto& first = m_data.top();
            m_data.emplace(std::forward<Args>(args)...);
            if (m_compare(m_data.top(), first)) {
                m_available.notify_all();
            }
        }
    }

    /**
     * Retrieves and removes the head of this queue, waiting if necessary
     * until an element with an expired delay is available in this queue.
     *
     * @return the head of this queue
     */
    E wait_and_pop() {
        std::unique_lock<std::mutex> lk(m_mutex);
        for (;;) {
            if (m_data.empty()) {
                m_available.wait(lk);
            }
            else {
                auto& first = m_data.top();
                const auto delay = first.get_delay();
                if (delay > Duration::zero()) {
                    m_available.wait_for(lk, delay);
                }
                else {
                    // remove const to allow move
                    E ret = std::move(const_cast<E&>(first));
                    m_data.pop();
                    if (!m_data.empty()) {
                        m_available.notify_all();
                    }
                    return std::move(ret);
                }
            }
        }
    }

    /*!
     * @return Number of elements in the queue.
     */
    size_t size() {
        std::unique_lock<std::mutex> lk(m_mutex);
        return m_data.size();
    }

    /*!
     * @brief Erases all the elements from the queue.
     */
    void clear() {
        std::unique_lock<std::mutex> lk(m_mutex);
        while(!m_data.empty()) {
            m_data.pop();
        };
    }

private:

    std::greater<E> m_compare{};
    std::priority_queue<E, std::vector<E>, std::greater<E>> m_data{};
    mutable std::mutex m_mutex{};
    std::condition_variable m_available{};
};

}
