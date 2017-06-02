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

#include "psme/rest/utils/status_helpers.hpp"
#include "psme/rest/endpoints/storage/logical_drive.hpp"
#include "psme/rest/model/handlers/generic_handler.hpp"
#include "psme/rest/validators/json_validator.hpp"
#include "psme/rest/validators/schemas/logical_drive.hpp"
#include "agent-framework/module/requests/storage.hpp"
#include "agent-framework/module/responses/storage.hpp"
#include "psme/rest/server/error/error_factory.hpp"



using namespace psme::rest;
using namespace psme::rest::constants;
using namespace psme::rest::validators;

namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Services/1/LogicalDrives/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#LogicalDrive.v1_0_0.LogicalDrive";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Logical Drive";
    r[Common::DESCRIPTION] = "Logical Drive description";
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[LogicalDrive::TYPE] = json::Value::Type::NIL;
    r[LogicalDrive::MODE] = json::Value::Type::NIL;
    r[LogicalDrive::PROTECTED] = json::Value::Type::NIL;
    r[LogicalDrive::CAPACITY] = json::Value::Type::NIL;
    r[LogicalDrive::IMAGE] = json::Value::Type::NIL;
    r[LogicalDrive::BOOTABLE] = json::Value::Type::NIL;
    r[LogicalDrive::SNAPSHOT] = json::Value::Type::NIL;
    r[Common::OEM] = json::Value::Type::OBJECT;
    r[Common::LINKS][LogicalDrive::LOGICAL_DRIVES] = json::Value::Type::ARRAY;
    r[Common::LINKS][LogicalDrive::PHYSICAL_DRIVES] = json::Value::Type::ARRAY;
    r[Common::LINKS][LogicalDrive::MASTER_DRIVE] = json::Value::Type::NIL;
    r[Common::LINKS][Drive::USED_BY] = json::Value::Type::ARRAY;
    r[Common::LINKS][LogicalDrive::TARGETS] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::OEM] = json::Value::Type::OBJECT;

    return r;
}


void add_master_drive_link(json::Value& json, const server::Request& req,
                           const agent_framework::model::LogicalDrive& drive) {

    json[Common::LINKS][LogicalDrive::MASTER_DRIVE] = json::Value::Type::NIL;

    auto master = drive.get_master();
    // if there is no master - master UUID should be an empty string
    if ((!master.has_value()) || master.value().empty()) {
        return;
    }

    try {
        // get master drive ID basing on the UUID
        auto master_id = agent_framework::module::StorageComponents::get_instance()->
            get_logical_drive_manager().get_entry(master.value()).get_id();

        // fill the link
        json[Common::LINKS]
        [constants::LogicalDrive::MASTER_DRIVE]
        [Common::ODATA_ID] = endpoint::PathBuilder(PathParam::BASE_URL)
            .append(Root::SERVICES)
            .append(req.params[PathParam::SERVICE_ID])
            .append(constants::StorageService::LOGICAL_DRIVES)
            .append(master_id).build();
    }
    catch (const agent_framework::exceptions::InvalidValue&) {
        log_error(GET_LOGGER("rest"), "Model LogicalDrive/MasterDrive link error");
        log_error(GET_LOGGER("rest"), "Master Logical drive " << master.value()
                                                              << " does not exist but is used by another drive");
    }
}


void add_targets_link(json::Value& json, const server::Request& req,
                      const std::string& parent, const std::string& uuid) {

    json[Common::LINKS][LogicalDrive::TARGETS] = json::Value::Type::ARRAY;

    // get all iSCSITarget UUIDs from the manager
    auto targets_id = agent_framework::module::StorageComponents::get_instance()->
        get_iscsi_target_manager().get_keys(parent);

    // for each UUID, convert it into the model object and get TargetLuns
    for (const auto& target_id : targets_id) {
        auto target = agent_framework::module::StorageComponents::get_instance()->
            get_iscsi_target_manager().get_entry(target_id);

        auto luns = target.get_target_lun();
        for (const auto& lun : luns) {

            // if TargetLun does not continue selected drive, go to the next one
            if (lun.get_logical_drive() != uuid) {
                continue;
            }

            // fill the link
            json::Value link;
            link[Common::ODATA_ID] =
                endpoint::PathBuilder(PathParam::BASE_URL)
                    .append(Root::SERVICES)
                    .append(req.params[PathParam::SERVICE_ID])
                    .append(constants::StorageService::TARGETS)
                    .append(target.get_id()).build();

            json[Common::LINKS][constants::LogicalDrive::TARGETS].push_back(std::move(link));
        }
    }
}


