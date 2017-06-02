/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

    explicit PowerZone(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::None);
    ~PowerZone();

    PowerZone(const PowerZone&) = default;
    PowerZone& operator=(const PowerZone&) = default;
    PowerZone(PowerZone&&) = default;
    PowerZone& operator=(PowerZone&&) = default;

    /*!
     * @brief construct an object of class PowerZone from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed PowerZone object
     */
    static PowerZone from_json(const Json::Value& json);

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to Json::Value
     */
    Json::Value to_json() const;

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
     * @brief return power_capacity of type double
     *
     * @return the power_capacity value
     */
    OptionalField<double> get_power_capacity() const {
        return m_power_capacity;
    }

    /*!
     * @brief setter for power_capacity attribute
     *
     * @param power_capacity of type double
     */
    void set_power_capacity(const OptionalField<double> power_capacity) {
        m_power_capacity = power_capacity;
    }

    /*!
     * @brief return power_input of type double
     *
     * @return the power_input value
     */
    OptionalField<double> get_power_input() const {
        return m_power_input;
    }

    /*!
     * @brief setter for power_input attribute
     *
     * @param power_input of type double
     */
    void set_power_input(const OptionalField<double> power_input) {
        m_power_input = power_input;
    }

    /*!
     * @brief return power_output of type double
     *
     * @return the power_output value
     */
    OptionalField<double> get_power_output() const {
        return m_power_output;
    }

    /*!
     * @brief setter for power_output attribute
     *
     * @param power_output of type double
     */
    void set_power_output(const OptionalField<double> power_output) {
        m_power_output = power_output;
    }

private:

    OptionalField<double> m_power_input{0.0};
    OptionalField<double> m_power_output{0.0};
    OptionalField<double> m_power_capacity{0.0};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}
