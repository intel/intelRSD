/*!
 * @brief Switch Port Vlan class declaration.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file switch_port_vlan.hpp
 */

#pragma once

#include "json-wrapper/json-wrapper.hpp"
#include <cstdint>
#include <vector>
#include <string>

namespace agent {
namespace network {
namespace hal {

/*! Switch Port Vlan class */
class SwitchPortVlan {
public:

    using VlanId = std::uint16_t;
    using PortId = std::uint16_t;
    using PortIdentifier = std::string;
    using SwitchIdentifier = std::string;
    using VlanList = std::vector<std::string>;

    /*! Vlan port info class */
    class VlanPortInfo {
    private:
        bool m_tagged;
    public:
        /*!
         * @brief Set vlan tag.
         * @param[in] tagged Vlan tag (false=untagged,true=tagged).
         */
        void set_tagged(bool tagged) { m_tagged = tagged; }

        /*!
         * @brief Gets tag info.
         * @return Tag info (false=untagged,true=tagged).
         */
        bool get_tagged() const {
            return m_tagged;
        }
    };

    /*!
     * @brief Default constructor.
     */
    SwitchPortVlan() = default;

    /*!
     * @brief Constructor.
     * @param[in] vlan_id Vlan index.
     */
    SwitchPortVlan(VlanId vlan_id);

    /*!
     * @brief Default destructor.
     */
    virtual ~SwitchPortVlan();

    /*!
     * @brief Add port to vlan.
     * @param[in] port Port identifier.
     * @param[in] tagged Vlan tag(false/true).
     */
    virtual void add_port(const PortIdentifier& port, bool tagged);

    /*!
     * @brief Remove port from vlan.
     * @param[in] port Port identifier.
     * @param[in] tagged Vlan tag(false/true).
     */
    virtual void remove_port(const PortIdentifier& port, bool tagged);

    /*!
     * @brief Get port primary vlan.
     * @param[in] port Port identifier.
     * @return Vlan index.
     */
    virtual VlanId get_port_primary_vlan(const PortIdentifier& port);

    /*!
     * @brief Set port primary vlan.
     * @param[in] port Port identifier.
     * @param[in] vlan_id Vlan id.
     */
    virtual void set_port_primary_vlan(const PortIdentifier& port,
                                       VlanId vlan_id);

    /*!
     * @brief Set VLAN attribute.
     * @param[in] vlan_uuid UUID of a port VLAN.
     * @param[in] attribute_name name of the attribute.
     * @param[in] value Value of the attribute.
     */
    virtual void set_attribute(const std::string& vlan_uuid,
                               const std::string& attribute_name,
                               const json::Json& value);

    /*!
     * @brief Gets Vlan present flag
     * @return present flag.
     */
    bool get_is_present() const {
        return m_is_present;
    }

    /*!
     * @brief Sets Vlan present flag
     * @param is_present Port present plag.
     */
    void set_is_present(const bool is_present) {
        m_is_present = is_present;
    }

    /*!
     * @brief Gets vlan index.
     * @return Vlan index.
     */
    VlanId get_vlan_id() const {
        return m_vlan_id;
    }

    /*!
     * @brief Set vlan index.
     * @param[in] vlan_id Vlan Id.
     */
    void set_vlan_id(VlanId vlan_id) {
        m_vlan_id = vlan_id;
    }

private:
    bool m_is_present{false};
    VlanId m_vlan_id{0};
};

}
}
}
