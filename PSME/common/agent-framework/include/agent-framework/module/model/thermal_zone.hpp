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
 * @brief Thermal zone
 * */

#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"



namespace agent_framework {
namespace model {

/*! Thermal zone class */
class ThermalZone : public Resource {
public:

    explicit ThermalZone(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::None);


    ~ThermalZone();


    ThermalZone(const ThermalZone&) = default;


    ThermalZone& operator=(const ThermalZone&) = default;


    ThermalZone(ThermalZone&&) = default;


    ThermalZone& operator=(ThermalZone&&) = default;


    /*!
     * @brief construct an object of class ThermalZone from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed ThermalZone object
     */
    static ThermalZone from_json(const json::Json& json);


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
        return ThermalZone::collection_name;
    }


    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return ThermalZone::component;
    }


    /*!
     * @brief Return volumetric airflow
     * @return The volumetric airflow value
     */
    OptionalField<std::int32_t> get_volumetric_airflow_cfm() const {
        return m_volumetric_airflow_cfm;
    }


    /*!
     * @brief Setter for volumetric airflow
     * @param volumetric_airflow_cfm Volumetric airflow
     */
    void set_volumetric_airflow_cfm(const OptionalField<std::int32_t> volumetric_airflow_cfm) {
        m_volumetric_airflow_cfm = volumetric_airflow_cfm;
    }


    /*!
     * @brief Return desired speed PWM
     * @return The desired speed PWM value
     * */
    OptionalField<std::int32_t> get_desired_speed_pwm() const {
        return m_desired_speed_pwm;
    }


    /*!
     * @brief Setter for desired speed PWM
     * @param desired_speed_pwm Desired speed PWM
     */
    void set_desired_speed_pwm(const OptionalField<std::int32_t> desired_speed_pwm) {
        m_desired_speed_pwm = desired_speed_pwm;
    }


private:

    // Telemetry: The following field is to be removed
    OptionalField<std::int32_t> m_volumetric_airflow_cfm{};

    OptionalField<std::int32_t> m_desired_speed_pwm{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}
