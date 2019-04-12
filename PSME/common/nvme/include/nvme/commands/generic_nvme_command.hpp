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
 * @file nvme/commands/generic_nvme_command.hpp
 */

#pragma once

#include "nvme/definitions.hpp"

#include <cstdint>

namespace nvme {
namespace commands {

/*! Type of the nvme command - specifies ioctl number */
enum class NvmeCommandType : uint8_t {
    AdminCommand,
    NvmCommand,
    ControllerReset
};

/*! Base class for all NVMe commands - may only be derived */
class GenericNvmeCommand {
public:

    /*!
     * @brief Returns the command data block representing the nvme commands
     * @return Reference to the command data struct
     */
    CommandData& get_data() {
        return m_data;
    }

    /*!
     * @brief Returns type of the NVMe command
     * @return NVMe command type
     */
    NvmeCommandType get_type() const {
        return m_type;
    }

protected:

    /*!
     * @brief Constructs NVMe command of a specified type
     * @param type Type of the command to be constructed
     */
    explicit GenericNvmeCommand(NvmeCommandType type);

    virtual ~GenericNvmeCommand();

    CommandData m_data{};

    NvmeCommandType m_type{};

};

}
}
