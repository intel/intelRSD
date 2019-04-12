/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */

#include "smbios/utilities/conversions.hpp"

#include "gtest/gtest.h"

#include <cstdint>
#include <algorithm>
#include <iterator>

using namespace smbios::utilities;

class ConversionsTest : public ::testing::Test {
public:
    virtual ~ConversionsTest();

    void SetUp() {
        std::uint8_t raw_uuid[16] = {0x33, 0x22, 0x11, 0x00, 0x055, 0x44, 0x77, 0x66, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
        std::copy(std::begin(raw_uuid), std::end(raw_uuid), std::begin(m_system_data.uuid));
    }

    void TearDown() { }

    const smbios::parser::SYSTEM_INFO_DATA& get_system_data() {
        return m_system_data;
    }

protected:
    smbios::parser::SYSTEM_INFO_DATA m_system_data{};
};

ConversionsTest::~ConversionsTest() { }

TEST_F(ConversionsTest, UUIDConversionTest) {
    ASSERT_EQ("00112233-4455-6677-8899-aabbccddeeff", get_system_uuid(get_system_data()));
}
