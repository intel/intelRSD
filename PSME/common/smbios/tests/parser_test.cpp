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
 * */

#include <cstdio>
#include <fstream>
#include <sstream>

#include "test_fixture.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"



using ::testing::Return;
using ::testing::Throw;
using namespace smbios::parser;

class SmbiosParserTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        m_entry_point = fixture::entry_point.entry_point;
    }


    virtual void TearDown() {}


    virtual ~SmbiosParserTest();


    union {
        Smbios21EntryPoint::EntryPointStructure m_entry_point;
        uint8_t m_blob[1024];
    };


    uint8_t* get_blob() {
        return reinterpret_cast<uint8_t*>(&m_blob);
    }
};


SmbiosParserTest::~SmbiosParserTest() {}


TEST_F(SmbiosParserTest, ExceptionWhenBlobIsNull) {
    ASSERT_THROW(SmbiosParser(nullptr, 100000), SmbiosParser::Exception);
}


TEST_F(SmbiosParserTest, ExceptionWhenBlobIsTooShort) {
    ASSERT_THROW(SmbiosParser(m_blob, 10), SmbiosEntryPoint::Exception);
}


TEST_F(SmbiosParserTest, AnchorStringIncorrect) {
    m_entry_point.anchor[0] = 'A'; // invalid anchor string
    ASSERT_THROW(SmbiosParser(get_blob(), sizeof(m_entry_point)), SmbiosEntryPoint::Exception);
}


TEST_F(SmbiosParserTest, IntermediateAnchorStringIncorrect) {
    m_entry_point.intermediate_anchor[0] = 'A'; // invalid anchor string
    ASSERT_THROW(SmbiosParser(get_blob(), sizeof(m_entry_point)), SmbiosEntryPoint::Exception);
}


TEST_F(SmbiosParserTest, ChecksumIncorrect) {
    m_entry_point.checksum = 123; // invalid checksum
    ASSERT_THROW(SmbiosParser(get_blob(), sizeof(m_entry_point)), SmbiosEntryPoint::Exception);
}


TEST_F(SmbiosParserTest, IntermediateChecksumIncorrect) {
    m_entry_point.intermediate_checksum = 123; // invalid checksum
    ASSERT_THROW(SmbiosParser(get_blob(), sizeof(m_entry_point)), SmbiosEntryPoint::Exception);
}


TEST_F(SmbiosParserTest, AcceptsCorrectEntryPoint) {
    ASSERT_NO_THROW(SmbiosParser(get_blob(), sizeof(m_entry_point)));
}


TEST_F(SmbiosParserTest, ThrowsOnUnknownVersion) {
    ASSERT_THROW(SmbiosParser(reinterpret_cast<const uint8_t*>(fixture::entry_point_3_0_unknown_revision),
                              sizeof(Smbios30EntryPoint::EntryPointStructure)), SmbiosEntryPoint::Exception);
}


TEST_F(SmbiosParserTest, AcceptsCorrect3_0EntryPoint) {
    ASSERT_NO_THROW(SmbiosParser(reinterpret_cast<const uint8_t*>(fixture::entry_point_3_0),
                                 sizeof(Smbios30EntryPoint::EntryPointStructure)));
}


TEST_F(SmbiosParserTest, ReadAndParseCompleteBlob) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios), sizeof(fixture::smbios));

    std::stringstream ss;
    ASSERT_NO_THROW(ss << parser);
}


TEST_F(SmbiosParserTest, ReadAndParseComplete3Blob) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios3_0), sizeof(fixture::smbios3_0));

    std::stringstream ss;
    ASSERT_NO_THROW(ss << parser);
}


TEST_F(SmbiosParserTest, IterateOverSystems) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios), sizeof(fixture::smbios));

    auto systems = parser.get_all<SMBIOS_SYSTEM_INFO_DATA>();
    ASSERT_EQ(1, systems.size());
    EXPECT_STREQ("ABCDEF", systems[0].get_string(systems[0].data.manufacturer).c_str());
}


TEST_F(SmbiosParserTest, IterateOverPcie) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios), sizeof(fixture::smbios));

    auto pcie = parser.get_all<SMBIOS_PCIE_INFO_DATA>();
    ASSERT_EQ(3, pcie.size());
    EXPECT_EQ(0xee, pcie[0].data.SlotNo);
    EXPECT_EQ(0x04, pcie[1].data.SlotNo);
    EXPECT_EQ(0x00, pcie[2].data.SlotNo);
}


