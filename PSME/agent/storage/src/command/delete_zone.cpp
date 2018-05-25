/*!
 * @brief Implementation of DeleteZonecommand.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation
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
 * @header{Files}
 * @file delete_zone.cpp
 */

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/storage_commands.hpp"
#include "database/persistent_attributes.hpp"
#include "database/aggregate.hpp"
#include "iscsi/utils.hpp"



using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace database;

namespace {

void delete_persistent_attributes(const Uuid& zone_uuid) {
    auto& pa = PersistentAttributes::get_instance();
    pa.remove_value(zone_uuid, literals::Zone::ZONE);
}


void clear_relations(const Uuid& zone_uuid) {
    auto& zone_endpoint_manager = agent_framework::module::get_m2m_manager<Zone, Endpoint>();
    const auto endpoints = zone_endpoint_manager.get_children(zone_uuid);
    for (const auto& endpoint_uuid : endpoints) {
        zone_endpoint_manager.remove_entry(zone_uuid, endpoint_uuid);
    }

}


void delete_zone(const DeleteZone::Request& req, DeleteZone::Response&) {
    log_debug("storage-agent", "Deleting zone.");
    // check if zone exists
    get_manager<Zone>().get_entry(req.get_zone());

    try {
        agent::storage::iscsi::tgt::remove_iscsi_targets_from_endpoints(
            agent_framework::module::get_m2m_manager<Zone, Endpoint>().get_children(req.get_zone()));
    }
    catch (...) {
        log_debug("storage-agent", "Error while removing iSCSI Targets, verify TGT daemon status");
    }

    delete_persistent_attributes(req.get_zone());
    clear_relations(req.get_zone());
    get_manager<Zone>().remove_entry(req.get_zone());
    log_debug("storage-agent", "Removed zone with UUID '" + req.get_zone() + "'");
}
}

REGISTER_COMMAND(DeleteZone, ::delete_zone);
