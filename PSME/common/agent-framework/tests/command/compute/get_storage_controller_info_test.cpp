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

#include "agent-framework/module/constants/compute.hpp"
#include "agent-framework/command-ref/compute_commands.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace agent_framework::command_ref;
using namespace agent_framework::model;
using namespace agent_framework::model::literals;

// FIXME this is not used? Tests don't pass!
// static constexpr enums::StorageProtocol TEST_INTERFACE{enums::StorageProtocol::PCIe};
static constexpr char TEST_SERIAL[] = "TestSerialNumber";
static constexpr char TEST_MANUFACTURER[] = "TestManufacturer";
static constexpr char TEST_MODEL[] = "TestModelNumber";
static constexpr char TEST_PART[] = "TestPartNumber";

class MyGetStorageControllerInfo {
private:
    std::string m_controller{};

public:
    MyGetStorageControllerInfo(
        const std::string& controller) {
        m_controller = controller;
        }

    void execute(const GetStorageControllerInfo::Request& request,
                 GetStorageControllerInfo::Response& response) {
        const auto& controller_uuid = request.get_uuid();

        if (controller_uuid != m_controller) {
            throw std::runtime_error("Not found");
        }

        agent_framework::model::StorageController storage_controller{};
        storage_controller.set_status(agent_framework::model::attribute::Status(enums::State::InTest, enums::Health::Warning));
        storage_controller.set_fru_info(agent_framework::model::attribute::FruInfo(
          TEST_SERIAL, TEST_MANUFACTURER, TEST_MODEL, TEST_PART));
        storage_controller.set_oem(agent_framework::model::attribute::Oem());
        response = std::move(storage_controller);
    }

    virtual ~MyGetStorageControllerInfo();
};

MyGetStorageControllerInfo::~MyGetStorageControllerInfo() { }

TEST(GetStorageControllerInfoTest, PositiveExecute) {
    MyGetStorageControllerInfo command{"TestControllerId"};
    GetStorageControllerInfo::Request request{""};
    GetStorageControllerInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::StorageController::CONTROLLER] = "TestControllerId";

    EXPECT_NO_THROW(request = GetStorageControllerInfo::Request::from_json(params));
    EXPECT_NO_THROW(command.execute(request, response));
    EXPECT_NO_THROW(result = response.to_json());

    ASSERT_TRUE(result.isObject());
    ASSERT_TRUE(result[Status::STATUS].isObject());
    ASSERT_TRUE(result[Status::STATUS][Status::STATE].isString());
    ASSERT_TRUE(result[Status::STATUS][Status::HEALTH].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO].isObject());
    ASSERT_TRUE(result[FruInfo::FRU_INFO][FruInfo::SERIAL].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO][FruInfo::MANUFACTURER].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO][FruInfo::MODEL].isString());
    ASSERT_TRUE(result[FruInfo::FRU_INFO][FruInfo::PART].isString());
    ASSERT_EQ(result[Status::STATUS][Status::STATE], "InTest");
    ASSERT_EQ(result[Status::STATUS][Status::HEALTH], "Warning");
    ASSERT_EQ(result[FruInfo::FRU_INFO][FruInfo::SERIAL].asString(), TEST_SERIAL);
    ASSERT_EQ(result[FruInfo::FRU_INFO][FruInfo::MANUFACTURER].asString(), TEST_MANUFACTURER);
    ASSERT_EQ(result[FruInfo::FRU_INFO][FruInfo::MODEL].asString(), TEST_MODEL);
    ASSERT_EQ(result[FruInfo::FRU_INFO][FruInfo::PART].asString(), TEST_PART);
}

TEST(GetStorageControllerInfoTest, NegativeModuleNotFound) {
    MyGetStorageControllerInfo command{"TestControllerId"};
    GetStorageControllerInfo::Request request{""};
    GetStorageControllerInfo::Response response{};
    json::Json params = json::Json();
    json::Json result = json::Json();

    params[literals::StorageController::CONTROLLER] = "OtherTestControllerId";

    EXPECT_NO_THROW(request = GetStorageControllerInfo::Request::from_json(params));
    EXPECT_THROW(command.execute(request, response), std::runtime_error);
}
