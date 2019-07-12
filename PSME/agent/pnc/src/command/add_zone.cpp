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
 * @file add_zone.cpp
 * @brief AddZone implementation
 * */

#include "agent-framework/database/database_entities.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/pnc_commands.hpp"
#include "discovery/builders/zone_builder.hpp"
#include "tools/toolset.hpp"
#include "agent-framework/action/task_creator.hpp"
#include "agent-framework/action/task_runner.hpp"
#include "tools/database_keys.hpp"
#include "tree_stability/pnc_stabilizer.hpp"
#include "discovery/discovery_actions.hpp"
#include "tree_stability/helpers/update_relations.hpp"



using namespace agent::pnc;
using namespace agent::pnc::tools;
using namespace agent::pnc::discovery;
using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent_framework::model::attribute;
using namespace agent_framework::exceptions;

namespace {

void throw_if_duplicates(const attribute::Array<Uuid>& endpoints) {
    std::set<Uuid> endpoints_set(endpoints.begin(), endpoints.end());
    if (endpoints_set.size() != endpoints.size()) {
        THROW(InvalidValue, "pnc-gami",
              "Cannot create zone with duplicate endpoints.");
    }
}


void throw_if_connected_entities_empty(const Endpoint& endpoint) {
    if (endpoint.get_connected_entities().empty()) {
        THROW(InvalidValue, "pnc-gami",
              "No connected entity found in endpoint.");
    }

}


void throw_if_more_than_one_initiator(const Endpoint& endpoint, uint16_t& initiator_counter) {
    if (enums::EntityRole::Initiator == endpoint.get_connected_entities().front().get_entity_role()) {
        initiator_counter++;
        if (1 < initiator_counter) {
            THROW(InvalidValue, "pnc-gami",
                  "Zone cannot have more than one initiator endpoint.");
        }
    }
}


void throw_if_endpoint_already_in_zone(const Endpoint& endpoint) {
    auto& manager = get_m2m_manager<Zone, Endpoint>();
    if (!manager.get_parents(endpoint.get_uuid()).empty()) {
        THROW(InvalidValue, "pnc-gami",
              "Endpoint is already in a zone.");
    }
}


void throw_if_endpoints_are_bound(const attribute::Array<Uuid>& endpoint_uuids) {
    for (const auto& endpoint_uuid : endpoint_uuids) {
        auto state = get_manager<Endpoint>().get_entry(endpoint_uuid).get_status().get_state();
        if (state != enums::State::Enabled) {
            THROW(agent_framework::exceptions::InvalidValue, "pnc-gami",
                  "Endpoint " + endpoint_uuid + " is in wrong state: " + state.to_string() + ".");
        }
        if (get_m2m_manager<Zone, Endpoint>().child_exists(endpoint_uuid)) {
            THROW(agent_framework::exceptions::InvalidValue, "pnc-gami",
                  "Endpoint " + endpoint_uuid + " already assigned to a zone.");
        }
    }
}


void validate_zone_endpoints(const attribute::Array<Uuid>& endpoints) {
    throw_if_duplicates(endpoints);
    throw_if_endpoints_are_bound(Toolset::get().model_tool->get_endpoints_with_role(endpoints, EntityRole::Target));
    uint16_t initiator_counter{0};
    for (const auto& endpoint_uuid : endpoints) {
        auto endpoint = get_manager<Endpoint>().get_entry(endpoint_uuid);
        throw_if_connected_entities_empty(endpoint);
        throw_if_more_than_one_initiator(endpoint, initiator_counter);
        throw_if_endpoint_already_in_zone(endpoint);
    }
}


void bind_endpoints(const attribute::Array<Uuid>& host_endpoints_uuids,
                    const attribute::Array<Uuid>& target_endpoints_uuids,
                    const Uuid& zone_uuid) {

    log_debug("pnc-gami", "Starting binding Endpoints for zone with UUID '" + zone_uuid + "'");
    // currently we support only one switch
    Zone zone = get_manager<Zone>().get_entry(zone_uuid);
    Switch sw = get_manager<Switch>().get_entry(zone.get_switch_uuid());
    auto gas = agent::pnc::gas::GlobalAddressSpaceRegisters::get_default(sw.get_memory_path());

    auto tools = agent::pnc::tools::Toolset::get();

    std::for_each(host_endpoints_uuids.begin(), host_endpoints_uuids.end(), [&](const Uuid& host_endpoint_uuid) {
        get_m2m_manager<Zone, Endpoint>().add_entry(zone.get_uuid(), host_endpoint_uuid);
        log_debug("pnc-gami", "Added host endpoints: " + host_endpoint_uuid + " to zone: " + zone_uuid);
    });

    std::for_each(target_endpoints_uuids.begin(), target_endpoints_uuids.end(), [&](const Uuid& target_endpoint_uuid) {
        tools.gas_tool->bind_endpoint_to_zone(gas, zone_uuid, target_endpoint_uuid);
        get_m2m_manager<Zone, Endpoint>().add_entry(zone.get_uuid(), target_endpoint_uuid);
        log_debug("pnc-gami", "Added target endpoints: " + target_endpoint_uuid + " to zone: " + zone_uuid);
    });

    log_debug("pnc-gami", "Finished binding Endpoints for zone with UUID '" + zone_uuid + "'");
}


void store_zone_in_db(const attribute::Array<Uuid>& host_endpoints_uuids,
                      const attribute::Array<Uuid>& target_endpoints_uuids,
                      const Uuid& zone_uuid,
                      const Uuid& fabric_uuid) {

    log_debug("pnc-gami", "Store zone in db, zone uuid: " << zone_uuid);

    agent_framework::database::FabricEntity(fabric_uuid).append(db_keys::ZONES, zone_uuid);
    agent_framework::database::ZoneEntity zone_db{zone_uuid};

    std::for_each(host_endpoints_uuids.begin(), host_endpoints_uuids.end(),
                  [&](const Uuid& host_endpoints_uuid) {
                      zone_db.append(db_keys::ENDPOINTS, host_endpoints_uuid);
                      log_debug("pnc-gami", "Host endpoint stored in zone db, endpoint uuid: " << host_endpoints_uuid);
                  });

    std::for_each(target_endpoints_uuids.begin(), target_endpoints_uuids.end(),
                  [&](const Uuid& target_endpoint_uuid) {
                      zone_db.append(db_keys::ENDPOINTS, target_endpoint_uuid);
                      log_debug("pnc-gami",
                                "Target endpoint stored in zone db, endpoint uuid: " << target_endpoint_uuid);
                  });

}


/*!
 * @brief Creates task for binding Target Endpoints to Zone
 * @param host_endpoints_uuids UUIDs of Host Endpoints to bind
 * @param target_endpoints_uuids UUIDs of Target Endpoints to bind
 * @param zone_uuid Zone to bind Endpoints to
 * @param fabric_uuid Uuid of the fabric
 * @return Task UUID
 */
Uuid run_add_zone_task(const attribute::Array<Uuid>& host_endpoints_uuids,
                       const attribute::Array<Uuid>& target_endpoints_uuids,
                       const Uuid& zone_uuid,
                       const Uuid& fabric_uuid) {
    log_info("pnc-gami", "Adding endpoints to zone.");

    auto response_builder = [zone_uuid]() {
        AddZone::Response response{};
        response.set_zone(zone_uuid);
        return response.to_json();
    };

    auto exception_builder = [](const agent_framework::exceptions::GamiException& exception) {
        return agent_framework::exceptions::PncError("Could not add endpoints to zone. " + exception.get_message());
    };

    agent_framework::action::TaskCreator task_creator{};

    task_creator.prepare_task();

    task_creator.add_prerun_action(std::bind(throw_if_endpoints_are_bound,
                                             target_endpoints_uuids));

    task_creator.add_subtask(std::bind(bind_endpoints,
                                       host_endpoints_uuids,
                                       target_endpoints_uuids,
                                       zone_uuid));

    task_creator.add_subtask(std::bind(store_zone_in_db,
                                       host_endpoints_uuids,
                                       target_endpoints_uuids,
                                       zone_uuid,
                                       fabric_uuid));

    task_creator.set_promised_error_thrower(exception_builder);
    task_creator.set_promised_response(response_builder);

    task_creator.register_task();
    auto task = task_creator.get_task();
    agent_framework::action::TaskRunner::get_instance().run(task);

    return task_creator.get_task_resource().get_uuid();
}


void add_initiator_zone(const attribute::Array<Uuid>& host_endpoints_uuids,
                        const Uuid& zone_uuid,
                        const Uuid& fabric_uuid) {
    log_info("pnc-gami", "Adding endpoints to zone.");

    std::for_each(host_endpoints_uuids.begin(), host_endpoints_uuids.end(), [&](const Uuid& host_endpoint_uuid) {
        get_m2m_manager<Zone, Endpoint>().add_entry(zone_uuid, host_endpoint_uuid);
        log_debug("pnc-gami", "Added host endpoints: " + host_endpoint_uuid + " to zone: " + zone_uuid);
    });

    const attribute::Array<Uuid> target_endpoints_uuids{};
    store_zone_in_db(host_endpoints_uuids, target_endpoints_uuids, zone_uuid, fabric_uuid);
}


void add_zone(const AddZone::Request& request, AddZone::Response& response) {

    log_debug("pnc-gami", "Adding zone");

    validate_zone_endpoints(request.get_endpoints());

    get_manager<Fabric>().get_entry(request.get_fabric());  // Throws if Fabric isn't in the model

    auto zone_builder = agent::pnc::discovery::builders::ZoneBuilder();
    zone_builder.init(request.get_fabric());

    Uuid switch_uuid = get_manager<Switch>().get_keys().front(); // We assume there is exactly 1 Switch

    zone_builder.update_links(switch_uuid);
    Zone zone = zone_builder.build();
    Uuid zone_uuid = log_and_add<Zone>(zone);

    response.set_zone(zone_uuid);

    // add host endpoints:
    auto host_endpoints_uuids = Toolset::get().model_tool->get_endpoints_with_role(request.get_endpoints(),
                                                                                   EntityRole::Initiator);

    // add target endpoints:
    auto target_endpoints_uuids = Toolset::get().model_tool->get_endpoints_with_role(request.get_endpoints(),
                                                                                     EntityRole::Target);

    if (host_endpoints_uuids.empty() && target_endpoints_uuids.empty()) {
        store_zone_in_db(host_endpoints_uuids, target_endpoints_uuids, zone_uuid, request.get_fabric());
    }
    else if (target_endpoints_uuids.empty()) {
        add_initiator_zone(host_endpoints_uuids, zone_uuid, request.get_fabric());
    }
    else {
        auto task_uuid = run_add_zone_task(host_endpoints_uuids,
                                           target_endpoints_uuids,
                                           zone_uuid,
                                           request.get_fabric());
        response.set_task(task_uuid);
    }

    log_info("pnc-gami", "Added zone with UUID '" + zone.get_uuid() + "'");
}

}

REGISTER_COMMAND(AddZone, ::add_zone);
