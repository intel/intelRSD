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
 * @file ipmb/message_handler.cpp
 * @brief IPMI message handler.
 * */

#include <ipmb/dispatcher.hpp>
#include <ipmb/utils.hpp>
#include <ipmb/command/command.hpp>
#include <ipmb/command/command_factory.hpp>

#include <logger/logger_factory.hpp>

using namespace agent::chassis::ipmb;
using namespace agent::chassis::ipmb::command;

CommandUniquePtr Dispatcher::dispatch(const IpmiMessage& msg) {
    CommandFactory factory {};

    NetFn netfn = NetFn(msg.get_netfn());
    CmdCode cmd_code = CmdCode(msg.get_cmd());
    uint8_t dlun = uint8_t(msg.get_dlun());

    auto command = factory.get_command(dlun, netfn, cmd_code);

    log_debug(LOGUSR, "IPMI Message dispatched.");
    log_debug(LOGUSR, msg.print_msg());

    return command;
}

