/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file status/status_manager.cpp
 * @brief StatusManager stub implementation
 * */

#include "status/module_hardware_status.hpp"
#include "status/module_software_status.hpp"
#include "status/status_manager.hpp"

using namespace agent::pnc::status;

StatusManager::StatusManager(){}
StatusManager::~StatusManager(){}

ModuleStatus::Status StatusManager::get_hw_status() {
    ModuleHardwareStatus hw_status {};
    try {
        return hw_status.read_status();
    }
    catch(...) {
        return ModuleStatus::Status::UNKNOWN;
    }
}

ModuleStatus::Status StatusManager::get_sw_status() {
    ModuleSoftwareStatus sw_status {};
    try {
        return sw_status.read_status();
    }
    catch(...) {
        return ModuleStatus::Status::UNKNOWN;
    }
}
