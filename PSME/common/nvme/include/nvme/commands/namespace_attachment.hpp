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
 * @file nvme/commands/namespace_attachment.hpp
 */

#pragma once

#include "nvme/commands/generic_nvme_command.hpp"
#include "nvme/nvme_exception.hpp"

namespace nvme {
namespace commands {

class NamespaceAttachment final : public GenericNvmeCommand {
public:

    /*!
     * @brief Constructs a valid NamespaceAttachment command
     * @param namespace_id Id of the affected namespace
     * @param sel Type of the attachment operation
     */
    NamespaceAttachment(uint32_t namespace_id, NamespaceAttachmentSel sel);

    /*!
     * @brief Adds controller to the list
     * @param controller_id Controller id to be added
     */
    void add_controller(uint16_t controller_id) {
        if (m_controller_id_list.size >= CONTROLLER_ID_LIST_MAX_SIZE) {
            throw NvmeException("Too many controllers in the Namespace Attachment NVMe command");
        }
        m_controller_id_list.controller_id[m_controller_id_list.size++] = controller_id;
    }

    virtual ~NamespaceAttachment();

private:

    ControllerIdList m_controller_id_list{};

};

}
}
