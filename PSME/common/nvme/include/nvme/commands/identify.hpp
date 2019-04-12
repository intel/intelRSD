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
 * @file nvme/commands/identify.hpp
 */

#pragma once

#include "nvme/commands/generic_nvme_command.hpp"

namespace nvme {
namespace commands {

class Identify final : public GenericNvmeCommand {
public:

    static constexpr uint16_t BUFFER_SIZE = 4096;

    /*!
     * @brief Constructs a valid Identify command
     * @param namespace_id Id of the controller
     * @param controller_id Id of the namespace
     * @param cns Controller/namespace setting for the command
     */
    Identify(uint16_t controller_id, uint32_t namespace_id, IdentifyCns cns);

    virtual ~Identify();

    /*!
     * @brief Returns data interpreted as controller data
     * @return Controller data
     */
    const ControllerData& get_controller_data() const {
        return m_controller_data;
    }

    /*!
     * @brief Returns data interpreted as namespace data
     * @return Namespace data
     */
    const NamespaceData& get_namespace_data() const {
        return m_namespace_data;
    }

    /*!
     * @brief Returns data interpreted as controller id list
     * @return Controller id list
     */
    const ControllerIdList& get_controller_id_list() const {
        return m_controller_id_list;
    }

    /*!
     * @brief Returns data interpreted as namespace id list
     * @return Namespace id list
     */
    const NamespaceIdList& get_namespace_id_list() const {
        return m_namespace_id_list;
    }

private:

    union {
        ControllerData m_controller_data;
        NamespaceData m_namespace_data;
        ControllerIdList m_controller_id_list;
        NamespaceIdList m_namespace_id_list;
        uint8_t m_raw_buffer[BUFFER_SIZE]{};
    };

};

}
}
