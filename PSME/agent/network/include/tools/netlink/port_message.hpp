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
 * @file port_message.hpp
 *
 * @brief Switch port netlink message class
 * */

#pragma once

#include "tools/netlink/link_message.hpp"

#include <linux/rtnetlink.h>

namespace agent {
namespace network {
namespace tools {
namespace netlink {

/*! Port Message class */
class PortMessage : public agent::network::tools::netlink::LinkMessage {
public:
    /*!
     * @brief Default constructor
     *
     * @param[in] ifname Interface name
     * */
    PortMessage(const std::string& ifname);

    /*!
     * @brief Default destructor.
     * */
    virtual ~PortMessage();

    /*!
     * @brief Get master port
     *
     * @return Master port
     * */
    const std::string& get_master() { return m_master; }

    /*!
     * @brief Check if the port in a member
     *
     * @return true if port is member otherwise false
     * */
    bool is_member() const { return m_member; }

    /*!
     * @brief Get MTU.
     *
     * @return MTU
     * */
    uint32_t get_mtu() const { return m_mtu; }

protected:
    /*!
     * @brief Prepare message to be sent.
     * */
    void prepare_link_message(struct nl_msg*) override;

    /*!
     * @brief Message handler.
     * */
    void process_link_message(struct nl_msg*, const struct ifinfomsg*) override;

    /*!
     * @brief Get filter mask.
     *
     * @return Filter mask.
     * */
    uint32_t get_filter_mask() const { return m_filter_mask; }

    /*!
     * @brief Set filter mask.
     *
     * @param[in] filter_mask Filter mask.
     * */
    void set_filter_mask(uint32_t filter_mask) { m_filter_mask = filter_mask; }

private:

    /*! is the member port */
    bool m_member{false};

    /*! master interface name */
    std::string m_master{};

    /*! Filter mask */
    uint32_t m_filter_mask{RTEXT_FILTER_VF};

    /*! MTU */
    uint32_t m_mtu{0};
};

} /* netlink */
} /* tools */
} /* network */
} /* agent */
