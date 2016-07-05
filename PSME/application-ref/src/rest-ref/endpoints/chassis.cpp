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

#include "psme/rest-ref/endpoints/chassis.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "agent-framework/module-ref/model/chassis.hpp"

using namespace psme::rest;
using namespace app::rest;
using namespace app::rest::constants;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);

    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Chassis/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Chassis.1.0.0.Chassis";
    r[Common::ID] = json::Value::Type::NIL;
    r[Chassis::CHASSIS_TYPE] = json::Value::Type::NIL;
    r[Common::NAME] = "Chassis";
    r[Common::DESCRIPTION] = "Chassis description";

    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[Common::MODEL] = json::Value::Type::NIL;
    r[Chassis::SKU] = json::Value::Type::NIL;
    r[Common::SERIAL] = json::Value::Type::NIL;
    r[Common::PART_NUMBER] = json::Value::Type::NIL;
    r[Common::ASSET_TAG] = json::Value::Type::NIL;
    r[Chassis::INDICATOR_LED] = json::Value::Type::NIL;
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;

    json::Value rs;
    rs[Common::ODATA_TYPE] = "#Intel.Oem.Chassis";
    rs[Common::LOCATION][Common::ID] = json::Value::Type::NIL;
    rs[Common::LOCATION][Chassis::PARENT_ID] = json::Value::Type::NIL;
    r[Common::OEM][Common::RACKSCALE] = std::move(rs);

    r[Common::LINKS][Chassis::CONTAINS] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::CONTAINED_BY] = json::Value::Type::NIL;
    r[Common::LINKS][Chassis::COMPUTER_SYSTEMS] = json::Value::Type::ARRAY;
    r[Common::LINKS][Chassis::SWITCHES] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::MANAGED_BY] = json::Value::Type::ARRAY;
    r[Common::LINKS][Chassis::MANAGERS_IN] = json::Value::Type::ARRAY;
    r[Common::LINKS][Common::OEM] = json::Value::Type::OBJECT;

    return r;
}

void fill_containing_links(const agent_framework::model::Chassis& chassis, json::Value& r) {
    auto is_drawer = [](const agent_framework::model::Chassis& ch)
        -> bool{return ch.get_type() == agent_framework::model::enums::ChassisType::Drawer;};

    // find manager of the chassis
    r[Common::OEM][Common::RACKSCALE][Common::LOCATION][Common::ID] =
        std::to_string(chassis.get_location_offset());
    if(is_drawer(chassis))
    {
        r[Common::OEM][Common::RACKSCALE][Common::LOCATION][constants::Chassis::PARENT_ID] = chassis.get_parent_id();
        for(const auto& chassis_id : agent_framework::module::ComputeManager::get_instance()
            ->get_chassis_manager().get_ids() )
            {
                if(chassis_id != chassis.get_id()) {
                    json::Value link;
                    link[Common::ODATA_ID] = "/redfish/v1/Chassis/" + std::to_string(chassis_id);
                    r[Common::LINKS][Chassis::CONTAINS].push_back(std::move(link));
                }
            }
        for(const auto& chassis_id : agent_framework::module::NetworkManager::get_instance()
            ->get_chassis_manager().get_ids() )
            {
                if(chassis_id != chassis.get_id()) {
                    json::Value link;
                    link[Common::ODATA_ID] = "/redfish/v1/Chassis/" + std::to_string(chassis_id);
                    r[Common::LINKS][Chassis::CONTAINS].push_back(std::move(link));
                }
            }
    }
    else {
        try {
            bool found = false;
            for(const auto& chass : agent_framework::module::ChassisManager::get_instance()->
                get_chassis_manager().get_keys(is_drawer) ) {
                auto ch = agent_framework::module::ChassisManager::get_instance()->
                    get_chassis_manager().get_entry_reference(chass);
                json::Value link;
                link[Common::ODATA_ID] = "/redfish/v1/Chassis/" + std::to_string(ch->get_id());
                r[Common::LINKS][Common::CONTAINED_BY] = std::move(link);
                r[Common::OEM][Common::RACKSCALE][Common::LOCATION][constants::Chassis::PARENT_ID] =
                    std::to_string(ch->get_location_offset());
                found = true;
            }
            if (!found) {
                for(const auto& chass : agent_framework::module::ComputeManager::get_instance()->
                        get_chassis_manager().get_keys(is_drawer)) {
                    auto ch = agent_framework::module::ComputeManager::get_instance()->
                        get_chassis_manager().get_entry_reference(chass);
                    json::Value link;
                    link[Common::ODATA_ID] = "/redfish/v1/Chassis/" + std::to_string(ch->get_id());
                    r[Common::LINKS][Common::CONTAINED_BY] = std::move(link);
                    r[Common::OEM][Common::RACKSCALE][Common::LOCATION][constants::Chassis::PARENT_ID] =
                        std::to_string(ch->get_location_offset());
                }
            }

        } catch (const std::exception& e) {
            log_error(GET_LOGGER("app"), "Exception caught during filling Chassis"
                    "links Contains and ContainedBy: " << e.what());
        }
    }

}

