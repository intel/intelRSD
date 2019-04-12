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
 * @file unknown_command.cpp
 * @brief IPMI unknown command.
 * */

#include <ipmb/command/unknown_command.hpp>
#include <ipmb/ipmi_message.hpp>
#include <ipmb/utils.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb;
using namespace agent::chassis::ipmb::command;

UnknownCommand::~UnknownCommand() {}

void UnknownCommand::Response::add_data(IpmiMessage& msg) {
    auto data = msg.get_data();

    data[OFFSET_CC] = uint8_t(m_cc);

    msg.add_len(get_len());
}

void UnknownCommand::unpack(IpmiMessage& msg){
    log_debug(LOGUSR, "Unpacking unknown message.");
    msg.set_to_request();
}

void UnknownCommand::pack(IpmiMessage& msg){

    log_debug(LOGUSR, "Packing unknown message.");

    m_response.set_cc(CompletionCode::CC_INVALID_CMD);

    msg.set_to_response();
    m_response.set_len(CMD_RESPONSE_DATA_LENGHT);
    m_response.add_data(msg);
}


