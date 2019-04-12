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
 * @file power_zone.hpp
 * @brief Power zone
 * */

#pragma once



#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/enum/common.hpp"



namespace agent_framework {
namespace model {

/*! Chassis zone class */
class PowerZone : public Resource {
public:

    explicit PowerZone(const std::string& parent_uuid = {},
                       enums::Component parent_type = enums::Component::None);


    ~PowerZone();


    PowerZone(const PowerZone&) = default;


    PowerZone& operator=(const PowerZone&) = default;


    PowerZone(PowerZone&&) = default;


    PowerZone& operator=(PowerZone&&) = default;


    /*!
     * @brief construct an object of class PowerZone from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed PowerZone object
     */
    static PowerZone from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return PowerZone::collection_name;
    }


    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return PowerZone::component;
    }


    /*!
     * @brief Returns consumed power in Watts
     * @return The consumed power value [W]
     * */
    OptionalField<std::int32_t> get_power_consumed_watts() const {
        return m_power_consumed_watts;
    }


    /*!
     * @brief Setter for consumed power in Watts
     * @param power_consumed Consumed power [W]
     * */
    void set_power_consumed_watts(const OptionalField<std::int32_t> power_consumed) {
        m_power_consumed_watts = power_consumed;
    }


    /*!
     * @brief Returns requested power in Watts
     * @return The requested power value [W]
     * */
    OptionalField<std::int32_t> get_power_requested_watts() const {
        return m_power_requested_watts;
    }


    /*!
     * @brief Setter for requested power in Watts
     * @param power_requested Requested power [W]
     * */
    void set_power_requested_watts(const OptionalField<std::int32_t> power_requested) {
        m_power_requested_watts = power_requested;
    }


    /*!
     * @brief Returns available power in Watts
     * @return The available power value [W]
     * */
    OptionalField<std::int32_t> get_power_available_watts() const {
        return m_power_available_watts;
    }


    /*!
     * @brief Setter for available power in Watts
     * @param power_available Available power [W]
     * */
    void set_power_available_watts(const OptionalField<std::int32_t> power_available) {
        m_power_available_watts = power_available;
    }


    /*!
     * @brief Returns power capacity in Watts
     * @return The power capacity value [W]
     * */
    OptionalField<std::int32_t> get_power_capacity_watts() const {
        return m_power_capacity_watts;
    }


    /*!
     * @brief Setter for power capacity in Watts
     * @param power_capacity Power capacity [W]
     * */
    void set_power_capacity_watts(const OptionalField<std::int32_t> power_capacity) {
        m_power_capacity_watts = power_capacity;
    }


    /*!
     * @brief Returns allocated power in Watts
     * @return The allocated power value [W]
     * */
    OptionalField<std::int32_t> get_power_allocated_watts() const {
        return m_power_allocated_watts;
    }


    /*!
     * @brief Setter for allocated power in Watts
     * @param power_allocated Allocated power [W]
     * */
    void set_power_allocated_watts(const OptionalField<std::int32_t> power_allocated) {
        m_power_allocated_watts = power_allocated;
    }


private:

    // Telemetry: The following field is to be removed
    OptionalField<std::int32_t> m_power_consumed_watts{};

    OptionalField<std::int32_t> m_power_requested_watts{};
    OptionalField<std::int32_t> m_power_available_watts{};
    OptionalField<std::int32_t> m_power_capacity_watts{};
    OptionalField<std::int32_t> m_power_allocated_watts{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;
};

}
}
