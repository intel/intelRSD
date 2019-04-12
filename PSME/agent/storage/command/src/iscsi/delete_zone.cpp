/*!
 * @brief Implementation of DeleteZone command.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file delete_zone.cpp
 */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/database/database_entity.hpp"
#include "storage_agent_commands.hpp"
#include "agent/utils/iscsi/utils.hpp"



using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::storage;

namespace {

void delete_persistent_attributes(const Uuid& zone_uuid) {
    auto& pa = database::PersistentAttributes::get_instance();
    pa.remove_value(zone_uuid, literals::Zone::ZONE);
}


void clear_relations(const Uuid& zone_uuid) {
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    const auto endpoints = zone_endpoint_manager.get_children(zone_uuid);
    for (const auto& endpoint_uuid : endpoints) {
        zone_endpoint_manager.remove_entry(zone_uuid, endpoint_uuid);
    }
}


void delete_zone(DeleteZone::ContextPtr context, const DeleteZone::Request& request, DeleteZone::Response&) {
    log_info("storage-agent", "Deleting zone...");
    // check if zone exists
    get_manager<Zone>().get_entry(request.get_zone());

    try {
        agent::storage::utils::remove_iscsi_targets_from_endpoints(context,
            agent_framework::module::get_m2m_manager<Zone, Endpoint>().get_children(request.get_zone()));
    }
    catch (...) {
        log_error("storage-agent", "Error while removing iSCSI Targets, verify TGT daemon status...");
    }

    delete_persistent_attributes(request.get_zone());
    clear_relations(request.get_zone());
    get_manager<Zone>().remove_entry(request.get_zone());
    log_info("storage-agent", "Removed zone with UUID '" + request.get_zone() + "'");
}
}

REGISTER_STORAGE_COMMAND(DeleteZone, ::delete_zone);
