/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file static_mac.hpp
 * @brief Static MAC model interface
 * */
#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/network.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! Static Mac */
class StaticMac : public Resource {
public:
    explicit StaticMac(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::EthernetSwitchPort);
    ~StaticMac();

    StaticMac(const StaticMac&) = default;
    StaticMac& operator=(const StaticMac&) = default;
    StaticMac(StaticMac&&) = default;
    StaticMac& operator=(StaticMac&&) = default;

    /*!
     * @brief construct an object of class Static MAC from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed Static MAC object
     * */
    static StaticMac from_json(const json::Json& json);

    /*!
     * @brief transform the object to Json
     *
     * @return the object serialized to json::Json
     * */
    json::Json to_json() const;

    /*!
     * @brief Get collection name
     *
     * @return collection name
     * */
    static enums::CollectionName get_collection_name() {
        return StaticMac::collection_name;
    }

    /*!
     * @brief Get component name
     *
     * @return component name
     * */
    static enums::Component get_component() {
        return StaticMac::component;
    }

    /*!
     * @brief Set MAC address
     *
     * @param[in] address MAC address
     * */
    void set_address(const OptionalField<std::string>& address) {
        m_address = address;
    }

    /*!
     * @brief Get MAC address
     *
     * @return MAC address
     * */
    const OptionalField<std::string>& get_address() const {
        return m_address;
    }

    /*!
     * @brief Set vlan identifier
     *
     * @param[in] vlan_id vlan identifier
     * */
    void set_vlan_id(const OptionalField<uint32_t>& vlan_id) {
        m_vlan_id = vlan_id;
    }

    /*!
     * @brief Get vlan id
     *
     * @return vlan id
     * */
    const OptionalField<uint32_t>& get_vlan_id() const {
        return m_vlan_id;
    }

private:
    static const enums::CollectionName collection_name;
    static const enums::Component component;

    OptionalField<uint32_t> m_vlan_id{};
    OptionalField<std::string> m_address{};
};

} // namespace model
} // namespace agent_framework

