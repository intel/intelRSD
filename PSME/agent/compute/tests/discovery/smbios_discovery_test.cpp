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


#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/common_components.hpp"
#include "discovery/discovery_manager.hpp"
#include "discovery/smbios/processor.hpp"
#include "discovery/smbios/fpga.hpp"
#include "discovery/smbios/memory.hpp"
#include "discovery/smbios/storage_device.hpp"
#include "discovery/smbios/txt_and_tpm.hpp"
#include "loader/compute_loader.hpp"

#include "mock/mock_const.hpp"
#include "mock/mock_management_controller.hpp"
#include "mock/configuration_full.hpp"
#include "mock/smbios_mock.hpp"

#include <gtest/gtest.h>



using smbios::parser::SmbiosParser;
using namespace agent::compute::discovery::smbios;
using namespace agent_framework::model;
using namespace agent_framework::module;

class SmbiosDiscoveryTest : public ::testing::Test {
public:
    SmbiosDiscoveryTest() {
        mocked_smbios.reset(
            new SmbiosParser(reinterpret_cast<const uint8_t*>(mocked_smbios::smbios), sizeof(mocked_smbios::smbios)));
    };


    virtual ~SmbiosDiscoveryTest() { };
protected:
    std::shared_ptr<SmbiosParser> mocked_smbios{};
};

TEST_F(SmbiosDiscoveryTest, DiscoverTPM) {
    agent_framework::model::System system{"1234"};
    discover_tpm(mocked_smbios, system);
    const auto tpm_uuids = get_manager<TrustedModule>().get_keys();
    const auto tpm_1 = get_manager<TrustedModule>().get_entry(tpm_uuids[0]);
    const auto tpm_2 = get_manager<TrustedModule>().get_entry(tpm_uuids[1]);
    ASSERT_EQ(tpm_uuids.size(), 2);
    EXPECT_EQ(tpm_1.get_configuration_index(), 1);
    EXPECT_STREQ("TPM2_0", tpm_1.get_interface_type().value().to_string());
    EXPECT_EQ(tpm_2.get_configuration_index(), 2);
    EXPECT_STREQ("TPM1_2", tpm_2.get_interface_type().value().to_string());
}


TEST_F(SmbiosDiscoveryTest, TXTEnabled) {
    agent_framework::model::System system{"1234"};
    discover_txt(mocked_smbios, system);
    ASSERT_EQ(system.is_txt_enabled(), true);
}


TEST_F(SmbiosDiscoveryTest, CPUID) {
    agent_framework::model::Processor processor{"1"};
    processor.set_socket("CPU5");
    bool ok = discover_cpu_id(mocked_smbios, processor);
    EXPECT_TRUE(ok);

    const auto& cpu_id = processor.get_extended_cpu_id();

    EXPECT_TRUE(cpu_id.get_eax_00h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_00h().value().c_str(), "0x000102030405060708090a0b0c0d0e0f");

    EXPECT_TRUE(cpu_id.get_eax_01h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_01h().value().c_str(), "0x00102030405060708090a0b0c0d0e0f0");

    EXPECT_TRUE(cpu_id.get_eax_02h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_02h().value().c_str(), "0x112233445566778899aabbccddeeff00");

    EXPECT_TRUE(cpu_id.get_eax_03h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_03h().value().c_str(), "0x00000000000000000000000000000000");

    EXPECT_TRUE(cpu_id.get_eax_04h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_04h().value().c_str(), "0xffffffffffffffffffffffffffffffff");

    EXPECT_TRUE(cpu_id.get_eax_05h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_05h().value().c_str(), "0x101112131415161718191a1b1c1d1e1f");

    EXPECT_TRUE(cpu_id.get_eax_06h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_06h().value().c_str(), "0x88000000000000000000000000000000");

    EXPECT_TRUE(cpu_id.get_eax_07h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_07h().value().c_str(), "0x00880000000000000000000000000000");

    EXPECT_TRUE(cpu_id.get_eax_09h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_09h().value().c_str(), "0x00008800000000000000000000000000");

    EXPECT_TRUE(cpu_id.get_eax_0ah().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_0ah().value().c_str(), "0x00000088000000000000000000000000");

    EXPECT_TRUE(cpu_id.get_eax_0bh().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_0bh().value().c_str(), "0x00000000880000000000000000000000");

    EXPECT_TRUE(cpu_id.get_eax_0dh().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_0dh().value().c_str(), "0x00000000008800000000000000000000");

    EXPECT_TRUE(cpu_id.get_eax_0fh().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_0fh().value().c_str(), "0x00000000000088000000000000000000");

    EXPECT_TRUE(cpu_id.get_eax_10h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_10h().value().c_str(), "0x00000000000000880000000000000000");

    EXPECT_TRUE(cpu_id.get_eax_14h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_14h().value().c_str(), "0x00000000000000008800000000000000");

    EXPECT_TRUE(cpu_id.get_eax_15h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_15h().value().c_str(), "0x00000000000000000088000000000000");

    EXPECT_TRUE(cpu_id.get_eax_16h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_16h().value().c_str(), "0x00000000000000000000880000000000");

    EXPECT_TRUE(cpu_id.get_eax_17h_ecx_00h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_17h_ecx_00h().value().c_str(), "0x00000000000000000000008800000000");

    EXPECT_TRUE(cpu_id.get_eax_17h_ecx_01h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_17h_ecx_01h().value().c_str(), "0x00000000000000000000000088000000");

    EXPECT_TRUE(cpu_id.get_eax_17h_ecx_02h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_17h_ecx_02h().value().c_str(), "0x00000000000000000000000000880000");

    EXPECT_TRUE(cpu_id.get_eax_17h_ecx_03h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_17h_ecx_03h().value().c_str(), "0x00000000000000000000000000008800");

    EXPECT_TRUE(cpu_id.get_eax_80000000h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_80000000h().value().c_str(), "0x00000000000000000000000000000088");

    EXPECT_TRUE(cpu_id.get_eax_80000001h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_80000001h().value().c_str(), "0x12003400560078009a00bc00de00f000");

    EXPECT_TRUE(cpu_id.get_eax_80000002h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_80000002h().value().c_str(), "0x0112233445566778899aabbccddedff0");

    EXPECT_TRUE(cpu_id.get_eax_80000003h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_80000003h().value().c_str(), "0x0012003400560078009a00bc00de00f0");

    EXPECT_TRUE(cpu_id.get_eax_80000004h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_80000004h().value().c_str(), "0xffffffffffffffff0000000000000000");

    EXPECT_TRUE(cpu_id.get_eax_80000006h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_80000006h().value().c_str(), "0x11001100110011000011001100110011");

    EXPECT_TRUE(cpu_id.get_eax_80000007h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_80000007h().value().c_str(), "0x00110011001100111100110011001100");

    EXPECT_TRUE(cpu_id.get_eax_80000008h().has_value());
    EXPECT_STRCASEEQ(cpu_id.get_eax_80000008h().value().c_str(), "0x80088008800880080880088008800880");
}


