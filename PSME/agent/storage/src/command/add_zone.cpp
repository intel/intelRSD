/*!
 * @brief Implementation of AddZone command.
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
 * @file add_zone.cpp
 */

#include "iscsi/utils.hpp"
#include "agent-framework/command/registry.hpp"
#include "agent-framework/command/storage_commands.hpp"
#include "database/persistent_attributes.hpp"
#include "database/aggregate.hpp"



using namespace agent_framework::command;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework;
using namespace database;

namespace {

void store_persistent_attributes(const Uuid& zone_uuid, const AddZone::Request& req) {
    auto& pa = PersistentAttributes::get_instance();
    Aggregate<PersistentAttributes> aggr_zone{pa, zone_uuid, literals::Zone::ZONE};
    for (const Uuid& endpoint_uuid : req.get_endpoints()) {
        aggr_zone.append(literals::Endpoint::ENDPOINT, endpoint_uuid);
    }
    aggr_zone.dump();
}

void add_zone(const AddZone::Request& req, AddZone::Response& rsp) {
    log_debug("storage-agent", "Adding zone.");

    auto& zone_endpoint_manager = get_m2m_manager<Zone, Endpoint>();
    get_manager<Fabric>().get_entry(req.get_fabric());
    Zone zone{req.get_fabric()};

    agent::storage::iscsi::tgt::check_endpoints_added_to_zone(req.get_endpoints());
    for (const Uuid& endpoint_uuid : req.get_endpoints()) {
        zone_endpoint_manager.add_entry(zone.get_uuid(), endpoint_uuid);
    }

    // iSCSI Target can be only created if Zone contains Initiator Endpoint
    if (agent::storage::iscsi::tgt::is_role_present(req.get_endpoints(), enums::EntityRole::Initiator)) {
        try {
            agent::storage::iscsi::tgt::create_iscsi_targets_from_endpoints(req.get_endpoints());
        }
        catch (const exceptions::InvalidValue&) {
            log_error("storage-agent", "Could not create iSCSI Target for zone: " + zone.get_uuid());
            agent::storage::iscsi::tgt::remove_iscsi_targets_from_endpoints(req.get_endpoints());
            for (const Uuid& endpoint_uuid : req.get_endpoints()) {
                zone_endpoint_manager.remove_entry(zone.get_uuid(), endpoint_uuid);
            }
            throw;
        }
    }

    // Zones are stored in database so stabilization is not needed.
    store_persistent_attributes(zone.get_uuid(), req);

    zone.set_status({enums::State::Enabled, enums::Health::OK});
    zone.add_collection(model::attribute::Collection(
        model::enums::CollectionName::Endpoints,
        model::enums::CollectionType::Endpoints
    ));

    rsp.set_zone(zone.get_uuid());
    get_manager<Zone>().add_entry(zone);
    log_debug("storage-agent", "Added zone with UUID '" + zone.get_uuid() + "'");
}
}

REGISTER_COMMAND(AddZone, ::add_zone);
