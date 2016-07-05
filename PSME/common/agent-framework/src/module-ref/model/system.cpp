/*!
 * @section LICENSE
 *
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
 *
 * @section DESCRIPTION
 *
 * */

#include "agent-framework/module-ref/model/system.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"

using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component System::component = enums::Component::System;
const enums::CollectionName System::collection_name = enums::CollectionName::Systems;

System::System(const std::string& parent_uuid) : Resource{parent_uuid} {}

System::~System() {}

Json::Value System::to_json() const {
    Json::Value result;
    result[literals::System::STATUS] = get_status().to_json();
    result[literals::System::BIOS_VERSION] = get_bios_version();
    result[literals::System::BOOT_OVERRIDE] = get_boot_override().to_string();
    result[literals::System::BOOT_OVERRIDE_TARGET] =
        get_boot_override_target().to_string();
    result[literals::System::BOOT_OVERRIDE_SUPPORTED] =
        get_boot_override_supported().to_json();
    result[literals::System::UEFI_TARGET] = get_uefi_target();
    result[literals::System::POWER_STATE] = get_power_state().to_string();
    result[literals::System::PCI_DEVICES] = get_pci_devices().to_json();
    result[literals::System::USB_DEVICES] = get_usb_devices().to_json();
    result[literals::System::FRU_INFO] = get_fru_info().to_json();
    result[literals::System::SKU] = get_sku();
    result[literals::System::ASSET_TAG] = get_asset_tag();
    result[literals::System::INDICATOR_LED] = get_indicator_led();
    result[literals::System::COLLECTIONS] = get_collections().to_json();
    result[literals::System::CHASSIS] = get_chassis();
    result[literals::System::OEM] = get_oem().to_json();
    result[literals::System::GUID] = get_guid();
    return result;
}

System System::from_json(const Json::Value& json) {
    System sys;

    sys.set_status(attribute::Status::from_json(json[literals::System::STATUS]));
    sys.set_bios_version(json[literals::System::BIOS_VERSION]);
    sys.set_boot_override(enums::BootOverride::from_string(
        json[literals::System::BOOT_OVERRIDE].asString()));
    sys.set_boot_override_target(enums::BootOverrideTarget::from_string(
        json[literals::System::BOOT_OVERRIDE_TARGET].asString()));
    sys.set_boot_override_supported(BootOverrideSupported::from_json(
        json[literals::System::BOOT_OVERRIDE_SUPPORTED]));
    sys.set_uefi_target(json[literals::System::UEFI_TARGET]);
    sys.set_power_state(enums::PowerState::from_string(
        json[literals::System::POWER_STATE].asString()));
    sys.set_pci_devices(PciDevices::from_json(
        json[literals::System::PCI_DEVICES]));
    sys.set_usb_devices(UsbDevices::from_json(
        json[literals::System::USB_DEVICES]));
    sys.set_fru_info(attribute::FruInfo::from_json(
        json[literals::System::FRU_INFO]));
    sys.set_sku(json[literals::System::SKU]);
    sys.set_asset_tag(json[literals::System::ASSET_TAG]);
    sys.set_indicator_led(json[literals::System::INDICATOR_LED]);
    sys.set_collections(Collections::from_json(
        json[literals::System::COLLECTIONS]));
    sys.set_chassis(json[literals::System::CHASSIS]);
    sys.set_oem(attribute::Oem::from_json(json[literals::System::OEM]));
    sys.set_resource_hash(compute_hash(json));
    sys.set_guid(json[literals::System::GUID].asString());

    return sys;
}