TEST_F(SmbiosParserTest, IterateOverNics) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios), sizeof(fixture::smbios));

    auto nic = parser.get_all<SMBIOS_NIC_INFO_DATA>();
    ASSERT_EQ(1, nic.size());
    EXPECT_EQ(0x1234, nic[0].data.VendorID);
}


TEST_F(SmbiosParserTest, IterateOverNicsV2) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios), sizeof(fixture::smbios));

    auto nic = parser.get_all<SMBIOS_NIC_INFO_DATA_V2>();
    ASSERT_EQ(1, nic.size());
    EXPECT_EQ(0x2244, nic[0].data.VendorID);
    EXPECT_STREQ("VERSION_123", nic[0].get_string(nic[0].data.FirmwareVersion).c_str());
}


TEST_F(SmbiosParserTest, IterateOverStorage) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios), sizeof(fixture::smbios));

    auto hdd = parser.get_all<SMBIOS_STORAGE_INFO_DATA>();//(nullptr, SMBIOS_STORAGE_INFO_DATA::LEN);
    ASSERT_EQ(1, hdd.size());
    EXPECT_STREQ("IMPEL SSD", reinterpret_cast<const char*>(hdd[0].data.RsaHddInfoData.HDDName));
}


TEST_F(SmbiosParserTest, IterateOverStorageV2) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios), sizeof(fixture::smbios));

    auto hdd = parser.get_all<SMBIOS_STORAGE_INFO_DATA_V2>();//(nullptr, SMBIOS_STORAGE_INFO_DATA_V2::LEN);
    ASSERT_EQ(1, hdd.size());
    EXPECT_STREQ("IMPEL SSD                               ",
                 std::string(reinterpret_cast<const char*>(hdd[0].data.RsaHddInfoData.HDDName),
                             HDD_INFO_DATA_V2::HDD_NAME_STRING_SIZE).c_str());
    EXPECT_STREQ("VCIL134410FX21E0    ",
                 std::string(reinterpret_cast<const char*>(hdd[0].data.RsaHddInfoData.HDDSerialNumber),
                             HDD_INFO_DATA_V2::HDD_SERIAL_NUMBER_SIZE).c_str());
}


TEST_F(SmbiosParserTest, IterateOverFPGA) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios), sizeof(fixture::smbios));

    auto fpgas = parser.get_all<SMBIOS_FPGA_DATA>();
    ASSERT_EQ(1, fpgas.size());
    auto fpga = fpgas.front();
    EXPECT_EQ(0xc6, fpga.header.type);
    EXPECT_EQ(36, fpga.header.length);
    EXPECT_EQ(uint16_t(0x00 << 8 | 0x73), fpga.header.handle);
    EXPECT_EQ(1, fpga.data.fpga_index);
    EXPECT_EQ(0, static_cast<uint32_t>(fpga.data.fpga_type));
    EXPECT_EQ(1, static_cast<uint32_t>(fpga.data.fpga_status));
    EXPECT_EQ(58, fpga.data.socket_identifier);
    EXPECT_EQ(1, fpga.data.fpga_vendor);
    EXPECT_STREQ("AB", fpga.get_string(fpga.data.fpga_vendor).c_str());
    EXPECT_EQ(2, fpga.data.fpga_family);
    EXPECT_STREQ("CD", fpga.get_string(fpga.data.fpga_family).c_str());
    EXPECT_EQ(3, fpga.data.fpga_model);
    EXPECT_STREQ("EFF", fpga.get_string(fpga.data.fpga_model).c_str());
    EXPECT_EQ(4, fpga.data.fpga_bit_stream_version);
    EXPECT_STREQ("GHH", fpga.get_string(fpga.data.fpga_bit_stream_version).c_str());
    EXPECT_EQ(0x22, fpga.data.fpga_hps_core_count);
    EXPECT_EQ(0x03, fpga.data.fpga_hps_isa);
    EXPECT_EQ(0x00, static_cast<uint8_t>(fpga.data.fpga_hssi_configuration));
    EXPECT_EQ(0x07, fpga.data.fpga_hssi_port_count);
    EXPECT_EQ(0x08, fpga.data.fpga_hssi_port_speed);
    EXPECT_EQ(5, fpga.data.fpga_hssi_side_band_configuration);
    EXPECT_STREQ("IPQ", fpga.get_string(fpga.data.fpga_hssi_side_band_configuration).c_str());
    EXPECT_EQ(0x36, fpga.data.fpga_reconfiguration_slots);
    EXPECT_EQ(uint16_t(0x76 << 8 | 0x26), fpga.data.fpga_pcie_slot_number);
    EXPECT_EQ(0x66, fpga.data.fpga_pcie_bus_identifier);
    EXPECT_EQ(0x55, fpga.data.fpga_pcie_device_identifier);
    EXPECT_EQ(0x21, fpga.data.fpga_pcie_function_identifier);
    EXPECT_EQ(uint32_t(((0x12 << 8 | 0x61) << 8 | 0x32) << 8 | 0x02), fpga.data.thermal_design_power);
    EXPECT_EQ(0x02, static_cast<uint8_t>(fpga.data.memory_technology));
    EXPECT_EQ(uint32_t(((0x38 << 8 | 0x47) << 8 | 0x55) << 8 | 0x01), fpga.data.integrated_memory_capacity);
    EXPECT_EQ(uint16_t(0x63 << 8 | 0x42), fpga.data.integrated_memory_speed);
}


