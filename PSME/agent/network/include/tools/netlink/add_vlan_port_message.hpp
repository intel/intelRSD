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
 * @file add_vlan_port_message.hpp
 *
 * @brief Add vlan port message
 * */

#pragma once

#include "tools/netlink/link_message.hpp"
#include "tools/netlink/vlan_info.hpp"

namespace agent {
namespace network {
namespace tools {
namespace netlink {

/*! Add Vlan Port Message */
class AddVlanPortMessage: public agent::network::tools::netlink::LinkMessage {
public:
    /*!
     * @brief Default constructor
     *
     * @param[in] port Port identifier
     * @param[in] vlan Vlan id
     * @param[in] tagged Vlan tag
     * @param[in] pvid PVID
     * */
    AddVlanPortMessage(const std::string& port, uint32_t vlan, bool tagged, bool pvid);

    /*!
     * @brief Default destructor.
     */
    virtual ~AddVlanPortMessage();

protected:
    /*!
     * @brief Prepare message to be sent
     */
    void prepare_link_message(struct nl_msg*) override;

private:
    agent::network::tools::netlink::VlanInfo m_vlan_info;
};

}
}
}
}
