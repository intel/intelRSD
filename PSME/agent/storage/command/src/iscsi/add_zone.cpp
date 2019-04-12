/*!
 * @brief Implementation of AddZone command.
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
 * @file add_zone.cpp
 */

#include "storage_agent_commands.hpp"
#include "agent/utils/iscsi/utils.hpp"
#include "agent-framework/database/database_entity.hpp"
#include "agent-framework/module/storage_components.hpp"
#include "agent-framework/discovery/builders/zone_builder.hpp"


using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::storage;

namespace {

void store_persistent_attributes(const Uuid& zone_uuid, const AddZone::Request& request) {
    auto& pa = database::PersistentAttributes::get_instance();
    database::Aggregate<database::PersistentAttributes> aggr_zone{pa, zone_uuid, literals::Zone::ZONE};
    for (const Uuid& endpoint_uuid : request.get_endpoints()) {
        aggr_zone.append(literals::Endpoint::ENDPOINT, endpoint_uuid);
    }
    aggr_zone.dump();
}

void add_zone(AddZone::ContextPtr context, const AddZone::Request& request, AddZone::Response& response) {
    log_info("storage-agent", "Adding zone.");

    get_manager<Fabric>().get_entry(request.get_fabric());
    Zone zone = agent_framework::discovery::ZoneBuilder::build_default(request.get_fabric());

    agent::storage::utils::check_endpoints_added_to_zone(context, request.get_endpoints());
    for (const Uuid& endpoint_uuid : request.get_endpoints()) {
        get_m2m_manager<Zone, Endpoint>().add_entry(zone.get_uuid(), endpoint_uuid);
    }

    // iSCSI Target can be only created if Zone contains Initiator Endpoint
    if (agent::storage::utils::is_role_present(request.get_endpoints(), enums::EntityRole::Initiator)) {
        try {
            agent::storage::utils::create_iscsi_targets_from_endpoints(context, request.get_endpoints());
        }
        catch (const agent_framework::exceptions::InvalidValue&) {
            log_error("storage-agent", "Could not create iSCSI Target for zone: " + zone.get_uuid());
            agent::storage::utils::remove_iscsi_targets_from_endpoints(context, request.get_endpoints());
            for (const Uuid& endpoint_uuid : request.get_endpoints()) {
                get_m2m_manager<Zone, Endpoint>().remove_entry(zone.get_uuid(), endpoint_uuid);
            }
            throw;
        }
    }

    // Zones are stored in database so stabilization is not needed.
    store_persistent_attributes(zone.get_uuid(), request);

    response.set_zone(zone.get_uuid());
    get_manager<Zone>().add_entry(zone);
    log_info("storage-agent", "Added zone with UUID '" + zone.get_uuid() + "'");
}
}

REGISTER_STORAGE_COMMAND(AddZone, ::add_zone);
