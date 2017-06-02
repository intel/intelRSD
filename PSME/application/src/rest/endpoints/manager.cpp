/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * */

#include "agent-framework/service_uuid.hpp"
#include "agent-framework/module/model/manager.hpp"
#include "psme/rest/constants/constants.hpp"
#include "psme/rest/endpoints/manager.hpp"
#include "psme/rest/utils/status_helpers.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;

using agent_framework::generic::ServiceUuid;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Manager.Manager";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Manager.v1_2_0.Manager";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Manager";
    r[Common::DESCRIPTION] = "Manager description";
    r[Manager::MANAGER_TYPE] = json::Value::Type::NIL;
    r[Manager::POWER_STATE] = agent_framework::model::enums::ManagerPowerState(
        agent_framework::model::enums::ManagerPowerState::On).to_string();
    r[Common::MODEL] = json::Value::Type::NIL;
    r[Common::UUID] = json::Value::Type::NIL;

    r[Manager::DATE_TIME] = json::Value::Type::NIL;
    r[Manager::DATE_TIME_LOCAL_OFFSET] = json::Value::Type::NIL;
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    r[Manager::ETHERNET_INTERFACES] = json::Value::Type::NIL;

    /* TODO for BVT, cannot be read right now, will be used later
     * r[Manager::GRAPHICAL_CONSOLE] = json::Value::Type::NIL;
     * r[Manager::SERIAL_CONSOLE] = json::Value::Type::NIL;
     * r[Manager::COMMAND_SHELL] = json::Value::Type::NIL;
     * */

    r[Common::FIRMWARE_VERSION] = json::Value::Type::NIL;
    r[Common::OEM] = json::Value::Type::OBJECT;
    r[Common::LINKS][Common::ODATA_TYPE] = "#Manager.v1_1_0.Links";
    r[Common::LINKS][Manager::MANAGER_FOR_CHASSIS] = json::Value::Type::ARRAY;
    r[Common::LINKS][Manager::MANAGER_FOR_SERVERS] = json::Value::Type::ARRAY;
    r[Common::LINKS][Manager::MANAGER_IN_CHASSIS] = json::Value::Type::NIL;
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][Common::ODATA_TYPE] = "#Intel.Oem.ManagerLinks";
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][Manager::MANAGER_FOR_SERVICES] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::OEM][Common::RACKSCALE][Manager::MANAGER_FOR_SWITCHES] = json::Value::Type::ARRAY;

    r[Manager::NETWORK_PROTOCOL][Common::ODATA_ID] = json::Value::Type::OBJECT;
    return r;
}


void fill_links(const agent_framework::model::Manager& manager, json::Value& r) {
    // find all chassis managed by this manager
    auto chassis_uuids = agent_framework::module::get_manager<agent_framework::model::Chassis>().get_keys(
        manager.get_uuid());
    for (const auto& chassis_uuid : chassis_uuids) {
        json::Value chassis_link;
        chassis_link[Common::ODATA_ID] =
            psme::rest::endpoint::utils::get_component_url(agent_framework::model::enums::Component::Chassis,
                                                           chassis_uuid);

        r[Common::LINKS][constants::Manager::MANAGER_FOR_CHASSIS].push_back(chassis_link);

        // chassis managed by the manager is at the same time the chassis manager is located in
        r[Common::LINKS][constants::Manager::MANAGER_IN_CHASSIS] = chassis_link;
    }

    // find all systems managed by this manager
    auto system_ids = agent_framework::module::CommonComponents::get_instance()->get_system_manager().get_ids(
        manager.get_uuid());
    for (auto system_id : system_ids) {
        json::Value link;
        link[Common::ODATA_ID] = psme::rest::endpoint::PathBuilder(constants::PathParam::BASE_URL)
            .append(constants::Root::SYSTEMS)
            .append(system_id)
            .build();
        r[Common::LINKS][constants::Manager::MANAGER_FOR_SERVERS].push_back(std::move(link));
    }

    // find all switches managed by this manager
    auto switch_ids = agent_framework::module::NetworkComponents::get_instance()->get_switch_manager().get_ids(
        manager.get_uuid());
    for (auto switch_id : switch_ids) {
        json::Value link;
        link[Common::ODATA_ID] = psme::rest::endpoint::PathBuilder(constants::PathParam::BASE_URL)
            .append(constants::Root::ETHERNET_SWITCHES)
            .append(switch_id)
            .build();
        r[Common::LINKS][Common::OEM][Common::RACKSCALE][constants::Manager::MANAGER_FOR_SWITCHES].push_back(
            std::move(link));
    }

    // find all services managed by this manager
    auto service_ids = agent_framework::module::StorageComponents::get_instance()->
        get_storage_services_manager().get_ids(manager.get_uuid());
    for (auto service_id : service_ids) {
        json::Value link;
        link[Common::ODATA_ID] = psme::rest::endpoint::PathBuilder(constants::PathParam::BASE_URL)
            .append(constants::Root::SERVICES)
            .append(service_id)
            .build();
        r[Common::LINKS][Common::OEM][Common::RACKSCALE][Manager::MANAGER_FOR_SERVICES].push_back(std::move(link));
    }
}
}


