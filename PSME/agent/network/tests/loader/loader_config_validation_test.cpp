/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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

#include "loader/network_loader.hpp"
#include "configuration_full.hpp"
#include "json/value.hpp"
#include "json/deserializer.hpp"

using namespace agent::network::loader;

class NetworkLoaderConfigFieldValidationTest: public ::testing::Test {
protected:
    NetworkLoader loader{};
    json::Value config;
    json::Deserializer deserializer{};
public:
    void SetUp() {
        deserializer << NETWORK_FULL_CONFIGURATION;
    }

    virtual ~NetworkLoaderConfigFieldValidationTest();
};

// To avoid vtable warning.
NetworkLoaderConfigFieldValidationTest::~NetworkLoaderConfigFieldValidationTest() { }

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadFullConfiguration_LoadShouldBePositive) {
    deserializer >> config;

    bool is_loaded = loader.load(config);

    ASSERT_TRUE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutAgent_LoadShouldBeNegative) {
    deserializer >> config;

    config["agent"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutAgentVendor_LoadShouldBeNegative) {
    deserializer >> config;

    config["agent"]["vendor"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutAgentCapabilities_LoadShouldBeNegative) {
    deserializer >> config;

    config["agent"]["capabilities"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutServer_LoadShouldBeNegative) {
    deserializer >> config;

    config["server"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutServerPort_LoadShouldBeNegative) {
    deserializer >> config;

    config["server"]["port"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistration_LoadShouldBeNegative) {
    deserializer >> config;

    config["registration"]= json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistrationIpv4_LoadShouldBeNegative) {
    deserializer >> config;

    config["registration"]["ipv4"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistrationPort_LoadShouldBeNegative) {
    deserializer >> config;

    config["registration"]["port"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistrationInterval_LoadShouldBeNegative) {
    deserializer >> config;

    config["registration"]["interval"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutSlot_LoadShouldBePositive) {
    deserializer >> config;

    config["logger"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_TRUE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutManagers_LoadShouldBeNegative) {
    deserializer >> config;

    config["managers"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithZeroManagers_LoadShouldBeNegative) {
    deserializer >> config;

    config["managers"] = json::Value::Type::ARRAY;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutIpv4_LoadShouldBeNegative) {
    deserializer >> config;

    // Remove ipv4 field from first manager.
    config["managers"].as_array().front()["ipv4"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutSerialConsole_LoadShouldBeNegative) {
    deserializer >> config;

    // Remove serialConsoleEnabled field from first manager.
    config["managers"].as_array().front()["serialConsoleEnabled"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

