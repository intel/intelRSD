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
 * @file chassis_sensor.hpp
 * @brief ChassisSensor model interface
 * */
#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/chassis.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! ChassisSensor */
class ChassisSensor : public Resource {
public:
    explicit ChassisSensor(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::None);
    ~ChassisSensor();

    ChassisSensor(const ChassisSensor&) = default;
    ChassisSensor& operator=(const ChassisSensor&) = default;
    ChassisSensor(ChassisSensor&&) = default;
    ChassisSensor& operator=(ChassisSensor&&) = default;

    /*!
     * @brief construct an object of class ChassisSensor from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed ChassisSensor object
     */
    static ChassisSensor from_json(const json::Json& json);

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
        return ChassisSensor::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return ChassisSensor::component;
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
     * @brief Returns reading_units
     * @return Current value
     */
    const OptionalField<enums::ReadingUnits>& get_reading_units() const {
        return m_reading_units;
    }

    /*!
     * @brief Sets reading_units
     * @param[in] reading_units New value
     */
    void set_reading_units(const OptionalField<enums::ReadingUnits>& reading_units) {
        m_reading_units = reading_units;
    }

    /*!
     * @brief Returns reading
     * @return Current value
     */
    const OptionalField<std::uint32_t>& get_reading() const {
        return m_reading;
    }

    /*!
     * @brief Sets reading
     * @param[in] reading New value
     */
    void set_reading(const OptionalField<std::uint32_t>& reading) {
        m_reading = reading;
    }

    /*!
     * @brief Returns sensor_number
     * @return Current value
     */
    const OptionalField<std::uint32_t>& get_sensor_number() const {
        return m_sensor_number;
    }

    /*!
     * @brief Sets sensor_number
     * @param[in] sensor_number New value
     */
    void set_sensor_number(const OptionalField<std::uint32_t>& sensor_number) {
        m_sensor_number = sensor_number;
    }

private:

    OptionalField<std::uint32_t> m_reading{};
    OptionalField<enums::PhysicalContext> m_physical_context{};
    OptionalField<enums::ReadingUnits> m_reading_units{};
    OptionalField<std::uint32_t> m_sensor_number{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}
