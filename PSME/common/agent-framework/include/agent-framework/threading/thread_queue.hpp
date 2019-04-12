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
 *
 * @file thread_queue.hpp
 *
 * @brief Thread Queue
 * */


#pragma once
#include <list>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <chrono>

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace threading {

/*!
 * @brief Generic event queue
 */
template <typename T>
class ThreadQueue {
public:
    /*! Default constructor */
    ThreadQueue() = default;

    /*!
     * @brief Appends the given element value to the front of the container
     *
     * @param value The value of the element to append
     */
    void push_front(T value);

    /*!
     * @brief Appends the given element value to the end of the container
     *
     * @param value The value of the element to append
     */
    void push_back(T value);

    /*!
     * @brief Try retrive value from queue
     *
     * @param ret queue value store in ret object
     *
     * @return true on success
     */
    bool try_pop(T& ret);

    /*!
     * @brief Try retrive queue value as shared_ptr
     *
     * @return shared_ptr queue value
     */
    std::shared_ptr<T> try_pop();

    /*!
     * @brief Wait and pop value from event queue
     *
     * @param ret queue value store in ret object
     */
    void wait_and_pop(T& ret);

    /*!
     * @brief Wait and pop value as share_ptr from event queue
     *
     * @return queue value as shared_ptr
     */
    std::shared_ptr<T> wait_and_pop();

    /*!
     * @brief Wait given period of time [and] pop value from event queue
     *
     * @param ret queue value store in ret object
     * @param wait_time waiting time
     *
     * @return On success true, on timeout false is returned
     */
    bool
    wait_for_and_pop(T& ret, const std::chrono::milliseconds& wait_time);

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
    wait_for_and_pop(const std::chrono::milliseconds& wait_time);

    /*!
     * @brief Check queue is empty
     *
     * @return True if queue is empty, false otherwise
     */
    bool empty() const;

    /*!
     * @brief Get number of elements in the queue
     * @return number of tasks
     */
    std::size_t size() const;

private:
    mutable std::mutex m_mutex{};
    std::list<T> m_data{};
    std::condition_variable m_cv{};
};

#include "thread_queue.tpp"

}
}

