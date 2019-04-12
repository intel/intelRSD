/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or impliedev.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section EnumStateMachineTest
 * */


#include "state_machine/enum_state_machine.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace agent::pnc::state_machine;

enum class TestState : uint8_t { Initial, State1, State2, GT0, LT0 };
enum class TestEvent : uint8_t {
    InitFailedTransition, InitFailedGuard, InitOk, NotUsed, Check0, ToState2};

class TestStateMachine : public EnumStateMachine<TestState, TestEvent> {
public:

    TestStateMachine(const TestState& init_state, const TestStateMachine::TransitionTable& transition_table):
        EnumStateMachine<TestState, TestEvent>(init_state, transition_table) {}

    virtual ~TestStateMachine();

    virtual bool do_guard_check(const Transition& transition) {
        ++m_cg;
        return EnumStateMachine<TestState, TestEvent>::do_guard_check(transition);
    }

    virtual bool do_transition(const Transition& transition) {
        ++m_dt;
        return EnumStateMachine<TestState, TestEvent>::do_transition(transition);
    }

    virtual void do_on_event_action(const TestEvent& ev) {
        ++m_pec;
        EnumStateMachine<TestState, TestEvent>::do_on_event_action(ev);
    }

    int m_cg{0};
    int m_dt{0};
    int m_pec{0};
};

TestStateMachine::~TestStateMachine() {}

using TS = TestState;
using TE = TestEvent;
using TT = TestStateMachine::Transition;

class EnumStateMachineTest: public ::testing::Test {
public:
    EnumStateMachineTest() {}
    virtual ~EnumStateMachineTest();

protected:

    void SetUp() override {
        sm.reset(new TestStateMachine( TS::Initial, TestStateMachine::TransitionTable( {
            // transitions for various initial checks
            TT{ TS::Initial, TE::InitOk,               TS::State1, always_true                    },
            TT{ TS::Initial, TE::InitFailedTransition, TS::State1, always_false                   },
            TT{ TS::Initial, TE::InitFailedGuard,      TS::State1, always_true,  always_false     },
            // transitions for testing guards
            TT{ TS::State1,  TE::Check0,               TS::GT0,    always_true,  greater_zero     },
            TT{ TS::State1,  TE::Check0,               TS::LT0,    always_true,  not_greater_zero },
            TT{ TS::GT0,     TE::Check0,               TS::GT0,    always_true,  greater_zero     },
            TT{ TS::GT0,     TE::Check0,               TS::LT0,    always_true,  not_greater_zero },
            TT{ TS::LT0,     TE::Check0,               TS::GT0,    always_true,  greater_zero     },
            TT{ TS::LT0,     TE::Check0,               TS::LT0,    always_true,  not_greater_zero }
        } )));
    }

    void TearDown() override {
        sm.reset(nullptr);
    }

    static bool always_true(const TT&) {
        return true;
    }

    static bool always_false(const TT&) {
        return false;
    }

    static bool greater_zero(const TT&) {
        return zero_test > 0;
    }

    static bool not_greater_zero(const TT&) {
        return zero_test <= 0;
    }

    static int zero_test;

    std::unique_ptr<TestStateMachine> sm;
};

int EnumStateMachineTest::zero_test = 0;

EnumStateMachineTest::~EnumStateMachineTest() {}

TEST_F(EnumStateMachineTest, SimpleGuardsAndTransitionsAreWorking) {
    EXPECT_EQ(sm->get_current_state(), TS::Initial);
    EXPECT_EQ(true, sm->send_event(TE::ToState2));
    EXPECT_EQ(sm->get_current_state(), TS::Initial);
    sm->add_transition(TS::Initial, TE::ToState2, TS::State2);
    EXPECT_EQ(true, sm->send_event(TE::ToState2));
    EXPECT_EQ(sm->get_current_state(), TS::State2);
}

