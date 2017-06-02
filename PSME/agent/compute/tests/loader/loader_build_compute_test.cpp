/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/model/manager.hpp"
#include "loader/compute_loader.hpp"

#include "configuration_full.hpp"
#include "json/value.hpp"
#include "json/deserializer.hpp"

#include "gtest/gtest.h"

using namespace agent::compute::loader;

// TODO: For refactor purpose only. Remove after name unification.
using agent_framework::module::CommonComponents;

class ComputeLoaderBuildComputeTest: public ::testing::Test {
protected:
    ComputeLoader loader{};
    json::Value config;
    json::Deserializer deserializer{};

    // Just to prevent from second initialization.
    static bool is_initialized;

    void init_loader_with_config_file_or_throw_exception() {
        deserializer << COMPUTE_FULL_CONFIGURATION;
        deserializer >> config;
        bool is_loaded = loader.load(config);

        if (!is_loaded) {
            throw std::runtime_error("Load must be successful in this test! DOA!") ;
        }
    }
public:

    ComputeLoaderBuildComputeTest() {
        // We need to initialize the loader once. Then tests the object in trees.
        if (is_initialized) {
            return;
        }

        init_loader_with_config_file_or_throw_exception();

        is_initialized = true;
    }

    virtual ~ComputeLoaderBuildComputeTest();
};

bool ComputeLoaderBuildComputeTest::is_initialized{false};

// To avoid vtable warning.
ComputeLoaderBuildComputeTest::~ComputeLoaderBuildComputeTest() { }

/**
 * Config file for this test is constant and well known.
 * Recognizes a entry basing on slot, then compare read data to known data from test config file.
 */
TEST_F(ComputeLoaderBuildComputeTest, LoadFullConfiguration_ManagersShouldHaveProperData) {
    auto& module_manager = CommonComponents::get_instance()->get_module_manager();

    auto keys_vector = module_manager.get_keys("");

    ASSERT_EQ(4, keys_vector.size()) << "Test config file defines 4 managers. Got: " << keys_vector.size();


    for (const auto& key: keys_vector) {
        auto manager = module_manager.get_entry(key);
        auto conn = manager.get_connection_data();
        const auto slot = manager.get_slot();

        if (1 == slot) {
            ASSERT_STREQ("1.1.2.1", conn.get_ip_address().c_str());
            ASSERT_STREQ("user1", conn.get_username().c_str());
            ASSERT_STREQ("password1", conn.get_password().c_str());
            ASSERT_EQ(623, conn.get_port());
            ASSERT_STREQ("sw0p37", manager.get_switch_port_identifier().c_str());
        }
        else if(2 == slot) {
            ASSERT_STREQ("1.1.2.2", conn.get_ip_address().c_str());
            ASSERT_STREQ("user2", conn.get_username().c_str());
            ASSERT_STREQ("password2", conn.get_password().c_str());
            ASSERT_EQ(624, conn.get_port());
            ASSERT_STREQ("sw0p41", manager.get_switch_port_identifier().c_str());
        }
        else if(3 == slot) {
            ASSERT_STREQ("1.1.2.3", conn.get_ip_address().c_str());
            ASSERT_STREQ("user3", conn.get_username().c_str());
            ASSERT_STREQ("password3", conn.get_password().c_str());
            ASSERT_EQ(625, conn.get_port());
            ASSERT_STREQ("sw0p39", manager.get_switch_port_identifier().c_str());
        }
        else if(4 == slot) {
            ASSERT_STREQ("1.1.2.4", conn.get_ip_address().c_str());
            ASSERT_STREQ("user4", conn.get_username().c_str());
            ASSERT_STREQ("password4", conn.get_password().c_str());
            ASSERT_EQ(626, conn.get_port());
            ASSERT_STREQ("sw0p43", manager.get_switch_port_identifier().c_str());
        }
        else {
            // Only 1 - 4 slots are defined in test-config file.
            ASSERT_TRUE(false) << "Only 1 - 4 slots defined in test-config. Got: " << slot;
        }
    }
}

TEST_F(ComputeLoaderBuildComputeTest, LoadFullConfiguration_EachManagerShouldHaveChassis) {
    auto& module_manager = CommonComponents::get_instance()->get_module_manager();
    auto& chassis_manager = CommonComponents::get_instance()->get_chassis_manager();

    auto managers_keys_vector = module_manager.get_keys("");

    ASSERT_EQ(4, managers_keys_vector.size()) << "Test config file defines 4 managers. Got: "
                                              << managers_keys_vector.size();

    for (const auto& manager_key: managers_keys_vector) {
      auto manager = module_manager.get_entry(manager_key);
      const auto& manager_uuid = manager.get_uuid();

      auto chassis_keys_vector = chassis_manager.get_keys(manager_uuid);
      ASSERT_FALSE(chassis_keys_vector.empty()) << "Each Manager should have a Chassis.";

      for (const auto& chassis_key: chassis_keys_vector) {
          try {
              auto chassis = chassis_manager.get_entry(chassis_key);
          }
          catch (const agent_framework::exceptions::InvalidUuid&) {
              ASSERT_TRUE(false) << "Each Manager should have a Chassis.";
          }
      }
    }

}

TEST_F(ComputeLoaderBuildComputeTest, LoadFullConfiguration_EachChassisShouldBeModule) {
    auto& module_manager = CommonComponents::get_instance()->get_module_manager();
    auto& chassis_manager = CommonComponents::get_instance()->get_chassis_manager();

    auto managers_keys_vector = module_manager.get_keys("");

    ASSERT_EQ(4, managers_keys_vector.size())
        << "Test config file defines 4 managers. Got: " << managers_keys_vector.size();

    for (const auto& manager_key: managers_keys_vector) {
        auto manager = module_manager.get_entry(manager_key);
        const auto& manager_uuid = manager.get_uuid();

        auto chassis_keys_vector = chassis_manager.get_keys(manager_uuid);
        ASSERT_FALSE(chassis_keys_vector.empty()) <<
            "Each Manager should have a Chassis.";

        for (const auto& chassis_key: chassis_keys_vector) {
            auto chassis = chassis_manager.get_entry(chassis_key);
            ASSERT_EQ(2, uint8_t((chassis.get_type())))
                << "Chassis type should be Module. Got:" << chassis.get_type();
        }
    }
}
