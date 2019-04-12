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

#include "agent-framework/module/common_components.hpp"
#include "loader/chassis_loader.hpp"
#include "configuration_full.hpp"

#include <gtest/gtest.h>

using namespace agent::chassis::loader;
using namespace agent_framework::module;
using namespace agent_framework::model;

using agent_framework::module::CommonComponents;

const constexpr uint32_t CONFIG_DRAWER_MANAGERS_COUNT = 1;
const constexpr uint32_t CONFIG_BLADE_MANAGERS_COUNT = 4;

class ChassisLoaderBuildChassisTest: public ::testing::Test {
protected:
    ChassisLoader loader{};
    json::Json config = json::Json();

    // Just to prevent from second initialization.
    static bool is_initialized;

    std::vector<std::string> drawer_manager_keys{};
    std::vector<std::string> blade_manager_keys{};
    std::vector<std::string> drawer_chassis_keys{};
    std::vector<std::string> blade_chassis_keys{};
    Manager drawer_manager{};
    Manager first_blade_manager{};
    Chassis drawer_chassis{};
    Chassis blade_chassis{};

    void init_loader_with_config_file_or_throw_exception() {
        config = json::Json::parse(CHASSIS_FULL_CONFIGURATION);
        bool is_loaded = loader.load(config);

        if (!is_loaded) {
            throw std::runtime_error("Load must be successful in this test! DOA!") ;
        }
    }

    void SetUp() {
        drawer_manager_keys = CommonComponents::get_instance()->get_module_manager().get_keys("");
        blade_manager_keys = CommonComponents::get_instance()->
                get_module_manager().get_keys(drawer_manager_keys.front());

        drawer_manager = CommonComponents::get_instance()->
                get_module_manager().get_entry(drawer_manager_keys.front());
        first_blade_manager = CommonComponents::get_instance()->
                get_module_manager().get_entry(blade_manager_keys.front());

        drawer_chassis_keys = CommonComponents::get_instance()->
                get_chassis_manager().get_keys(drawer_manager.get_uuid());
        drawer_chassis = CommonComponents::get_instance()->
                get_chassis_manager().get_entry(drawer_chassis_keys.front());

        blade_chassis_keys = CommonComponents::get_instance()->
                get_chassis_manager().get_keys(first_blade_manager.get_uuid());
        blade_chassis = CommonComponents::get_instance()->
                get_chassis_manager().get_entry(blade_chassis_keys.front());
    }

public:

    ChassisLoaderBuildChassisTest() {
        // We need to initialize the loader once. Then tests the object in trees.
        if (is_initialized) {
            return;
        }

        init_loader_with_config_file_or_throw_exception();

        is_initialized = true;
    }

    virtual ~ChassisLoaderBuildChassisTest();
};

bool ChassisLoaderBuildChassisTest::is_initialized{false};

// To avoid vtable warning.
ChassisLoaderBuildChassisTest::~ChassisLoaderBuildChassisTest() { }

TEST_F(ChassisLoaderBuildChassisTest, LoadFullConfiguration_ManagersHierarchyIsCorrect) {
    /* Drawer manager */
    ASSERT_EQ(CONFIG_DRAWER_MANAGERS_COUNT, drawer_manager_keys.size())
                                << "Test config file defines " << CONFIG_DRAWER_MANAGERS_COUNT
                                << " drawer managers. Got: " << drawer_manager_keys.size();

    /* Blade manager */
    ASSERT_EQ(CONFIG_BLADE_MANAGERS_COUNT, blade_manager_keys.size())
                                << "Test config file defines "
                                << CONFIG_BLADE_MANAGERS_COUNT
                                << " Blade manager1. Got: " << blade_manager_keys.size();

    for (const auto& blade_manager_key:blade_manager_keys) {
        auto blade_manager = CommonComponents::get_instance()->
                get_module_manager().get_entry(blade_manager_key);

        ASSERT_STREQ(drawer_manager.get_uuid().c_str(), blade_manager.get_parent_uuid().c_str())
                                << "Blade parent UUID:"
                                << blade_manager.get_parent_uuid()
                                << " should be equal to Drawer UUID:"
                                << drawer_manager.get_uuid();
    }
}

TEST_F(ChassisLoaderBuildChassisTest, LoadFullConfiguration_DrawerManagerFieldsValuesAreCorrect) {
    ASSERT_EQ(1, drawer_manager.get_slot());
}

TEST_F(ChassisLoaderBuildChassisTest, LoadFullConfiguration_BladeManagerFieldsValuesAreCorrect) {
    ASSERT_EQ(1, first_blade_manager.get_slot());
}

TEST_F(ChassisLoaderBuildChassisTest, LoadFullConfiguration_BladeManagersFieldsValuesAreCorrect) {

    for (const auto& blade_manager_key:blade_manager_keys) {
        auto blade_manager = CommonComponents::get_instance()->
                get_module_manager().get_entry(blade_manager_key);

        auto slot = blade_manager.get_slot();
        if (1 == slot) {
            ASSERT_STREQ("1.1.2.1", blade_manager.get_ipv4_address().value().c_str());
            ASSERT_STREQ("put_username_hash_here", blade_manager.get_connection_data().get_username().c_str());
            ASSERT_STREQ("put_password_hash_here", blade_manager.get_connection_data().get_password().c_str());
            ASSERT_EQ(623, blade_manager.get_connection_data().get_port());
        }
        else if (2 == slot) {
            ASSERT_STREQ("1.1.2.2", blade_manager.get_ipv4_address().value().c_str());
            ASSERT_STREQ("put_username_hash_here", blade_manager.get_connection_data().get_username().c_str());
            ASSERT_STREQ("put_password_hash_here", blade_manager.get_connection_data().get_password().c_str());
            ASSERT_EQ(623, blade_manager.get_connection_data().get_port());
        }
        else if (3 == slot) {
            ASSERT_STREQ("1.1.2.3", blade_manager.get_ipv4_address().value().c_str());
            ASSERT_STREQ("put_username_hash_here", blade_manager.get_connection_data().get_username().c_str());
            ASSERT_STREQ("put_password_hash_here", blade_manager.get_connection_data().get_password().c_str());
            ASSERT_EQ(623, blade_manager.get_connection_data().get_port());
        }
        else if (4 == slot) {
            ASSERT_STREQ("1.1.2.4", blade_manager.get_ipv4_address().value().c_str());
            ASSERT_STREQ("put_username_hash_here", blade_manager.get_connection_data().get_username().c_str());
            ASSERT_STREQ("put_password_hash_here", blade_manager.get_connection_data().get_password().c_str());
            ASSERT_EQ(623, blade_manager.get_connection_data().get_port());
        }
        else {
            ASSERT_TRUE(false) << "Only 1 - 4 slots defined in test-config. Got: " << slot;
        }
    }
}

TEST_F(ChassisLoaderBuildChassisTest, LoadFullConfiguration_ChassisFieldsValuesAreCorrect) {
    ASSERT_STREQ("Drawer", drawer_chassis.get_type().to_string());
    ASSERT_STREQ("BDCR", drawer_chassis.get_platform().to_string());
    ASSERT_EQ(1, drawer_chassis.get_location_offset());

    ASSERT_STREQ("Module", blade_chassis.get_type().to_string());
}
