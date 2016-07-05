/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file message.cpp
 *
 * @brief ...
 * */

#include "ipmi/message.hpp"

using namespace ipmi;

Message::Message(uint8_t fn, Cmd cmd): network_function(fn), command_code(cmd) {}
Message::~Message() {}

Cmd Message::get_command() const {
    return command_code;
}

uint8_t Message::get_network_function() const {
    return network_function;
}

void Message::set_command(Cmd cmd) {
    command_code = cmd;
}

void Message::set_network_function(uint8_t fn) {
    network_function = fn;
}
