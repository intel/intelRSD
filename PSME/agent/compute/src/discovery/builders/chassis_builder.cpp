/*!
 * @brief Chassis builder class implementation.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file chassis_builder.cpp
 */

#include "discovery/builders/chassis_builder.hpp"



using namespace agent::compute::discovery;
using namespace agent_framework::model;


Chassis ChassisBuilder::build_default(const std::string& parent_uuid) {
    Chassis chassis{parent_uuid};

    chassis.add_collection({enums::CollectionName::Drives,
                            enums::CollectionType::Drives
    });

    chassis.add_collection({enums::CollectionName::ThermalZones,
                            enums::CollectionType::ThermalZones
    });

    chassis.add_collection({enums::CollectionName::PowerZones,
                            enums::CollectionType::PowerZones
    });
    chassis.set_status({enums::State::Enabled, {}});
    return chassis;
}


void ChassisBuilder::update_smbios_module_info(agent_framework::model::Chassis& chassis,
                                               const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_MODULE_INFO_DATA>& smbios_data) {
    auto fru_info = chassis.get_fru_info();

    fru_info.set_model_number(smbios_data.get_string(smbios_data.data.product));

    chassis.set_fru_info(fru_info);
}


void ChassisBuilder::update_smbios_system_enclosure_or_chassis_info(agent_framework::model::Chassis& chassis,
                                                                    const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_SYSTEM_ENCLOSURE_OR_CHASSIS>& smbios_data) {
    const auto& manufacturer = smbios_data.get_string(smbios_data.data.manufacturer);
    const auto& sku = smbios_data.get_string(smbios_data.data.get_sku());
    const auto& serial_number = smbios_data.get_string(smbios_data.data.serial_number);
    const auto& part_number = smbios_data.get_string(smbios_data.data.version);
    const auto& asset_tag = smbios_data.get_string(smbios_data.data.asset_tag_number);

    auto fru_info = chassis.get_fru_info();
    fru_info.set_manufacturer(manufacturer);
    fru_info.set_serial_number(serial_number);
    fru_info.set_part_number(part_number);

    chassis.set_fru_info(fru_info);
    chassis.set_sku(sku);
    chassis.set_asset_tag(asset_tag);
}


void ChassisBuilder::update_device_id(agent_framework::model::Chassis& chassis,
                                      const ipmi::command::generic::response::GetDeviceId& get_device_id_response) {
    auto fru_info = chassis.get_fru_info();

    fru_info.set_manufacturer(get_device_id_response.get_manufacturer_name());
    fru_info.set_model_number(get_device_id_response.get_product_name());

    chassis.set_fru_info(fru_info);
}
