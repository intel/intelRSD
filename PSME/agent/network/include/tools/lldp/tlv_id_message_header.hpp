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
 * @file tlv_id_message_header.hpp
 *
 * @brief LLDP TLV ID message header
 * */

#pragma once

#include "tlv_id.hpp"
#include "message_header.hpp"

namespace agent {
namespace network {
namespace api {
namespace lldp {

/*!
 * @brief LLDP TLV ID message header class
 * */
class TlvIdMessageHeader final : public MessageHeader {
public:
    /*! Default constructor */
    explicit TlvIdMessageHeader() = default;

    /*!
     * @brief Default constructor
     *
     * @param[in] tlv_id TLV id
     *
     * */
    explicit TlvIdMessageHeader(TlvId tlv_id);

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

    /*!Convert class to TLV ID */
    explicit operator TlvId() const { return m_tlv_id; }

private:
    TlvId m_tlv_id{TlvId::UNKNOWN};
};

}
}
}
}
