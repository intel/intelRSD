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
 * @file link_message.hpp
 *
 * @brief Link netlink message class
 * */

#pragma once

#include "netlink/rt_message.hpp"

struct ifinfomsg;

namespace agent {
namespace network {
namespace tools {
namespace netlink {

/*! Link Message class */
class LinkMessage : public netlink_base::RtMessage {
public:
    /*!
     * @brief Default constructor
     *
     * @param[in] ifname Interface name
     * */
    LinkMessage(const std::string& ifname);

    /*!
     * @brief Default destructor.
     * */
    virtual ~LinkMessage();

protected:

    /*! Prepare route-link message to be sent */
    virtual void prepare_link_message(struct nl_msg*);

    /*! Process route-link message */
    virtual void process_link_message(struct nl_msg*, const struct ifinfomsg*);

    /*! Prepare interface info message */
    virtual void prepare_ifi_message(struct ifinfomsg&);

    /*! Set route family */
    void set_family(uint16_t family) { m_family = family; }

    /*! Set Interface index */
    void set_index(uint32_t index) { m_index = index; }

private:

    /*! Prepare message to be sent */
    void prepare_message(struct nl_msg*) override final;

    /*! Message handler */
    void process_message(struct nl_msg*) override final;

private:
    uint16_t m_family{};
    uint32_t m_index{};
};

} /* netlink */
} /* tools */
} /* network */
} /* agent */
