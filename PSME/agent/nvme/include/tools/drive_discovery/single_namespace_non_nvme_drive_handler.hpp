/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file single_namespace_non_nvme_drive_handler.hpp
 */

#pragma once

#include "tools/drive_discovery/single_namespace_drive_handler.hpp"

namespace agent {
namespace nvme {
namespace tools {

/*!
 * Drive handler implementation for single namespace NVMe drives that are not NVMe-compliant.
 */
class SingleNamespaceNonNvmeDriveHandler : public SingleNamespaceDriveHandler {
public:

    /*!
    * @brief Constructs a valid handler
    * @param name Name of the drive device
    * @param nvme_interface NVMe interface for querying NVMe commands
    * @param drive_interface Drive interface for reading drives
    */
    SingleNamespaceNonNvmeDriveHandler(const std::string& name,
        std::shared_ptr<::nvme::AbstractNvmeInterface> nvme_interface,
        std::shared_ptr<::partition::AbstractDriveInterface> drive_interface)

        : SingleNamespaceDriveHandler(name, nvme_interface, drive_interface) {
    }

    /*!
     * @brief virtual Destructor
     */
    virtual ~SingleNamespaceNonNvmeDriveHandler();

protected:

    /*! Reimplemented, for NVMe non-compliant drives default namespace id has to be hardcoded */
    virtual uint32_t get_primary_namespace_id() const override;

private:

    static constexpr uint32_t DEFAULT_NAMESPACE_ID = 1;

};

}
}
}
