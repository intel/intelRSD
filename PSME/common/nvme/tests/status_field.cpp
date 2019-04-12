/*!
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
 * @file tests/status_field.cpp
 */

#include "nvme/status_field.hpp"

#include "gtest/gtest.h"

using namespace nvme;

namespace testing {

TEST(StatusFieldTests, ExtractCodesFromRawStatusField) {

    /*
     * 16 bits of the status field
     * 0      phase tag, not interesting
     * 8:1    status code
     * 11:9   status code type
     * 13:12  reserved
     * 14     more, not interesting
     * 15     do not retry, not interesting
     */

    StatusField sf(0x5678);

    ASSERT_EQ(sf.get_code(), (0x5678 & 0x01FE) >> 1);
    ASSERT_EQ(sf.get_type(), (0x5678 & 0x0E00) >> 9);
    ASSERT_EQ(sf.get_raw_value(), 0x5678);

    // good cases
    ASSERT_EQ(StatusField(0x0000).is_successful(), true);
    ASSERT_EQ(StatusField(0xF001).is_successful(), true);
    // bad cases
    ASSERT_EQ(StatusField(0x000E).is_successful(), false);
    ASSERT_EQ(StatusField(0xF00F).is_successful(), false);
    ASSERT_EQ(StatusField(0x0210).is_successful(), false);
    ASSERT_EQ(StatusField(0xF211).is_successful(), false);

}

}
