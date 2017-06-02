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

#include <cstdio>

#include "../mocks/mock_logger_ext.hpp"
#include "../mocks/mock_module_status.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "agent-framework/state_machine/state_machine.hpp"
#include "agent-framework/state_machine/state_machine_exception.hpp"
#include "agent-framework/state_machine/state_machine_transition.hpp"
#include "agent-framework/status/module_status.hpp"
#include "agent-framework/status/module_hardware_status.hpp"
#include "agent-framework/status/module_software_status.hpp"

using namespace agent_framework::state_machine;
using namespace agent_framework::status;
using namespace agent_framework::testing;
using ::testing::Return;
using ::testing::Throw;


class StateMachineTest : public ::testing::Test {
protected:
    StateMachine state_machine_unknown;
    StateMachine state_machine_up;
    StateMachine state_machine_down;
    StateMachine state_machine_missing;

    ModuleStatus::Status hw_status = ModuleStatus::Status::UNKNOWN;
    ModuleStatus::Status sw_status = ModuleStatus::Status::UNKNOWN;

    logger_cpp::Logger* LOGUSRMOCK = nullptr;

    virtual void SetUp() {

        LOGUSRMOCK = new MockLogger;

        /* State Machine in Unknown state. */
        state_machine_unknown.start();

        /* State Machine in Missing state. */
        state_machine_missing.start();

        hw_status = ModuleStatus::Status::NOT_PRESENT;
        sw_status = ModuleStatus::Status::NOT_PRESENT;

        state_machine_missing.set_next_state(hw_status, sw_status);

        /* State Machine in Up state. */
        state_machine_up.start();

        hw_status = ModuleStatus::Status::PRESENT;
        sw_status = ModuleStatus::Status::PRESENT;

        state_machine_up.set_next_state(hw_status, sw_status);

        /* State Machine in Down state. */
        state_machine_down.start();

        hw_status = ModuleStatus::Status::PRESENT;
        sw_status = ModuleStatus::Status::NOT_PRESENT;

        state_machine_down.set_next_state(hw_status, sw_status);
    }

    virtual void TearDown() {
        state_machine_unknown.stop();
        state_machine_up.stop();
        state_machine_down.stop();
        state_machine_missing.stop();
        delete LOGUSRMOCK;
    }

    virtual ~StateMachineTest();
};

StateMachineTest::~StateMachineTest() {}

/* UNKNOWN */

TEST_F(StateMachineTest, PositiveCreateInUnknown) {
    StateMachine *state_machine = nullptr;

    state_machine = &state_machine_unknown;

    ASSERT_EQ(state_machine->get_transition(),
            enums::Transition::IDLE);
    ASSERT_EQ(state_machine->get_state(), enums::State::UNKNOWN);
}

TEST_F(StateMachineTest, PositiveTransitionFromUnknownToMissing) {
    StateMachine *state_machine = nullptr;

    state_machine = &state_machine_missing;

    ASSERT_EQ(state_machine->get_transition(),
            enums::Transition::DISCOVERY_MISSING);
    ASSERT_EQ(state_machine->get_state(), enums::State::ABSENT);
}

TEST_F(StateMachineTest, PositiveTransitionFromUnknownToUp) {
    StateMachine *state_machine = nullptr;

    state_machine = &state_machine_up;

    ASSERT_EQ(state_machine->get_transition(),
            enums::Transition::DISCOVERY_UP);
    ASSERT_EQ(state_machine->get_state(), enums::State::ENABLED);
}

TEST_F(StateMachineTest, PositiveTransitionFromUnknownToDown) {
    StateMachine *state_machine = nullptr;

    state_machine = &state_machine_down;

    ASSERT_EQ(state_machine->get_transition(),
            enums::Transition::DISCOVERY_DOWN);
    ASSERT_EQ(state_machine->get_state(), enums::State::UNAVAILABLE_OFFLINE);
}

TEST_F(StateMachineTest, PositiveTransitiontoMissingWhenStatusUnknown) {
    StateMachine *state_machine = nullptr;

    state_machine = &state_machine_unknown;

    hw_status = ModuleStatus::Status::UNKNOWN;
    sw_status = ModuleStatus::Status::UNKNOWN;

    state_machine->set_next_state(hw_status, sw_status);

    ASSERT_EQ(state_machine->get_transition(),
            enums::Transition::DISCOVERY_MISSING);
    ASSERT_EQ(state_machine->get_state(), enums::State::ABSENT);
}

/* MISSING */

