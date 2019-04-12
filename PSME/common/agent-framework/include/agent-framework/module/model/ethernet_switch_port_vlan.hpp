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
 * @file ethernet_switch_port_vlan.hpp
 * @brief EthernetSwitchPortVlan model interface
 * */

#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

class EthernetSwitchPortVlan : public Resource {
public:
    explicit EthernetSwitchPortVlan(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::EthernetSwitchPort);
    ~EthernetSwitchPortVlan();

    EthernetSwitchPortVlan(const EthernetSwitchPortVlan&) = default;
    EthernetSwitchPortVlan& operator=(const EthernetSwitchPortVlan&) = default;
    EthernetSwitchPortVlan(EthernetSwitchPortVlan&&) = default;
    EthernetSwitchPortVlan& operator=(EthernetSwitchPortVlan&&) = default;

    /*!
     * @brief construct an object of class EthernetSwitchPortVlan from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed EthernetSwitchPortVlan object
     */
    static EthernetSwitchPortVlan from_json(const json::Json& json);

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
        return EthernetSwitchPortVlan::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return EthernetSwitchPortVlan::component;
    }
    /*!
     * @brief Sets EthernetSwitchPortVlan id
     * @param[in] port_vlan_id EthernetSwitchPortVlan id
     * */
    void set_vlan_id(const OptionalField<uint32_t> port_vlan_id) {
        m_vlan_id = port_vlan_id;
    }

    /*!
     * @brief Gets EthernetSwitchPortVlan id
     * @return EthernetSwitchPortVlan id
     * */
    OptionalField<uint32_t> get_vlan_id() const {
        return m_vlan_id;
    }

    /*!
     * @brief Sets EthernetSwitchPortVlan name
     * @param[in] port_vlan_name  EthernetSwitchPortVlan name
     * */
    void set_vlan_name(const OptionalField<std::string>& port_vlan_name) {
        m_vlan_name = port_vlan_name;
    }

    /*!
     * @brief Gets EthernetSwitchPortVlan name
     * @return EthernetSwitchPortVlan name
     * */
    const OptionalField<std::string>& get_vlan_name() const {
        return m_vlan_name;
    }

    /*!
     * @brief Sets PortVlans enable state.
     * @param[in] port_vlan_enable Boolean value.
     * */
    void set_vlan_enable(const OptionalField<bool> port_vlan_enable) {
        m_vlan_enable = port_vlan_enable;
    }

    /*!
     * @brief Gets EthernetSwitchPortVlan enable
     * @return EthernetSwitchPortVlan enable
     * */
    OptionalField<bool> get_vlan_enable() const {
       return m_vlan_enable;
    }

    /*!
     * @brief Sets PortVlans tagged state.
     * @param[in] tagged Boolean value.
     * */
    void set_tagged(const OptionalField<bool> tagged) {
        m_tagged = tagged;
    }

    /*!
     * @brief Gets tagged
     * @return EthernetSwitchPortVlan tagged
     * */
    OptionalField<bool> get_tagged() const {
       return m_tagged;
    }

private:

    OptionalField<std::uint32_t> m_vlan_id{};
    OptionalField<std::string> m_vlan_name{};
    OptionalField<bool> m_vlan_enable{};
    OptionalField<bool> m_tagged{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

