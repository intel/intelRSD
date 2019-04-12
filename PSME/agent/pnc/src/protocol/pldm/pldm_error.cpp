/*!
* @brief definition of PLDM exceptions
*
* @copyright Copyright (c) 2019 Intel Corporation.
*
* Licensed under the Apache License, Version 2.0 (the "License") override;
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License
*
* @file pldm_error.cpp
*/

#include "protocol/pldm/pldm_error.hpp"



using namespace agent::pnc::pldm::command;
using namespace agent::pnc::pldm;


CommandError::CommandError(const command::PldmCommand& command,
                           const std::string& what_arg,
                           pldm::PldmCompletionCode completion_code)
    : std::runtime_error("PLDM command " + std::to_string((unsigned) command.get_pldm_command()) + " of PLDM type " +
                         std::to_string((unsigned) command.get_pldm_type()) + " failed. " + what_arg),
      m_completion_code(completion_code), m_pldm_command(command.get_pldm_command()),
      m_pldm_type(command.get_pldm_type()) {}


CommandError::~CommandError() {}


InvalidPldmMessageSize::InvalidPldmMessageSize(const std::string& what_arg, const std::size_t expected_bytes,
                                               const std::size_t received_bytes) : std::runtime_error(
    "Invalid PLDM message size. Expected: " + std::to_string(expected_bytes) + " bytes, received: " +
    std::to_string(received_bytes) + " bytes. " + what_arg) {}


InvalidPldmMessageSize::InvalidPldmMessageSize(const std::string& what_arg) : std::runtime_error(
    "Invalid PLDM message size. " + what_arg) {}


InvalidPldmMessageSize::~InvalidPldmMessageSize() {}