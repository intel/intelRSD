/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file remote_switch.hpp
 * @brief Remote Switch model interface
 * */
#ifndef AGENT_FRAMEWORK_MODULE_MODEL_REMOTE_SWITCH_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_REMOTE_SWITCH_HPP

#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/model/resource.hpp"
#include "agent-framework/module-ref/enum/network.hpp"
#include "agent-framework/module-ref/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! RemoteSwitch */
class RemoteSwitch : public Resource {
    template<class T>
    using OptionalField = agent_framework::module::utils::OptionalField<T>;
public:
    explicit RemoteSwitch(const std::string& parent_uuid = {});
    ~RemoteSwitch();

    RemoteSwitch(const RemoteSwitch&) = default;
    RemoteSwitch& operator=(const RemoteSwitch&) = default;
    RemoteSwitch(RemoteSwitch&&) = default;
    RemoteSwitch& operator=(RemoteSwitch&&) = default;

    /*!
     * @brief construct an object of class RemoteSwitch from JSON
     *
     * @return the newly constructed RemoteSwitch object
     */
    static RemoteSwitch from_json(const Json::Value&);


    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to Json::Value
     */
    Json::Value to_json() const;

    /**
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return RemoteSwitch::collection_name;
    }

    /**
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return RemoteSwitch::component;
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
    void set_next_hop(const types::Array<attribute::NextHop>& next_hop) {
        m_next_hop = next_hop;
    }

    /*!
     * @brief Get next hop
     * @return Array of next hop objects
     * */
    const types::Array<attribute::NextHop>& get_next_hop() const {
        return m_next_hop;
    }

    /*!
     * @brief Add next hop
     * @param[in] next_hop Next hop object
     */
    void add_next_hop(const attribute::NextHop& next_hop) {
        m_next_hop.push_back(next_hop);
    }

private:
    OptionalField<std::string> m_switch_identifier{};
    OptionalField<std::string> m_mac_address{};
    types::Array<attribute::NextHop> m_next_hop{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

} // namespace model
} // namespace agent_framework

#endif /* AGENT_FRAMEWORK_MODULE_MODEL_REMOTE_SWITCH_HPP */
