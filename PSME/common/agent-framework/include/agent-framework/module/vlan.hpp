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

#ifndef AGENT_FRAMEWORK_MODULE_VLAN_HPP
#define AGENT_FRAMEWORK_MODULE_VLAN_HPP

#include "agent-framework/module/port.hpp"
#include "agent-framework/module/status.hpp"
#include "agent-framework/module/oem_data.hpp"

#include "uuid++.hh"

#include <vector>

namespace agent_framework {
namespace generic {

using PortSharedPtr = Port::PortSharedPtr;

class Vlan {
    uuid m_name{};
    uint32_t m_id{};
    std::string m_vlan_name{};
    bool m_enabled{};
    Status m_status{};
    OEMData m_oem_data{};

public:
    /*! Vlan shared pointer */
    using VlanSharedPtr = std::shared_ptr<Vlan>;

    /*! Creates Vlan shared object */
    static VlanSharedPtr make_vlan() {
        return VlanSharedPtr{ new Vlan() };
    }

    /*! Default constructor */
    Vlan(){
        m_name.make(UUID_MAKE_V1);
    }

    /*!
     * @brief Load settings from configuration
     * @param[in] vlan_configuration Vlan configuration section
     * */
    void read_configuration(const json::Value& vlan_configuration);

    /*!
     * @brief Sets Vlan id
     * @param[in] id Vlan id
     * */
    void set_id(const uint32_t id) {
        m_id = id;
    }

    /*!
     * @brief Gets Vlan id
     * @return Vlan id
     * */
    uint32_t get_id() const {
        return m_id;
    }

    /*!
     * @brief Sets Vlan name
     * @param[in] vlan_name  Vlan name
     * */
    void set_vlan_name(const std::string& vlan_name) {
        m_vlan_name = vlan_name;
    }

    /*!
     * @brief Gets Vlan name
     * @return Vlan name
     * */
    const std::string& get_vlan_name() {
        return m_vlan_name;
    }

    /*!
     * @brief Sets Vlans enabled state.
     * @param[in] enabled Boolean value.
     * */
    void set_enabled(const bool enabled) {
        m_enabled = enabled;
    }

    /*!
     * @brief Gets Vlan enabled
     * @return Vlan enabled
     * */
    bool get_enabled() const {
       return m_enabled;
    }

    /*!
     * Gets status
     *
     * @return Status reference
     * */
    const Status& get_status() const { return m_status; }

    /*!
     * Sets status
     *
     * @param[in] status Status
     * */
    void set_status(const Status& status) {
        m_status = status;
    }

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

    ~Vlan();
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_VLAN_HPP */

