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


#include "gtest/gtest.h"

#include "loader/network_loader.hpp"
#include "configuration_full.hpp"
#include "json-wrapper/json-wrapper.hpp"


using namespace agent::network::loader;

class NetworkLoaderConfigFieldValidationTest: public ::testing::Test {
protected:
    NetworkLoader loader{};
    json::Json config = json::Json();
public:
    void SetUp() {
        config = json::Json::parse(NETWORK_FULL_CONFIGURATION);
    }

    virtual ~NetworkLoaderConfigFieldValidationTest();
};

// To avoid vtable warning.
NetworkLoaderConfigFieldValidationTest::~NetworkLoaderConfigFieldValidationTest() { }

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadFullConfiguration_LoadShouldBePositive) {

    bool is_loaded = loader.load(config);

    ASSERT_TRUE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutAgent_LoadShouldBeNegative) {
    config["agent"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutAgentVendor_LoadShouldBeNegative) {
    config["agent"]["vendor"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutAgentCapabilities_LoadShouldBeNegative) {
    config["agent"]["capabilities"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutServer_LoadShouldBeNegative) {
    config["server"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutServerPort_LoadShouldBeNegative) {
    config["server"]["port"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistration_LoadShouldBeNegative) {
    config["registration"]= json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistrationIpv4_LoadShouldBeNegative) {
    config["registration"]["ipv4"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistrationPort_LoadShouldBeNegative) {
    config["registration"]["port"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistrationInterval_LoadShouldBeNegative) {
    config["registration"]["interval"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutSlot_LoadShouldBePositive) {
    config["logger"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_TRUE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutManagers_LoadShouldBeNegative) {
    config["managers"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithZeroManagers_LoadShouldBeNegative) {
    config["managers"] = json::Json::value_t::array;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutIpv4_LoadShouldBeNegative) {
    // Remove ipv4 field from first manager.
    config["managers"].front()["ipv4"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NetworkLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutSerialConsole_LoadShouldBeNegative) {
    // Remove serialConsoleEnabled field from first manager.
    config["managers"].front()["serialConsoleEnabled"] = json::Json::value_t::null;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

