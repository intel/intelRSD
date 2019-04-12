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
 * */


#include "ipmi/command/sdv/rmm/set_pwm.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace ipmi::command::sdv;

namespace {

constexpr const std::uint8_t pwm_index{0x05};
constexpr const std::uint8_t pwm_value{0x20};

}

TEST(SetPwmRequest, Pack) {
    ipmi::IpmiInterface::ByteBuffer expected = {pwm_index, pwm_value};

    request::SetPwm request;
    request.set_asset_index(pwm_index);
    request.set_pwm_value(pwm_value);
    auto actual = request.do_pack();

    ASSERT_EQ(expected, actual);
}
