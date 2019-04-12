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

#include "agent-framework/module/model/system.hpp"
#include "agent-framework/module/constants/compute.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::utils;

const enums::Component System::component = enums::Component::System;
const enums::CollectionName System::collection_name = enums::CollectionName::Systems;


System::System(const std::string& parent_uuid, enums::Component parent_type) :
    Resource{parent_uuid, parent_type} {}


System::~System() {}


json::Json System::to_json() const {
    json::Json result = json::Json();
    result[literals::System::STATUS] = get_status().to_json();
    result[literals::System::TYPE] = get_system_type();
    result[literals::System::BIOS_VERSION] = get_bios_version();
    result[literals::System::BOOT_OVERRIDE] = get_boot_override();
    result[literals::System::BOOT_OVERRIDE_MODE] = get_boot_override_mode();
    result[literals::System::BOOT_OVERRIDE_TARGET] = get_boot_override_target();
    result[literals::System::BOOT_OVERRIDE_SUPPORTED] = get_boot_override_supported().to_json();
    result[literals::System::UEFI_TARGET] = get_uefi_target();
    result[literals::System::POWER_STATE] = get_power_state();
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
    result[literals::System::CABLE_IDS] = get_cable_ids().to_json();
    result[literals::System::TXT_ENABLED] = is_txt_enabled();
    result[literals::System::USER_MODE_ENABLED] = is_user_mode_enabled();
    result[literals::System::CURRENT_PERFORMANCE_CONFIGURATION] = get_current_performance_configuration();
    result[literals::System::PERFORMANCE_CONFIGURATIONS] = get_performance_configurations().to_json();
    return result;
}


System System::from_json(const json::Json& json) {
    System sys{};

    sys.set_status(attribute::Status::from_json(json[literals::System::STATUS]));
    sys.set_system_type(json[literals::System::TYPE]);
    sys.set_bios_version(json[literals::System::BIOS_VERSION]);
    sys.set_boot_override(json[literals::System::BOOT_OVERRIDE]);
    sys.set_boot_override_mode(json[literals::System::BOOT_OVERRIDE_MODE]);
    sys.set_boot_override_target(json[literals::System::BOOT_OVERRIDE_TARGET]);
    sys.set_boot_override_supported(BootOverrideSupported::from_json(json[literals::System::BOOT_OVERRIDE_SUPPORTED]));
    sys.set_uefi_target(json[literals::System::UEFI_TARGET]);
    sys.set_power_state(json[literals::System::POWER_STATE]);
    sys.set_pci_devices(PciDevices::from_json(json[literals::System::PCI_DEVICES]));
    sys.set_usb_devices(UsbDevices::from_json(json[literals::System::USB_DEVICES]));
    sys.set_fru_info(attribute::FruInfo::from_json(json[literals::System::FRU_INFO]));
    sys.set_sku(json[literals::System::SKU]);
    sys.set_asset_tag(json[literals::System::ASSET_TAG]);
    sys.set_indicator_led(json[literals::System::INDICATOR_LED]);
    sys.set_collections(Collections::from_json(json[literals::System::COLLECTIONS]));
    sys.set_chassis(json[literals::System::CHASSIS]);
    sys.set_oem(attribute::Oem::from_json(json[literals::System::OEM]));
    sys.set_guid(json[literals::System::GUID]);
    sys.set_cable_ids(CableIds::from_json(json[literals::System::CABLE_IDS]));
    sys.set_txt_enabled(json[literals::System::TXT_ENABLED]);
    sys.set_user_mode_enabled(json[literals::System::USER_MODE_ENABLED]);
    sys.set_current_performance_configuration(json[literals::System::CURRENT_PERFORMANCE_CONFIGURATION]);
    sys.set_performance_configurations(PerformanceConfigurations::from_json(json[literals::System::PERFORMANCE_CONFIGURATIONS]));
    return sys;
}
