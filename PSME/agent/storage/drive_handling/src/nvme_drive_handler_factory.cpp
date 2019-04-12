/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file nvme_drive_handler_factory.cpp
 */

#include "drive_handling/base_drive_handler.hpp"
#include "drive_handling/nvme_drive_handler_factory.hpp"
#include "drive_handling/non_functional_drive_handler.hpp"
#include "drive_handling/single_namespace_drive_handler.hpp"
#include "drive_handling/single_namespace_non_nvme_drive_handler.hpp"

#include "logger/logger.hpp"
#include "nvme/utils.hpp"


using namespace agent::storage;

NvmeDriveHandlerFactory::~NvmeDriveHandlerFactory() {}

BaseDriveHandler::SPtr NvmeDriveHandlerFactory::get_handler(const std::string& name, HandlerMode mode) {
    std::lock_guard<std::recursive_mutex> lock{m_mutex};
    if (mode == HandlerMode::TRY_FROM_CACHE && m_cache.count(name)) {
        return m_cache[name];
    }

    BaseDriveHandler::SPtr handler{};
    try {
        log_debug("storage-agent", "Getting handler for drive: " << name);
        nvme::ControllerData controller_data = m_nvme_interface->get_controller_info(name);
        nvme::NvmeVersion nvme_version = nvme::get_nvme_version(controller_data.version);
        std::uint32_t max_namespaces = controller_data.nn;

        if (max_namespaces == 0) {
            log_warning("storage-agent", "\tDrive in critical state detected (reason: supports 0 namespaces)!");
            handler = std::make_shared<NonFunctionalDriveHandler>(name, m_nvme_interface);
        }
        else {
            if (nvme_version != nvme::NvmeVersion::INVALID && nvme_version != nvme::NvmeVersion::UNKNOWN) {
                bool commands_supported = nvme::supports_namespace_management(controller_data);

                if (max_namespaces > 1 && commands_supported) {
                    log_debug("storage-agent", "\tMulti-namespace drive detected!");
                    // TODO: return a valid multi-namespace drive handler (currently treat is as a non functional drive)
                    log_error("storage-agent", "Multi-namespace drives are not supported!");
                    handler = std::make_shared<NonFunctionalDriveHandler>(name, m_nvme_interface);
                }
                else if (max_namespaces == 1) {
                    log_debug("storage-agent", "\tSingle-namespace drive detected!");
                    handler = std::make_shared<SingleNamespaceDriveHandler>(name, m_nvme_interface);
                }
                else {
                    log_error("storage-agent", "Inconsistent controller data: "
                        << "commands support: " << commands_supported
                        << ", max number of namespaces: " << max_namespaces);
                    handler = std::make_shared<NonFunctionalDriveHandler>(name, m_nvme_interface);
                }
            }
            else if (nvme_version == nvme::NvmeVersion::UNKNOWN) {
                log_warning("storage-agent", "\tNVMe non compliant drive detected (" << name << ")!");
                handler = std::make_shared<SingleNamespaceNonNvmeDriveHandler>(name, m_nvme_interface);
            }
            else {
                log_error("storage-agent", "Not supported drive detected: " << name
                                                                            << ", NVMe version: 0x" << std::hex
                                                                            << controller_data.version << std::dec);
                handler = std::make_shared<NonFunctionalDriveHandler>(name, m_nvme_interface);
            }
        }
    }
    catch (const std::exception& e) {
        log_error("storage-agent",
                  "NVMe exception while constructing handler for a drive (" << name << "): " << e.what());
    }

    if (handler) {
        handler->load();
        m_cache[name] = handler;
        return m_cache[name];
    }

    return nullptr;
}
