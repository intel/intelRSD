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
 * @file tests/utils.cpp
 */

#include "nvme/utils.hpp"

#include "gtest/gtest.h"

using namespace nvme;

namespace testing {

TEST(NvmeDefinitionsTest, GetNvmeVersionTest) {
    ASSERT_EQ(get_nvme_version(0x00000000), NvmeVersion::UNKNOWN);
    ASSERT_EQ(get_nvme_version(0x00010000), NvmeVersion::NVME_1_0_0);
    ASSERT_EQ(get_nvme_version(0x000100FF), NvmeVersion::NVME_1_0_0);
    ASSERT_EQ(get_nvme_version(0x00010100), NvmeVersion::NVME_1_1_0);
    ASSERT_EQ(get_nvme_version(0x000101FF), NvmeVersion::NVME_1_1_0);
    ASSERT_EQ(get_nvme_version(0x00010200), NvmeVersion::NVME_1_2_0);
    ASSERT_EQ(get_nvme_version(0x00010201), NvmeVersion::NVME_1_2_1);
    ASSERT_EQ(get_nvme_version(0x00010300), NvmeVersion::NVME_1_3_0);
    ASSERT_EQ(get_nvme_version(0xF0010000), NvmeVersion::INVALID);
    ASSERT_EQ(get_nvme_version(0xF001000F), NvmeVersion::INVALID);
    ASSERT_EQ(get_nvme_version(0xF0010301), NvmeVersion::INVALID);
}

TEST(NvmeDefinitionsTest, SupportsNamespaceManagementTest) {
    ControllerData cd{};
    cd.oacs = 0x0008;
    ASSERT_EQ(supports_namespace_management(cd), true);
    cd.oacs = 0xFFF7;
    ASSERT_EQ(supports_namespace_management(cd), false);
}

TEST(NvmeDefinitionsTest, GetFormattedLbaDataSizeTest) {
    NamespaceData nd{};
    nd.flbas = 0xFF;
    ASSERT_EQ(get_formatted_lba_data_size(nd), 512);
    nd.flbas = 0x03;
    nd.lbaf[3].lba_data_size = 10;
    ASSERT_EQ(get_formatted_lba_data_size(nd), (1 << 10));
}

}
