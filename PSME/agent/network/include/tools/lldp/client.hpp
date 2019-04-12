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
 * @file client.hpp
 *
 * @brief LLDP client
 * */

#pragma once

#include "tlv_payload.hpp"
#include "tlv_id.hpp"

#include <string>

namespace agent {
namespace network {
namespace api {
namespace lldp {

/*!
 * @brief LLDP Client class
 * */
class Client final {
public:
    /*!
     * @enum Mode
     * @brief LLDP mode
     *
     * @var Mode Mode::DISABLED
     * LLDP disable mode (default)
     *
     * @var Mode Mode::RXTX
     * LLDP receive and transmit mode
     *
     * @var Mode Mode::RX
     * LLDP receive mode (LLDPDU are received)
     *
     * @var Mode Mode::TX
     * LLDP transmit mode (LLDPDU are sent)
     *
     * */
    enum class Mode {
        DISABLED,
        RXTX,
        RX,
        TX
    };

    /*!
     * @brief Default constructor
     *
     * @param[in] ifname Interface name
     *
     * */
    Client(const std::string& ifname);

    /*!
     * @brief Reload LLDP agent daemon
     *
     * */
    void agent_reload() const;

    /*!
     * @brief Set LLDP mode
     *
     * @param[in] mode LLDP mode
     *
     * */
    void set_mode(Mode mode);

    /*!
     * @brief Get LLDP mode
     *
     * @return LLDP mode
     *
     * */
    Mode get_mode() const;

    /*!
     * @brief Enable/disable LLDP TLV
     *
     * @param[in] tlv_id LLDP TLV id
     *
     * @param[in] enable enable flag
     *
     * */
    void enable_tlv(TlvId tlv_id, bool enable);

    /*!
     * @brief Get neighbor LLDP TLV
     *
     * @return LLDP TLV information
     *
     * */
    template <typename T>
    auto get_neighbor_tlv() -> T const {
        return T{get_neighbor_tlv(T::TLV_ID)};
    }

private:
    /*!
     * @brief Get neighbor LLDP TLV
     *
     * @param[in] tlv_id LLDP TLV id
     *
     * @return TLV Payload
     *
     * */
    TlvPayload get_neighbor_tlv(TlvId tlv_id) const;

    /* class members */
    std::string m_ifname{};
};

}
}
}
}
