/*!
 * @copyright
 * Copyright (c) 2017 Intel Corporation
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
 * @file registration_data.cpp
 * */

#include "agent-framework/registration/registration_data.hpp"



using namespace agent_framework::generic;

namespace {

// @TODO: Use constants in loader and validation also. Waiting for AgentBase class...
const constexpr char REGISTRATION[] = "registration";
const constexpr char IPv4[] = "ipv4";
const constexpr char PORT[] = "port";
const constexpr char INTERVAL[] = "interval";

}


RegistrationData::RegistrationData(const json::Value& config) :
    m_ipv4_address(config[::REGISTRATION][::IPv4].as_string()),
    m_port(static_cast<std::uint16_t>(config[::REGISTRATION][::PORT].as_uint())),
    m_interval(config[::REGISTRATION][::INTERVAL].as_uint()) {
}


RegistrationData::~RegistrationData() {
}
