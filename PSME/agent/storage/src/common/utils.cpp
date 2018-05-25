/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * */

#include "agent-framework/exceptions/exception.hpp"
#include "common/utils.hpp"

std::string agent::storage::utils::get_name_from_device_path(const std::string& device_path) {
    auto position = device_path.find_last_of('/');
    if (std::string::npos == position) {
        THROW(agent_framework::exceptions::IscsiError,
              "storage-agent", "Unknown device name " + device_path);
    }
    return device_path.substr(position + 1);
}
