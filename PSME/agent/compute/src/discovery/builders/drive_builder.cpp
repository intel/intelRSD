/*!
 * @brief Drive builder class implementation.
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
 * @file drive_builder.cpp
 */

#include "discovery/builders/drive_builder.hpp"

#include <bitset>
#include <algorithm>



using namespace agent::compute::discovery;
using namespace smbios::parser;
using namespace agent_framework::model;
using namespace agent_framework::model::enums;

namespace {

std::string& rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
            s.end());
    return s;
}


OptionalField<enums::TransportProtocol>
get_protocol(const SmbiosParser::StructEnhanced<SMBIOS_STORAGE_DEVICE_INFO_DATA>& storage) {
    switch (storage.data.connector_type) {
        case ConnectorType::SATA:
            return enums::TransportProtocol::SATA;
        case ConnectorType::SAS:
            return enums::TransportProtocol::SAS;
        case ConnectorType::PCIE:
            return enums::TransportProtocol::PCIe;
        case ConnectorType::USB:
            return enums::TransportProtocol::USB;
        case ConnectorType::M_2:
        case ConnectorType::U_2:
        case ConnectorType::UNKNOWN:
        default:
            return OptionalField<enums::TransportProtocol>{};
    }
}


OptionalField<enums::DriveType> get_type(const SmbiosParser::StructEnhanced<SMBIOS_STORAGE_DEVICE_INFO_DATA>& storage) {
    switch (storage.data.device_type) {
        case StorageDeviceType::HDD:
            return enums::DriveType::HDD;
        case StorageDeviceType::SSD:
            return enums::DriveType::SSD;
        case StorageDeviceType::DVD:
        case StorageDeviceType::BLUE_RAY:
        case StorageDeviceType::USB:
        default:
            return OptionalField<enums::DriveType>{};
    }
}

}


agent_framework::model::Drive DriveBuilder::build_default(const std::string& parent_uuid) {
    return Drive{parent_uuid};
}


void DriveBuilder::update_smbios_data(agent_framework::model::Drive& drive,
                                      const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_STORAGE_INFO_DATA>& smbios_data) {
    drive.set_status({enums::State::Enabled, {}});

    drive.set_capacity_gb(smbios_data.data.RsaHddInfoData.HDDSizeInGB);

    std::string string_name(reinterpret_cast<const char*>(smbios_data.data.RsaHddInfoData.HDDName),
                            HDD_INFO_DATA::HDD_NAME_STRING_SIZE);

    attribute::FruInfo fru;
    fru.set_model_number(rtrim(string_name));
    drive.set_fru_info(std::move(fru));

    // In the Drive model, these fields have default values.
    // We reset them to null optionals.
    drive.set_interface(OptionalField<enums::TransportProtocol>{});
    drive.set_type(OptionalField<enums::DriveType>{});

    log_debug("smbios-discovery", "Drive was discovered.");
}


void DriveBuilder::update_smbios_data(agent_framework::model::Drive& drive,
                                      const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_STORAGE_INFO_DATA_V2>& smbios_data) {
    drive.set_status({enums::State::Enabled, {}});

    drive.set_capacity_gb(smbios_data.data.RsaHddInfoData.HDDSizeInGB);

    std::string string_name(reinterpret_cast<const char*>(smbios_data.data.RsaHddInfoData.HDDName),
                            HDD_INFO_DATA_V2::HDD_NAME_STRING_SIZE);
    std::string serial_number(reinterpret_cast<const char*>(smbios_data.data.RsaHddInfoData.HDDSerialNumber),
                              HDD_INFO_DATA_V2::HDD_SERIAL_NUMBER_SIZE);

    attribute::FruInfo fru;
    fru.set_model_number(rtrim(string_name));
    fru.set_serial_number(rtrim(serial_number));
    drive.set_fru_info(std::move(fru));

    // In the Drive model, these fields have default values.
    // We reset them to null optionals.
    drive.set_interface(OptionalField<enums::TransportProtocol>{});
    drive.set_type(OptionalField<enums::DriveType>{});

    log_debug("smbios-discovery", "Drive V2 was discovered.");
}


void DriveBuilder::update_smbios_data(agent_framework::model::Drive& drive,
                                      const smbios::parser::SmbiosParser::StructEnhanced<smbios::parser::SMBIOS_STORAGE_DEVICE_INFO_DATA>& smbios_data) {
    drive.set_status({enums::State::Enabled, {}});

    drive.set_capacity_gb(smbios_data.data.device_capacity);
    attribute::FruInfo fru;
    fru.set_model_number(smbios_data.get_string(smbios_data.data.device_model));
    fru.set_serial_number(smbios_data.get_string(smbios_data.data.device_serial));

    std::stringstream stream;
    stream << "0x" << std::hex << std::setfill('0') << std::setw(4) << smbios_data.data.vendor_id;
    fru.set_manufacturer(stream.str());
    drive.set_fru_info(std::move(fru));

    drive.set_interface(get_protocol(smbios_data));
    drive.set_type(get_type(smbios_data));
    drive.set_firmware_version(smbios_data.get_string(smbios_data.data.firmware_version));
    drive.set_rpm(smbios_data.data.device_rpm);


    log_debug("smbios-discovery", "Drive of StorageDevice type was discovered.");
}
