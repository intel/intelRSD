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
 * @file command/sdv/delete_zone_endpoints.cpp
 * @brief DeleteZoneEndpoints sdv implementation
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
#include "tools/database_keys.hpp"
#include "agent-framework/database/database_entities.hpp"



using namespace agent_framework::command;
using namespace agent_framework::module;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;
using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using namespace agent::pnc::tools;

namespace {

// @TODO consider std::partition_copy
void partition_target_and_initiator_endpoints(const attribute::Array<Uuid>& input_endpoints,
                                              attribute::Array<Uuid>& target_endpoints,
                                              attribute::Array<Uuid>& initiator_endpoints) {
    for (const auto& endpoint_uuid : input_endpoints) {
        if (Endpoint::is_target(endpoint_uuid)) {
            target_endpoints.add_entry(endpoint_uuid);
        }
        else {
            initiator_endpoints.add_entry(endpoint_uuid);
        }
    }
}


void validate_endpoints_are_bound(const attribute::Array<Uuid>& endpoint_uuids, const Uuid& zone_uuid) {
    for (const auto& endpoint_uuid : endpoint_uuids) {
        if (!get_m2m_manager<Zone, Endpoint>().entry_exists(zone_uuid, endpoint_uuid)) {
            THROW(agent_framework::exceptions::InvalidValue, "pnc-gami", "Endpoint not present in given zone.");
        }
    }
}


void validate_duplicate_endpoints(const attribute::Array<Uuid>& endpoint_uuids) {
    std::set<Uuid> endpoints_set(endpoint_uuids.begin(), endpoint_uuids.end());
    if (endpoints_set.size() != endpoint_uuids.size()) {
        THROW(agent_framework::exceptions::InvalidValue, "pnc-gami", "Request contains duplicated endpoints.");
    }
}


void validate_request(const DeleteZoneEndpoints::Request& request) {
    validate_duplicate_endpoints(request.get_endpoints());
    validate_endpoints_are_bound(request.get_endpoints(), request.get_zone());
}


void unbind_endpoints(const attribute::Array<Uuid>& endpoint_uuids, const Uuid& zone_uuid) {
    log_debug("pnc-gami", "Unbinding " + std::to_string(int(endpoint_uuids.size())) +
                          " endpoints from zone on switch, zone uuid: " << zone_uuid);
    // currently we support only one switch
    auto zone = get_manager<Zone>().get_entry_reference(zone_uuid);
    Switch sw = get_manager<Switch>().get_entry(zone->get_switch_uuid());
    GlobalAddressSpaceRegisters gas = GlobalAddressSpaceRegisters::get_default(sw.get_memory_path());

    for (const auto& endpoint_uuid : endpoint_uuids) {
        if (Endpoint::is_target(endpoint_uuid)) {
            Toolset::get().gas_tool->unbind_endpoint_from_zone(gas, zone_uuid, endpoint_uuid);
        }
        else {
            log_debug("pnc-gami", "Removing zone id from zone.");
            zone->set_zone_id({});
        }

        log_debug("pnc-gami", "Endpoint detached from zone, endpoint uuid: " << endpoint_uuid);
    }
}


void remove_endpoints_from_zone_model(const attribute::Array<Uuid>& endpoint_uuids, const Uuid& zone_uuid) {
    log_debug("pnc-gami", "Remove " + std::to_string(endpoint_uuids.size()) + " endpoints from zone model, zone uuid: "
        << zone_uuid);
    Zone zone = get_manager<Zone>().get_entry(zone_uuid);

    for (const auto& endpoint_uuid : endpoint_uuids) {

        get_m2m_manager<Zone, Endpoint>().remove_entry(zone_uuid, endpoint_uuid);

        log_debug("pnc-gami", "Endpoint removed from zone model, endpoint uuid: " << endpoint_uuid);
    }
}


void remove_endpoints_from_zone_db(const attribute::Array<Uuid>& endpoints_uuids,
                                   const Uuid& zone_uuid) {

    log_debug("pnc-gami", "Remove endpoints from zone db, zone uuid: " << zone_uuid);

    agent_framework::database::ZoneEntity zone_db{zone_uuid};

    std::for_each(endpoints_uuids.begin(), endpoints_uuids.end(),
                  [&](const Uuid& endpoint_uuid) {
                      zone_db.del(endpoint_uuid);
                      log_debug("pnc-gami", "Endpoint removed from zone db, endpoint uuid: " << endpoint_uuid);
                  });
}


void delete_zone_endpoints(const DeleteZoneEndpoints::Request& request,
                           DeleteZoneEndpoints::Response& response) {
    log_info("pnc-gami", "Deleting endpoints from zone.");

    validate_request(request);

    auto response_builder = []() {
        DeleteZoneEndpoints::Response res{};
        return res.to_json();
    };

    auto exception_builder = [](const agent_framework::exceptions::GamiException& exception) {
        return agent_framework::exceptions::PncError("Could not unbind endpoint. " + exception.get_message());
    };

    agent_framework::action::TaskCreator task_creator{};
    task_creator.prepare_task();

    /*  Preparing data to analyze the request */
    attribute::Array<Uuid> request_target_endpoints{};
    attribute::Array<Uuid> request_initiator_endpoints{};
    partition_target_and_initiator_endpoints(request.get_endpoints(), request_target_endpoints,
                                             request_initiator_endpoints);

    /* Getting current zone configuration for comparison */
    attribute::Array<Uuid> endpoints_in_zone = get_m2m_manager<Zone, Endpoint>().get_children(request.get_zone());
    attribute::Array<Uuid> zone_target_endpoints{};
    attribute::Array<Uuid> zone_initiator_endpoints{};
    partition_target_and_initiator_endpoints(endpoints_in_zone, zone_target_endpoints, zone_initiator_endpoints);

    /* Preparing conditions that determine which actions need to be done */
    bool initiator_endpoint_in_zone = !zone_initiator_endpoints.empty();
    bool delete_initiator = !request_initiator_endpoints.empty();
    bool delete_targets = !request_target_endpoints.empty();

    /* Zone contains information about zone/partition on the switch */
    Zone zone = get_manager<Zone>().get_entry(request.get_zone());

    /* Preparing containers for taking actions */
    attribute::Array<Uuid> endpoints_to_unbind_on_switch{};
    attribute::Array<Uuid> endpoints_to_remove_from_model_db{};

    /* Filling containers with proper endpoints depending on the conditions */
    if (delete_initiator && delete_targets) {
        /* if we remove initiator and remove some targets, we unbind all endpoints from partition,
         remove requested targets and initiators from db and model,
         (initiator presence in the zone is necessary for establishing connections on the switch) */
        log_debug("pnc-gami", "Removing initiator and target endpoints");
        endpoints_to_unbind_on_switch = endpoints_in_zone;
        endpoints_to_remove_from_model_db = request.get_endpoints();
    }
    else if (delete_initiator && !delete_targets) {
        /* if we remove initiator and leave all targets, we unbind all targets on the switch,
         but leave in the db, and remove zone id from zone */
        log_notice("pnc-gami", "Removing only initiator endpoint");
        endpoints_to_unbind_on_switch = endpoints_in_zone;
        endpoints_to_remove_from_model_db = request_initiator_endpoints;

        if (zone_target_endpoints.empty()) {
            /* no target endpoints to unbind - take actions immediately and return */
            remove_endpoints_from_zone_model(endpoints_to_remove_from_model_db, request.get_zone());
            remove_endpoints_from_zone_db(endpoints_to_remove_from_model_db, request.get_zone());
            return;
        }
    }
    else { // no initiators to remove
        if (!delete_targets) {
            /* no initiators and targets to remove - return immediately */
            return;
        }
        if (initiator_endpoint_in_zone) {
            /* if we leave initiator and remove some targets, we unbind them and remove from model and db */
            log_notice("pnc-gami", "Removing and unbinding target endpoints");
            endpoints_to_unbind_on_switch = request_target_endpoints;
            endpoints_to_remove_from_model_db = request_target_endpoints;
        }
        else {
            /* nothing to unbind - update model and database immediately and return */
            log_notice("pnc-gami", "Removing target endpoints from model and db");
            endpoints_to_remove_from_model_db = request_target_endpoints;

            remove_endpoints_from_zone_model(endpoints_to_remove_from_model_db, request.get_zone());
            remove_endpoints_from_zone_db(endpoints_to_remove_from_model_db, request.get_zone());
            return;
        }
    }

    task_creator.add_subtask(
        std::bind(unbind_endpoints, endpoints_to_unbind_on_switch, request.get_zone()));
    task_creator.add_subtask(
        std::bind(remove_endpoints_from_zone_model, endpoints_to_remove_from_model_db, request.get_zone()));
    task_creator.add_subtask(
        std::bind(remove_endpoints_from_zone_db, endpoints_to_remove_from_model_db, request.get_zone()));

    task_creator.set_promised_error_thrower(exception_builder);
    task_creator.set_promised_response(response_builder);

    task_creator.register_task();
    auto task = task_creator.get_task();
    agent_framework::action::TaskRunner::get_instance().run(task);

    log_info("pnc-agent", "Endpoint unbinding started.");

    response.set_task(task_creator.get_task_resource().get_uuid());
}
}

REGISTER_COMMAND(DeleteZoneEndpoints, ::delete_zone_endpoints);
