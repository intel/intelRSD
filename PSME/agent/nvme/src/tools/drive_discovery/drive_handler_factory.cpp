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
 * @file drive_handler_factory.cpp
 */

#include "tools/drive_discovery/drive_handler_factory.hpp"
#include "tools/drive_discovery/single_namespace_drive_handler.hpp"
#include "tools/drive_discovery/single_namespace_non_nvme_drive_handler.hpp"
#include "tools/drive_discovery/non_functional_drive_handler.hpp"
#include "nvme/nvme_exception.hpp"
#include "nvme/utils.hpp"
#include "partition/file_drive_interface.hpp"
#include "logger/logger_factory.hpp"

using namespace agent::nvme::tools;
using namespace nvme;
using namespace partition;

DriveHandlerFactory::~DriveHandlerFactory() {}

std::shared_ptr<BaseDriveHandler> DriveHandlerFactory::get_handler(const std::string& name) const {
    std::lock_guard<std::recursive_mutex> lock{m_mutex};
    try {
        log_debug("nvme-agent", "Getting handler for drive: " << name);
        ControllerData controller_data = m_nvme_interface->get_controller_info(name);
        NvmeVersion nvme_version = get_nvme_version(controller_data.version);
        uint32_t max_namespaces = controller_data.nn;

        if (max_namespaces == 0) {
            log_debug("nvme-agent", "\tDrive in critical state detected (reason: supports 0 namespaces)!");
            return std::make_shared<NonFunctionalDriveHandler>(name, m_nvme_interface);
        }

        if (nvme_version != NvmeVersion::INVALID && nvme_version != NvmeVersion::UNKNOWN) {
            bool commands_supported = supports_namespace_management(controller_data);

            if (max_namespaces > 1 && commands_supported) {
                log_debug("nvme-agent", "\tMulti-namespace drive detected!");
                // TODO return a valid multi-namespace drive handler
                log_error("nvme-agent", "Multi-namespace drives are not supported!");
                // currently treat is as a non functional drive
                return std::make_shared<NonFunctionalDriveHandler>(name, m_nvme_interface);
            }
            else if (max_namespaces == 1) {
                log_debug("nvme-agent", "\tSingle-namespace drive detected!");
                return std::make_shared<SingleNamespaceDriveHandler>(name, m_nvme_interface, m_drive_interface);
            }
            else {
                log_error("nvme-agent", "Inconsistent controller data: "
                    << "commands support: " << commands_supported
                    << ", max number of namespaces: " << max_namespaces);
                return std::make_shared<NonFunctionalDriveHandler>(name, m_nvme_interface);
            }
        }
        else if (nvme_version == NvmeVersion::UNKNOWN) {
            log_warning("nvme-agent", "\tNVMe non compliant drive detected (" << name << ")!");
            return std::make_shared<SingleNamespaceNonNvmeDriveHandler>(name, m_nvme_interface, m_drive_interface);
        }
        else {
            log_error("nvme-agent", "Not supported drive detected: " << name
                << ", NVMe version: 0x" << std::hex << controller_data.version << std::dec);
            return std::make_shared<NonFunctionalDriveHandler>(name, m_nvme_interface);
        }
    }
    catch (const NvmeException& e) {
        log_error("nvme-agent", "NVMe exception while constructing handler for a drive (" << name
            << "): " << e.what());
    }
    return nullptr;
}
