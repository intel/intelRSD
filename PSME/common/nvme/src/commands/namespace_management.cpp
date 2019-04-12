/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file nvme/commands/namespace_management.cpp
 */

#include "nvme/commands/namespace_management.hpp"

using namespace nvme::commands;

NamespaceManagement::NamespaceManagement(NamespaceManagementSel sel)
    : GenericNvmeCommand(NvmeCommandType::AdminCommand) {

    m_data.cmd.opcode = static_cast<uint8_t>(AdminCommandOpcode::NamespaceManagement);
    m_data.cmd.namespace_management.sel = static_cast<uint8_t>(sel);
    if (sel == NamespaceManagementSel::Create) {
        m_data.cmd.data_pointer.data.address = reinterpret_cast<uint64_t>(&m_namespace_data);
        m_data.cmd.data_pointer.data.data_length = sizeof(m_namespace_data);
    }
    else {
        m_data.cmd.data_pointer.data.address = 0;
        m_data.cmd.data_pointer.data.data_length = 0;
    }

}

NamespaceManagement::~NamespaceManagement() {}
