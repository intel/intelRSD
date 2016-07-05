 /*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#include "psme/rest-ref/endpoints/physical_drive.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "agent-framework/module-ref/storage_manager.hpp"

using namespace psme::rest;
using namespace app::rest::constants;
using namespace app::rest;
using namespace agent_framework::module;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);
    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Drive/Links/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#PhysicalDrive.1.0.0.PhysicalDrive";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Physical Drive";
    r[Common::NAME] = "Physical Drive description";
    r[constants::Drive::INTERFACE] = json::Value::Type::NIL;
    r[constants::Drive::CAPACITY] = json::Value::Type::NIL;
    r[constants::Drive::TYPE] = json::Value::Type::NIL;
    r[constants::Drive::RPM] = json::Value::Type::NIL;
    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[Common::MODEL] = json::Value::Type::NIL;
    r[Common::SERIAL] = json::Value::Type::NIL;
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[Common::OEM] = json::Value::Type::OBJECT;
    r[Common::LINKS][constants::Drive::USED_BY] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::OEM] = json::Value::Type::OBJECT;

    return r;
}

void add_used_by_link(json::Value& json, const server::Request& req,
    const std::string& uuid) {

    json[Common::LINKS][constants::Drive::USED_BY] = json::Value::Type::ARRAY;

    // get many-to-many-manager for physical drives
    auto& physical_members_manager =
        agent_framework::module::StorageManager::get_instance()
            ->get_physical_drive_members_manager();

    // check if current drive is a child
    if (!physical_members_manager.child_exists(uuid)) {
        return ;
    }

    // get parents, there should be only one
    auto parents = physical_members_manager.get_parents(uuid);
    if (1 != parents.size()) {
        log_error(GET_LOGGER("rest"), "Model PhysicalDrive/UsedBy link error");
        log_error(GET_LOGGER("rest"), "Logical Drive " << uuid <<
            " is used by more than one logical drives!");
        return ;
    }

    try {
        // convert parent UUID into ID
        auto id = agent_framework::module::StorageManager::get_instance()
            ->get_logical_drive_manager().get_entry(parents.front()).get_id();

        // fill the link
        json::Value link;
        link[Common::ODATA_ID] = endpoint::PathBuilder(PathParam::BASE_URL)
                                    .append(Root::SERVICES)
                                    .append(req.params[PathParam::SERVICE_ID])
                                    .append(StorageService::LOGICAL_DRIVES)
                                    .append(id).build();
        json[Common::LINKS][constants::Drive::USED_BY].push_back(
            std::move(link));
    }
    catch (agent_framework::exceptions::InvalidParameters&) {
        log_error(GET_LOGGER("rest"), "Model PhysicalDrive/UsedBy link error");
        log_error(GET_LOGGER("rest"), "PhysicalDrive " << uuid << "is used by "
            << " a LogicalDrive (" << parents.front() << ") that does not exist");
    }
}

}

endpoint::PhysicalDrive::PhysicalDrive(const std::string& path) : Endpoint(path) {}
endpoint::PhysicalDrive::~PhysicalDrive() {}

void endpoint::PhysicalDrive::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();

    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = req.params[PathParam::DRIVE_ID];

    auto drive =
        psme::rest::model::Find<agent_framework::model::PhysicalDrive>(req.params[PathParam::DRIVE_ID]).
        via<agent_framework::model::StorageServices>(req.params[PathParam::SERVICE_ID]).get();

    endpoint::utils::status_to_json(drive, r);

    r[constants::Drive::INTERFACE] = drive.get_interface();
    if (drive.get_capacity_gb().has_value()) {
        r[constants::Drive::CAPACITY] = endpoint::utils::gb_to_gib(drive.get_capacity_gb().value());
    }
    r[constants::Drive::TYPE] = drive.get_type();
    r[constants::Drive::RPM] = drive.get_rpm();
    r[Common::MANUFACTURER] = drive.get_fru_info().get_manufacturer();
    r[Common::MODEL] = drive.get_fru_info().get_model_number();
    r[Common::SERIAL] = drive.get_fru_info().get_serial_number();

    add_used_by_link(r, req, drive.get_uuid());

    set_response(res, r);
}
