/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file storage/src/discovery-ref/discovery_manager.cpp
 *
 * */

#include "discovery-ref/discovery_manager.hpp"

#include "agent-framework/module-ref/storage_manager.hpp"

#include <stdexcept>

using std::string;
using std::runtime_error;
using namespace agent_framework::module;
using namespace agent::storage::discoveries;

void DiscoveryManager::discovery(const string& uuid) {
    /* uuid - manager uuid */
    auto& manager = StorageManager::get_instance()->get_module_manager();
    if (manager.entry_exists(uuid)) {
        throw runtime_error("Discovery not implemented yet");
    }
}

DiscoveryManager::~DiscoveryManager() { }
