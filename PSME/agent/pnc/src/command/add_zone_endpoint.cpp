/*!
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
 * @file command/sdv/add_zone_endpoint.cpp
 * @brief AddZoneEndpoint sdv implementation
 * */

#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/command-ref/registry.hpp"
#include "agent-framework/command-ref/pnc_commands.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task_creator.hpp"
#include "gas/access_interface_factory.hpp"
#include "gas/pcie_access_interface.hpp"
#include "gas/global_address_space_registers.hpp"
#include "tools/toolset.hpp"



using namespace agent_framework::command_ref;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc::tools;


namespace {

void validate_endpoints_are_unbound(const attribute::Array<std::string>& endpoint_uuids) {
    for (const auto& endpoint_uuid : endpoint_uuids) {
        if (get_m2m_manager<Zone, Endpoint>().child_exists(endpoint_uuid)) {
            THROW(agent_framework::exceptions::InvalidValue, "pnc-gami",
                  "Endpoint " + endpoint_uuid + " already assigned to a zone.");
        }
    }
}


void bind_endpoints(const attribute::Array<std::string>& endpoints_uuids, const std::string& zone_uuid) {

    // currently we support only one switch
    Zone zone = get_manager<Zone>().get_entry(zone_uuid);
    Switch sw = get_manager<Switch>().get_entry(zone.get_switch_uuid());
    GlobalAddressSpaceRegisters gas = GlobalAddressSpaceRegisters::get_default(sw.get_memory_path());

    Toolset tools = Toolset::get();
    for (const auto& endpoint_uuid : endpoints_uuids) {
        tools.gas_tool->bind_endpoint_to_zone(gas, zone_uuid, endpoint_uuid);
        get_m2m_manager<Zone, Endpoint>().add_entry(zone_uuid, endpoint_uuid);
    }
}



void add_zone_endpoint(const AddZoneEndpoint::Request& request, AddZoneEndpoint::Response& response) {
    log_info(GET_LOGGER("pnc-gami"), "Adding endpoints to zone.");

    auto response_builder = []() {
        AddZoneEndpoint::Response res{};
        return res.to_json();
    };

    auto exception_builder = []() {
        return agent_framework::exceptions::PncError("Could not add endpoints to zone");
    };

    agent_framework::action::TaskCreator task_creator{};
    task_creator.prepare_task();

    task_creator.add_prerun_action(std::bind(validate_endpoints_are_unbound, request.get_endpoints()));
    task_creator.add_subtask(std::bind(bind_endpoints, request.get_endpoints(), request.get_zone()));

    task_creator.set_promised_error_thrower(exception_builder);
    task_creator.set_promised_response(response_builder);

    task_creator.register_task();
    auto task = task_creator.get_task();
    agent_framework::action::TaskRunner::get_instance().run(task);

    response.set_task(task_creator.get_task_resource().get_uuid());
}
}

REGISTER_COMMAND(AddZoneEndpoint, ::add_zone_endpoint);
