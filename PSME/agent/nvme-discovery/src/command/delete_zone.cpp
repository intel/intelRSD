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

#include "nvme_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"
#include "loader/config.hpp"
#include "tools/databases.hpp"

using namespace agent::nvme;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent::nvme::tools;
using namespace agent::nvme::loader;

namespace {

void delete_zone(DeleteZone::ContextPtr, const DeleteZone::Request& req, DeleteZone::Response&) {
    const Uuid& zone{req.get_zone()};
    // check if zone exists
    get_manager<Zone>().get_entry(zone);

    // remove zone database
    ZoneDatabase zone_db{zone};
    zone_db.remove();
    // delete entry from fabric database
    FabricDatabase fabric_db{get_manager<Fabric>().get_keys().front()};
    fabric_db.del(zone);

    get_m2m_manager<Zone, Endpoint>().remove_parent(zone);
    get_manager<Zone>().remove_entry(zone);
    log_info("nvme-discovery-agent", "Removed zone with UUID '" + zone + "'");
}
}

REGISTER_NVME_COMMAND(DeleteZone, ::delete_zone);
