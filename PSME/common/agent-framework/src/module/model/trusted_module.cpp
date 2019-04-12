/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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

#include <agent-framework/module/constants/compute.hpp>
#include "agent-framework/module/model/trusted_module.hpp"


using namespace agent_framework::model;


const enums::Component TrustedModule::component = enums::Component::TrustedModule;
const enums::CollectionName TrustedModule::collection_name = enums::CollectionName::TrustedModules;

TrustedModule::TrustedModule(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}


TrustedModule::~TrustedModule() {}


json::Json TrustedModule::to_json() const {
    json::Json json = json::Json();
    json[literals::Status::STATUS] = get_status().to_json();
    json[literals::Oem::OEM] = get_oem().to_json();
    json[literals::TrustedModule::INTERFACE_TYPE] = get_interface_type();
    json[literals::TrustedModule::FIRMWARE_VERSION] = get_firmware_version();
    return json;
}


TrustedModule TrustedModule::from_json(const json::Json& json) {
    TrustedModule trusted_module{};
    trusted_module.set_status(attribute::Status::from_json(json[literals::Status::STATUS]));
    trusted_module.set_oem(attribute::Oem::from_json(json[literals::Oem::OEM]));
    trusted_module.set_interface_type(json[literals::TrustedModule::INTERFACE_TYPE]);
    trusted_module.set_firmware_version(json[literals::TrustedModule::FIRMWARE_VERSION]);

    return trusted_module;
}
