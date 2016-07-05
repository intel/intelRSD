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
 *
 * @file obj_reference.hpp
 * @brief Object Reference class implementation
 * */

#ifndef AGENT_FRAMEWORK_GENERIC_OBJ_REFERENCE_HPP
#define AGENT_FRAMEWORK_GENERIC_OBJ_REFERENCE_HPP

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
    ObjReference(ObjReference&) = delete;
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
     * @return data reference
     */
    const T& operator*() const {
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

#endif /* AGENT_FRAMEWORK_GENERIC_OBJ_REFERENCE_HPP */
