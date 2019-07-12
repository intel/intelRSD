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
 * @file command/add_zone_endpoints.cpp
 * @brief AddZoneEndpoints sdv implementation
 * */

#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/pnc_commands.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "agent-framework/action/task_creator.hpp"
#include "gas/access_interface_factory.hpp"
#include "gas/pcie_access_interface.hpp"
#include "gas/global_address_space_registers.hpp"
#include "tools/toolset.hpp"
#include "agent-framework/database/database_entities.hpp"
#include "tools/database_keys.hpp"



using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc::tools;

namespace {

void validate_endpoints_are_unbound(const attribute::Array<Uuid>& endpoint_uuids) {
    for (const auto& endpoint_uuid : endpoint_uuids) {
        auto state = get_manager<Endpoint>().get_entry(endpoint_uuid).get_status().get_state();
        if (state != enums::State::Enabled) {
            log_debug("pnc-gami", "Endpoint " + endpoint_uuid + " is in wrong state: " + state.to_string() + ".");
            THROW(agent_framework::exceptions::InvalidValue, "pnc-gami",
                  std::string("Endpoint is in wrong state: ") + state.to_string() + ".");
        }
        if (get_m2m_manager<Zone, Endpoint>().child_exists(endpoint_uuid)) {
            log_debug("pnc-gami", "Endpoint " + endpoint_uuid + " already assigned to a zone.");
            THROW(agent_framework::exceptions::InvalidValue, "pnc-gami",
                  "Endpoint already assigned to a zone.");
        }
    }
}


void validate_number_of_initiator_endpoints(const AddZoneEndpoints::Request& request) {
    std::vector<Uuid> initiator_endpoints_uuids = Toolset::get().model_tool->get_endpoints_with_role(
        request.get_endpoints(), EntityRole::Initiator);
    if (initiator_endpoints_uuids.size() > 1) {
        THROW(agent_framework::exceptions::InvalidValue, "pnc-gami",
              "Unable to add more than 1 Initiator Endpoint to the Zone");
    }
    else if (initiator_endpoints_uuids.size() == 1) {
        std::vector<Uuid> initiator_endpoints_in_zone_uuids = Toolset::get().model_tool->get_endpoints_with_role(
            get_m2m_manager<Zone, Endpoint>().get_children(request.get_zone()), EntityRole::Initiator);
        if (initiator_endpoints_in_zone_uuids.size() != 0) {
            THROW(agent_framework::exceptions::InvalidValue, "pnc-gami",
                  "Unable to add Initiator Endpoint to a Zone which already contains Initiator Endpoint");
        }
    }
}


void validate_duplicate_endpoints(const attribute::Array<Uuid>& endpoint_uuids) {
    std::set<Uuid> endpoints_set(endpoint_uuids.begin(), endpoint_uuids.end());
    if (endpoints_set.size() != endpoint_uuids.size()) {
        THROW(agent_framework::exceptions::InvalidValue, "pnc-gami", "Request contains duplicated endpoints.");
    }
}


void validate_request(const AddZoneEndpoints::Request& request) {
    validate_duplicate_endpoints(request.get_endpoints());
    validate_endpoints_are_unbound(request.get_endpoints());
    validate_number_of_initiator_endpoints(request);
}


void bind_endpoints(const attribute::Array<Uuid>& endpoints_uuids, const std::string& zone_uuid) {

    // currently we support only one switch
    Zone zone = get_manager<Zone>().get_entry(zone_uuid);
    Switch sw = get_manager<Switch>().get_entry(zone.get_switch_uuid());
    GlobalAddressSpaceRegisters gas = GlobalAddressSpaceRegisters::get_default(sw.get_memory_path());

    for (const auto& endpoint_uuid : endpoints_uuids) {
        get_m2m_manager<Zone, Endpoint>().add_entry(zone_uuid, endpoint_uuid);
    }

    Toolset tools = Toolset::get();
    std::vector<Uuid> initiator_endpoints_uuids =
        tools.model_tool->get_endpoints_with_role(endpoints_uuids, EntityRole::Initiator);

    std::vector<Uuid> target_endpoints_uuids;
    if (initiator_endpoints_uuids.size() == 0) {
        target_endpoints_uuids = tools.model_tool->get_endpoints_with_role(endpoints_uuids, EntityRole::Target);
    }
    else {
        target_endpoints_uuids = tools.model_tool->get_endpoints_with_role(
            get_m2m_manager<Zone, Endpoint>().get_children(zone_uuid), EntityRole::Target);
    }

    for (const auto& target_endpoint_uuid : target_endpoints_uuids) {
        tools.gas_tool->bind_endpoint_to_zone(gas, zone_uuid, target_endpoint_uuid);
    }
}


void store_endpoints_in_zone_db(const attribute::Array<Uuid>& endpoints_uuids,
                                const Uuid& zone_uuid) {

    log_debug("pnc-gami", "Store endpoints in zone db, zone uuid: " << zone_uuid);

    agent_framework::database::ZoneEntity zone_db{zone_uuid};

    std::for_each(endpoints_uuids.begin(), endpoints_uuids.end(),
                  [&](const Uuid& endpoint_uuid) {
                      zone_db.append(db_keys::ENDPOINTS, endpoint_uuid);
                      log_debug("pnc-gami", "Endpoint stored in zone db, endpoint uuid: "
                          << endpoint_uuid);
                  });
}


void add_zone_endpoints(const AddZoneEndpoints::Request& request, AddZoneEndpoints::Response& response) {
    log_info("pnc-gami", "Adding endpoints to zone.");

    validate_request(request);

    auto response_builder = []() {
        AddZoneEndpoints::Response res{};
        return res.to_json();
    };

    auto exception_builder = [](const agent_framework::exceptions::GamiException& exception) {
        return agent_framework::exceptions::PncError("Could not add endpoints to zone. " + exception.get_message());
    };

    agent_framework::action::TaskCreator task_creator{};
    task_creator.prepare_task();

    task_creator.add_subtask(std::bind(bind_endpoints, request.get_endpoints(), request.get_zone()));
    task_creator.add_subtask(std::bind(store_endpoints_in_zone_db, request.get_endpoints(), request.get_zone()));

    task_creator.set_promised_error_thrower(exception_builder);
    task_creator.set_promised_response(response_builder);

    task_creator.register_task();
    auto task = task_creator.get_task();
    agent_framework::action::TaskRunner::get_instance().run(task);

    response.set_task(task_creator.get_task_resource().get_uuid());
}
}

REGISTER_COMMAND(AddZoneEndpoints, ::add_zone_endpoints);
