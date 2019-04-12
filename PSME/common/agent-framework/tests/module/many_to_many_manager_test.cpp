/*!
 * @brief Unit tests for generation of UUIDv5
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
 * @file many_to_many_manager_test.cpp
 * */

#include "agent-framework/module/managers/many_to_many_manager.hpp"

#include "gtest/gtest.h"



using agent_framework::module::managers::ManyToManyManager;

class ManyToManyManagerTest : public ::testing::Test {
public:
    virtual ~ManyToManyManagerTest();


    virtual void SetUp();


    virtual void TearDown() {}

    ManyToManyManager my_manager{};
};

ManyToManyManagerTest::~ManyToManyManagerTest() {}


void ManyToManyManagerTest::SetUp() {
    // Fill the manager with data
    for (unsigned agent_id = 0u; agent_id < 2u; agent_id++) {
        for (unsigned parent_id = 2u * agent_id; parent_id < 2u * agent_id + 2u; parent_id++) {
            for (unsigned child_id = (agent_id + parent_id) * 2;
                 child_id < (agent_id + parent_id) * 2 + 4; child_id++) {
                my_manager.add_entry("parent_" + std::to_string(parent_id), "child_" + std::to_string(child_id),
                                     "agent_" + std::to_string(agent_id));
            }
        }
    }
}

TEST_F(ManyToManyManagerTest, UpdateParent) {
    // basic assertions to ensure that the generated data is consistent
    ASSERT_TRUE(my_manager.entry_exists("parent_0", "child_0"));
    ASSERT_TRUE(my_manager.entry_exists("parent_1", "child_3"));
    ASSERT_FALSE(my_manager.entry_exists("parent_0", "child_4"));
    ASSERT_FALSE(my_manager.entry_exists("parent_1", "child_0"));

    // update parent by changing its uuid from "parent_0" to "PARENT_0"
    my_manager.update_parent("parent_0", "PARENT_0");

    // assert that no entries with "parent_0" as parent uuid exists
    ASSERT_TRUE(my_manager.get_children("parent_0").empty());

    // assert that all entries with parent "parent_0" were updated
    for (unsigned child_id = 0u; child_id < 4; child_id++) {
        ASSERT_TRUE(my_manager.entry_exists("PARENT_0", "child_" + std::to_string(child_id)));
    }

    // assert that the number of children of updated parent did not change
    ASSERT_EQ(4, my_manager.get_children("PARENT_0").size());
}


TEST_F(ManyToManyManagerTest, UpdateChildWithSingleParent) {
    // basic assertions to ensure that the generated data is consistent
    ASSERT_TRUE(my_manager.entry_exists("parent_0", "child_0"));
    ASSERT_TRUE(my_manager.entry_exists("parent_1", "child_3"));
    ASSERT_FALSE(my_manager.entry_exists("parent_0", "child_4"));
    ASSERT_FALSE(my_manager.entry_exists("parent_1", "child_0"));

    // update child by changing its uuid from "child_0" to "CHILD_0"
    my_manager.update_child("child_0", "CHILD_0");

    // assert that no entries with "child_0" as child uuid exists
    ASSERT_TRUE(my_manager.get_parents("child_0").empty());

    // assert that an updated entry exists
    ASSERT_TRUE(my_manager.entry_exists("parent_0", "CHILD_0"));
}

TEST_F(ManyToManyManagerTest, UpdateChildWithMultipleParents) {
    // basic assertions to ensure that the generated data is consistent
    ASSERT_TRUE(my_manager.entry_exists("parent_0", "child_0"));
    ASSERT_TRUE(my_manager.entry_exists("parent_1", "child_3"));
    ASSERT_FALSE(my_manager.entry_exists("parent_0", "child_4"));
    ASSERT_FALSE(my_manager.entry_exists("parent_1", "child_0"));

    // update child with multiple parents by changing its uuid from "child_3" to "CHILD_3"
    my_manager.update_child("child_3", "CHILD_3");

    // assert that no entries with "child_3" as child uuid exist
    my_manager.update_child("child_3", "CHILD_3");

    // assert that all entries with "child_3" as child uuid were updated
    ASSERT_TRUE(my_manager.entry_exists("parent_0", "CHILD_3"));
    ASSERT_TRUE(my_manager.entry_exists("parent_1", "CHILD_3"));

    // assert that no additional entries were added to the manager
    ASSERT_EQ(2, my_manager.get_parents("CHILD_3").size());
}
