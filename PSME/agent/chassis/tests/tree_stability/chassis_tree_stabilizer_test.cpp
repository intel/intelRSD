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
 * @file chassis_tree_stabilizer_test.cpp
 * */

#include "tree_stability/chassis_tree_stabilizer.hpp"
#include "agent-framework/module/chassis_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/service_uuid.hpp"

#include "gtest/gtest.h"

#include <string>



using namespace agent::chassis;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::generic;

namespace constants {

// Service UUID
constexpr const char SERIVCE_UUID[] = "e784d192-379c-11e6-bc47-0242ac110002";

// Expected values of generated persistent uuids
constexpr const char DRAWER_MANAGER_PERSISTENT_UUID[] = "bd124990-083f-5dc5-ab57-d75870917b5f";
constexpr const char SLED_MANAGER_1_PERSISTENT_UUID[] = "21c4dfdf-15e1-574e-9e2e-9d981c72b04f";
constexpr const char SLED_MANAGER_2_PERSISTENT_UUID[] = "efb134b7-6f5e-572b-9b6b-d956a3e63cc7";
constexpr const char SLED_MANAGER_3_PERSISTENT_UUID[] = "a7673e69-3459-5f47-bb42-1db218af5149";
constexpr const char SLED_MANAGER_4_PERSISTENT_UUID[] = "0ea46983-0942-5866-845c-7c73d15ec0c8";
constexpr const char CHASSIS_DRAWER_PERSISTENT_UUID[] = "832ffd00-18f4-5094-9aa9-c2673b2c0173";

}

class ChassisTreeStabilizerTest : public ::testing::Test {
public:
    virtual ~ChassisTreeStabilizerTest();


    virtual void SetUp();


    virtual void TearDown();
};


ChassisTreeStabilizerTest::~ChassisTreeStabilizerTest() { }


void ChassisTreeStabilizerTest::SetUp() {
    // Set service UUID
    ServiceUuid::get_instance()->set_service_uuid(constants::SERIVCE_UUID);

    // Create parent manager and add it to the model
    Manager drawer_manager{};
    CommonComponents::get_instance()->get_module_manager().add_entry(drawer_manager);

    // Create chassis of type drawer and add it to the model
    Chassis drawer_chassis{drawer_manager.get_uuid()};
    drawer_chassis.set_type(enums::ChassisType::Drawer);
    CommonComponents::get_instance()->get_chassis_manager().add_entry(drawer_chassis);

    // Create children (sled) managers and add it to the model
    for (unsigned manager_id = 0u; manager_id < 4u; manager_id++) {
        Manager sled_manager{drawer_manager.get_uuid()};
        sled_manager.set_connection_data({manager_id, std::to_string(manager_id), "user", "pass"});

        CommonComponents::get_instance()->get_module_manager().add_entry(sled_manager);
    }

    auto blade_manager_keys = CommonComponents::get_instance()->
        get_module_manager().get_keys(drawer_manager.get_uuid());

    // Construct the rest of the model in analogy to chassis loader
    for (const auto& key: blade_manager_keys) {
        Chassis chassis{key};
        chassis.set_type(enums::ChassisType::Module);
        CommonComponents::get_instance()->get_chassis_manager().add_entry(chassis);

        Psu psu{chassis.get_uuid()};
        ChassisComponents::get_instance()->get_psu_manager().add_entry(psu);

        PowerZone power_zone{chassis.get_uuid()};
        ChassisComponents::get_instance()->get_power_zone_manager().add_entry(power_zone);

        ThermalZone thermal_zone{chassis.get_uuid()};
        ChassisComponents::get_instance()->get_thermal_zone_manager().add_entry(thermal_zone);

        Fan fan{chassis.get_uuid()};
        ChassisComponents::get_instance()->get_fan_manager().add_entry(fan);

        ChassisTreeStabilizer().stabilize(key);
    }
}


void ChassisTreeStabilizerTest::TearDown() {
    CommonComponents::get_instance()->get_module_manager().clear_entries();
    CommonComponents::get_instance()->get_chassis_manager().clear_entries();
    ChassisComponents::get_instance()->get_psu_manager().clear_entries();
    ChassisComponents::get_instance()->get_fan_manager().clear_entries();
    ChassisComponents::get_instance()->get_power_zone_manager().clear_entries();
    ChassisComponents::get_instance()->get_thermal_zone_manager().clear_entries();
}


TEST_F(ChassisTreeStabilizerTest, NoNewOrDeletedResourcesAndParentUUIDChangedTest) {
    // Assert that all resources are still present in the resource tree
    // One drawer manager should be present
    ASSERT_EQ(1, CommonComponents::get_instance()->get_module_manager().get_keys("").size());
    const std::string drawer_manager_uuid = CommonComponents::get_instance()->
        get_module_manager().get_keys("").front();

    // Four module managers should be present
    ASSERT_EQ(4, CommonComponents::get_instance()->
        get_module_manager().get_keys(drawer_manager_uuid).size());
    const auto module_managers_uuids = CommonComponents::get_instance()->
        get_module_manager().get_keys(drawer_manager_uuid);

    // One drawer chassis should be present
    ASSERT_EQ(1, CommonComponents::get_instance()->get_chassis_manager().get_keys(drawer_manager_uuid).size());

    // One sled chassis should be present under each sled manager
    for (const auto& module_manager_uuid : module_managers_uuids) {
        ASSERT_EQ(1, CommonComponents::get_instance()->get_chassis_manager().get_keys(module_manager_uuid).size());
    }
}


TEST_F(ChassisTreeStabilizerTest, PersistentUUIDsGeneratedTest) {
    // Assert that all resources now have persistent UUIDs
    ASSERT_NO_THROW(CommonComponents::get_instance()->
                    get_module_manager().get_entry(constants::DRAWER_MANAGER_PERSISTENT_UUID));
    ASSERT_NO_THROW(CommonComponents::get_instance()->
                    get_module_manager().get_entry(constants::SLED_MANAGER_1_PERSISTENT_UUID));
    ASSERT_NO_THROW(CommonComponents::get_instance()->
                    get_module_manager().get_entry(constants::SLED_MANAGER_2_PERSISTENT_UUID));
    ASSERT_NO_THROW(CommonComponents::get_instance()->
                    get_module_manager().get_entry(constants::SLED_MANAGER_3_PERSISTENT_UUID));
    ASSERT_NO_THROW(CommonComponents::get_instance()->
                    get_module_manager().get_entry(constants::SLED_MANAGER_4_PERSISTENT_UUID));
    ASSERT_NO_THROW(CommonComponents::get_instance()->
                    get_chassis_manager().get_entry(constants::CHASSIS_DRAWER_PERSISTENT_UUID));
}
