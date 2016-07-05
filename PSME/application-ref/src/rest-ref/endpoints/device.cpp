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

#include "psme/rest-ref/endpoints/device.hpp"
#include "psme/rest-ref/constants/constants.hpp"
#include "agent-framework/module-ref/compute_manager.hpp"
#include <regex>

using namespace psme::rest;
using namespace app::rest::constants;
using namespace app::rest;



namespace {
json::Value make_prototype() {
    json::Value r(json::Value::Type::OBJECT);
    r[Common::ODATA_CONTEXT] = "/redfish/v1/$metadata#Systems/Members/__SYSTEM_ID__/Adapters/Members/__ADAPTER_ID__/Devices/Members/$entity";
    r[Common::ODATA_ID] = json::Value::Type::NIL;
    r[Common::ODATA_TYPE] = "#Device.1.0.0.Device";
    r[Common::ID] = json::Value::Type::NIL;
    r[Common::NAME] = "Device";
    r[Common::DESCRIPTION] = "Device description";
    r[Drive::INTERFACE] = json::Value::Type::NIL;
    r[Drive::CAPACITY] = json::Value::Type::NIL;
    r[Drive::TYPE] = json::Value::Type::NIL;
    r[Drive::BUS_INFO] = json::Value::Type::NIL;
    r[Common::FIRMWARE_VERSION] = json::Value::Type::NIL;
    r[Drive::RPM] = json::Value::Type::NIL;
    r[Common::MANUFACTURER] = json::Value::Type::NIL;
    r[Common::MODEL] = json::Value::Type::NIL;
    r[Common::SERIAL] = json::Value::Type::NIL;
    r[Common::STATUS][Common::STATE] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH] = json::Value::Type::NIL;
    r[Common::STATUS][Common::HEALTH_ROLLUP] = json::Value::Type::NIL;
    r[Common::OEM] = json::Value::Type::OBJECT;
    r[Common::LINKS][Common::CONTAINED_BY] = json::Value::Type::NIL;
    r[Common::LINKS][Common::OEM] = json::Value::Type::OBJECT;

    return r;
}

}

endpoint::Device::Device(const std::string& path) : Endpoint(path) {}
endpoint::Device::~Device() {}

void endpoint::Device::get(const server::Request& req, server::Response& res) {
    auto r = make_prototype();

    auto system_id = psme::rest::model::Find<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID])
        .get_one()->get_id();
    auto adapter_id = psme::rest::model::Find<agent_framework::model::StorageController>(req.params[PathParam::ADAPTER_ID])
        .via<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID]).get_one()->get_id();

    auto d =  psme::rest::model::Find<agent_framework::model::Drive>(req.params[PathParam::DEVICE_ID])
        .via<agent_framework::model::System>(req.params[PathParam::SYSTEM_ID])
        .via<agent_framework::model::StorageController>(req.params[PathParam::ADAPTER_ID])
        .get();

    r[Common::ODATA_ID] = PathBuilder(req).build();
    r[Common::ODATA_CONTEXT] = std::regex_replace(r[Common::ODATA_CONTEXT].as_string(),
                                                  std::regex("__SYSTEM_ID__"), std::to_string(system_id));
    r[Common::ODATA_CONTEXT] = std::regex_replace(r[Common::ODATA_CONTEXT].as_string(),
                                                  std::regex("__ADAPTER_ID__"), std::to_string(adapter_id));
    r[Common::ID] = req.params[PathParam::DEVICE_ID];

    r[Drive::INTERFACE] = d.get_interface();
    if (d.get_capacity_gb().has_value()) {
        r[Drive::CAPACITY] = endpoint::utils::gb_to_gib(d.get_capacity_gb().value());
    }
    r[Drive::TYPE] = d.get_type();
    r[Drive::BUS_INFO] = d.get_physical_id();
    r[Common::FIRMWARE_VERSION] = d.get_firmware_version();
    r[Drive::RPM] = d.get_rpm();
    r[Common::MANUFACTURER] = d.get_fru_info().get_manufacturer();
    r[Common::MODEL] = d.get_fru_info().get_model_number();
    r[Common::SERIAL] = d.get_fru_info().get_serial_number();
    endpoint::utils::status_to_json(d,r);

    r[Common::LINKS]
     [Common::CONTAINED_BY]
     [Common::ODATA_ID] = PathBuilder(PathParam::BASE_URL)
                            .append(constants::Root::SYSTEMS)
                            .append(req.params[PathParam::SYSTEM_ID])
                            .append(constants::System::STORAGE_ADAPTERS)
                            .append(req.params[PathParam::ADAPTER_ID]).build();

    set_response(res, r);
}
