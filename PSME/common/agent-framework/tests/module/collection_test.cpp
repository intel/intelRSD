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
 * @file collection_test.cpp
 *
 * @brief Unit test for Collection and CollectionEntry class
 * */

#include "agent-framework/module/collection.hpp"
#include "gtest/gtest.h"

#include <uuid++.hh>

using namespace agent_framework::generic;

static constexpr const char ENTRY_NAME[] = "name";
static constexpr const char ENTRY_SLOT_MASK[] = "mask";
static constexpr const char ENTRY_TYPE[] = "type";
static constexpr const char EMPTY_STRING[] = "";

/* Positive tests */

TEST(CollectionTest, PositiveSettersGettersTest) {
    Collection collection{};
    Collection::CollectionEntry entry(EMPTY_STRING, EMPTY_STRING);

    EXPECT_NO_THROW(entry.set_name(ENTRY_NAME));
    EXPECT_NO_THROW(entry.set_slot_mask(ENTRY_SLOT_MASK));
    EXPECT_NO_THROW(entry.set_type(ENTRY_TYPE));

    collection.add_collection_entry(entry);

    const auto& collection_with_entry = collection.get_collection();
    ASSERT_EQ(collection_with_entry.size(), 1);
    const auto& entry_from_collection = collection_with_entry.at(0);

    ASSERT_EQ(entry_from_collection.get_name(), ENTRY_NAME);
    ASSERT_EQ(entry_from_collection.get_slot_mask(), ENTRY_SLOT_MASK);
    ASSERT_EQ(entry_from_collection.get_type(), ENTRY_TYPE);
}

TEST(CollectionTest, PositiveCopyMoveTest) {
    Collection::CollectionEntry entry(ENTRY_NAME, ENTRY_TYPE);

    /* Copy test */
    Collection::CollectionEntry copy_of_entry(EMPTY_STRING, EMPTY_STRING);
    copy_of_entry = entry;

    ASSERT_EQ(copy_of_entry.get_name(), entry.get_name());
    ASSERT_EQ(copy_of_entry.get_slot_mask(), entry.get_slot_mask());
    ASSERT_EQ(copy_of_entry.get_type(), entry.get_type());

    /* Move test */
    Collection::CollectionEntry moved_entry(std::move(copy_of_entry));

    ASSERT_EQ(moved_entry.get_name(), entry.get_name());
    ASSERT_EQ(moved_entry.get_slot_mask(), entry.get_slot_mask());
    ASSERT_EQ(moved_entry.get_type(), entry.get_type());
}