void add_physical_drives_link(json::Value& json, const server::Request& req,
                              const std::string& uuid) {

    json[Common::LINKS][LogicalDrive::PHYSICAL_DRIVES] = json::Value::Type::ARRAY;

    // get many-to-many-manager for physical drives
    auto& physical_members_manager =
        agent_framework::module::StorageComponents::get_instance()
            ->get_physical_drive_members_manager();

    // check if current drive is a parent of physical drives
    if (!physical_members_manager.parent_exists(uuid)) {
        return;
    }

    // get all children of the drive
    auto phys_members = physical_members_manager.get_children(uuid);
    for (const auto& phys_member : phys_members) {
        try {
            // convert children UUID into ID
            auto phys_id = agent_framework::module::StorageComponents::get_instance()->
                get_physical_drive_manager().get_entry(phys_member).get_id();

            // fill the link
            json::Value link;
            link[Common::ODATA_ID] =
                endpoint::PathBuilder(PathParam::BASE_URL)
                    .append(Root::SERVICES)
                    .append(req.params[PathParam::SERVICE_ID])
                    .append(constants::StorageService::DRIVES)
                    .append(phys_id).build();
            json[Common::LINKS][constants::LogicalDrive::PHYSICAL_DRIVES].push_back(
                std::move(link));
        }
        catch (const agent_framework::exceptions::InvalidUuid&) {
            log_error(GET_LOGGER("rest"), "Model LogicalDrive/PhysicalDrives link error");
            log_error(GET_LOGGER("rest"), "PhysicalDrive " << phys_member <<
                                                           " is present in the PhysicalDrivesMembers table but it does not exist as a resource");
        }
    }
}


void add_logical_drives_link(json::Value& json, const server::Request& req,
                             const std::string& uuid) {

    json[Common::LINKS][LogicalDrive::LOGICAL_DRIVES] = json::Value::Type::ARRAY;

    // get many-to-many-manager for logical drives
    auto& logical_members_manager =
        agent_framework::module::StorageComponents::get_instance()->get_logical_drive_members_manager();

    // check if current drive is a parent of logical drives
    if (!logical_members_manager.parent_exists(uuid)) {
        return;
    }

    // get all children of the drive
    auto log_members = logical_members_manager.get_children(uuid);
    for (const auto& log_member : log_members) {
        try {
            // convert children UUID into ID
            auto log_id = agent_framework::module::StorageComponents::get_instance()->
                get_logical_drive_manager().get_entry(log_member).get_id();

            // fill the link
            json::Value link;
            link[Common::ODATA_ID] =
                endpoint::PathBuilder(PathParam::BASE_URL)
                    .append(Root::SERVICES)
                    .append(req.params[PathParam::SERVICE_ID])
                    .append(constants::StorageService::LOGICAL_DRIVES)
                    .append(log_id).build();
            json[Common::LINKS][constants::LogicalDrive::LOGICAL_DRIVES].push_back(std::move(link));
        }
        catch (const agent_framework::exceptions::InvalidUuid&) {
            log_error(GET_LOGGER("rest"), "Model LogicalDrive/LogicalDrives link error");
            log_error(GET_LOGGER("rest"), "LogicalDrive " << log_member <<
                                                          " is present in the LogicalDrivesMembers table but it does not exist as a resource");
        }
    }
}


void add_used_by_link(json::Value& json, const server::Request& request, const std::string& uuid) {

    json[Common::LINKS][Drive::USED_BY] = json::Value::Type::ARRAY;

    // get many-to-many-manager for logical drives
    auto& logical_members_manager =
        agent_framework::module::StorageComponents::get_instance()->get_logical_drive_members_manager();

    // check if current drive is a child
    if (!logical_members_manager.child_exists(uuid)) {
        return;
    }

    // get parents, there should be only one
    auto parents = logical_members_manager.get_parents(uuid);
    if (1 != parents.size()) {
        log_error(GET_LOGGER("rest"), "Model LogicalDrive/UsedBy link error");
        log_error(GET_LOGGER("rest"), "Logical Drive " << uuid <<
                                                       " is used by more than one logical drives!");
        return;
    }

    try {
        // convert parent UUID into ID
        auto id = agent_framework::module::StorageComponents::get_instance()->
            get_logical_drive_manager().get_entry(parents.front()).get_id();

        // fill the link
        json::Value link;
        link[Common::ODATA_ID] =
            endpoint::PathBuilder(PathParam::BASE_URL)
                .append(Root::SERVICES)
                .append(request.params[PathParam::SERVICE_ID])
                .append(constants::StorageService::LOGICAL_DRIVES)
                .append(id).build();
        json[Common::LINKS][constants::Drive::USED_BY].push_back(
            std::move(link));
    }
    catch (const agent_framework::exceptions::InvalidUuid&) {
        log_error(GET_LOGGER("rest"), "Model LogicalDrive/UsedBy link error");
        log_error(GET_LOGGER("rest"), "LogicalDrive " << uuid << "is used by "
                                                      << " a LogicalDrive (" << parents.front()
                                                      << ") that does not exist");
    }
}

static const std::map<std::string, std::string> gami_to_rest_attributes = {
    {agent_framework::model::literals::LogicalDrive::BOOTABLE, constants::LogicalDrive::BOOTABLE}
};

}


