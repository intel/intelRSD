/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 *
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module/model/network_device.hpp"
#include "agent-framework/module/constants/compute.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component NetworkDevice::component =
    enums::Component::NetworkDevice;
const enums::CollectionName NetworkDevice::collection_name =
    enums::CollectionName::NetworkDevices;

NetworkDevice::NetworkDevice(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}

NetworkDevice::~NetworkDevice() {}

json::Json NetworkDevice::to_json() const {
    json::Json result = json::Json();
    result[literals::NetworkDevice::STATUS] = get_status().to_json();
    result[literals::NetworkDevice::COLLECTIONS] = get_collections().to_json();
    result[literals::NetworkDevice::OEM] = get_oem().to_json();
    return result;
}

NetworkDevice NetworkDevice::from_json(const json::Json& json) {
    NetworkDevice device{};

    device.set_status(attribute::Status::from_json(json[literals::NetworkDevice::STATUS]));
    device.set_collections(Collections::from_json(json[literals::NetworkDevice::COLLECTIONS]));
    device.set_oem(attribute::Oem::from_json(json[literals::NetworkDevice::OEM]));

    return device;
}
