/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @section Declaration of event queue
 * @file event_queue.hpp
 *
 * @brief Declaration of Event queue
 */
#ifndef PSME_REST_EVENTING_EVENT_QUEUE_HPP
#define PSME_REST_EVENTING_EVENT_QUEUE_HPP

#include "event.hpp"

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <map>
#include <memory>

namespace psme {
namespace rest {
namespace eventing {

using std::chrono::steady_clock;

/*!
 * @brief EventQueue
 *
 * Contains all events to be send to EventService subscribers.
 * Each event has an associated timepoint.
 * Events are sorted according to their timepoint.
 * Event is ready and will be poped from queue when it's timepoint has passed.
 * Mutliple threads may push new events, but only one can consume them.
 */
class EventQueue {
public:
    using Timepoint = std::chrono::steady_clock::time_point;
    ~EventQueue();

    /*!
     * @brief Add the given element value with timepoint = now()
     *
     * @param value The value of the element to append
     */
    void push_back(EventUPtr value) {
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
    void push_back(EventUPtr value, Timepoint::duration delay) {
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
    void push_back(EventUPtr value, Timepoint timepoint) {
        std::lock_guard<std::mutex> lock{m_data_mutex};
        m_data.emplace(timepoint, std::move(value));

        std::lock_guard<std::mutex> lk{m_new_item_mutex};
        m_new_item = true;
        m_new_data_available.notify_all();
    }

    /*!
     * @brief Pop ready event from queue
     *
     * @return Event which timepoint has passed.
     */
    EventUPtr pop() {
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
     * @brief Wait for and pop ready event from queue
     *
     * @return Event which timepoint has passed.
     */
    EventUPtr wait_and_pop() {
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
     * @brief Wait for and pop ready event from queue
     *
     * @return Event which timepoint has passed
     *         or nullptr if interval passed and there was no ready event.
     */
    EventUPtr wait_for_and_pop(Timepoint::duration interval) {
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
                            return pop_item(it);
                        } else {
                            interval -= steady_clock::now() - start;
                            continue;
                        }
                    }
                }
            }
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
    using Queue = std::multimap<Timepoint, EventUPtr>;

    EventUPtr pop_item(typename Queue::iterator& it) {
        std::lock_guard<std::mutex> lock{m_data_mutex};
	auto ret = std::move(it->second);
	m_data.erase(it);
	return ret;
    }

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

#endif	/* PSME_REST_EVENTING_EVENT_QUEUE_HPP */

