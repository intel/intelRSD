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
 * @file info_vlan_port_message.hpp
 *
 * @brief Vlan information message
 * */

#pragma once

#include "tools/netlink/link_message.hpp"
#include "tools/netlink/vlan_info.hpp"

#include <map>
#include <vector>

namespace agent {
namespace network {
namespace tools {
namespace netlink {

/*! Info Vlan Port Message */
class InfoVlanPortMessage final : public agent::network::tools::netlink::LinkMessage {
public:
    using VlanInfo = agent::network::tools::netlink::VlanInfo;
    using VlanInfoList = std::vector<VlanInfo>;

    /*! Default constructor */
    InfoVlanPortMessage();

    /*!
     * @brief check if the port is vlan tagged
     *
     * @param[in] port Port identifier
     * @param[in] vlan Vlan id
     *
     * @return true - tagged, false - untagged
     * */
    bool is_tagged(const std::string& port, uint32_t vlan);

    /*!
     * @brief Get vlan list on a port
     *
     * @param[in] ifname Interface name
     *
     * @return Vlan list on the interface
     * */
    const VlanInfoList& get_vlan_list(const std::string& ifname) const;

protected:
    /*!
     * @brief Prepare message to be sent
     */
    void prepare_link_message(struct nl_msg*) override;

    /*!
     * @brief Message handler
     */
    void process_link_message(struct nl_msg*, const struct ifinfomsg*) override;

private:
    std::map<std::string, VlanInfoList> m_vlan_port_info{};
    const VlanInfoList m_empty{};
};

}
}
}
}
