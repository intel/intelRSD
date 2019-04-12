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

#include "ipmi/command/sdv/rmm/get_psshelf_critical.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace ipmi::command::sdv;

TEST(GetPsShelfCriticalRequest, PackRequest) {
    request::GetPsShelfCritical request;

    auto request_data = request.do_pack();

    ASSERT_TRUE(request_data.empty());
}


TEST(GetPsShelfCriticalResponse, IsCriticalTrue) {
    ipmi::IpmiInterface::ByteBuffer byte_vector = {0x00, 0x1};
    response::GetPsShelfCritical response;

    response.do_unpack(byte_vector);

    ASSERT_TRUE(response.is_critical());
}

TEST(GetPsShelfCriticalResponse, IsCriticalFalse) {
    ipmi::IpmiInterface::ByteBuffer byte_vector = {0x00, 0x0};
    response::GetPsShelfCritical response;

    response.do_unpack(byte_vector);

    ASSERT_FALSE(response.is_critical());
}
