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
 * @file vlan.hpp
 * @brief Vlan
 * */

#ifndef AGENT_FRAMEWORK_MODULE_VLANPORT_HPP
#define AGENT_FRAMEWORK_MODULE_VLANPORT_HPP

#include "agent-framework/module/status.hpp"
#include "agent-framework/module/oem_data.hpp"
#include "agent-framework/module/ipv4_address.hpp"
#include "agent-framework/module/ipv6_address.hpp"
#include "agent-framework/module/port.hpp"
#include "agent-framework/module/vlan.hpp"

#include "uuid++.hh"

namespace agent_framework {
namespace generic {

using PortSharedPtr = Port::PortSharedPtr;
using VlanSharedPtr = Vlan::VlanSharedPtr;

class VlanPort {
    uuid m_name{};
    bool m_tagged{};
    PortSharedPtr m_port{};
    VlanSharedPtr m_vlan{};
    Status m_status{};
    IPv4Address m_ipv4_address{};
    IPv6Address m_ipv6_address{};
    OEMData m_oem_data{};
public:
    /*! VlanPort unique pointer */
    using VlanPortUniquePtr = std::unique_ptr<VlanPort>;

    /*! Creates VlanPort unique object */
    static VlanPortUniquePtr make_vlanport() {
        return VlanPortUniquePtr{ new VlanPort() };
    }

    /*! Default constructor */
    VlanPort(){
        m_name.make(UUID_MAKE_V1);
    }

    /*!
     * @brief Gets unique name of VlanPort.
     * @return Name of VlanPort represented by UUID.
     * */
    const std::string get_name() {
        return std::string(m_name.string());
    }

    /*!
     * @brief Sets Vlan tagging.
     * @param[in] tagged Boolean value
     * */
    void set_tagging(const bool tagged) {
        m_tagged = tagged;
    }

    /*!
     * @brief Gets Vlan tagging
     * @return Vlan tagging
     * */
    bool get_tagging() const {
        return m_tagged;
    }

    /*!
     * @brief Atach port to VlanPort
     * @param[in] port Pointer to Port
     * */
    void attach_port(const PortSharedPtr port) {
        m_port = port;
    }

    /*!
     * @brief Returns Port attached to VlanPort
     * @return Pointer to Port
     * */
    PortSharedPtr get_port() const {
        return m_port;
    }

    /*!
     * @brief Atach vlan to VlanPort
     * @param[in] vlan  Pointer to Vlan
     * */
    void attach_vlan(const VlanSharedPtr vlan) {
        m_vlan = vlan;
    }

    /*!
     * @brief Returns Vlan  attached to VlanPort
     * @return Pointer to Vlan
     * */
    VlanSharedPtr get_vlan() const {
        return m_vlan;
    }

    /*!
     * @brief Gets status
     * @return Status reference
     * */
    const Status& get_status() const { return m_status; }

    /*!
     * Sets status
     * @param[in] status Status
     * */
    void set_status(const Status& status) {
        m_status = status;
    }

    /*!
     * @brief Returns IPv4 address.
     * @return Reference to IPv4Address.
     * */
    const IPv4Address& get_ipv4_address() const { return m_ipv4_address; }

    /*!
     * @brief Returns IPv6 address.
     * @return Reference to IPv6Address.
     * */
    const IPv6Address& get_ipv6_address() const { return m_ipv6_address; }

    /*!
     * @brief Set Vlan oem data
     * @param oem_data Vlan oem data
     */
    void set_oem_data(const OEMData& oem_data) {
        m_oem_data = oem_data;
    }

    /*!
     * @brief Get Vlan oem data
     * @return Vlan oem data
     */
    const OEMData& get_oem_data() const {
        return m_oem_data;
    }

    ~VlanPort();
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_VLANPORT_HPP */
