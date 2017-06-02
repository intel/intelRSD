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
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed Fan object
     */
    static Fan from_json(const Json::Value& json);

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
     * @brief return slot of type std::uint32_t
     *
     * @return the slot value
     */
    OptionalField<std::uint32_t> get_slot() const {
        return m_slot;
    }

    /*!
     * @brief setter for slot attribute
     *
     * @param slot of type uint32_t
     */
    void set_slot(const OptionalField<uint32_t> slot) {
        m_slot = slot;
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
     * @brief return desired_speed of type std::uint32_t
     *
     * @return the desired_speed value
     */
    OptionalField<std::uint32_t> get_desired_speed() const {
        return m_desired_speed;
    }

    /*!
     * @brief setter for desired_speed attribute
     *
     * @param desired_speed of type uint32_t
     */
    void set_desired_speed(const OptionalField<uint32_t> desired_speed) {
        m_desired_speed = desired_speed;
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

private:

    OptionalField<std::uint32_t> m_slot{0u};
    OptionalField<std::uint32_t> m_current_speed{0u};
    OptionalField<std::uint32_t> m_desired_speed{0u};
    attribute::FruInfo m_fru_info{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

