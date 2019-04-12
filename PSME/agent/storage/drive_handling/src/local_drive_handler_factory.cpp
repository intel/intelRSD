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
 * @file local_drive_handler_factory.cpp
 */

#include "drive_handling/base_drive_handler.hpp"
#include "drive_handling/local_drive_handler_factory.hpp"
#include "drive_handling/local_drive_handler.hpp"
#include "ioctl/ioctl_interface.hpp"

#include "logger/logger.hpp"
#include <thread>

using namespace agent::storage;

LocalDriveHandlerFactory::~LocalDriveHandlerFactory() {}

BaseDriveHandler::SPtr LocalDriveHandlerFactory::get_handler(const std::string& name, HandlerMode mode) {
    log_debug("storage-agent", "Getting handler for drive: " << name);
    if (mode == HandlerMode::TRY_FROM_CACHE && m_cache.count(name)) {
        return m_cache[name];
    }

    auto ioctl_interface = std::make_shared<ioctl::IoctlInterface>(ioctl::IoctlInterface{name});
    auto handler = std::make_shared<LocalDriveHandler>(name, m_sysfs_interface, ioctl_interface);
    std::this_thread::sleep_for(std::chrono::seconds(3)); // Waiting for device to stabilize
    handler->load();
    m_cache[name] = handler;
    return m_cache[name];
}