endpoint::LogicalDrive::LogicalDrive(const std::string& path) : EndpointBase(path) {}


endpoint::LogicalDrive::~LogicalDrive() {}


void endpoint::LogicalDrive::get(const server::Request& req, server::Response& res) {
    auto r = ::make_prototype();

    r[constants::Common::ODATA_ID] = PathBuilder(req).build();
    r[constants::Common::ID] = req.params[PathParam::LOGICAL_DRIVE_ID];

    auto drive =
        psme::rest::model::Find<agent_framework::model::LogicalDrive>(req.params[PathParam::LOGICAL_DRIVE_ID])
            .via<agent_framework::model::StorageServices>(req.params[PathParam::SERVICE_ID]).get();

    endpoint::status_to_json(drive, r);
    r[Common::STATUS][Common::HEALTH_ROLLUP] = endpoint::HealthRollup<agent_framework::model::LogicalDrive>().get(
        drive.get_uuid());

    r[Drive::TYPE] = drive.get_type();
    r[constants::LogicalDrive::MODE] = drive.get_mode();
    r[constants::LogicalDrive::PROTECTED] = drive.get_protected();
    if (drive.get_capacity_gb().has_value()) {
        r[constants::LogicalDrive::CAPACITY] = endpoint::utils::gb_to_gib(drive.get_capacity_gb().value());
    }
    r[constants::LogicalDrive::IMAGE] = drive.get_image();
    r[constants::LogicalDrive::BOOTABLE] = drive.get_bootable();
    r[constants::LogicalDrive::SNAPSHOT] = drive.is_snapshot();

    // fill links
    add_master_drive_link(r, req, drive);
    add_targets_link(r, req, drive.get_parent_uuid(), drive.get_uuid());
    add_physical_drives_link(r, req, drive.get_uuid());
    add_logical_drives_link(r, req, drive.get_uuid());
    add_used_by_link(r, req, drive.get_uuid());

    set_response(res, r);
}


void endpoint::LogicalDrive::patch(const server::Request& request, server::Response& response) {
    auto drive = model::Find<agent_framework::model::LogicalDrive>(request.params[PathParam::LOGICAL_DRIVE_ID])
        .via<agent_framework::model::StorageServices>(request.params[PathParam::SERVICE_ID]).get();

    const auto json = JsonValidator::validate_request_body<schema::LogicalDrivePatchSchema>(request);

    agent_framework::model::attribute::Attributes attributes{};

    if (json.is_member(constants::LogicalDrive::BOOTABLE)) {
        const auto is_protected = json[constants::LogicalDrive::BOOTABLE].as_bool();
        attributes.set_value(agent_framework::model::literals::LogicalDrive::BOOTABLE, is_protected);
    }

    if (!attributes.empty()) {
        agent_framework::model::requests::SetComponentAttributes
            set_component_attributes_request{drive.get_uuid(), attributes};

        const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(drive.get_agent_id());

        auto set_drive_attributes = [&, gami_agent] {
                // Call set component attribute method
            const auto& set_component_attributes_response = gami_agent->
                execute<agent_framework::model::responses::SetComponentAttributes>(set_component_attributes_request);

            const auto& result_statuses = set_component_attributes_response.get_statuses();
            if (!result_statuses.empty()) {
                const auto& error = error::ErrorFactory::create_error_from_set_component_attributes_results(
                    result_statuses, gami_to_rest_attributes);
                throw error::ServerException(error);
            }

            psme::rest::model::handler::HandlerManager::get_instance()->get_handler(
                agent_framework::model::enums::Component::LogicalDrive)->
                load(gami_agent,
                     drive.get_parent_uuid(),
                     agent_framework::model::enums::Component::StorageServices,
                     drive.get_uuid(),
                     false);
        };

        gami_agent->execute_in_transaction(set_drive_attributes);
    }
    get(request, response);
}


void endpoint::LogicalDrive::del(const server::Request& req, server::Response& res) {
    using HandlerManager = psme::rest::model::handler::HandlerManager;

    auto drive =
        psme::rest::model::Find<agent_framework::model::LogicalDrive>(req.params[PathParam::LOGICAL_DRIVE_ID])
            .via<agent_framework::model::StorageServices>(req.params[PathParam::SERVICE_ID])
            .get();

    auto gami_req = agent_framework::model::requests::DeleteLogicalDrive(drive.get_uuid());

    const auto& gami_agent = psme::core::agent::AgentManager::get_instance()->get_agent(drive.get_agent_id());

    auto remove_drive = [&, gami_agent] {
        // try removing drive from agent's model
        gami_agent->execute<agent_framework::model::responses::DeleteLogicalDrive>(gami_req);

        // remove drive from REST model
        HandlerManager::get_instance()->
            get_handler(agent_framework::model::enums::Component::LogicalDrive)->
            remove(drive.get_uuid());

        res.set_status(server::status_2XX::NO_CONTENT);
    };

    gami_agent->execute_in_transaction(remove_drive);
}
