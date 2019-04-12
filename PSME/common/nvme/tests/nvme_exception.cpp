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
 * @file tests/nvme_exception.cpp
 */

#include "nvme/nvme_exception.hpp"

#include "gtest/gtest.h"

using namespace nvme;

namespace testing {

TEST(NvmeExceptionTest, SerializationTest) {
    ASSERT_EQ(NvmeException(0xF1FF).is_nvme(), true);
    ASSERT_EQ(NvmeException(StatusField(0xF1FF)).is_nvme(), true);
    ASSERT_EQ(NvmeException("Test").is_nvme(), false);

    ASSERT_EQ(NvmeException(0xF1FF).get_nvme_status_type(), StatusCodeType::Generic);
    ASSERT_EQ(NvmeException(0xF3FF).get_nvme_status_type(), StatusCodeType::CommandSpecific);
    ASSERT_EQ(NvmeException(0xF5FF).get_nvme_status_type(), StatusCodeType::MediaDataIntegrity);
    ASSERT_EQ(NvmeException(0xFFFF).get_nvme_status_type(), StatusCodeType::VendorSpecific);

    ASSERT_EQ(NvmeException(0x0007).get_code_as_generic(), GenericStatusCode::IdConflict);
    ASSERT_EQ(NvmeException(0x0007).get_code_as_command_specific(), CommandSpecificStatusCode::AbortCommadLimitExceeded);

    ASSERT_EQ(NvmeException(0x0103).get_code_as_generic(), GenericStatusCode::CapacityExceeded);
    ASSERT_EQ(NvmeException(0x0103).get_code_as_command_specific(), CommandSpecificStatusCode::InvalidProtectionInformation);
    ASSERT_EQ(NvmeException(0x0103).get_code_as_media_data_integrity(), MediaDataIntegrityStatusCode::UnrecoveredReadError);
}

}
