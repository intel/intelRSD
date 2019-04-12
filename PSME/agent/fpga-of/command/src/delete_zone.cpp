/*!
* @brief Implementation of DeleteZone command.
*
* @copyright Copyright (c) 2018-2019 Intel Corporation
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

#include "fpgaof_agent_commands.hpp"
#include "agent-framework/module/common_components.hpp"
#include "opaepp/opae-proxy/opae_proxy_host_api.hpp"
#include "agent-framework/database/database_keys.hpp"
#include "agent-framework/database/database_entities.hpp"



using namespace agent::fpgaof;
using namespace agent_framework::model;
using namespace agent_framework::module;
using namespace agent_framework::database;
using namespace agent::fpgaof::loader;
using namespace opaepp;

namespace {

void delete_zone(DeleteZone::ContextPtr ctx, const DeleteZone::Request& req, DeleteZone::Response&) {
    const Uuid& zone{req.get_zone()};
    // check if zone exists
    get_manager<Zone>().get_entry(zone);

    if (Zone::is_initiator_in_zone(zone) && Zone::is_target_in_zone(zone)) {
        const auto& connected_devices = Zone::get_connected_devices(zone);
        for (const auto& device_uuid : connected_devices) {
            OpaeProxyHostApi::remove_device_ownership(*ctx->opae_proxy_context, device_uuid);
            auto host_uuid = Zone::get_initiator_endpoint(zone).value().get_uuid();
            log_debug("opae-proxy", "Host '" + host_uuid + "' lost ownership of device '" + device_uuid + "'");
        }
    }

    ZoneEntity::remove(zone);
    FabricEntity(get_manager<Fabric>().get_keys().front()).del(zone);

    get_m2m_manager<Zone, Endpoint>().remove_parent(zone);
    get_manager<Zone>().remove_entry(zone);
    log_info("fpgaof-agent", "Removed zone with UUID '" + zone + "'");
}

}

REGISTER_FPGAOF_COMMAND(DeleteZone, ::delete_zone);

