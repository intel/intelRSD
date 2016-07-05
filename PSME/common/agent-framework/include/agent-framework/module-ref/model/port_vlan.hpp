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
 * @file port_vlan.hpp
 * @brief PortVlan model interface
 * */

#ifndef AGENT_FRAMEWORK_MODULE_MODEL_PORT_VLAN_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_PORT_VLAN_HPP

#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/model/resource.hpp"
#include "agent-framework/module-ref/enum/common.hpp"

namespace agent_framework {
namespace model {

class PortVlan : public Resource {
    template<class T>
    using OptionalField = agent_framework::module::utils::OptionalField<T>;
public:
    explicit PortVlan(const std::string& parent_uuid = {});
    ~PortVlan();

    PortVlan(const PortVlan&) = default;
    PortVlan& operator=(const PortVlan&) = default;
    PortVlan(PortVlan&&) = default;
    PortVlan& operator=(PortVlan&&) = default;

    /*!
     * @brief construct an object of class PortVlan from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed PortVlan object
     */
    static PortVlan from_json(const Json::Value& json);

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
        return PortVlan::collection_name;
    }

    /**
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return PortVlan::component;
    }
    /*!
     * @brief Sets PortVlan id
     * @param[in] port_vlan_id PortVlan id
     * */
    void set_vlan_id(const OptionalField<uint32_t> port_vlan_id) {
        m_vlan_id = port_vlan_id;
    }

    /*!
     * @brief Gets PortVlan id
     * @return PortVlan id
     * */
    OptionalField<uint32_t> get_vlan_id() const {
        return m_vlan_id;
    }

    /*!
     * @brief Sets PortVlan name
     * @param[in] port_vlan_name  PortVlan name
     * */
    void set_vlan_name(const OptionalField<std::string>& port_vlan_name) {
        m_vlan_name = port_vlan_name;
    }

    /*!
     * @brief Gets PortVlan name
     * @return PortVlan name
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
     * @brief Gets PortVlan enable
     * @return PortVlan enable
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
     * @return PortVlan tagged
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

#endif /* AGENT_FRAMEWORK_MODULE_MODEL_PORT_VLAN_HPP */
