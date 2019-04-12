/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file capacity.hpp
 * @brief Capacity
 * */

#pragma once



#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <string>



namespace agent_framework {
namespace model {
namespace attribute {

/*! Capacity class */
class Capacity {
public:

    /*! Default constructor */
    explicit Capacity() {}


    Capacity(Capacity&&) = default;


    Capacity(const Capacity&) = default;


    Capacity& operator=(Capacity&&) = default;


    Capacity& operator=(const Capacity&) = default;


    virtual ~Capacity();


    /*!
     * @brief Gets consumed bytes
     * @return consumed bytes
     * */
    const OptionalField<std::int64_t>& get_consumed_bytes() const {
        return m_consumed_bytes;
    }


    /*!
     * @brief Sets consumed bytes
     * @param[in] consumed_bytes of type uint64_t
     */
    void set_consumed_bytes(const OptionalField<std::int64_t>& consumed_bytes) {
        m_consumed_bytes = consumed_bytes;
    }


    /*!
     * @brief Gets allocated bytes
     * @return allocated bytes
     * */
    const OptionalField<std::int64_t>& get_allocated_bytes() const {
        return m_allocated_bytes;
    }


    /*!
     * @brief Sets allocated bytes
     * @param[in] allocated_bytes of type uint64_t
     */
    void set_allocated_bytes(const OptionalField<std::int64_t>& allocated_bytes) {
        m_allocated_bytes = allocated_bytes;
    }


    /*!
     * @brief Gets guaranteed bytes
     * @return guaranteed bytes
     * */
    const OptionalField<std::int64_t>& get_guaranteed_bytes() const {
        return m_guaranteed_bytes;
    }


    /*!
     * @brief Sets guaranteed bytes
     * @param[in] guaranteed_bytes of type uint64_t
     */
    void set_guaranteed_bytes(const OptionalField<std::int64_t>& guaranteed_bytes) {
        m_guaranteed_bytes = guaranteed_bytes;
    }


    /*!
     * @brief Gets provisioned bytes
     * @return provisioned bytes
     * */
    const OptionalField<std::int64_t>& get_provisioned_bytes() const {
        return m_provisioned_bytes;
    }


    /*!
     * @brief Sets provisioned bytes
     * @param[in] provisioned_bytes of type uint64_t
     */
    void set_provisioned_bytes(const OptionalField<std::int64_t>& provisioned_bytes) {
        m_provisioned_bytes = provisioned_bytes;
    }


    /*!
     * @brief Indicates that capacity is thin provisioned.
     * @return True if is thin provisioned, false otherwise.
     */
    const OptionalField<bool> is_thin_provisioned() const {
        return m_is_thin_provisioned;
    }


    /*!
     * @brief Set flag for thin provisioning of this capacity.
     * @param is_thin_provisioned Indicates that capacity is thin provisioned.
     */
    void set_thin_provisioned(const OptionalField<bool>& is_thin_provisioned) {
        m_is_thin_provisioned = is_thin_provisioned;
    }


    /*!
     * @brief Converts this object to json representation
     * @return Json representation of this object
     * */
    json::Json to_json() const;


    /*!
     * @brief construct an object of class Capacity from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed Capacity object
     */
    static Capacity from_json(const json::Json& json);


private:
    OptionalField<std::int64_t> m_consumed_bytes{};
    OptionalField<std::int64_t> m_allocated_bytes{};
    OptionalField<std::int64_t> m_guaranteed_bytes{};
    OptionalField<std::int64_t> m_provisioned_bytes{};
    OptionalField<bool> m_is_thin_provisioned{};
};

}
}
}
