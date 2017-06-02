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


#include "gtest/gtest.h"

#include "loader/compute_loader.hpp"
#include "configuration_full.hpp"
#include "json/value.hpp"
#include "json/deserializer.hpp"

using namespace agent::compute::loader;

class ComputeLoaderConfigFieldValidationTest: public ::testing::Test {
protected:
    ComputeLoader loader{};
    json::Value config;
    json::Deserializer deserializer{};
public:
    void SetUp() {
        deserializer << COMPUTE_FULL_CONFIGURATION;
    }

    virtual ~ComputeLoaderConfigFieldValidationTest();
};

// To avoid vtable warning.
ComputeLoaderConfigFieldValidationTest::~ComputeLoaderConfigFieldValidationTest() { }

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadFullConfiguration_LoadShouldBePositive) {
    deserializer >> config;

    bool is_loaded = loader.load(config);

    ASSERT_TRUE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutAgent_LoadShouldBeNegative) {
    deserializer >> config;

    config["agent"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutAgentVendor_LoadShouldBeNegative) {
    deserializer >> config;

    config["agent"]["vendor"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutAgentCapabilities_LoadShouldBeNegative) {
    deserializer >> config;

    config["agent"]["capabilities"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutServer_LoadShouldBeNegative) {
    deserializer >> config;

    config["server"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutServerPort_LoadShouldBeNegative) {
    deserializer >> config;

    config["server"]["port"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutRegistration_LoadShouldBeNegative) {
    deserializer >> config;

    config["registration"]= json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutRegistrationIpv4_LoadShouldBeNegative) {
    deserializer >> config;

    config["registration"]["ipv4"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutRegistrationPort_LoadShouldBeNegative) {
    deserializer >> config;

    config["registration"]["port"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutRegistrationInterval_LoadShouldBeNegative) {
    deserializer >> config;

    config["registration"]["interval"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithManagerWithoutSlot_LoadShouldBePositive) {
    deserializer >> config;

    config["logger"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_TRUE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithoutManagers_LoadShouldBeNegative) {
    deserializer >> config;

    config["managers"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithZeroManagers_LoadShouldBeNegative) {
    deserializer >> config;

    config["managers"] = json::Value::Type::ARRAY;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithManagerWithoutIpv4_LoadShouldBeNegative) {
    deserializer >> config;

    // Remove ipv4 field from first manager.
    config["managers"].as_array().front()["ipv4"] = json::Value::Type::ARRAY;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithManagerWithoutUsername_LoadShouldBeNegative) {
    deserializer >> config;

    // Remove username field from first manager.
    config["managers"].as_array().front()["username"] = json::Value::Type::ARRAY;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithManagerWithoutPassword_LoadShouldBeNegative) {
    deserializer >> config;

    // Remove password field from first manager.
    config["managers"].as_array().front()["password"] = json::Value::Type::ARRAY;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithManagerWithoutPort_LoadShouldBeNegative) {
    deserializer >> config;

    // Remove port field from first manager.
    config["managers"].as_array().front()["port"] = json::Value::Type::ARRAY;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithManagerWithoutSlot_LoadShouldBeNegative) {
    deserializer >> config;

    // Remove slot field from first manager.
    config["managers"].as_array().front()["slot"] = json::Value::Type::ARRAY;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(ComputeLoaderConfigFieldValidationTest, LoadConfigurationWithManagerWithoutSwitchPort_LoadShouldBeNegative) {
    deserializer >> config;

    // Remove switch port identifier field from first manager.
    config["managers"].as_array().front()["switchPortIdentifier"] = json::Value::Type::ARRAY;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}
