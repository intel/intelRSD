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
 * @file singleton.hpp
 * @brief Generic singleton implementation
 * */

#pragma once
#include <mutex>
#include <memory>

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace generic {

/*! Module Manager */
template <typename T>
class Singleton {
public:
    virtual ~Singleton() {}

    template <typename... Args>
    static T* get_instance(Args&&... args) {
        std::call_once(m_once_flag, [](Args&&... arg) {
                m_instance.reset(new T{std::forward<Args>(arg)...});
        }, std::forward<Args>(args)...);
        return m_instance.get();
    }
protected:
    Singleton() = default;

private:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static std::unique_ptr<T> m_instance;
    static std::once_flag m_once_flag;
};

template <typename T>
std::unique_ptr<T> Singleton<T>::m_instance{nullptr};

template <typename T>
std::once_flag Singleton<T>::m_once_flag{};


}
}

