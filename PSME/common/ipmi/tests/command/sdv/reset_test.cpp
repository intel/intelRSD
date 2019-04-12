/*!
 * @brief Reset command unit tests.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file reset_test.cpp
 */

#include "ipmi/command/sdv/rmm/reset.hpp"
#include <gtest/gtest.h>



using namespace ipmi::command::sdv;

TEST(ResetTest, TestRequestPacking) {
    ipmi::IpmiInterface::ByteBuffer request_buffer{};
    ipmi::IpmiInterface::ByteBuffer expected_packed_buffer{0x05};

    request::Reset reset_request{};

    reset_request.set_component_to_reset(request::Reset::ComponentToReset::RRC);

    request_buffer = reset_request.do_pack();

    ASSERT_EQ(expected_packed_buffer, request_buffer);
}
