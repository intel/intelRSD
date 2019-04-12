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

#include "ipmi/command/generic/get_fru_inventory_area_info.hpp"
#include <gtest/gtest.h>

using namespace std;
using namespace ipmi::command::generic;

TEST(GetFruInventoryAreaInfo, Pack) {
    request::GetFruInventoryAreaInfo req{};
    req.set_device_id(0x12);
    auto data = req.do_pack();
    ASSERT_EQ(1, data.size());
    ASSERT_EQ(0x12, data[0]);
}

TEST(GetFruInventoryAreaInfo, UnpackByteAccess) {
    ipmi::IpmiInterface::ByteBuffer data = {
        ipmi::COMPLETION_CODE_NORMAL,
        0x12,
        0x34,
        0x56
    };
    response::GetFruInventoryAreaInfo response;
    response.do_unpack(data);

    ASSERT_EQ(0x3412, response.get_size());
    ASSERT_EQ(response::GetFruInventoryAreaInfo::FruAccessMode::BYTE, response.get_access_mode());
}

TEST(GetFruInventoryAreaInfo, UnpackWordAccess) {
    ipmi::IpmiInterface::ByteBuffer data = {
        ipmi::COMPLETION_CODE_NORMAL,
        0x12,
        0x34,
        0x57
    };
    response::GetFruInventoryAreaInfo response;
    response.do_unpack(data);

    ASSERT_EQ(0x3412, response.get_size());
    ASSERT_EQ(response::GetFruInventoryAreaInfo::FruAccessMode::WORD, response.get_access_mode());
}
