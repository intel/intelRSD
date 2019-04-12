/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * */

#include "agent-framework/module/model/pcie_device.hpp"
#include "agent-framework/module/constants/pnc.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

const enums::Component PcieDevice::component = enums::Component::PcieDevice;
const enums::CollectionName PcieDevice::collection_name =
    enums::CollectionName::PcieDevices;


PcieDevice::PcieDevice(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{ parent_uuid, parent_type } { }

PcieDevice::~PcieDevice() { }

json::Json PcieDevice::to_json() const {
    json::Json result = json::Json();
    result[literals::PcieDevice::DEVICE_ID] = get_device_id();
    result[literals::PcieDevice::DEVICE_CLASS] = get_device_class();
    result[literals::PcieDevice::SPEED_GBPS] = get_speed_gbps();
    result[literals::PcieDevice::WIDTH] = get_width();
    result[literals::PcieDevice::MAX_SPEED_GBPS] = get_max_speed_gbps();
    result[literals::PcieDevice::MAX_WIDTH] = get_max_width();
    result[literals::PcieDevice::FIRMWARE_VERSION] = get_firmware_version();
    result[literals::PcieDevice::ASSET_TAG] = get_asset_tag();
    result[literals::PcieDevice::SKU] = get_sku();
    result[literals::PcieDevice::CHASSIS] = get_chassis();
    result[literals::PcieDevice::FRU_INFO] = get_fru_info().to_json();
    result[literals::PcieDevice::COLLECTIONS] = get_collections().to_json();
    result[literals::PcieDevice::STATUS] = get_status().to_json();
    result[literals::PcieDevice::OEM] = get_oem().to_json();
    return result;
}

PcieDevice PcieDevice::from_json(const json::Json& json) {
    PcieDevice pcie_device{};
    pcie_device.set_device_id(json[literals::PcieDevice::DEVICE_ID]);
    pcie_device.set_device_class(json[literals::PcieDevice::DEVICE_CLASS]);
    pcie_device.set_speed_gbps(json[literals::PcieDevice::SPEED_GBPS]);
    pcie_device.set_width(json[literals::PcieDevice::WIDTH]);
    pcie_device.set_max_speed_gbps(json[literals::PcieDevice::MAX_SPEED_GBPS]);
    pcie_device.set_max_width(json[literals::PcieDevice::MAX_WIDTH]);
    pcie_device.set_firmware_version(json[literals::PcieDevice::FIRMWARE_VERSION]);
    pcie_device.set_asset_tag(json[literals::PcieDevice::ASSET_TAG]);
    pcie_device.set_sku(json[literals::PcieDevice::SKU]);
    pcie_device.set_chassis(json[literals::PcieDevice::CHASSIS]);

    pcie_device.set_fru_info(FruInfo::from_json(json[literals::PcieDevice::FRU_INFO]));
    pcie_device.set_collections(Collections::from_json(json[literals::PcieDevice::COLLECTIONS]));
    pcie_device.set_status(Status::from_json(json[literals::PcieDevice::STATUS]));
    pcie_device.set_oem(Oem::from_json(json[literals::PcieDevice::OEM]));
    return pcie_device;
}
