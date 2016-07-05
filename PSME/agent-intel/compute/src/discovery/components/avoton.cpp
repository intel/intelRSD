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
 * @file discovery/components/avoton.cpp
 *
 * @brief Implementation discovery avoton
 * */

#include "ipmi/get_device_id.hpp"
#include "discovery/components/utils.hpp"
#include "ipmi/management_controller.hpp"
#include "discovery/components/utils.hpp"
#include "discovery/components/avoton.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"

using namespace ipmi;
using namespace agent::compute::discovery;
using namespace agent_framework::module;

namespace {
void process_blade(ManagementController& mc, const std::string& blade) {
    ipmi::request::GetDeviceId request;
    ipmi::response::GetDeviceId response;

    try {
        mc.connect();
        mc.send(request, response);
        mc.disconnect();
    }
    catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("agent"), "GetDeviceId request error: " << e.what());
        ComputeManager::get_instance()->
                            get_blade_manager().set_present(blade, false);
        return;
    }

    if (check_ipmi_response(response, ipmi::Response::COMPLETION_CODE_NORMAL)) {
        ComputeManager::get_instance()->
                        get_blade_manager().set_present(blade, true);
    } else {
        ComputeManager::get_instance()->
                        get_blade_manager().set_present(blade, false);
    }
}

void set_blade_connection_data(ManagementController& mc,
                               const std::string& blade) {
    const auto conn = ComputeManager::get_instance()->
                                get_blade_manager().get_connection_data(blade);

    mc.set_port(conn.get_port());
}

void set_module_connection_data(ManagementController& mc,
                                                const std::string& module) {
    const auto conn = ComputeManager::get_instance()->
                        get_compute_module_manager().get_connection_data(module);

    mc.set_port(conn.get_port());
}

}

void DiscoveryAvoton::discovery(ManagementController& mc,
                              const std::string& module) const {
    const auto keys = ComputeManager::get_instance()->
                      get_blade_manager().get_keys(module);

    for (std::size_t i = 0, size = keys.size(); i < size; ++i) {
        ::set_blade_connection_data(mc, keys[i]);
        ::process_blade(mc, keys[i]);
    }
    ::set_module_connection_data(mc, module);
}

