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
 * @file tlv_payload.hpp
 *
 * @brief LLDP TLV payload
 * */

#pragma once

#include <memory>

namespace agent {
namespace network {
namespace api {
namespace lldp {

/*!
 * @brief LLDP TLV payload class
 * */
class TlvPayload final {
public:
    /*! @brief Default constructor */
    TlvPayload() = default;

    /*!
     * @brief Change payload size
     *
     * @param[in] size Payload size
     *
     * */
    void resize(std::uint16_t size);

    /*!
     * @brief Get TLV payload size
     *
     * @return TLV payload size
     *
     * */
    std::uint16_t get_size() const { return m_size; }

    /*!
     * @brief Get TLV payload
     *
     * @return TLV payload
     *
     * */
    std::uint8_t* get() { return m_data.get(); }

    /*!
     * @brief Get TLV payload
     *
     * @return TLV payload
     *
     * */
    const std::uint8_t* get() const { return m_data.get(); }

private:
    std::uint16_t m_size{0};
    std::shared_ptr<std::uint8_t> m_data{};
};

}
}
}
}
