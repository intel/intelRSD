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
 * @file port.hpp
 * @brief Port
 * */

#ifndef AGENT_FRAMEWORK_MODULE_PORT_HPP
#define AGENT_FRAMEWORK_MODULE_PORT_HPP

#include "agent-framework/logger_ext.hpp"
#include "agent-framework/module/status.hpp"
#include "agent-framework/module/oem_data.hpp"

#include "uuid++.hh"

#include <memory>
#include <map>

namespace agent_framework {
namespace generic {

class Port {
public:
    /*!
     * @enum LinkState
     * @brief Port`s link states.
     *
     * @var LinkState Port::UP
     * UP state.
     *
     * @var LinkState Port::DOWN
     * DOWN state.
     *
     * @var LinkState Port::UNKNOWN
     * UNKNOWN state.
     * */
    enum class LinkState {
        UP,
        DOWN,
        UNKNOWN
    };

private:
    uuid m_name{};
    uint32_t m_id{};
    LinkState m_link_state{ LinkState::DOWN };
    std::string m_iface_mode{};
    std::string m_autoneg_mode{};
    std::string m_type{};
    OEMData m_oem_data{};
    Status m_status{};
public:

    /*! Port shared pointer */
    using PortSharedPtr = std::shared_ptr<Port>;

    /*! Creates Port shared object */
    static PortSharedPtr make_port() {
        return PortSharedPtr{ new Port() };
    }

    /*! Default constructor */
    Port(){
        m_name.make(UUID_MAKE_V1);
    }

    /*!
     * @brief Load settings from configuration
     * @param[in] port_configuration Port configuration section
     * */
    void read_configuration(const json::Value& port_configuration);

    /*!
     * @brief Sets port id
     * @param[in] id Port id
     * */
    void set_id(const uint32_t id) {
        m_id = id;
    }

    /*!
     * @brief Gets port id
     * @return Port id
     * */
    uint32_t get_id() const {
        return m_id;
    }

    /*!
     * @brief Sets port state
     * @param link_state Port state
     * */
    void set_link_state(const LinkState link_state) {
        m_link_state = link_state;
    }

    /*!
     * @brief Gets port state
     * @return Port state
     * */
    LinkState get_link_state() const {
        return m_link_state;
    }

    /*!
     * @brief Gets status
     * @return Status reference
     * */
    const Status& get_status() const { return m_status; }

    /*!
     * @brief Sets status
     * @param[in] status Status
     * */
    void set_status(const Status& status) {
        m_status = status;
    }

    /*!
     * @brief Get interface mode
     * @return Interafce mode defined by OEM switch API.
     * */
    const std::string& get_iface_mode() const {
        return m_iface_mode;
    }

    /*!
     * @brief Sets interface mode.
     * @param iface_mode Interface mode defined by OEM switch API.
     * */
    void set_iface_mode(const std::string& iface_mode) {
        m_iface_mode = iface_mode;
    }

    /*!
     * @brief Get port type.
     * @return Port type.
     * */
    const std::string& get_type() const {
        return m_type;
    }

    /*!
     * @brief Set port type.
     * @param[in] type Port type.
     * */
    void set_type(const std::string& type) {
        m_type = type;
    }

    /*!
     * @brief Get autonegotiation mode
     * @return Autonegotiation mode defined by OEM switch API.
     * */
    const std::string& get_autoneg_mode() const {
        return m_autoneg_mode;
    }

    /*!
     * @brief Sets autonegotiation mode.
     * @param autoneg_mode Autonegotiation mode defined by OEM switch API.
     * */
    void set_autoneg_mode(const std::string& autoneg_mode) {
        m_autoneg_mode = autoneg_mode;
    }

    /*!
     * @brief Set target oem data
     * @param oem_data target oem data
     */
    void set_oem_data(const OEMData& oem_data) {
        m_oem_data = oem_data;
    }

    ~Port();
private:
    std::map<const std::string, LinkState> m_state_string_to_enum_map {
        { "up", LinkState::UP },
        { "down", LinkState::DOWN }
    };

    LinkState m_state_string_to_enum(const std::string& state_str);
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_PORT_HPP */
