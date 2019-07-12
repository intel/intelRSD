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
 * @file command/delete_zone.cpp
 * @brief DeleteZone implementation
 * */

#include "agent-framework/action/task_creator.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/pnc_commands.hpp"
#include "gas/global_address_space_registers.hpp"
#include "tools/toolset.hpp"
#include "tools/database_keys.hpp"
#include "agent-framework/database/database_entities.hpp"



using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent::pnc::gas;
using namespace agent::pnc::tools;

namespace {

void remove_zone_from_model_and_db(const Uuid& zone_uuid) {

    // delete zone from model
    get_manager<Zone>().remove_entry(zone_uuid);
    // delete zone from fabric's database
    agent_framework::database::FabricEntity(get_manager<Fabric>().get_keys().front()).del(zone_uuid);
    // delete zone database
    agent_framework::database::ZoneEntity::remove(zone_uuid);

    log_debug("pnc-gami", "Zone removed from model, zone uuid: " << zone_uuid);
}


void unbind_endpoints(const attribute::Array<std::string>& endpoint_uuids, const std::string& zone_uuid) {

    // currently we support only one switch
    Zone zone = get_manager<Zone>().get_entry(zone_uuid);
    Switch sw = get_manager<Switch>().get_entry(zone.get_switch_uuid());
    GlobalAddressSpaceRegisters gas = GlobalAddressSpaceRegisters::get_default(sw.get_memory_path());

    for (const auto& endpoint_uuid : endpoint_uuids) {

        if (Endpoint::is_target(endpoint_uuid)) {
            Toolset::get().gas_tool->unbind_endpoint_from_zone(gas, zone_uuid, endpoint_uuid);
        }

        log_debug("pnc-gami", "Endpoint detached from zone, endpoint uuid: " << endpoint_uuid);
    }
}


void remove_endpoints_from_zone_model(const attribute::Array<std::string>& endpoint_uuids,
                                      const std::string& zone_uuid) {

    Zone zone = get_manager<Zone>().get_entry(zone_uuid);

    for (const auto& endpoint_uuid : endpoint_uuids) {

        get_m2m_manager<Zone, Endpoint>().remove_entry(zone_uuid, endpoint_uuid);

        log_debug("pnc-gami", "Endpoint removed from zone model, endpoint uuid: " << endpoint_uuid);
    }
}


void delete_zone(const DeleteZone::Request& request, DeleteZone::Response& response) {

    const Uuid& zone_uuid = request.get_zone();
    Zone zone = get_manager<Zone>().get_entry(zone_uuid);
    //validate
    auto endpoints_uuids = get_m2m_manager<Zone, Endpoint>().get_children(zone_uuid);

    log_info("pnc-gami", "Deleting endpoints from zone.");

    if (Zone::is_initiator_in_zone(zone_uuid) && Zone::is_target_in_zone(zone_uuid)) {
        auto response_builder = []() -> json::Json {
            DeleteZone::Response res{};
            return res.to_json();
        };

        auto exception_builder = [](const agent_framework::exceptions::GamiException& exception) {
            return agent_framework::exceptions::PncError("Could not unbind endpoint. " + exception.get_message());
        };

        agent_framework::action::TaskCreator task_creator{};
        task_creator.prepare_task();

        auto initiator_counter = std::count_if(endpoints_uuids.begin(), endpoints_uuids.end(),
                                               [](const Uuid& endpoint_uuid) {
                                                   return Endpoint::is_initiator(endpoint_uuid);
                                               });

    if (initiator_counter) {
        task_creator.add_subtask(std::bind(unbind_endpoints, endpoints_uuids, zone_uuid));
    }

    task_creator.add_subtask(std::bind(remove_endpoints_from_zone_model, endpoints_uuids, zone_uuid));
    task_creator.add_subtask(std::bind(remove_zone_from_model_and_db, zone_uuid));

        task_creator.set_promised_error_thrower(exception_builder);
        task_creator.set_promised_response(response_builder);

        task_creator.register_task();
        auto task = task_creator.get_task();
        agent_framework::action::TaskRunner::get_instance().run(task);

        log_info("pnc-agent", "Endpoint unbinding started.");

        response.set_task(task_creator.get_task_resource().get_uuid());
    }
    else {
        /* no need to unbind so do not create task - take actions immediately */
        remove_endpoints_from_zone_model(endpoints_uuids, zone_uuid);
        remove_zone_from_model_and_db(zone_uuid);
    }
}
}

REGISTER_COMMAND(DeleteZone, ::delete_zone);
