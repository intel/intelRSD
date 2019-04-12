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
 * @file remote_ethernet_switch.hpp
 * @brief Remote Ethernet Switch model interface
 * */
#pragma once
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/network.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! RemoteEthernetSwitch */
class RemoteEthernetSwitch : public Resource {
public:
    explicit RemoteEthernetSwitch(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::None);
    ~RemoteEthernetSwitch();

    RemoteEthernetSwitch(const RemoteEthernetSwitch&) = default;
    RemoteEthernetSwitch& operator=(const RemoteEthernetSwitch&) = default;
    RemoteEthernetSwitch(RemoteEthernetSwitch&&) = default;
    RemoteEthernetSwitch& operator=(RemoteEthernetSwitch&&) = default;

    /*!
     * @brief construct an object of class RemoteEthernetSwitch from JSON
     *
     * @return the newly constructed RemoteEthernetSwitch object
     */
    static RemoteEthernetSwitch from_json(const json::Json&);


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
        return RemoteEthernetSwitch::collection_name;
    }

    /*!
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return RemoteEthernetSwitch::component;
    }

    /*!
     * @brief Set switch identifier
     * @param[in] switch_identifier switch identifier string
     * */
    void set_switch_identifier(const OptionalField<std::string>& switch_identifier) {
        m_switch_identifier = switch_identifier;
    }

    /*!
     * @brief Get switch identifier
     * @return switch identifier
     * */
    const OptionalField<std::string>& get_switch_identifier() const {
        return m_switch_identifier;
    }

    /*!
     * @brief Set mac address
     * @param[in] mac_address switch mac address
     * */
    void set_mac_address(const OptionalField<std::string>& mac_address) {
        m_mac_address = mac_address;
    }

    /*!
     * @brief Get mac address
     * @return mac address string
     * */
    const OptionalField<std::string>& get_mac_address() const {
        return m_mac_address;
    }

    /*!
     * @brief Set next hop
     * @param[in] next_hop Array of next hop objects
     * */
    void set_next_hop(const attribute::Array<attribute::NextHop>& next_hop) {
        m_next_hop = next_hop;
    }

    /*!
     * @brief Get next hop
     * @return Array of next hop objects
     * */
    const attribute::Array<attribute::NextHop>& get_next_hop() const {
        return m_next_hop;
    }

    /*!
     * @brief Add next hop
     * @param[in] next_hop Next hop object
     */
    void add_next_hop(const attribute::NextHop& next_hop) {
        m_next_hop.add_entry(next_hop);
    }

private:
    OptionalField<std::string> m_switch_identifier{};
    OptionalField<std::string> m_mac_address{};
    attribute::Array<attribute::NextHop> m_next_hop{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

} // namespace model
} // namespace agent_framework

