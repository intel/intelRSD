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
 * @file obj_reference.hpp
 * @brief Object Reference class implementation
 * */

#pragma once

#include <mutex>
#include <type_traits>

/*! Psme namespace */
namespace agent_framework {
namespace generic {

/*! Object Reference class */
template <class T, class Mutex>
class ObjReference {
public:
    /*! @brief Default constructor */
    ObjReference() = delete;

    /*! @brief Copy constructors */
    ObjReference(ObjReference& other) : m_mutex(other.m_mutex), m_data(other.m_data) {
        static_assert(
                std::is_base_of<std::recursive_mutex, Mutex>::value,
                "Works only with recursive mutex."
        );
        m_mutex.lock();
    }
    ObjReference(const ObjReference&) = delete;

    /*! @brief Copy assignments */
    ObjReference& operator=(ObjReference&) = delete;
    ObjReference& operator=(const ObjReference&) = delete;

    /*! @brief Move constructors */
    ObjReference(ObjReference&&) = default;
    ObjReference(const ObjReference&&) = delete;

    /*! @brief Move assignments */
    ObjReference& operator=(ObjReference&&) = delete;
    ObjReference& operator=(const ObjReference&&) = delete;

    /*!
     * @brief Default constructor
     * @param[in] data Data reference
     * @param[in] mutex Mutex data reference
     */
    ObjReference(T& data, Mutex& mutex) : m_mutex{mutex}, m_data{data} {
        m_mutex.lock();
    }

    /*!
     * @brief Get data pointer
     * @return Data pointer
     */
    T* operator->() {
        return &m_data;
    }

    /*!
     * @brief Get const data reference
     * @return const data reference
     */
    const T& operator*() const {
        return m_data;
    }

    /*!
     * @brief Get data reference
     *
     * This might be used to pass object wrapped by ObjReference to another function.
     * As an alternative programmers used to pass ObjReference&, which does not make sense,
     * since ObjReference does not have reference counting that could protect us from
     * doing manipulations on ObjReference that was already released.
     *
     * @return data reference
     */
    T& get_raw_ref() {
        return m_data;
    }

    /*! @brief Default destructor */
    virtual ~ObjReference() final {
        m_mutex.unlock();
    }

private:
    Mutex& m_mutex;
    T& m_data;
};

}
}

