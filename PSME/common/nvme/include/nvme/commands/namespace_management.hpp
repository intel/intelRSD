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
 * @file nvme/commands/namespace_management.hpp
 */

#pragma once

#include "nvme/commands/generic_nvme_command.hpp"

namespace nvme {
namespace commands {

class NamespaceManagement final : public GenericNvmeCommand {
public:

    /*!
     * @brief Constructs a valid NamespaceManagement command
     * @param sel Type of the management operation
     */
    NamespaceManagement(NamespaceManagementSel sel);

    virtual ~NamespaceManagement();

    /*!
     * @brief Returns namespace id after successful namespace CreateCompletionQueue
     * @return Namespace id
     */
    uint32_t get_namespace_id() const {
        return m_data.result;
    }

    /*!
     * @brief Used to set namespace id for delete commands
     * @param namespace_id Id of the namespace to be deleted
     */
    void set_namespace_id(uint32_t namespace_id) {
        m_data.cmd.namespace_id = namespace_id;
    }

    /*!
     * @brief Sets requested namespace size (in blocks)
     * @param size New size (in blocks)
     */
    void set_size(uint64_t size) {
        m_namespace_data.size = size;
    }

    /*!
     * @brief Sets requested namespace capacity (in blocks)
     * @param capacity New capacity (in blocks)
     */
    void set_capacity(uint64_t capacity) {
        m_namespace_data.capacity = capacity;
    }

    /*!
     * @brief Forces created namespace to be private
     */
    void set_as_private() {
        m_namespace_data.nmic = 1;
    }

    /*!
     * @brief Forces created namespace to be public
     */
    void set_as_public() {
        m_namespace_data.nmic = 0;
    }

private:

    NamespaceData m_namespace_data{};

};

}
}
