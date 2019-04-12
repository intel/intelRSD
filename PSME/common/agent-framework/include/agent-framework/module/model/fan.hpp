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
 * @file fan.hpp
 * @brief Fan model interface
 * */
#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/chassis.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! Fan */
class Fan : public Resource {
public:
    explicit Fan(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::None);
    ~Fan();

    Fan(const Fan&) = default;
    Fan& operator=(const Fan&) = default;
    Fan(Fan&&) = default;
    Fan& operator=(Fan&&) = default;

    /*!
     * @brief construct an object of class Fan from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Fan object
     */
    static Fan from_json(const json::Json& json);

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
        return Fan::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return Fan::component;
    }

    /*!
     * @brief Returns physical_context
     * @return Current value
     */
    const OptionalField<enums::PhysicalContext>& get_physical_context() const {
        return m_physical_context;
    }

    /*!
     * @brief Sets physical_context
     * @param[in] physical_context New value
     */
    void set_physical_context(const OptionalField<enums::PhysicalContext>& physical_context) {
        m_physical_context = physical_context;
    }

    /*!
     * @brief return current_speed of type std::uint32_t
     *
     * @return the current_speed value
     */
    OptionalField<std::uint32_t> get_current_speed() const {
        return m_current_speed;
    }

    /*!
     * @brief setter for current_speed attribute
     *
     * @param current_speed of type uint32_t
     */
    void set_current_speed(const OptionalField<uint32_t> current_speed) {
        m_current_speed = current_speed;
    }

    /*!
     * @brief Returns current_speed_units
     * @return Current value
     */
    const OptionalField<enums::SpeedUnits>& get_current_speed_units() const {
        return m_current_speed_units;
    }

    /*!
     * @brief Sets current_speed_units
     * @param[in] current_speed_units New value
     */
    void set_current_speed_units(const OptionalField<enums::SpeedUnits>& current_speed_units) {
        m_current_speed_units = current_speed_units;
    }

    /*!
     * @brief return an object fru_info of class attribute::FruInfo
     *
     * @return the fru_info value
     */
    const attribute::FruInfo& get_fru_info() const {
        return m_fru_info;
    }

    /*!
     * @brief setter for fru_info attribute
     *
     * @param fru_info of type attribute::FruInfo
     */
    void set_fru_info(const attribute::FruInfo& fru_info) {
        m_fru_info = fru_info;
    }

    /*!
     * @brief Gets slot id value
     * @return Slot id value
     */
    uint8_t get_slot_id() const {
        return m_slot_id;
    }

    /*!
     * @brief Sets slot id value
     * @param slot_id new slot id value
     */
    void set_slot_id(const uint8_t slot_id) {
        m_slot_id = slot_id;
    }

private:

    // Telemetry: The following 3 fields are to be removed
    OptionalField<std::uint32_t> m_current_speed{};
    OptionalField<enums::SpeedUnits> m_current_speed_units{};
    OptionalField<enums::PhysicalContext> m_physical_context{};

    attribute::FruInfo m_fru_info{};

    // agent only data
    uint8_t m_slot_id{};    // physical slot id

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}
