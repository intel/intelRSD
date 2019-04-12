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
 * @file tlv_message_header.hpp
 *
 * @brief LLDP TLV message header
 * */

#pragma once

#include "message_header.hpp"
#include "tlv_payload.hpp"
#include "tlv_id.hpp"

namespace agent {
namespace network {
namespace api {
namespace lldp {

/*!
 * @brief LLDP TLV message header class
 * */
class TlvMessageHeader final : public MessageHeader {
public:
    /*! Default constructor */
    explicit TlvMessageHeader() = default;

    /*!
     * @brief Parse raw message
     *
     * @param[in] raw Raw message
     *
     * */
    void parse_raw(const std::string& raw) override;

    /*!
     * @brief Get raw message
     *
     * @return Raw message
     *
     * */
    std::string get_raw() const override;

    /*!
     * @brief Get raw message size
     *
     * @return Raw message size
     *
     * */
    size_t get_raw_size() const override;

    /*!
     * @brief Get TLV ID
     *
     * @return TLV ID
     *
     * */
    TlvId get_id() const { return m_tlv_id; }

    /*!
     * @brief Get TLV payload
     *
     * @return TLV payload
     *
     * */
    const TlvPayload& get_payload() const { return m_payload; }

private:
    TlvPayload m_payload{};
    TlvId m_tlv_id{TlvId::UNKNOWN};
};

}
}
}
}