void fill_links(const agent_framework::model::Chassis& chassis, json::Value& r) {
    // find manager of this chassis
    try {
        json::Value managed_by;
        managed_by[Common::ODATA_ID] = "/redfish/v1/Managers/" +
            std::to_string(psme::rest::endpoint::utils::get_manager_by_uuid(
                    chassis.get_parent_uuid()).get_id());
        r[Common::LINKS][Common::MANAGED_BY].push_back(managed_by);
    } catch (agent_framework::exceptions::InvalidParameters&) {
    }

    //systems in chassis
    auto& system_manager = agent_framework::module::ComputeManager::
        get_instance()->get_system_manager();
    auto system_uuids = system_manager.get_keys();
    for(auto system_uuid : system_uuids){
        auto system = system_manager.get_entry_reference(system_uuid);
        if (system->get_chassis() == chassis.get_uuid()){
            json::Value link;
            link[Common::ODATA_ID] = "/redfish/v1/Systems/" + std::to_string(system->get_id());
            r[Common::LINKS][constants::Chassis::COMPUTER_SYSTEMS].push_back(std::move(link));
        }
    }


    //switches in chassis
    auto& switch_manager = agent_framework::module::NetworkManager::
        get_instance()->get_switch_manager();
    auto switch_uuids = switch_manager.get_keys();
    for(auto switch_uuid : switch_uuids){
        auto switch_ = switch_manager.get_entry_reference(switch_uuid);
        if (switch_->get_chassis() == chassis.get_uuid()){
            json::Value link;
            link[Common::ODATA_ID] = "/redfish/v1/EthernetSwitches/" + std::to_string(switch_->get_id());
            r[Common::LINKS][constants::Chassis::SWITCHES].push_back(std::move(link));
        }
    }

}

}

endpoint::Chassis::Chassis(const std::string& path) : Endpoint(path) {}
endpoint::Chassis::~Chassis() {}

void endpoint::Chassis::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();
    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ID] = req.params[PathParam::CHASSIS_ID];

    auto chassis = psme::rest::endpoint::utils::get_chassis(req.params[PathParam::CHASSIS_ID]);

    psme::rest::endpoint::utils::status_to_json(chassis, r);
    // for now, we assume that a chassis has no children
    r[Common::STATUS][Common::HEALTH_ROLLUP] = chassis.get_status().get_health().to_string();
    fill_links(chassis, r);
    fill_containing_links(chassis, r);

    r[constants::Chassis::CHASSIS_TYPE] = chassis.get_type().to_string();
    r[Common::MANUFACTURER] = chassis.get_fru_info().get_manufacturer();
    r[Common::MODEL] = chassis.get_fru_info().get_model_number();
    r[Common::SERIAL] = chassis.get_fru_info().get_serial_number();
    r[Common::PART_NUMBER] = chassis.get_fru_info().get_part_number();
    r[Common::ASSET_TAG] = chassis.get_asset_tag();
    r[constants::Chassis::SKU] = chassis.get_sku();
    r[constants::Chassis::INDICATOR_LED] = chassis.get_indicator_led();

    set_response(res, r);
}