/* TODO for BVT, cannot be read right now, will be used later
template <typename T>
json::Value console_to_json(const T& console)
{
    if (console.get_enabled().has_value()) {
        json::Value r;
        r[constants::Manager::SERVICE_ENABLED] = console.get_enabled();

        r[constants::Manager::MAX_CONCURRENT_SESSIONS] = 0;
        if(console.get_max_sessions().has_value()) {
            r[constants::Manager::MAX_CONCURRENT_SESSIONS] = console.get_max_sessions();
        }

        json::Value array_json = json::Value::Type::ARRAY;;
        const auto& supported = console.get_types_supported();
        for (const auto& entry : supported) {
            array_json.push_back(std::move(entry.to_string()));
        }
        r[constants::Manager::CONNECT_TYPES_SUPPORTED] = std::move(array_json);
        return r;
    }
    else {
        return json::Value::Type::NIL;
    }
}*/

endpoint::Manager::Manager(const std::string& path) : EndpointBase(path) {}


endpoint::Manager::~Manager() {}


void endpoint::Manager::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();

    using namespace agent_framework::model::enums;

    auto manager = psme::rest::model::Find<agent_framework::model::Manager>(req.params[PathParam::MANAGER_ID]).get();

    psme::rest::endpoint::status_to_json(manager, r);
    r[Common::STATUS][Common::HEALTH_ROLLUP] =
        psme::rest::endpoint::HealthRollup<agent_framework::model::Manager>().get(manager.get_uuid());
    fill_links(manager, r);

    if (ManagerInfoType::EnclosureManager == manager.get_manager_type()) {
        r[constants::Manager::SERVICE_ENTRY_POINT_UUID] =
            ServiceUuid::get_instance()->get_service_uuid();
    }

    r[constants::Manager::MANAGER_TYPE] = manager.get_manager_type();
    r[Common::MODEL] = manager.get_manager_model();
    r[Common::FIRMWARE_VERSION] = manager.get_firmware_version();
    r[Common::UUID] = manager.get_guid();

    /* TODO for BVT, cannot be read right now, will be used later
     * r[constants::Manager::GRAPHICAL_CONSOLE] = console_to_json(manager.get_graphical_console());
     * r[constants::Manager::SERIAL_CONSOLE] = console_to_json(manager.get_serial_console());
     * r[constants::Manager::COMMAND_SHELL] = console_to_json(manager.get_command_shell());
     * */

    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = req.params[PathParam::MANAGER_ID];
    r[constants::Manager::NETWORK_PROTOCOL][Common::ODATA_ID] =
        PathBuilder(req).append(constants::Manager::NETWORK_PROTOCOL).build();
    r[constants::Manager::ETHERNET_INTERFACES][Common::ODATA_ID] =
        PathBuilder(req).append(constants::Manager::ETHERNET_INTERFACES).build();

    set_response(res, r);
}
