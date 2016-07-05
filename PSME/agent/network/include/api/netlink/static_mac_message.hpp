/*!
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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
 * @file static_mac_message.hpp
 *
 * @brief Static MAC netlink message class
 * */

#pragma once
/* Internal headers */
#include "netlink/message.hpp"

/* C/C++ standard headers */
#include <string>

namespace agent {
namespace network {
namespace api {
namespace netlink {

/*! Static MAC Message class */
class StaticMacMessage: public netlink_base::Message {
public:
    /*!
     * @brief Default constructor.
     * */
    StaticMacMessage() = delete;

    /*!
     * @brief StaticMacMessage constructor.
     *
     * @param[in] add Specifies if it is Add command.
     * @param[in] port switch port identifier
     * @param[in] address MAC address.
     * @param[in] vlan_id VLAN ID.
     * */
    StaticMacMessage(bool add, const std::string& port,
                     const std::string& address, uint16_t vlan_id);

    /*!
     * @brief Default destructor.
     * */
    virtual ~StaticMacMessage();

    /*!
     * @brief Prepare Static MAC netlink message.
     *
     * @return Netlink message ready to be sent.
     * */
    Pointer prepare_netlink_msg() const override;

private:
    bool m_add{true};
    std::string m_port{};
    std::string m_address{};
    uint16_t m_vlan_id{};
};

} /* netlink */
} /* api */
} /* network */
} /* agent */
