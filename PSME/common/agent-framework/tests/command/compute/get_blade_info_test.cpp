/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */

#include "agent-framework/command/compute/get_system_info.hpp"
#include "agent-framework/command/compute/json/get_system_info.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command;
using namespace agent_framework::command::exception;
using namespace agent_framework::model;
using namespace agent_framework::model::literals;

static constexpr char TEST_BIOS_VERSION[] = "Test Bios Version";
static constexpr enums::BootOverride TEST_BOOT_OVERRIDE
    { enums::BootOverride::Disabled };
static constexpr enums::BootOverrideTarget TEST_BOOT_OVERRIDE_TARGET
    { enums::BootOverrideTarget::None };
static constexpr enums::BootOverrideSupported TEST_BOOT_OVERRIDE_SUPPORTED_1
    { enums::BootOverrideSupported::Pxe };
static constexpr enums::BootOverrideSupported TEST_BOOT_OVERRIDE_SUPPORTED_2
    { enums::BootOverrideSupported::UefiTarget };
static constexpr char TEST_UEFI_TARGET[] = "Test uefi Target";
static constexpr enums::PowerState TEST_POWER_STATE {enums::PowerState::On};
static constexpr char TEST_SERIAL[] = "TestSerialNumber";
static constexpr char TEST_MANUFACTURER[] = "TestManufacturer";
static constexpr char TEST_MODEL[] = "TestModelNumber";
static constexpr char TEST_PART[] = "TestPartNumber";

class GetSystemInfo : public compute::GetSystemInfo {
private:
    std::string m_blade{};
public:
    GetSystemInfo(const std::string& blade) { m_blade = blade; }

    using compute::GetSystemInfo::execute;

    void execute(const Request& request, Response& response) {
        const auto& blade_uuid = request.get_system();

        if (blade_uuid != m_blade) {
            throw exception::NotFound();
        }

        agent_framework::model::System blade{};
        blade.set_status(agent_framework::model::attribute::Status(enums::State::InTest, enums::Health::Warning));
        blade.set_bios_version(TEST_BIOS_VERSION);
        blade.set_boot_override(TEST_BOOT_OVERRIDE);
        blade.set_boot_override_target(
                TEST_BOOT_OVERRIDE_TARGET);
        blade.set_boot_override_supported({{TEST_BOOT_OVERRIDE_SUPPORTED_1, TEST_BOOT_OVERRIDE_SUPPORTED_2}});
	blade.set_uefi_target(TEST_UEFI_TARGET);
        blade.set_power_state(TEST_POWER_STATE);
        blade.set_fru_info(agent_framework::model::attribute::FruInfo(
          TEST_SERIAL, TEST_MANUFACTURER, TEST_MODEL, TEST_PART));
        blade.set_oem(agent_framework::model::attribute::Oem());
        response.set_system(std::move(blade));
    }

    virtual ~GetSystemInfo();
};

GetSystemInfo::~GetSystemInfo() { }

class GetSystemInfoTest : public ::testing::Test {
public:
    virtual ~GetSystemInfoTest();
};

GetSystemInfoTest::~GetSystemInfoTest() { }

TEST_F(GetSystemInfoTest, PositiveExecute) {
    compute::json::GetSystemInfo command_json;
    GetSystemInfo* command = new GetSystemInfo("TestSystemId");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::System::SYSTEM] = "TestSystemId";

    EXPECT_NO_THROW(command_json.method(params, result));

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[Status::STATUS].isObject());
    ASSERT_TRUE(result[Status::STATUS][Status::STATE].isString());
    ASSERT_TRUE(result[Status::STATUS][Status::HEALTH].isString());
    ASSERT_TRUE(result[literals::System::BIOS_VERSION].isString());
    ASSERT_TRUE(result[literals::System::BOOT_OVERRIDE].isString());
    ASSERT_TRUE(result[literals::System::BOOT_OVERRIDE_TARGET].isString());
    ASSERT_TRUE(result[literals::System::BOOT_OVERRIDE_SUPPORTED].isArray());
    ASSERT_TRUE(result[literals::System::BOOT_OVERRIDE_SUPPORTED][0].isString());
    ASSERT_TRUE(result[literals::System::BOOT_OVERRIDE_SUPPORTED][1].isString());
    ASSERT_TRUE(result[literals::System::UEFI_TARGET].isString());
    ASSERT_TRUE(result[literals::System::POWER_STATE].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO].isObject());
    ASSERT_TRUE(result[FruInfo::FRU_INFO][FruInfo::SERIAL].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO][FruInfo::MANUFACTURER].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO][FruInfo::MODEL].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO][FruInfo::PART].isString());
    ASSERT_EQ(result[Status::STATUS][Status::STATE], "InTest");
    ASSERT_EQ(result[Status::STATUS][Status::HEALTH], "Warning");
    ASSERT_EQ(result[literals::System::BIOS_VERSION].asString(), TEST_BIOS_VERSION);
    ASSERT_EQ(result[literals::System::BOOT_OVERRIDE].asString(), TEST_BOOT_OVERRIDE.to_string());
    ASSERT_EQ(result[literals::System::BOOT_OVERRIDE_TARGET].asString(), TEST_BOOT_OVERRIDE_TARGET.to_string());
    ASSERT_EQ(result[literals::System::BOOT_OVERRIDE_SUPPORTED][0].asString(), TEST_BOOT_OVERRIDE_SUPPORTED_1.to_string());
    ASSERT_EQ(result[literals::System::BOOT_OVERRIDE_SUPPORTED][1].asString(), TEST_BOOT_OVERRIDE_SUPPORTED_2.to_string());
    ASSERT_EQ(result[literals::System::UEFI_TARGET].asString(), TEST_UEFI_TARGET);
    ASSERT_EQ(result[literals::System::POWER_STATE].asString(), TEST_POWER_STATE.to_string());
    ASSERT_EQ(result[FruInfo::FRU_INFO][FruInfo::SERIAL].asString(), TEST_SERIAL);
    ASSERT_EQ(result[FruInfo::FRU_INFO][FruInfo::MANUFACTURER].asString(), TEST_MANUFACTURER);
    ASSERT_EQ(result[FruInfo::FRU_INFO][FruInfo::MODEL].asString(), TEST_MODEL);
    ASSERT_EQ(result[FruInfo::FRU_INFO][FruInfo::PART].asString(), TEST_PART);
}

TEST_F(GetSystemInfoTest, NegativeModuleNotFound) {
    compute::json::GetSystemInfo command_json;
    GetSystemInfo* command = new GetSystemInfo("TestSystemId");

    EXPECT_NO_THROW(command_json.set_command(command));

    Json::Value params;
    Json::Value result;

    params[literals::System::SYSTEM] = "OtherTestSystemId";

    EXPECT_THROW(command_json.method(params, result), exception::NotFound);
}