TEST_F(EnumStateMachineTest, AddTransitionCommandIsWorking) {
    EXPECT_EQ(sm->get_current_state(), TS::Initial);
    EXPECT_EQ(true, sm->send_event(TE::NotUsed));
    EXPECT_EQ(sm->get_current_state(), TS::Initial);
    EXPECT_EQ(false, sm->send_event(TE::InitFailedTransition));
    EXPECT_EQ(sm->get_current_state(), TS::Initial);
    EXPECT_EQ(true, sm->send_event(TE::InitFailedGuard));
    EXPECT_EQ(sm->get_current_state(), TS::Initial);
    EXPECT_EQ(true, sm->send_event(TE::InitOk));
    EXPECT_EQ(sm->get_current_state(), TS::State1);
}

TEST_F(EnumStateMachineTest, DoOnEventActionTemplateMethodIsCalled) {
    EXPECT_EQ(sm->m_pec, 0);
    EXPECT_EQ(true, sm->send_event(TE::NotUsed));
    EXPECT_EQ(sm->m_pec, 1);
    EXPECT_EQ(false, sm->send_event(TE::InitFailedTransition));
    EXPECT_EQ(sm->m_pec, 2);
    EXPECT_EQ(true, sm->send_event(TE::InitFailedGuard));
    EXPECT_EQ(sm->m_pec, 3);
    EXPECT_EQ(true, sm->send_event(TE::InitOk));
    EXPECT_EQ(sm->m_pec, 4);
}

TEST_F(EnumStateMachineTest, DoGuardCheckTemplateMethodIsCalled) {
    EXPECT_EQ(sm->m_cg, 0);
    EXPECT_EQ(true, sm->send_event(TE::NotUsed));
    EXPECT_EQ(sm->m_cg, 0);
    EXPECT_EQ(false, sm->send_event(TE::InitFailedTransition));
    EXPECT_EQ(sm->m_cg, 1);
    EXPECT_EQ(true, sm->send_event(TE::InitFailedGuard));
    EXPECT_EQ(sm->m_cg, 2);
    EXPECT_EQ(true, sm->send_event(TE::InitOk));
    EXPECT_EQ(sm->m_cg, 3);
}

TEST_F(EnumStateMachineTest, DoTransitionTemplateMethodIsCalled) {
    EXPECT_EQ(sm->m_dt, 0);
    EXPECT_EQ(true, sm->send_event(TE::NotUsed));
    EXPECT_EQ(sm->m_dt, 0);
    EXPECT_EQ(false, sm->send_event(TE::InitFailedTransition));
    EXPECT_EQ(sm->m_dt, 1);
    EXPECT_EQ(true, sm->send_event(TE::InitFailedGuard));
    EXPECT_EQ(sm->m_dt, 1);
    EXPECT_EQ(true, sm->send_event(TE::InitOk));
    EXPECT_EQ(sm->m_dt, 2);
}

TEST_F(EnumStateMachineTest, GuardsWorkCorrectly) {
    EXPECT_EQ(sm->get_current_state(), TS::Initial);
    EXPECT_EQ(true, sm->send_event(TE::InitOk));
    EXPECT_EQ(sm->get_current_state(), TS::State1);
    zero_test = 1;
    EXPECT_EQ(true, sm->send_event(TE::Check0));
    EXPECT_EQ(sm->get_current_state(), TS::GT0);
    EXPECT_EQ(true, sm->send_event(TE::Check0));
    EXPECT_EQ(sm->get_current_state(), TS::GT0);
    zero_test = -1;
    EXPECT_EQ(true, sm->send_event(TE::Check0));
    EXPECT_EQ(sm->get_current_state(), TS::LT0);
    EXPECT_EQ(true, sm->send_event(TE::Check0));
    EXPECT_EQ(sm->get_current_state(), TS::LT0);
    zero_test = 1;
    EXPECT_EQ(true, sm->send_event(TE::Check0));
    EXPECT_EQ(sm->get_current_state(), TS::GT0);
}
