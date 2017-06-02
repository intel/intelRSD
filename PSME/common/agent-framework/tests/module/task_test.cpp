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
 * @section Task timestamp tests
 * */

#include "agent-framework/module/model/task.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/enum/common.hpp"

#include "gtest/gtest.h"

#include <json/value.h>



using namespace agent_framework::model;

class TaskTest : public ::testing::Test {
public:
    virtual ~TaskTest();


    virtual void SetUp();


    virtual void TearDown();


    Json::Value task_json{};
};


TaskTest::~TaskTest() { }


void TaskTest::SetUp() {
    task_json[literals::Task::NAME] = "repaint the submarine";
    task_json[literals::Task::STATE] = enums::TaskState(enums::TaskState::New).to_string();
    task_json[literals::Task::START_TIME] = "2016-07-15T07:01+01:00";
    task_json[literals::Task::END_TIME] = "2016-07-07T14:45+02:00";
    task_json[literals::Task::STATUS] = enums::Health(enums::Health::OK).to_string();
    task_json[literals::Task::MESSAGES] = Json::ValueType::arrayValue;

    Json::Value message{};
    message[literals::Message::MESSAGE_ID] = "Base.1.0.BadColor";
    message[literals::Message::MESSAGE] = "Cannot repaint the submarine";
    message[literals::Message::RELATED_PROPERTIES] = Json::ValueType::arrayValue;
    message[literals::Message::RELATED_PROPERTIES].append("Color");
    message[literals::Message::MESSAGE_ARGS] = Json::ValueType::arrayValue;
    message[literals::Message::MESSAGE_ARGS].append("ColorValue");
    message[literals::Message::SEVERITY] = enums::Health(enums::Health::Critical).to_string();
    message[literals::Message::RESOLUTION] = "The submarine must be yellow!";
    message[literals::Message::OEM] = Json::ValueType::nullValue;

    task_json[literals::Task::MESSAGES].append(message);
}


void TaskTest::TearDown() { }


TEST_F(TaskTest, ConversionFromJsonValuesExist) {
    Task task = Task::from_json(task_json);

    ASSERT_TRUE(task.get_start_time().has_value());
    ASSERT_TRUE(task.get_end_time().has_value());
    ASSERT_TRUE(task.get_name().has_value());
    ASSERT_TRUE(task.get_status().has_value());
    ASSERT_TRUE(task.get_state().has_value());
    ASSERT_FALSE(task.get_messages().empty());

    attribute::Message message = task.get_messages()[0];
    ASSERT_TRUE(message.get_message_id().has_value());
    ASSERT_TRUE(message.get_content().has_value());
    ASSERT_TRUE(message.get_severity().has_value());
    ASSERT_TRUE(message.get_resolution().has_value());
    ASSERT_FALSE(message.get_related_properties().empty());
    ASSERT_FALSE(message.get_message_args().empty());
}


TEST_F(TaskTest, ConversionFromJsonValuesCorrect) {
    const Task task = Task::from_json(task_json);
    ASSERT_EQ(task.get_start_time(), "2016-07-15T07:01+01:00");
    ASSERT_EQ(task.get_end_time(), "2016-07-07T14:45+02:00");
    ASSERT_EQ(task.get_name(), "repaint the submarine");
    ASSERT_EQ(task.get_status(), enums::Health::OK);
    ASSERT_EQ(task.get_state(),enums::TaskState::New);

    const attribute::Message message = task.get_messages()[0];
    ASSERT_EQ(message.get_message_id(), "Base.1.0.BadColor");
    ASSERT_EQ(message.get_content(), "Cannot repaint the submarine");
    ASSERT_EQ(message.get_severity(), enums::Health::Critical);
    ASSERT_EQ(message.get_resolution(), "The submarine must be yellow!");
}


TEST_F(TaskTest, StartSetsStartTime) {
    Task task{};
    ASSERT_FALSE(task.get_start_time().has_value());
    ASSERT_FALSE(task.get_end_time().has_value());

    ASSERT_THROW(task.stop(), std::logic_error);

    task.start();
    ASSERT_TRUE(task.get_start_time().has_value());
    ASSERT_FALSE(task.get_end_time().has_value());

    ASSERT_THROW(task.start(), std::logic_error);
}


TEST_F(TaskTest, StopSetsStopTime) {
    Task task{};
    ASSERT_FALSE(task.get_start_time().has_value());
    ASSERT_FALSE(task.get_end_time().has_value());
    ASSERT_THROW(task.stop(), std::logic_error);

    task.start();
    task.stop();

    ASSERT_TRUE(task.get_start_time().has_value());
    ASSERT_TRUE(task.get_end_time().has_value());
    ASSERT_THROW(task.stop(), std::logic_error);
}



