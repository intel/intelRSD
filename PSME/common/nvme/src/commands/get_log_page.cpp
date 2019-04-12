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
 * @file nvme/commands/get_log_page.cpp
 */

#include "nvme/commands/get_log_page.hpp"

using namespace nvme::commands;

GetLogPage::GetLogPage(uint32_t namespace_id, LogPageId log_page_id)
    : GenericNvmeCommand(NvmeCommandType::AdminCommand) {

    m_data.cmd.opcode = static_cast<uint8_t>(AdminCommandOpcode::GetLogPage);
    m_data.cmd.namespace_id = namespace_id;
    m_data.cmd.get_log_page.log_page_id = static_cast<uint8_t>(log_page_id);
    m_data.cmd.get_log_page.num_of_dwords = (BUFFER_SIZE >> 2) - 1;
    m_data.cmd.data_pointer.data.address = reinterpret_cast<uint64_t>(&m_raw_buffer);
    m_data.cmd.data_pointer.data.data_length = BUFFER_SIZE;
}

GetLogPage::~GetLogPage() {}
