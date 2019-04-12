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
 * @file set_pwm.hpp
 *
 * @brief SetPwm IPMI command request and response.
 * */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief SetPwm request command.
 */
class SetPwm : public Request {
public:
    /*!
     * @brief Constructor.
     */
    explicit SetPwm();


    SetPwm(const SetPwm&) = default;


    SetPwm(SetPwm&&) = default;


    virtual ~SetPwm();


    /*!
     * @brief Asset index for which we set PWM value.
     * @param asset_index Asset index
     */
    void set_asset_index(std::uint8_t asset_index);


    /*!
     * @brief PWM value setter.
     * @param pwm_value PWM value to be set.
     */
    void set_pwm_value(std::uint8_t pwm_value);

    virtual const char* get_command_name() const override {
        return "SetPwm";
    }

private:
    void pack(IpmiInterface::ByteBuffer& data) const override;


    std::uint8_t m_asset_index{0};
    std::uint8_t m_pwm_value{0};
};

}

namespace response {

/*!
 * @brief SetPwm response command.
 */
class SetPwm : public Response {
public:
    /*!
     * @brief Constructor.
     */
    explicit SetPwm();


    SetPwm(const SetPwm&) = default;


    SetPwm(SetPwm&&) = default;


    virtual ~SetPwm();

    virtual const char* get_command_name() const override {
        return "SetPwm";
    }

private:
    static constexpr const std::size_t RESPONSE_SIZE = 1;


    void unpack(const IpmiInterface::ByteBuffer&) override {
        /*no data in response*/
    }
};

}

}
}
}
