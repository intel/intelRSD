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
 * @file thread_queue.tpp
 *
 * @brief Eventing Queue implementation
 * */

template <typename T>
void ThreadQueue<T>::push_front(T value) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_data.push_back(std::move(value));
    m_cv.notify_all();
}

template <typename T>
void ThreadQueue<T>::push_back(T value) {
    std::lock_guard<std::mutex> lock{m_mutex};
    m_data.push_back(std::move(value));
    m_cv.notify_all();
}

template <typename T>
bool ThreadQueue<T>::try_pop(T& ret) {
    std::lock_guard<std::mutex> lock{m_mutex};
    if (m_data.empty()) {
        return false;
    }
    ret = std::move(m_data.front());
    m_data.pop_front();
    return true;
}

template <typename T>
std::shared_ptr<T> ThreadQueue<T>::try_pop() {
    std::lock_guard<std::mutex> lock{m_mutex};
    if (m_data.empty()) {
       return {};
    }
    auto ret = std::make_shared<T>(std::move(m_data.front()));
    m_data.pop_front();
    return ret;
}

template <typename T>
void ThreadQueue<T>::wait_and_pop(T& ret) {
    std::unique_lock<std::mutex> lock{m_mutex};
    m_cv.wait(lock, [this] { return !m_data.empty(); });
    ret = std::move(m_data.front());
    m_data.pop_front();
}

template <typename T>
std::shared_ptr<T> ThreadQueue<T>::wait_and_pop() {
    std::unique_lock<std::mutex> lock{m_mutex};
    m_cv.wait(lock, [this] { return !m_data.empty(); });
    auto ret = std::make_shared<T>(std::move(m_data.front()));
    m_data.pop_front();
    return ret;
}

template <typename T>
bool ThreadQueue<T>::wait_for_and_pop(T& ret,
                                const std::chrono::milliseconds& wait_time) {
    std::unique_lock<std::mutex> lock{m_mutex};

    if (m_cv.wait_for(lock, wait_time, [this] { return !m_data.empty(); })) {
         ret = std::move(m_data.front());
         m_data.pop_front();
    }

    return false;
}

template <typename T>
std::shared_ptr<T> ThreadQueue<T>::wait_for_and_pop(
                                const std::chrono::milliseconds& wait_time) {
    std::unique_lock<std::mutex> lock{m_mutex};
    if (m_cv.wait_for(lock, wait_time, [this] { return !m_data.empty(); })) {
        auto ret = std::make_shared<T>(std::move(m_data.front()));
        m_data.pop_front();
        return ret;
    }
    return {};
}

template <typename T>
bool ThreadQueue<T>::empty() const {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_data.empty();
}

template <typename T>
std::size_t ThreadQueue<T>::size() const {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_data.size();
}
