/*!
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
 *
 * @file port_test.cpp
 *
 * @brief Unit test for Port class
 * */

#include "agent-framework/module/port.hpp"
#include "gtest/gtest.h"
#include "json/json.hpp"
#include <uuid++.hh>

using namespace agent_framework::generic;

/* Testing configuration */
static constexpr const char FULL_PORT_CONFIGURATION[] =
R"({
    "id" : 1,
    "link_state" : "up",
    "type": "test",
    "ethmod" : "AN_73",
    "autoneg" : "clause_73"
})";
static constexpr const char PORT_CONFIGURATION[] =
R"({ "id" : 2, "link_state" : "up" })";
static constexpr const char WRONG_LINK_STATE_CONFIGURATION[] =
R"({ "id" : 2, "link_state" : "test" })";


/* Positive tests */
/* read_configuration tests */

TEST(PortTest, PositiveReadConfiguration) {
    const std::string default_string = "default";
    const std::string empty_string = "";
    json::Value port_configuration;
    PORT_CONFIGURATION >> port_configuration;

    auto port = Port::make_port();
    ASSERT_NE(port.get(), nullptr);
    EXPECT_NO_THROW(port->read_configuration(port_configuration));

    ASSERT_EQ(port_configuration["id"].as_uint(), port->get_id());

    const auto& link_state = port_configuration["link_state"].as_string();
    if(link_state == "up") {
        ASSERT_EQ(Port::LinkState::UP, port->get_link_state());
    }
    else if(link_state == "down") {
        ASSERT_EQ(Port::LinkState::DOWN, port->get_link_state());
    }
    else {
        ASSERT_EQ(Port::LinkState::UNKNOWN, port->get_link_state());
    }

    ASSERT_EQ(default_string, port->get_iface_mode());
    ASSERT_EQ(empty_string, port->get_type());
    ASSERT_EQ(default_string, port->get_autoneg_mode());
}

TEST(PortTest, PositiveFullReadConfiguration) {
    json::Value port_configuration;
    FULL_PORT_CONFIGURATION >> port_configuration;

    auto port = Port::make_port();
    ASSERT_NE(port.get(), nullptr);
    EXPECT_NO_THROW(port->read_configuration(port_configuration));

    ASSERT_EQ(port_configuration["id"].as_uint(), port->get_id());

    const auto& link_state = port_configuration["link_state"].as_string();
    if(link_state == "up") {
        ASSERT_EQ(Port::LinkState::UP, port->get_link_state());
    }
    else if(link_state == "down") {
        ASSERT_EQ(Port::LinkState::DOWN, port->get_link_state());
    }
    else {
        ASSERT_EQ(Port::LinkState::UNKNOWN, port->get_link_state());
    }

    ASSERT_EQ(port_configuration["type"].as_string(), port->get_type());
    ASSERT_EQ(port_configuration["ethmod"].as_string(), port->get_iface_mode());
    ASSERT_EQ(port_configuration["autoneg"].as_string(),
                port->get_autoneg_mode());
}

/* Negative tests */
/* read_configuration tests */

TEST(PortTest, NegativeReadConfiguration) {
    const std::string empty_string = "";
    constexpr const unsigned int default_uint{};
    json::Value port_configuration;

    auto port = Port::make_port();
    ASSERT_NE(port.get(), nullptr);
    EXPECT_NO_THROW(port->read_configuration(port_configuration));

    ASSERT_EQ(default_uint, port->get_id());
    /* DOWN is default state of link */
    ASSERT_EQ(Port::LinkState::DOWN, port->get_link_state());
    ASSERT_EQ(empty_string, port->get_iface_mode());
    ASSERT_EQ(empty_string, port->get_type());
    ASSERT_EQ(empty_string, port->get_autoneg_mode());
}

TEST(PortTest, NegativeWrongLinkStateConfiguration) {
    const std::string default_string = "default";
    const std::string empty_string = "";
    json::Value port_configuration;
    WRONG_LINK_STATE_CONFIGURATION >> port_configuration;

    auto port = Port::make_port();
    ASSERT_NE(port.get(), nullptr);
    EXPECT_NO_THROW(port->read_configuration(port_configuration));

    ASSERT_EQ(port_configuration["id"].as_uint(), port->get_id());
    ASSERT_EQ(Port::LinkState::UNKNOWN, port->get_link_state());
    ASSERT_EQ(default_string, port->get_iface_mode());
    ASSERT_EQ(empty_string, port->get_type());
    ASSERT_EQ(default_string, port->get_autoneg_mode());
}
