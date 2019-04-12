/*!
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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file command.cpp
 * @brief MRCP Command interface
 * */

#include "gas/mrpc/command.hpp"



using namespace agent::pnc::gas::mrpc;


Command::~Command() {}


void Command::run() {
    uint8_t code = uint8_t(m_command);
    m_iface->write(&code, sizeof(uint8_t), MRPC_COMMAND_REG_OFFSET);
    log_debug("pnc-mrpc", "Running command " << get_command_name(std::uint32_t(m_command)));
}


CommandStatus Command::read_status() {
    std::uint8_t status[MRPC_STATUS_REG_SIZE];
    CommandStatus code{};

    m_iface->read(status, MRPC_STATUS_REG_SIZE, MRPC_STATUS_REG_OFFSET);
    code = CommandStatus(status[0] | (status[1] << 8) | (status[2] << 16) | (status[3] << 24));

    return code;

}
