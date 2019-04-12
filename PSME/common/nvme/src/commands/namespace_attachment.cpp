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
 * @file nvme/commands/namespace_attachment.cpp
 */

#include "nvme/commands/namespace_attachment.hpp"

using namespace nvme::commands;

NamespaceAttachment::NamespaceAttachment(uint32_t namespace_id, NamespaceAttachmentSel sel)
    : GenericNvmeCommand(NvmeCommandType::AdminCommand) {

    m_data.cmd.opcode = static_cast<uint8_t>(AdminCommandOpcode::NamespaceAttachment);
    m_data.cmd.namespace_id = namespace_id;
    m_data.cmd.namespace_attachment.sel = static_cast<uint8_t>(sel);
    m_data.cmd.data_pointer.data.address = reinterpret_cast<uint64_t>(&m_controller_id_list);
    m_data.cmd.data_pointer.data.data_length = sizeof(ControllerIdList);
}

NamespaceAttachment::~NamespaceAttachment() {}
