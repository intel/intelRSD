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
 * @file port_desc_tlv.hpp
 *
 * @brief LLDP Port Description TLV information
 * */

#pragma once

#include "tlv_payload.hpp"
#include "tlv_id.hpp"

namespace agent {
namespace network {
namespace api {
namespace lldp {

/*!
 * @brief LLDP Port Description TLV class
 * */
class PortDescTlv final {
public:
    /*!
     * @brief Default constructor
     *
     * @param[in] payload TLV payload
     *
     * */
    PortDescTlv(const TlvPayload& payload);

    /*!
     * @brief Get interface index
     *
     * @return Interface index
     *
     * */
    std::uint32_t get_ifindex() const { return m_ifindex; }

    /*!
     * @brief Get interface name
     *
     * @return Interface name
     *
     * */
    const std::string& get_ifname() const { return m_ifname; }

    /*!
     * @brief Get port description
     *
     * @return Port description
     *
     * */
    const std::string& get_description() const { return m_port_desc; }

    /*! Port description TLV id */
    static constexpr TlvId TLV_ID = TlvId::PORT_DESCRIPTION;

private:
    std::uint32_t m_ifindex{};
    std::string m_port_desc{};
    std::string m_ifname{};
};

}
}
}
}
