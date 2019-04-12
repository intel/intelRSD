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
 * @file neigh_message.hpp
 *
 * @brief Neighbor Netlink message
 * */

#pragma once

#include "netlink/rt_message.hpp"

struct ndmsg;

namespace netlink_base {

/*!
 * @brief Neighbor Netlink Message
 */
class NeighMessage : public RtMessage {
public:
    /*!
     * @brief Default constructor.
     *
     * @param[in] ifname Interface name
     */
    NeighMessage(const std::string& ifname);

    /*!
     * @brief Default destructor.
     */
    virtual ~NeighMessage();

protected:

    /*! Prepare neigh-link message to be sent */
    virtual void prepare_neigh_message(struct nl_msg*);

    /*! Process neigh-link message */
    virtual void process_neigh_message(struct nl_msg*, const struct ndmsg*);

    /*! Set family */
    void set_family(uint16_t family) { m_family = family; }

    /*! Set interface index */
    void set_ifindex(uint32_t ifindex) { m_ifindex = ifindex; }

    /*! Set state */
    void set_state(uint16_t state) { m_state = state; }

    /*! Set flags */
    void set_flags(uint16_t flags) { m_flags = flags; }

private:
    /*!
     * @brief Prepare message to be sent.
     */
    void prepare_message(struct nl_msg*) override final;

    /*!
     * @brief Message handler.
     */
    void process_message(struct nl_msg*) override final;

private:
    uint16_t m_flags{};
    uint16_t m_state{};
    uint16_t m_family{};
    uint32_t m_ifindex{};
};

}
