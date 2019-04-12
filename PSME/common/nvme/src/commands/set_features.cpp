/*!
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file nvme/commands/set_features.cpp
 */

#include "nvme/definitions.hpp"
#include "nvme/commands/set_features.hpp"

using namespace nvme::commands;

SetFeatures::SetFeatures(std::uint32_t namespace_id, std::uint8_t feature_identifier, std::uint32_t feature_value)
    : GenericNvmeCommand(NvmeCommandType::AdminCommand) {

    m_data.cmd.opcode = static_cast<std::uint8_t>(AdminCommandOpcode::SetFeatures);
    m_data.cmd.namespace_id = namespace_id;
    m_data.cmd.set_features.fid = feature_identifier;
    m_data.cmd.set_features.value = feature_value;
    m_data.cmd.set_features.save.sv = 1; // Controller shall save the setting over power-cycles
}

SetFeatures::~SetFeatures() {}
