/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file get_ps_input_power.hpp
 *
 * @brief GetPsInputPower IPMI command request and response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief GetPsInputPower request command.
 */
class GetPsInputPower : public Request {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetPsInputPower();

    const char* get_command_name() const override {
        return "GetPsInputPower";
    }

    /*!
     * @brief Sets queried psu id
     * @param psu_id Psu slot id
     */
    void set_psu_id(uint8_t psu_id) {
        m_psu_id = psu_id;
    }

private:
    void pack(IpmiInterface::ByteBuffer& data) const override;

    uint8_t m_psu_id{};
};

}

namespace response {

/*!
 * @brief GetPsInputPower response command.
 */
class GetPsInputPower : public Response {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetPsInputPower();

    /*!
     * @brief Returns total drawer power
     * @returns Total power used by drawer
     */
    double get_power() const {
        return m_power;
    }

    const char* get_command_name() const override {
        return "GetPsInputPower";
    }

private:
    static constexpr const std::size_t RESPONSE_SIZE = 3;

    void unpack(const IpmiInterface::ByteBuffer& data) override;

    double m_power{};
};

}

}
}
}
