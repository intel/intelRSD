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
 * @file capacity_source.hpp
 * @brief Capacity Source
 * */

#pragma once



#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"
#include "agent-framework/module/model/attributes/model_attributes.hpp"



namespace agent_framework {
namespace model {
namespace attribute {

/*! CapacitySource class */
class CapacitySource : public Capacity {
public:

    /*! Default constructor */
    explicit CapacitySource() = default;


    CapacitySource(CapacitySource&&) = default;


    CapacitySource(const CapacitySource&) = default;


    CapacitySource& operator=(CapacitySource&&) = default;


    CapacitySource& operator=(const CapacitySource&) = default;


    virtual ~CapacitySource();


    /*!
     * @brief Get providing drives UUIDs from request
     * @return UUIDs collection
     * */
    const attribute::Array<Uuid>& get_providing_drives() const {
        return m_providing_drives;
    }


    /*!
     * @brief Set providing drives UUIDs array
     * @param[in] providing_drives Providing drives UUIDs array
     */
    void set_providing_drives(const attribute::Array<Uuid>& providing_drives) {
        m_providing_drives = providing_drives;
    }


    /*!
     * @brief Add a providing drive UUID to the array
     * @param[in] providing_drive providing drive UUID
     */
    void add_providing_drive(const Uuid& providing_drive) {
        m_providing_drives.add_entry(providing_drive);
    }


    /*!
     * @brief Get providing volumes UUIDs from request
     * @return UUIDs collection
     * */
    const attribute::Array<Uuid>& get_providing_volumes() const {
        return m_providing_volumes;
    }


    /*!
     * @brief Set providing volumes UUIDs array
     * @param[in] providing_volumes Providing volumes UUIDs array
     */
    void set_providing_volumes(const attribute::Array<Uuid>& providing_volumes) {
        m_providing_volumes = providing_volumes;
    }


    /*!
     * @brief Add a providing volume UUID to the array
     * @param[in] providing_volume providing volume UUID
     */
    void add_providing_volume(const Uuid& providing_volume) {
        m_providing_volumes.add_entry(providing_volume);
    }


    /*!
     * @brief Get providing pools UUIDs from request
     * @return UUIDs collection
     * */
    const attribute::Array<Uuid>& get_providing_pools() const {
        return m_providing_pools;
    }


    /*!
     * @brief Set providing pools UUIDs array
     * @param[in] providing_pools Providing pools UUIDs array
     */
    void set_providing_pools(const attribute::Array<Uuid>& providing_pools) {
        m_providing_pools = providing_pools;
    }


    /*!
     * @brief Add a providing pool UUID to the array
     * @param[in] providing_pool providing pool UUID
     */
    void add_providing_pool(const Uuid& providing_pool) {
        m_providing_pools.add_entry(providing_pool);
    }


    /*!
     * @brief Converts this object to json representation
     * @return Json representation of this object
     * */
    json::Json to_json() const;


    /*!
     * @brief construct an object of class CapacitySource from JSON
     * @param json the json::Json deserialized to object
     * @return the newly constructed CapacitySource object
     */
    static CapacitySource from_json(const json::Json& json);


private:
    Array<Uuid> m_providing_drives{};
    Array<Uuid> m_providing_volumes{};
    Array<Uuid> m_providing_pools{};
};

}
}
}
