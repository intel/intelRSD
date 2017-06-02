/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * */

#include "status/status_manager.hpp"

using namespace agent::compute::status;

StatusManager::StatusManager(const std::uint32_t slot, const std::string& ip, const std::string& uuid) :
        m_hw_status{slot}, m_sw_status{ip, uuid} {
}

StatusManager::~StatusManager() { }

ModuleStatus::Status StatusManager::get_hw_status() {
    try {
        return m_hw_status.read_status();
    }
    catch (...) {
        return ModuleStatus::Status::UNKNOWN;
    }
}

ModuleStatus::Status StatusManager::get_sw_status() {
    try {
        return m_sw_status.read_status();
    }
    catch (...) {
        return ModuleStatus::Status::UNKNOWN;
    }
}
