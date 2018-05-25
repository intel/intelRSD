/*!
 * @brief Implementation of validation tests.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Files}
 * @file loader_validation_test.cpp
 */

#include "gtest/gtest.h"

#include "loader/loader.hpp"
#include "configuration_full.hpp"
#include "json/value.hpp"
#include "json/deserializer.hpp"

using namespace agent::nvme::loader;

class NvmeLoaderConfigFieldValidationTest: public ::testing::Test {
protected:
    NvmeLoader loader{};
    json::Value config;
    json::Deserializer deserializer{};
public:
    void SetUp() {
        deserializer << NVME_FULL_CONFIGURATION;
    }

    virtual ~NvmeLoaderConfigFieldValidationTest();
};

NvmeLoaderConfigFieldValidationTest::~NvmeLoaderConfigFieldValidationTest() { }

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadFullConfiguration_LoadShouldBePositive) {
    deserializer >> config;

    bool is_loaded = loader.load(config);

    ASSERT_TRUE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutAgent_LoadShouldBeNegative) {
    deserializer >> config;

    config["agent"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutAgentVendor_LoadShouldBeNegative) {
    deserializer >> config;

    config["agent"]["vendor"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutAgentCapabilities_LoadShouldBeNegative) {
    deserializer >> config;

    config["agent"]["capabilities"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutServer_LoadShouldBeNegative) {
    deserializer >> config;

    config["server"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutServerPort_LoadShouldBeNegative) {
    deserializer >> config;

    config["server"]["port"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistration_LoadShouldBeNegative) {
    deserializer >> config;

    config["registration"]= json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistrationIpv4_LoadShouldBeNegative) {
    deserializer >> config;

    config["registration"]["ipv4"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistrationPort_LoadShouldBeNegative) {
    deserializer >> config;

    config["registration"]["port"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutRegistrationInterval_LoadShouldBeNegative) {
    deserializer >> config;

    config["registration"]["interval"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutLogger_LoadShouldBePositive) {
    deserializer >> config;

    config["logger"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_TRUE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithoutManagers_LoadShouldBeNegative) {
    deserializer >> config;

    config["managers"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithZeroManagers_LoadShouldBeNegative) {
    deserializer >> config;

    config["managers"] = json::Value::Type::ARRAY;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutIpv4_LoadShouldBeNegative) {
    deserializer >> config;

    config["managers"].as_array().front()["ipv4"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutlocation_LoadShouldBeNegative) {
    deserializer >> config;

    config["managers"].as_array().front()["locationOffset"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutParent_LoadShouldBeNegative) {
    deserializer >> config;

    config["managers"].as_array().front()["parentId"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutNicDriver_LoadShouldBeNegative) {
    deserializer >> config;

    config["managers"].as_array().front()["nic-drivers"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}

TEST_F(NvmeLoaderConfigFieldValidationTest,
       LoadConfigurationWithManagerWithoutTarget_LoadShouldBeNegative) {
    deserializer >> config;

    config["managers"].as_array().front()["is-target"] = json::Value::Type::NIL;

    bool is_loaded = loader.load(config);

    ASSERT_FALSE(is_loaded);
}
