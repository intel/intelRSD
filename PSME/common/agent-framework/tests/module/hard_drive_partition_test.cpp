/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file hard_drive_partition_test.cpp
 *
 * @brief Unit test for HardDrivePartition class
 * */

#include "agent-framework/module/hard_drive_partition.hpp"
#include "gtest/gtest.h"

#include <uuid++.hh>

using namespace agent_framework::generic;

/* Positive tests */

TEST(HardDrivePartitionTest, PositiveSettersGettersTest) {
    constexpr const char TEST_LABEL[] = "TestLabel";
    constexpr const char TEST_TYPE[] = "TestType";
    constexpr const uint32_t TEST_CAPACITY = 1024;

    auto partition = HardDrivePartition::make_partition();
    partition->set_label(TEST_LABEL);
    partition->set_capacity_gb(TEST_CAPACITY);
    partition->set_type(TEST_TYPE);

    ASSERT_EQ(partition->get_capacity_gb(), TEST_CAPACITY);
    ASSERT_EQ(partition->get_label(), TEST_LABEL);
    ASSERT_EQ(partition->get_type(), TEST_TYPE);
    ASSERT_NO_THROW(partition->get_name());
}
