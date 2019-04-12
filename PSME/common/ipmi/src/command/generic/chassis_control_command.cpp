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
 *
 * @file chassis_control_command.cpp
 *
 * @brief IPMI Chassis Control Command
 * */

#include "ipmi/command/generic/chassis_control_command.hpp"
#include "ipmi/command/generic/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

/*
 * Mapping of PSME API and GAMI possible System power state to power levels
 * defined by IPMI specification.
 */
using PowerState = request::ChassisControlCommand::PowerState;

request::ChassisControlCommand::ChassisControlCommand() :
        Request(generic::NetFn::CHASSIS, generic::Cmd::CHASSIS_CONTROLL_COMMAND) {}
request::ChassisControlCommand::~ChassisControlCommand() {}

void request::ChassisControlCommand::pack(std::vector<std::uint8_t>& data) const {
    data.push_back(uint8_t(m_power_state));
}

void request::ChassisControlCommand::set_power_state(PowerState status) {
    m_power_state = status;
}

response::ChassisControlCommand::ChassisControlCommand() :
        Response(generic::NetFn::CHASSIS, generic::Cmd::CHASSIS_CONTROLL_COMMAND, RESPONSE_SIZE) {}
response::ChassisControlCommand::~ChassisControlCommand() {}

void response::ChassisControlCommand::unpack(const std::vector<std::uint8_t>&) {
}
