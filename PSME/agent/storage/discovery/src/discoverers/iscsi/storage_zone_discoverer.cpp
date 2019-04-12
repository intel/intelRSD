/*!
 * @brief Storage Zone discoverer implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file storage_zone_discoverer.cpp
 */


#include "storage_agent_context.hpp"
#include "discovery/discoverers/storage_zone_discoverer.hpp"

#include "agent-framework/database/database_entity.hpp"
#include "agent-framework/discovery/builders/zone_builder.hpp"
#include "agent-framework/module/common_components.hpp"



using namespace agent_framework::model;
using namespace agent::storage;
using namespace agent::storage::discovery;
using namespace database;

std::vector<Zone> StorageZoneDiscoverer::discover(const Uuid& parent_uuid) {

    std::vector<Zone> zones{};
    auto& pa = PersistentAttributes::get_instance();

    for (const auto& record : Aggregate<PersistentAttributes>::get_aggregates(pa, literals::Zone::ZONE)) {
        auto zone = agent_framework::discovery::ZoneBuilder::build_default(parent_uuid);
        zone.set_uuid(record.get_uuid());

        try {
            for (const auto& endpoint_uuid : record.get_multiple_values(literals::Endpoint::ENDPOINT)) {
                try {
                    agent_framework::module::get_manager<Endpoint>().get_entry(endpoint_uuid);
                    agent_framework::module::get_m2m_manager<Zone, Endpoint>().add_entry(record.get_uuid(), endpoint_uuid);
                }
                catch (const agent_framework::exceptions::InvalidUuid&) {
                    log_warning("storage-discovery",
                                "Zone " << record.get_uuid() << " points to a missing Endpoint " << endpoint_uuid);
                    zone.set_status({enums::State::Disabled, enums::Health::Critical});
                }
            }
        }
        catch (const std::out_of_range&) {
            log_debug("storage-discovery", "Zone " << record.get_uuid() << " has no Endpoints");
        }
        zones.emplace_back(std::move(zone));
    }

    return zones;
}