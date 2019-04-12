/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file get_port_state_message.hpp
 *
 * @brief Get port state message
 * */

#pragma once

#include "tools/netlink/port_message.hpp"
#include "hal/switch_port_info.hpp"

namespace agent {
namespace network {
namespace tools {
namespace netlink {

/*! Get Port State Message */
class GetPortStateMessage final : public agent::network::tools::netlink::PortMessage {
public:

    /*!
     * @brief Default constructor
     *
     * @param[in] ifname Interface name
     * */
    GetPortStateMessage(const std::string& ifname);

    /*! Deconstructor */
    virtual ~GetPortStateMessage();

    using State = agent::network::hal::State;

    /*!
     * @brief Set port state.
     *
     * @param state Port state.
     * */
    void set_link_state(State state) {
        m_link_state = state;
    }

    /*!
     * @brief Get port state.
     *
     * @return state Port state.
     * */
    State get_link_state() const { return m_link_state; }

    /*!
     * @brief Set operational port state.
     *
     * @param state Operational port state.
     * */
    void set_operational_state(State state) {
        m_operational_state = state;
    }

    /*!
     * @brief Get operational port state.
     *
     * @return state Operational port state.
     * */
    State get_operational_state() const {
        return m_operational_state;
    }

protected:
    /*!
     * @brief Message handler
     *
     * @param[out] msg Netlink message.
     * @param[out] ifi Interface information message.
     * */
    void process_link_message(struct nl_msg* msg,
                              const struct ifinfomsg* ifi) override;

private:
    State m_link_state{State::Down};
    State m_operational_state{State::Down};
};

}
}
}
}
