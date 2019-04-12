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
 * @file single_namespace_non_nvme_drive_handler.cpp
 */

#include "tools/drive_discovery/single_namespace_non_nvme_drive_handler.hpp"
#include "nvme/nvme_exception.hpp"
#include "nvme/utils.hpp"
#include "logger/logger_factory.hpp"

using namespace agent::nvme::tools;
using namespace nvme;

SingleNamespaceNonNvmeDriveHandler::~SingleNamespaceNonNvmeDriveHandler() {}

uint32_t SingleNamespaceNonNvmeDriveHandler::get_primary_namespace_id() const {
    return DEFAULT_NAMESPACE_ID;
}
