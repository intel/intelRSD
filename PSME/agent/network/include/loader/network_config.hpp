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
 * @file network_config.hpp
 * @brief Network config class
 * */

#pragma once
#include "agent-framework/generic/singleton.hpp"
#include "agent-framework/module/enum/enum_builder.hpp"
#include <mutex>
#include <map>

namespace agent {
namespace network {
namespace loader {

/*! @brief ENUM EthMode */
ENUM(EthMode, uint32_t, Default, Mode1000BaseKX, Mode1000BaseX, Mode10GBaseCR,
    Mode10GBaseCX4, Mode10GBaseKR, Mode10GBaseKX4, Mode10GBaseSR, Mode24GBaseCR4,
    Mode24GBaseKR4, Mode2500BaseX, Mode40GBaseCR4, Mode40GBaseKR4, Mode40GBaseSR4,
    Mode6GBaseCR, Mode6GBaseKR, ModeAN73, ModeDisabled, ModeSGMII, ModeXAUI,
    ModeXLAUI);

/*! @brief ENUM AutoNeg */
ENUM(AutoNeg, uint32_t, Default, SGMII, Clause37, Clause73);

class NetworkConfig final : public agent_framework::generic::Singleton<NetworkConfig> {
public:

    /*!
     * @brief Add port and its ethmode to internal map
     *
     * @param[in] port_identifier Port identifier
     * @param[in] mode Port ethernet mode
     * */
    void add_port_ethmode(const std::string& port_identifier, const EthMode mode);

    /*!
     * @brief Get port ethernet mode
     *
     * @param[in] port_identifier Port identifier
     * @return Port ethernet mode
     * */
    const EthMode get_port_ethmode(const std::string& port_identifier);

    /*!
     * @brief Add port and its autoneg to internal map
     *
     * @param[in] port_identifier Port identifier
     * @param[in] autoneg Port autoneg
     * */
    void add_port_autoneg(const std::string& port_identifier, const AutoNeg autoneg);

    /*!
     * @brief Get port aotoneg
     *
     * @param[in] port_identifier Port identifier
     *
     * @return Port aotoneg
     * */
    const AutoNeg get_port_autoneg(const std::string& port_identifier);

    /*!
     * @brief Add switch and its mgmt port name
     *
     * @param[in] switch_identifier Switch identifier
     * @param[in] mgmt_port Mgmt port
     * */
    void add_switch_mgmt_port(const std::string& switch_identifier, const std::string& mgmt_port);

    /*!
     * @brief Get switch mgmt port name
     *
     * @param[in] switch_identifier Switch identifier
     *
     * @return Switch mgmt port name
     * */
    const std::string& get_switch_mgmt_port(const std::string& switch_identifier);

    /*!
     * @brief Add public vlan to internal map
     *
     * @param[in] vlan_id Vlan id
     * */
    void add_public_vlan(uint32_t vlan_id);

    /*!
     * @brief Get public vlan list
     *
     * @return Public vlan list
     * */
    std::vector<uint32_t> get_public_vlans();

    /*
     * @brief Default destructor
     * */
    virtual ~NetworkConfig();

    /*!
     * @brief Save ACL name for given UUID.
     *
     * @param[in] uuid UUID of the ACL.
     * @param[in] name ACL name.
     * */
    void add_acl_name(const std::string& uuid, const std::string& name);

    /*!
     * @brief Get ACL name for given ACL UUID if exist.
     *
     * @param[in] uuid UUID of the ACL.
     *
     * @return ACL name if found in the map, UUID if not.
     * */
    const std::string& get_acl_name(const std::string& uuid);

private:
    std::mutex m_mutex{};
    std::vector<uint32_t> m_public_vlans{};
    std::map<std::string, EthMode> m_port_eth_mode_map{};
    std::map<std::string, AutoNeg> m_port_autoneg_map{};
    std::map<std::string, std::string> m_switch_mgmt_map{};
    std::map<std::string, std::string> m_acl_map{};
    const std::string DEFAULT_MGMT_PORT{"None"};
    const std::string DEFAULT_SYSFS_DRIVER{"None"};
};

}
}
}

