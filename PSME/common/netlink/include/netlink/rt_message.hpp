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
 * @file rt_message.hpp
 *
 * @brief Route Netlink message
 * */

#pragma once

#include "netlink/nl_message.hpp"

#include <string>

namespace netlink_base {

/*!
 * @brief Route Netlink Message
 */
class RtMessage : public NlMessage {
public:
    /*!
     * @brief Default constructor.
     *
     * @param[in] iface Interface name
     */
    RtMessage(const std::string& iface);

    /*!
     * @brief Get interface name
     *
     * @return Interface name
     * */
    const std::string& get_ifname() const {
        return m_ifname;
    }

    /*!
     * @brief Default destructor.
     */
    virtual ~RtMessage();

protected:
    /*!
     * @brief Get interface index
     *
     * @return Interface index
     * */
    int get_ifindex() const {
        return m_ifindex;
    }

    /*!
     * @brief Check if an interface exists
     *
     * @return true if exists otherwise false
     * */
    bool iface_exists() const {
        return 0 != m_ifindex;
    }

    /*!
     * @brief Check if an interface name is valid
     *
     * @return true exists otherwise false
     * */
    bool is_ifname_valid() const;

private:
    std::string m_ifname{};
    int m_ifindex{};
};

}
