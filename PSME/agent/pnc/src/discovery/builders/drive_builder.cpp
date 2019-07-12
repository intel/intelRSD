/*!
 * @section LICENSE
 *
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
 * @file drive_builder.cpp
 *
 * @brief DriveBuilder implementation
 * */

#include "discovery/builders/drive_builder.hpp"
#include "gas/csr/configuration_space_register.hpp"
#include "discovery/discovery_utils.hpp"
#include "agent-framework/module/utils/to_hex_string.hpp"

using namespace agent::pnc::discovery::builders;
using namespace agent_framework::model;
using namespace agent_framework::model::utils;
using namespace agent::pnc::sysfs;
using namespace agent::pnc::nvme;
using namespace agent::pnc::gas::csr;
using namespace agent::pnc::discovery::utils;
using namespace agent::pnc::tools;
using namespace fru_eeprom::parser;

void DriveBuilder::build_default() {
    attribute::Status status;
    status.set_health(enums::Health::Critical);
    status.set_state(enums::State::Absent);
    m_obj.set_status(std::move(status));

    m_obj.set_type(enums::DriveType::SSD);
    m_obj.set_interface(enums::TransportProtocol::NVMe);

    m_obj.add_collection(attribute::Collection(
        enums::CollectionName::PcieFunctions,
        enums::CollectionType::PCIeFunctions
    ));
}

DriveBuilder::ReturnType DriveBuilder::update_sysfs_drive_data(const SysfsDrive& sysfs_drive) {
    m_obj.set_capacity_gb(static_cast<double>(sysfs_drive.size_bytes) / 1000.0 / 1000.0 / 1000.0);
    return *this;
}

DriveBuilder::ReturnType DriveBuilder::update_sysfs_device_data(const SysfsDevice& sysfs_device) {
    // normally both pcie registers would be decoded in the sysfs_decoder, however the same
    // decoding code is used in the ConfigurationSpaceRegister. Therefore in the sysfs discovery
    // we just store registers and here we create artificial CSR object to decode registers' values.
    if (sysfs_device.has_pcie_capabilities) {
        ConfigurationSpaceRegister csr{};
        csr.output.pci_caps.fields.link_capability = sysfs_device.pcie_link_capability_register;
        csr.output.pci_caps.fields.link_status = sysfs_device.pcie_link_status_register;
        try {
            m_obj.set_negotiated_speed_gbs(csr.get_speed());
            m_obj.set_capable_speed_gbs(csr.get_max_speed());
        }
        catch (const std::exception& e) {
            log_error("pnc-discovery", "Invalid speed/max speed of the drive: " << e.what());
            throw std::runtime_error("Invalid CSR data format");
        }
    }
    return *this;
}

DriveBuilder::ReturnType DriveBuilder::update_location(const Port& port) {
    attribute::Location location{};
    location.set_info_format("Port:number");
    location.set_info(std::string{"Port:"} + port.get_port_id());
    m_obj.add_location(std::move(location));
    return *this;
}

DriveBuilder::ReturnType DriveBuilder::update_vpd(const VitalProductData& vpd) {
    attribute::FruInfo fru_info{};
    fru_info.set_manufacturer(to_hex_string<2>(vpd.fields.vendor_id));
    fru_info.set_serial_number(get_null_terminated_string(
        vpd.fields.serial_number, vpd.NVME_SERIAL_NUMBER_SIZE_BYTES));
    fru_info.set_model_number(get_null_terminated_string(
        vpd.fields.model_number, vpd.NVME_MODEL_NUMBER_SIZE_BYTES));
    m_obj.set_fru_info(std::move(fru_info));
    return *this;
}

DriveBuilder::ReturnType DriveBuilder::update_vpd(const FruEepromParser& parser) {
    attribute::FruInfo fru_info{};
    auto product_info_area = parser.get_product_info();
    fru_info.set_manufacturer(get_null_terminated_optional_string(
            reinterpret_cast<uint8_t*>(product_info_area.manufacturer.data), product_info_area.manufacturer.data_len));
    fru_info.set_serial_number(get_null_terminated_optional_string(
            reinterpret_cast<uint8_t*>(product_info_area.serial_number.data), product_info_area.serial_number.data_len));
    fru_info.set_part_number(get_null_terminated_optional_string(
            reinterpret_cast<uint8_t*>(product_info_area.product_name.data), product_info_area.product_name.data_len));
    fru_info.set_model_number(get_null_terminated_optional_string(
            reinterpret_cast<uint8_t*>(product_info_area.model_number.data), product_info_area.model_number.data_len));
    m_obj.set_fru_info(fru_info);
    m_obj.set_revision(get_null_terminated_optional_string(
            reinterpret_cast<uint8_t*>(product_info_area.product_version.data), product_info_area.product_version.data_len));
    m_obj.set_asset_tag(get_null_terminated_optional_string(
            reinterpret_cast<uint8_t*>(product_info_area.asset_tag.data), product_info_area.asset_tag.data_len));
    return *this;
}

DriveBuilder::ReturnType DriveBuilder::update_smart(const Toolset& t, const Smart& smart) {
    m_obj.set_status(t.map_tool->get_status_from_smart(smart));
    int32_t percentage_left = 100 - smart.fields.percentage_drive_life_used;
    m_obj.set_predicted_media_life_left(percentage_left);
    m_obj.set_failure_predicted(percentage_left > 0 ? false : true);
    return *this;
}

DriveBuilder::ReturnType DriveBuilder::update_firmware_version(const FirmwareVersion& fw_ver) {
    m_obj.set_firmware_version(get_null_terminated_string(fw_ver.fields.firmware_version,
        fw_ver.FW_VERSION_SIZE_BYTES));
    return *this;
}

DriveBuilder::ReturnType DriveBuilder::update_critical_state() {
    attribute::Status status{};
    status.set_health(enums::Health::Critical);
    status.set_state(enums::State::Disabled);
    m_obj.set_status(std::move(status));
    m_obj.set_is_in_critical_discovery_state(true);
    return *this;
}

DriveBuilder::ReturnType DriveBuilder::add_dsp_port_uuid(const std::string& dsp_port_uuid) {
    m_obj.add_dsp_port_uuid(dsp_port_uuid);
    return *this;
}

DriveBuilder::ReturnType DriveBuilder::update_fru_info(const attribute::FruInfo& fru_info) {
    m_obj.set_fru_info(fru_info);
    return *this;
}