TEST_F(SmbiosParserTest, IterateOverFPGAOem) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios), sizeof(fixture::smbios));

    auto fpgas = parser.get_all<SMBIOS_FPGA_DATA_OEM>();
    ASSERT_EQ(1, fpgas.size());
    auto fpga = fpgas.front();
    EXPECT_EQ(0xc6, fpga.header.type);
    EXPECT_EQ(35, fpga.header.length);
    EXPECT_EQ(uint16_t(0x00 << 8 | 0x79), fpga.header.handle);
    EXPECT_EQ(1, fpga.data.fpga_index);
    EXPECT_EQ(0, static_cast<uint32_t>(fpga.data.fpga_type));
    EXPECT_EQ(1, static_cast<uint32_t>(fpga.data.fpga_status));
    EXPECT_EQ(58, fpga.data.socket_identifier);
    EXPECT_EQ(1, fpga.data.fpga_vendor);
    EXPECT_STREQ("AB", fpga.get_string(fpga.data.fpga_vendor).c_str());
    EXPECT_EQ(2, fpga.data.fpga_family);
    EXPECT_STREQ("CD", fpga.get_string(fpga.data.fpga_family).c_str());
    EXPECT_EQ(3, fpga.data.fpga_model);
    EXPECT_STREQ("EFF", fpga.get_string(fpga.data.fpga_model).c_str());
    EXPECT_EQ(4, fpga.data.fpga_bit_stream_version);
    EXPECT_STREQ("GHH", fpga.get_string(fpga.data.fpga_bit_stream_version).c_str());
    EXPECT_EQ(0x22, fpga.data.fpga_hps_core_count);
    EXPECT_EQ(0x03, fpga.data.fpga_hps_isa);
    EXPECT_EQ(0x00, static_cast<uint8_t>(fpga.data.fpga_hssi_configuration));
    EXPECT_EQ(0x07, fpga.data.fpga_hssi_port_count);
    EXPECT_EQ(0x08, fpga.data.fpga_hssi_port_speed);
    EXPECT_EQ(5, fpga.data.fpga_hssi_side_band_configuration);
    EXPECT_STREQ("IPQ", fpga.get_string(fpga.data.fpga_hssi_side_band_configuration).c_str());
    EXPECT_EQ(0x36, fpga.data.fpga_reconfiguration_slots);
    EXPECT_EQ(uint16_t(0x76 << 8 | 0x26), fpga.data.fpga_pcie_slot_number);
    EXPECT_EQ(0x66, fpga.data.fpga_pcie_bus_identifier);
    EXPECT_EQ(0x55, fpga.data.fpga_pcie_device_identifier);
    EXPECT_EQ(0x21, fpga.data.fpga_pcie_function_identifier);
    EXPECT_EQ(uint32_t(((0x12 << 8 | 0x61) << 8 | 0x32) << 8 | 0x02), fpga.data.thermal_design_power);
    EXPECT_EQ(0x02, static_cast<uint8_t>(fpga.data.memory_technology));
    EXPECT_EQ(uint32_t(((0x38 << 8 | 0x47) << 8 | 0x55) << 8 | 0x01), fpga.data.integrated_memory_capacity);
    EXPECT_EQ(uint16_t(0x63 << 8 | 0x42), fpga.data.integrated_memory_speed);
}


