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
 * @file discovery/components/haswell.cpp
 *
 * @brief Implementation discovery haswell
 * */

#include "ipmi/management_controller.hpp"
#include "discovery/components/haswell.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"

using namespace ipmi;
using namespace agent::compute::discovery;
using namespace agent_framework::module;

namespace {
void setup_connection_data(const std::string& module, const std::string& blade) {
    auto con_data = ComputeManager::get_instance()->
                        get_compute_module_manager().
                            get_connection_data(module);

    ComputeManager::get_instance()->
                        get_blade_manager().
                            set_connection_data(blade, std::move(con_data));
}

void set_absent(const std::string& module) {
    const auto blades = ComputeManager::get_instance()->
                      get_blade_manager().get_keys(module);

    for (const auto& blade : blades) {
        ComputeManager::get_instance()->
        get_blade_manager().set_present(blade, false);
    }
}

void set_present_first_submodule(const std::string& module) {
    const auto keys = ComputeManager::get_instance()->
                      get_blade_manager().get_keys(module);

    for (std::size_t i = 0; i < 1; ++i) {
        ComputeManager::get_instance()->
        get_blade_manager().set_present(keys[i], true);
        ::setup_connection_data(module, keys[i]);
    }
}
}

void DiscoveryHaswell::discovery(ipmi::ManagementController&,
                   const std::string& parent_uuid) const {
    ::set_absent(parent_uuid);
    ::set_present_first_submodule(parent_uuid);

    log_debug(GET_LOGGER("agent"), "Haswell blade status");
    const auto keys = ComputeManager::get_instance()->
                      get_blade_manager().get_keys(parent_uuid);
    for (const auto& key : keys) {
        log_debug(GET_LOGGER("agent"),
                    "Blade " << key << " status " <<
                                            ComputeManager::get_instance()->
                                            get_blade_manager().get_present(key));
    }
}

