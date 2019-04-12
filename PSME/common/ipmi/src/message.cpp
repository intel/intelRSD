/*!
 * @brief Represents Message used to communicate through IPMI.
 *
 * It's a base class for Request and Response.
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file ipmi/message.cpp
 */

#include "ipmi/message.hpp"

using namespace ipmi;

Message::Message(IpmiInterface::NetFn fn, IpmiInterface::Cmd cmd): network_function(fn), command_code(cmd) { }

Message::~Message() { }

IpmiInterface::Cmd Message::get_command() const {
    return command_code;
}
void Message::set_command(IpmiInterface::Cmd cmd) {
    command_code = cmd;
}

IpmiInterface::NetFn Message::get_network_function() const {
    return network_function;
}
void Message::set_network_function(IpmiInterface::NetFn fn) {
    network_function = fn;
}

IpmiInterface::Lun Message::get_lun() const {
    return lun;
}
void Message::set_lun(IpmiInterface::Lun _lun) {
    lun = _lun;
}