TEST_F(SmbiosParserTest, IterateOverStorageDevices) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios), sizeof(fixture::smbios));

    auto storages = parser.get_all<SMBIOS_STORAGE_DEVICE_INFO_DATA>();
    ASSERT_EQ(1, storages.size());
    auto storage = storages.front();
    EXPECT_EQ(0xc2, storage.header.type);
    EXPECT_EQ(27, storage.header.length);
    EXPECT_EQ(uint16_t(0x00 << 8 | 0x74), storage.header.handle);
    EXPECT_EQ(7, storage.data.port_designation);
    EXPECT_EQ(3, storage.data.device_index);
    EXPECT_EQ(0x01, static_cast<uint8_t>(storage.data.connector_type));
    EXPECT_EQ(0x02, static_cast<uint8_t>(storage.data.device_protocol));
    EXPECT_EQ(0x03, static_cast<uint8_t>(storage.data.device_type));
    EXPECT_EQ(uint32_t(((0x38 << 8 | 0x47) << 8 | 0x55) << 8 | 0x01), storage.data.device_capacity);
    EXPECT_EQ(uint16_t(0x63 << 8 | 0x42), storage.data.device_rpm);
    EXPECT_EQ(1, storage.data.device_model);
    EXPECT_STREQ("321DRIVE", storage.get_string(storage.data.device_model).c_str());
    EXPECT_EQ(2, storage.data.device_serial);
    EXPECT_STREQ("13265487", storage.get_string(storage.data.device_serial).c_str());
    EXPECT_EQ(0x67, storage.data.pci_class_code);
    EXPECT_EQ(uint16_t(0x76 << 8 | 0x26), storage.data.vendor_id);
    EXPECT_EQ(uint16_t(0x13 << 8 | 0x34), storage.data.device_id);
    EXPECT_EQ(uint16_t(0xde << 8 | 0xac), storage.data.sub_vendor_id);
    EXPECT_EQ(uint16_t(0xaf << 8 | 0x53), storage.data.sub_device_id);
    EXPECT_EQ(3, storage.data.firmware_version);
    EXPECT_STREQ("ABCD2468", storage.get_string(storage.data.firmware_version).c_str());
}


TEST_F(SmbiosParserTest, IterateOverTPMs) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios), sizeof(fixture::smbios));
    auto tpms = parser.get_all<SMBIOS_TPM_INFO_DATA>();
    ASSERT_EQ(2, tpms.size());
    auto tpm_1 = tpms.front();
    EXPECT_EQ(0xc3, tpm_1.header.type);
    EXPECT_EQ(7, tpm_1.header.length);
    EXPECT_EQ(uint16_t(0x00 << 8 | 0x75), tpm_1.header.handle);
    EXPECT_EQ(1, tpm_1.data.tpm_configuration_index);
    EXPECT_EQ(1, tpm_1.data.tpm_version);
    EXPECT_STREQ("TPM 2.0", tpm_1.get_string(tpm_1.data.tpm_version).c_str());

    auto tpm_2 = tpms.back();
    EXPECT_EQ(0xc3, tpm_2.header.type);
    EXPECT_EQ(7, tpm_2.header.length);
    EXPECT_EQ(uint16_t(0x00 << 8 | 0x76), tpm_2.header.handle);
    EXPECT_EQ(2, tpm_2.data.tpm_configuration_index);
    EXPECT_EQ(1, tpm_2.data.tpm_version);
    EXPECT_STREQ("TPM 1.2", tpm_2.get_string(tpm_2.data.tpm_version).c_str());
}


TEST_F(SmbiosParserTest, IterateOverTxts) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios), sizeof(fixture::smbios));
    auto txts = parser.get_all<SMBIOS_TXT_INFO_DATA>();
    ASSERT_EQ(1, txts.size());
    auto txt = txts.front();
    EXPECT_EQ(0xc4, txt.header.type);
    EXPECT_EQ(5, txt.header.length);
    EXPECT_EQ(uint16_t(0x00 << 8 | 0x77), txt.header.handle);
    EXPECT_EQ(0x01, static_cast<uint8_t>(txt.data.status));
}


