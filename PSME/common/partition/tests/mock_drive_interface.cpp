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
 * @file tests/mock_drive_interface.cpp
 */

#include "mock_drive_interface.hpp"

#include "gtest/gtest.h"

using namespace utils;

namespace {

bool buffers_equal(const ByteBuffer& bb, const std::vector<uint8_t>& v) {
    return bb.get_raw_vector() == v;
}

}

namespace testing {

  TEST(MbrPartitionTableTests, TestMockInterfaceReadOnEmptyMocks) {
      MockDriveInterface mdi{};
      EXPECT_TRUE(buffers_equal(mdi.read({}, 0, 0), {}));
      EXPECT_TRUE(buffers_equal(mdi.read({}, 0, 1), {0}));
      EXPECT_TRUE(buffers_equal(mdi.read({}, 1, 0), {}));
      EXPECT_TRUE(buffers_equal(mdi.read({}, 2, 3), {0, 0, 0}));
  }

TEST(MbrPartitionTableTests, TestMockInterfaceRead) {
    MockDriveInterface mdi{};
    mdi.mocked_data = {0, 1, 2, 3};
    EXPECT_TRUE(buffers_equal(mdi.mocked_data, {0, 1, 2, 3}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 0, 0), {}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 0, 1), {0}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 0, 2), {0, 1}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 0, 3), {0, 1, 2}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 0, 4), {0, 1, 2, 3}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 0, 5), {0, 1, 2, 3, 0}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 2, 0), {}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 2, 1), {2}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 2, 2), {2, 3}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 2, 3), {2, 3, 0}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 2, 4), {2, 3, 0, 0}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 2, 5), {2, 3, 0, 0, 0}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 4, 4), {0, 0, 0, 0}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 1, 0), {}));
    EXPECT_TRUE(buffers_equal(mdi.read({}, 4, 3), {0, 0, 0}));
}

TEST(MbrPartitionTableTests, TestMockInterfaceWrite) {
    MockDriveInterface mdi{};

    mdi.mocked_data = {0, 0, 0, 0};
    mdi.write({}, 0, std::vector<uint8_t>{1});
    EXPECT_TRUE(buffers_equal(mdi.mocked_data, {1, 0, 0, 0}));

    mdi.mocked_data = {0, 0, 0, 0};
    mdi.write({}, 3, std::vector<uint8_t>{1});
    EXPECT_TRUE(buffers_equal(mdi.mocked_data, {0, 0, 0, 1}));

    mdi.mocked_data = {0, 0, 0, 0};
    mdi.write({}, 4, std::vector<uint8_t>{1});
    EXPECT_TRUE(buffers_equal(mdi.mocked_data, {0, 0, 0, 0}));

    mdi.mocked_data = {0, 0, 0, 0};
    mdi.write({}, 0, std::vector<uint8_t>{1, 2});
    EXPECT_TRUE(buffers_equal(mdi.mocked_data, {1, 2, 0, 0}));

    mdi.mocked_data = {0, 0, 0, 0};
    mdi.write({}, 1, std::vector<uint8_t>{1, 2});
    EXPECT_TRUE(buffers_equal(mdi.mocked_data, {0, 1, 2, 0}));

    mdi.mocked_data = {0, 0, 0, 0};
    mdi.write({}, 2, std::vector<uint8_t>{1, 2});
    EXPECT_TRUE(buffers_equal(mdi.mocked_data, {0, 0, 1, 2}));

    mdi.mocked_data = {0, 0, 0, 0};
    mdi.write({}, 3, std::vector<uint8_t>{1, 2});
    EXPECT_TRUE(buffers_equal(mdi.mocked_data, {0, 0, 0, 1}));

    mdi.mocked_data = {0, 0, 0, 0};
    mdi.write({}, 4, std::vector<uint8_t>{1, 2});
    EXPECT_TRUE(buffers_equal(mdi.mocked_data, {0, 0, 0, 0}));

    mdi.mocked_data = {0, 0, 0, 0};
    mdi.write({}, 0, std::vector<uint8_t>{1, 2, 3, 4});
    EXPECT_TRUE(buffers_equal(mdi.mocked_data, {1, 2, 3, 4}));

    mdi.mocked_data = {0, 0, 0, 0};
    mdi.write({}, 3, std::vector<uint8_t>{1, 2, 3, 4});
    EXPECT_TRUE(buffers_equal(mdi.mocked_data, {0, 0, 0, 1}));

    mdi.mocked_data = {0, 0, 0, 0};
    mdi.write({}, 2, std::vector<uint8_t>{1, 2, 3, 4, 5, 6});
    EXPECT_TRUE(buffers_equal(mdi.mocked_data, {0, 0, 1, 2}));
}

}
