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
 * @file persistent_uuid_generation_test.cpp
 * */



#include "agent-framework/module/model/resource.hpp"

#include "gtest/gtest.h"

#include <string>



using namespace agent_framework::model;



namespace {
    constexpr const char* PARENT_UUID = "parent_uuid";
    constexpr const char* DUMMY_KEY = "dummy";
}



/*!
 * @brief Test fixture verifying persistent UUID generation
 * */
class PersistentUuidGenerationTest : public testing::Test {
public:
    /*! Destructor */
    virtual ~PersistentUuidGenerationTest();


    /*!
     * Sets up test fixture. Required by gtest framework.
     * */
    virtual void SetUp() {}


    /*!
     * Cleans up after test fixture. Required by gtest framework.
     * */
    virtual void TearDown() {}

};


PersistentUuidGenerationTest::~PersistentUuidGenerationTest() {}


class TestResource : public Resource {
public:
    TestResource(const Uuid& parent) : Resource(parent) { }

    virtual ~TestResource();

    json::Json to_json() const {
        return json::Json{};
    }
};


TestResource::~TestResource() {}


/*!
 * Test unique key accessor functions. This test verifies that
 * the unique key is empty on resource object creation and that
 * accessor functions are working properly.
 * */
TEST_F(PersistentUuidGenerationTest, UniqueKeyOperations) {
    TestResource resource{PARENT_UUID};

    /* Verify that the unique key is empty by default */
    ASSERT_FALSE(resource.get_unique_key().has_value());

    /* Assign unique key value to the resource */
    resource.set_unique_key(DUMMY_KEY);
    /* Assert that the uniwue key was set */
    ASSERT_TRUE(resource.get_unique_key().has_value());
    ASSERT_EQ(resource.get_unique_key().value(), DUMMY_KEY);
}


/*!
 * Test persistent UUID generation without setting unique key.
 * This test verifies that persistent UUID is not generated without setting
 * the unique key for resource.
 * */
TEST_F(PersistentUuidGenerationTest, UuidGenerationWithoutUniqueKey) {
    TestResource resource{PARENT_UUID};
    TestResource clone_resource(PARENT_UUID);

    /* Verify that resources have different temporary UUIDs */
    ASSERT_TRUE(resource.get_uuid() != clone_resource.get_uuid());

    std::string temporary_resource_uuid = resource.get_uuid();
    std::string temporary_clone_resource_uuid = clone_resource.get_uuid();

    /* Attempt to generate persistent UUIDs */
    resource.make_persistent_uuid();
    clone_resource.make_persistent_uuid();

    /* Verify that resources UUIDs did not change */
    ASSERT_TRUE(resource.get_uuid() != clone_resource.get_uuid());
    ASSERT_TRUE(temporary_resource_uuid == resource.get_uuid());

    /* Verify that the persistent UUID flag is not set */
    ASSERT_FALSE(resource.has_persistent_uuid());
}


/*!
 * Test persistent UUID generation with unique key value set.
 * This test verifies that resources under the same agent are given the
 * same persistent UUID if their unique keys are the same.
 * */
TEST_F(PersistentUuidGenerationTest, UuidWithUniqueKey) {
    TestResource resource{PARENT_UUID};
    TestResource clone_resource(PARENT_UUID);

    /* Set unique keys for both resources to the same value */
    resource.set_unique_key(DUMMY_KEY);
    clone_resource.set_unique_key(DUMMY_KEY);

    /* Verify that the resources UUIDs are still different */
    ASSERT_TRUE(resource.get_uuid() != clone_resource.get_uuid());

    /* Attempt to create persistent UUIDs for both resources */
    resource.make_persistent_uuid();
    clone_resource.make_persistent_uuid();

    /* Verify that resources UUIDs are now equal */
    ASSERT_TRUE(resource.get_uuid() == clone_resource.get_uuid());

    /* Verify that the persistent UUID flag is set */
    ASSERT_TRUE(resource.has_persistent_uuid());
}
