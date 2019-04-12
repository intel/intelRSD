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
 * @file get_pwm.hpp
 *
 * @brief GetPwm IPMI command request and response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief GetPwm request command.
 */
class GetPwm : public Request {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetPwm();

    /*!
     * @brief Asset index for which we read PWM value.
     * @param asset_index Asset index
     */
    void set_asset_index(const std::uint8_t asset_index) {
        m_asset_index = asset_index;
    }

    virtual const char* get_command_name() const override {
        return "GetPwm";
    }

private:
    void pack(IpmiInterface::ByteBuffer& data) const override;

    std::uint8_t m_asset_index{0};
};

}

namespace response {

/*!
 * @brief GetPwm response command.
 */
class GetPwm : public Response {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetPwm();

    /*!
     * @brief PWM value getter.
     * @return PWM value.
     */
    std::uint8_t get_pwm_value() const;

    virtual const char* get_command_name() const override {
        return "GetPwm";
    }

private:
    static constexpr const std::size_t RESPONSE_SIZE = 2;

    std::uint8_t m_pwm_value{0};

    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

}

}
}
}