TEST_F(SmbiosParserTest, IterateOverMemoryExtended) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios), sizeof(fixture::smbios));
    auto memories = parser.get_all<SMBIOS_MEMORY_DEVICE_EXTENDED_INFO_DATA>();
    ASSERT_EQ(1, memories.size());
    auto memory = memories.front();
    EXPECT_EQ(0xc5, memory.header.type);
    EXPECT_EQ(15, memory.header.length);
    EXPECT_EQ(uint16_t(0x00 << 8 | 0x78), memory.header.handle);
    EXPECT_EQ(uint16_t(0x00 << 8 | 0x01), memory.data.memory_device_handle);
    EXPECT_EQ(0x01, static_cast<uint8_t>(memory.data.type));
    EXPECT_EQ(0x02, static_cast<uint8_t>(memory.data.media));
    EXPECT_EQ(1, memory.data.firmware_revision);
    EXPECT_STREQ("123MEMORY", memory.get_string(memory.data.firmware_revision).c_str());
    EXPECT_EQ(2, memory.data.firmware_api_version);
    EXPECT_STREQ("XYZ_5678", memory.get_string(memory.data.firmware_api_version).c_str());
    EXPECT_EQ(uint32_t(((0x67 << 8 | 0x45) << 8 | 0x23) << 8 | 0x12), memory.data.max_tdp);
    EXPECT_EQ(0x07, memory.data.smbus_address);
}


TEST_F(SmbiosParserTest, SpeedSelectWithZeroConfigs) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios_v3_with_zero_speed_select_configs),
                        sizeof(fixture::smbios_v3_with_zero_speed_select_configs));
    auto speed_select_infos = parser.get_all<SMBIOS_SPEED_SELECT_INFO_DATA>();
    ASSERT_EQ(1, speed_select_infos.size());
    auto speed_select = speed_select_infos.front();
    EXPECT_EQ(210, speed_select.header.type);
    EXPECT_EQ(16, speed_select.header.length);
    EXPECT_EQ(1, speed_select.header.handle);
    EXPECT_EQ(0, speed_select.data.socket_number);
    EXPECT_EQ(1, speed_select.data.structure_version);
    EXPECT_EQ(0, speed_select.data.number_of_configs);
    EXPECT_EQ(0, speed_select.data.current_config);
    ASSERT_EQ(0, speed_select.configs.size());
}

TEST_F(SmbiosParserTest, SpeedSelectWithOneConfig) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios_v3_with_one_speed_select_config),
        sizeof(fixture::smbios_v3_with_one_speed_select_config));
    auto speed_select_infos = parser.get_all<SMBIOS_SPEED_SELECT_INFO_DATA>();
    ASSERT_EQ(1, speed_select_infos.size());
    auto speed_select = speed_select_infos.front();
    EXPECT_EQ(210, speed_select.header.type);
    EXPECT_EQ(48, speed_select.header.length);
    EXPECT_EQ(1, speed_select.header.handle);
    EXPECT_EQ(0, speed_select.data.socket_number);
    EXPECT_EQ(1, speed_select.data.structure_version);
    EXPECT_EQ(1, speed_select.data.number_of_configs);
    EXPECT_EQ(0, speed_select.data.current_config);
    ASSERT_EQ(1, speed_select.configs.size());
    auto config = speed_select.configs.front();
    EXPECT_EQ(0, config.configuration_number);
    EXPECT_EQ(18, config.high_priority_core_count);
}


TEST_F(SmbiosParserTest, SpeedSelectWithThreeConfigs) {
    SmbiosParser parser(reinterpret_cast<const uint8_t*>(fixture::smbios_v3_with_three_speed_select_configs),
                        sizeof(fixture::smbios_v3_with_three_speed_select_configs));
    auto speed_select_infos = parser.get_all<SMBIOS_SPEED_SELECT_INFO_DATA>();
    ASSERT_EQ(1, speed_select_infos.size());
    auto speed_select = speed_select_infos.front();
    EXPECT_EQ(210, speed_select.header.type);
    EXPECT_EQ(112, speed_select.header.length);
    EXPECT_EQ(1, speed_select.header.handle);
    EXPECT_EQ(0, speed_select.data.socket_number);
    EXPECT_EQ(1, speed_select.data.structure_version);
    EXPECT_EQ(3, speed_select.data.number_of_configs);
    EXPECT_EQ(0, speed_select.data.current_config);
    ASSERT_EQ(3, speed_select.configs.size());
    auto config = speed_select.configs.front();
    EXPECT_EQ(0, config.configuration_number);
    EXPECT_EQ(18, config.high_priority_core_count);
    EXPECT_EQ("TBD0", speed_select.get_string(config.high_priority_code_apic_ids));
}