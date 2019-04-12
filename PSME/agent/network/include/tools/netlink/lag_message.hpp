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
 * @file lag_message.hpp
 *
 * @brief Switch Lag configuration
 * */

#pragma once
#include "tools/netlink/link_message.hpp"

namespace agent {
namespace network {
namespace tools {
namespace netlink {

/*! Lag Message */
class LagMessage: public agent::network::tools::netlink::LinkMessage {
public:
    /*!
     * @brief Default constructor.
     *
     * @param[in] lag Lag interface name.
     * @param[in] port Port interface name.
     * */
    LagMessage(const std::string& lag, const std::string& port);

    /*!
     * @brief Default destructor.
     * */
    virtual ~LagMessage();

    /*!
     * @brief Set Lag port name.
     *
     * @param[in] port Lag port interface name.
     * */
    void set_member_name(const std::string& port) { m_port = port; }

    /*!
     * @brief Get Lag port name.
     *
     * @return m_port Lag port interface name.
     * */
    const std::string& get_member_name() const { return m_port; }

protected:
    /*!
     * @brief Prepare message to be sent
     *
     * @param[out] msg message to be prepared.
     * */
    void prepare_link_message(struct nl_msg* msg) override;

    /*! @brief Linux driver kind name used for
     *         hardware offloading of LAG feature. */
    static const constexpr char* TEAM = "team";

private:

    std::string m_port;
};

}
}
}
}