TEST_F(SmbiosDiscoveryTest, DiscoverFPGA) {
    System system{"1234"};
    discover_fpga(mocked_smbios, system);
    auto& manager = get_manager<agent_framework::model::Processor>();
    auto uuid_vector = manager.get_keys(system.get_uuid(),
                                        [](const agent_framework::model::Processor& processor) {
                                            return processor.get_processor_type() ==
                                                   agent_framework::model::enums::ProcessorType::FPGA;
                                        });
    ASSERT_EQ(1, uuid_vector.size());
    auto fpga = get_manager<agent_framework::model::Processor>().get_entry(uuid_vector.front());
    EXPECT_STREQ(fpga.get_cpu_id().get_vendor_id().value().c_str(), "ALTERA");
    EXPECT_STREQ(fpga.get_cpu_id().get_family().value().c_str(), "0xFF");
    EXPECT_STREQ(fpga.get_model_name().value().c_str(), "CPU Integrated FPGA Accelerator XYZ 8008");
    EXPECT_EQ(fpga.get_total_cores(), 8);
    EXPECT_DOUBLE_EQ(fpga.get_tdp_watt(), 308359682);
    EXPECT_STREQ(fpga.get_socket().value().c_str(), "0");
    EXPECT_EQ(fpga.get_processor_architecture(), enums::ProcessorArchitecture::OEM);
    EXPECT_STREQ(fpga.get_fpga().get_bit_stream_version().value().c_str(), "BLUE1");
    EXPECT_EQ(fpga.get_fpga().get_type(), enums::FpgaType::Integrated);
    EXPECT_EQ(fpga.get_fpga().get_hssi_sideband(), enums::HssiSideband::I2C);
    EXPECT_EQ(fpga.get_fpga().get_hssi_configuration(), enums::HssiConfiguration::_4x10G);
    EXPECT_EQ(fpga.get_fpga().get_reconfiguration_slots(), 54);
    ASSERT_EQ(fpga.get_integrated_memory().size(), 1);
    EXPECT_EQ(fpga.get_integrated_memory()[0].get_type(), enums::IntegratedMemoryType::HBM);
    EXPECT_EQ(fpga.get_integrated_memory()[0].get_capacity_mb(), 944198913);
    EXPECT_EQ(fpga.get_integrated_memory()[0].get_speed_mhz(), 25410);
}


TEST_F(SmbiosDiscoveryTest, DiscoverStorage) {
    System system{"1234"};
    Chassis chassis{"12345"};
    discover_storage_device(mocked_smbios, system, chassis.get_uuid());
    auto& manager = get_manager<agent_framework::model::Drive>();
    auto uuid_vector = manager.get_keys(chassis.get_uuid());
    ASSERT_EQ(uuid_vector.size(), 1);
    auto drive = manager.get_entry(uuid_vector.front());
    EXPECT_EQ(255, drive.get_capacity_gb());
    EXPECT_EQ(enums::TransportProtocol::SATA, drive.get_interface());
    EXPECT_EQ(enums::DriveType::HDD, drive.get_type());
    EXPECT_STREQ("ABC789", drive.get_firmware_version().value().c_str());
    EXPECT_STREQ("321Model", drive.get_fru_info().get_model_number().value().c_str());
    EXPECT_STREQ("INTEL123", drive.get_fru_info().get_serial_number().value().c_str());
}


TEST_F(SmbiosDiscoveryTest, DiscoverMemory) {
    System system{"1234"};
    discover_memory_modules(mocked_smbios, system);
    auto& manager = get_manager<agent_framework::model::Memory>();
    auto uuid_vector = manager.get_keys(system.get_uuid());
    ASSERT_EQ(uuid_vector.size(), 1);
    auto memory = manager.get_entry(uuid_vector.front());
    EXPECT_STREQ("bvg", memory.get_fru_info().get_manufacturer().value().c_str());
    EXPECT_STREQ("123MEMORY", memory.get_firmware_revision().value().c_str());
    EXPECT_STREQ("XYZ_5678", memory.get_firmware_api_version().value().c_str());
}