TEST_F(StateMachineTest, PositiveTransitionFromMissingToUp) {
    StateMachine *state_machine = nullptr;

    state_machine = &state_machine_missing;

    hw_status = ModuleStatus::Status::PRESENT;
    sw_status = ModuleStatus::Status::PRESENT;

    state_machine->set_next_state(hw_status, sw_status);

    ASSERT_EQ(state_machine->get_transition(),
            enums::Transition::INSERTION);
    ASSERT_EQ(state_machine->get_state(), enums::State::ENABLED);
}

TEST_F(StateMachineTest, PositiveTransitionStayInMissing) {
    StateMachine *state_machine = nullptr;

    state_machine = &state_machine_missing;

    hw_status = ModuleStatus::Status::NOT_PRESENT;
    sw_status = ModuleStatus::Status::NOT_PRESENT;

    state_machine->set_next_state(hw_status, sw_status);

    ASSERT_EQ(state_machine->get_transition(),
            enums::Transition::IDLE);
    ASSERT_EQ(state_machine->get_state(), enums::State::ABSENT);
}

/* DOWN */

TEST_F(StateMachineTest, PositiveTransitionFromDownToMissing) {
    StateMachine *state_machine = nullptr;

    state_machine = &state_machine_down;

    hw_status = ModuleStatus::Status::NOT_PRESENT;
    sw_status = ModuleStatus::Status::NOT_PRESENT;

    state_machine->set_next_state(hw_status, sw_status);

    ASSERT_EQ(state_machine->get_transition(),
            enums::Transition::EXTRACTION);
    ASSERT_EQ(state_machine->get_state(), enums::State::ABSENT);
}

TEST_F(StateMachineTest, PositiveTransitionFromDownToUp) {
    StateMachine *state_machine = nullptr;

    state_machine = &state_machine_down;

    hw_status = ModuleStatus::Status::PRESENT;
    sw_status = ModuleStatus::Status::PRESENT;

    state_machine->set_next_state(hw_status, sw_status);

    ASSERT_EQ(state_machine->get_transition(),
            enums::Transition::CAME_UP);
    ASSERT_EQ(state_machine->get_state(), enums::State::ENABLED);
}

TEST_F(StateMachineTest, PositiveTransitionStayInDown) {
    StateMachine *state_machine = nullptr;

    state_machine = &state_machine_down;

    hw_status = ModuleStatus::Status::PRESENT;
    sw_status = ModuleStatus::Status::NOT_PRESENT;

    state_machine->set_next_state(hw_status, sw_status);

    ASSERT_EQ(state_machine->get_transition(),
            enums::Transition::IDLE);
    ASSERT_EQ(state_machine->get_state(), enums::State::UNAVAILABLE_OFFLINE);
}

/* UP */

TEST_F(StateMachineTest, PositiveTransitionFromUpToMissing) {
    StateMachine *state_machine = nullptr;

    state_machine = &state_machine_up;

    hw_status = ModuleStatus::Status::NOT_PRESENT;
    sw_status = ModuleStatus::Status::NOT_PRESENT;

    state_machine->set_next_state(hw_status, sw_status);

    ASSERT_EQ(state_machine->get_transition(),
            enums::Transition::EXTRACTION);
    ASSERT_EQ(state_machine->get_state(), enums::State::ABSENT);
}

TEST_F(StateMachineTest, PositiveTransitionFromUpToDown) {
    StateMachine *state_machine = nullptr;

    state_machine = &state_machine_up;

    hw_status = ModuleStatus::Status::PRESENT;
    sw_status = ModuleStatus::Status::NOT_PRESENT;

    state_machine->set_next_state(hw_status, sw_status);

    ASSERT_EQ(state_machine->get_transition(),
            enums::Transition::WENT_DOWN);
    ASSERT_EQ(state_machine->get_state(), enums::State::UNAVAILABLE_OFFLINE);
}

TEST_F(StateMachineTest, PositiveTransitionStayInUp) {
    StateMachine *state_machine = nullptr;

    state_machine = &state_machine_up;

    hw_status = ModuleStatus::Status::PRESENT;
    sw_status = ModuleStatus::Status::PRESENT;

    state_machine->set_next_state(hw_status, sw_status);

    ASSERT_EQ(state_machine->get_transition(),
            enums::Transition::IDLE);
    ASSERT_EQ(state_machine->get_state(), enums::State::ENABLED);
}

TEST_F(StateMachineTest, PositiveStateMachineErrorMessage) {

    const char *msg = "Test message.";
    StateMachineError *error = new StateMachineError(msg);

    ASSERT_STREQ(msg, error->what());

    delete error;
}


