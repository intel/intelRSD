/*!
 * @brief Declaration of event array queue
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation.
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
 * @file rest/eventing/event_array_queue.hpp
 */
#pragma once
#include "event_array.hpp"

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <map>
#include <memory>
#include <algorithm>

namespace psme {
namespace rest {
namespace eventing {

using std::chrono::steady_clock;

/*!
 * @brief EventArrayQueue
 *
 * Contains all event arrays to be send to EventService subscribers.
 * Each event array has an associated timepoint.
 * Event arrays are sorted according to their timepoint.
 * Event array is ready and will be popped from queue when it's timepoint has passed.
 * Multiple threads may push new event arrays, but only one can consume them.
 */
class EventArrayQueue {
public:
    using Timepoint = std::chrono::steady_clock::time_point;
    ~EventArrayQueue();

    /*!
     * @brief Add the given element value with timepoint = now()
     *
     * @param value The value of the element to append
     */
    void push_back(EventArrayUPtr value) {
        std::lock_guard<std::mutex> lock{m_data_mutex};
        m_data.emplace(std::chrono::steady_clock::now(), std::move(value));

        std::lock_guard<std::mutex> lk{m_new_item_mutex};
        m_new_item = true;
        m_new_data_available.notify_all();
    }

    /*!
     * @brief Add the given element value with given timepoint
     *
     * @param value The value of the element to append
     * @param delay Delay used to create timepoint = now() + delay
     */
    void push_back(EventArrayUPtr value, Timepoint::duration delay) {
        std::lock_guard<std::mutex> lock{m_data_mutex};
        m_data.emplace(steady_clock::now() + delay, std::move(value));

        std::lock_guard<std::mutex> lk{m_new_item_mutex};
        m_new_item = true;
        m_new_data_available.notify_all();
    }

    /*!
     * @brief Add the given element value with given timepoint
     *
     * @param value The value of the element to append
     * @param timepoint Timepoint associated with value
     */
    void push_back(EventArrayUPtr value, Timepoint timepoint) {
        std::lock_guard<std::mutex> lock{m_data_mutex};
        m_data.emplace(timepoint, std::move(value));

        std::lock_guard<std::mutex> lk{m_new_item_mutex};
        m_new_item = true;
        m_new_data_available.notify_all();
    }

    /*!
     * @brief Pop ready event array from queue
     *
     * @return Event array which timepoint has passed.
     */
    EventArrayUPtr pop() {
        std::lock_guard<std::mutex> lock{m_data_mutex};
        typename Queue::iterator it = m_data.begin();
        if (it != m_data.end()) {
            auto sleep = std::chrono::steady_clock::now() - it->first;
            if (sleep <= Timepoint::duration::zero()) {
                auto ret = std::move(it->second);
                m_data.erase(it);
                return ret;
            }
        }
        return nullptr;
    }

    /*!
     * @brief Wait for and pop ready event array from queue
     *
     * @return Event array which timepoint has passed.
     */
    EventArrayUPtr wait_and_pop() {
        for (;;) {
            {
                std::unique_lock<std::mutex> lock{m_data_mutex};
                typename Queue::iterator it = m_data.begin();
                if (it != m_data.end()) {
                    lock.unlock();
                    auto sleep = it->first - std::chrono::steady_clock::now();
                    if (sleep <= Timepoint::duration::zero()) {
                        return pop_item(it);
                    } else if (false == wait(sleep)) {
                        return pop_item(it);
                    } else {
                        continue;
                    }
                }
            }

            std::unique_lock<std::mutex> lk{m_new_item_mutex};
            m_new_data_available.wait(lk, [this] { return true == m_new_item; });
            m_new_item = false;
        }
    }

    /*!
     * @brief Wait for and pop ready event array from queue
     *
     * @return Event array which timepoint has passed
     *         or nullptr if interval passed and there was no ready event.
     */
    EventArrayUPtr wait_for_and_pop(Timepoint::duration interval) {
        while (interval >= Timepoint::duration::zero()) {
            {
                std::unique_lock<std::mutex> lock(m_data_mutex);
                typename Queue::iterator it = m_data.begin();
                if (it != m_data.end()) {
                    lock.unlock();
                    auto start = steady_clock::now();
                    auto sleep = it->first - start;
                    if (sleep <= Timepoint::duration::zero()) {
                        return pop_item(it);
                    } else if (sleep <= interval) {
                        if (false == wait(sleep)) {
                            // if no new items were inserted while sleeping, pop first item in queue
                            return pop_item(it);
                        } else {
                            // a new item was inserted. Update the interval and look again at the head of the queue
                            interval -= steady_clock::now() - start;
                            continue;
                        }
                    }
                }
            }
            // if the queue is empty or the first element is more than 'interval' distant from now()
            // sleep for the whole interval or until a new item was inserted
            if (interval > Timepoint::duration::zero()) {
                auto start = steady_clock::now();

                std::unique_lock<std::mutex> lk{m_new_item_mutex};
                if (m_new_data_available.wait_for(lk, interval,
                        [this] { return (true == m_new_item); })) {
                    m_new_item = false;
                }

                interval -= steady_clock::now() - start;
            } else {
                return nullptr;
            }
        }
        return nullptr;
    }

    /*!
     * @brief Test if queue is empty
     *
     * @return Empty queue flag
     */
    bool empty() const {
        std::lock_guard<std::mutex> lock{m_data_mutex};
        return m_data.empty();
    }

    /*!
     * @brief Get queue size
     *
     * @return Queue size
     */
    size_t size() const {
        std::lock_guard<std::mutex> lock{m_data_mutex};
        return m_data.size();
    }

    /*!
     * @brief Clear queue
     */
    void clear() {
        std::lock_guard<std::mutex> lock{m_data_mutex};
        m_data.clear();
    }

private:
    using Queue = std::multimap<Timepoint, EventArrayUPtr>;

    EventArrayUPtr pop_item(typename Queue::iterator& it) {
        std::lock_guard<std::mutex> lock{m_data_mutex};
        auto ret = std::move(it->second);
        m_data.erase(it);
        return ret;
    }

    /*!
     * @brief Sleep for a given period or until a new item is inserted into the Queue
     *
     * @param interval The intended sleeping interval
     * @return true if a new item was inserted; false if the queue slept through the whole interval
     */
    bool wait(Timepoint::duration interval) {
        const std::chrono::hours MAX_SLEEP(12);
        while (interval > Timepoint::duration::zero()) {
            auto start = steady_clock::now();
            auto sleep = (interval <= MAX_SLEEP) ? interval : MAX_SLEEP;

            std::unique_lock<std::mutex> lk{m_new_item_mutex};
            if (m_new_data_available.wait_for(lk, sleep,
                    [this] { return (true == m_new_item); })) {
                m_new_item = false;
                return true;
            }

            interval -= steady_clock::now() - start;
        }
        return false;
    }

private:
    Queue m_data{};
    mutable std::mutex m_data_mutex{};

    mutable std::mutex m_new_item_mutex{};
    volatile bool m_new_item{false};
    std::condition_variable m_new_data_available{};
};

}
}
}


