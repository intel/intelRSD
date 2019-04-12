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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or impliedev.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section SysfsDecoderTests
 * */


#include "sysfs/sysfs_reader_interface.hpp"
#include "sysfs/sysfs_decoder.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <regex>

using namespace agent::pnc::sysfs;

namespace {
    constexpr const uint8_t HEADER_TYPE_1 = 0x01;
    constexpr const uint8_t HEADER_TYPE_0 = 0x00;

    constexpr const uint16_t VENDOR_PMC_SIERRA = 0x11f8;
    constexpr const uint16_t DEVICE_PMC_SIERRA = 0x8546;

    constexpr const uint8_t CLASS_STORAGE = 0x01;
    constexpr const uint8_t CLASS_MEMORY = 0x05;
    constexpr const uint8_t CLASS_BRIDGE = 0x06;

    constexpr const uint8_t SUBCLASS_NVME = 0x08;
    constexpr const uint8_t SUBCLASS_MEMORY = 0x80;
    constexpr const uint8_t SUBCLASS_PCI_BRIDGE = 0x04;

    constexpr const uint8_t PROG_NVME = 0x02;

    constexpr const uint8_t PCI_DEVICE_TYPE_MASK = 0x80;
    constexpr const uint16_t PCI_STATUS_CAPABILITY_BIT_MASK = 0x0010;
    constexpr const uint8_t PCI_EXPRESS_CAPABILITY_ID = 0x10;
    constexpr const uint16_t PCI_SERIAL_NUMBER_EXTENDED_CAPABILITY_ID = 0x0003;
    constexpr const uint16_t PCI_EXTENDED_CAPABILITY_OFFSET = 0x0100;
    constexpr const uint8_t PCI_EXTENDED_CAPABILITY_POINTER_SHIFT = 0x04;
}

/*! Fluent interface to make code cleaner */
class DevBuilder {
public:
    DevBuilder() = default;
    DevBuilder(const DevBuilder&) = default;
    DevBuilder& operator=(const DevBuilder&) = default;

    operator RawSysfsDevice() {
        return dev;
    }

    DevBuilder& add_cap_pcie(uint8_t addr, uint32_t link_cap, uint16_t link_stat) {
        PciExpressCapability* cap =
            reinterpret_cast<PciExpressCapability*>(dev.configuration.raw + addr);
        cap->link_capabilities = link_cap;
        cap->link_status = link_stat;
        return *this;
    }

    DevBuilder& add_ext_cap_header(uint16_t addr, uint16_t id, uint16_t nxt) {
        PciExtendedCapabilityHeader* header =
            reinterpret_cast<PciExtendedCapabilityHeader*>(dev.configuration.raw + addr);
        header->capability_id = id;
        header->version_next_pointer |= uint16_t(nxt << ::PCI_EXTENDED_CAPABILITY_POINTER_SHIFT);
        return *this;
    }

    DevBuilder& add_ext_cap_sn(uint16_t addr, uint32_t sn_lower, uint32_t sn_upper) {
        PciSerialNumberExtendedCapability* cap =
            reinterpret_cast<PciSerialNumberExtendedCapability*>(dev.configuration.raw + addr);
        cap->serial_number_lower = sn_lower;
        cap->serial_number_upper = sn_upper;
        return *this;
    }

    DevBuilder& add_cap_header(uint8_t addr, uint8_t id, uint8_t nxt) {
        PciCapabilityHeader* header =
            reinterpret_cast<PciCapabilityHeader*>(dev.configuration.raw + addr);
        header->capability_id = id;
        header->next_capability_pointer = nxt;
        return *this;
    }

    DevBuilder& set_class(uint8_t code, uint8_t subclass, uint8_t prog) {
        dev.configuration.fields.common.class_code = code;
        dev.configuration.fields.common.subclass = subclass;
        dev.configuration.fields.common.prog_if = prog;
        return *this;
    }

    DevBuilder& set_type0_caps(uint8_t ptr) {
        dev.configuration.fields.common.status |= ::PCI_STATUS_CAPABILITY_BIT_MASK;
        dev.configuration.fields.extended.type0.capability_pointer = ptr;
        return *this;
    }

    DevBuilder& set_type1_header(uint8_t sec_bus) {
        dev.configuration.fields.common.header_type |= ::HEADER_TYPE_1;
        dev.configuration.fields.extended.type1.secondary_bus_number = sec_bus;
        return *this;
    }

    DevBuilder& set_type0_header(uint16_t sub_vendor, uint16_t subsystem) {
        dev.configuration.fields.common.header_type |= ::HEADER_TYPE_0;
        dev.configuration.fields.extended.type0.subsystem_vendor_id = sub_vendor;
        dev.configuration.fields.extended.type0.subsystem_id = subsystem;
        return *this;
    }

    DevBuilder& set_common_header(uint16_t vendor, uint16_t device, uint8_t rev,
            bool is_multi = false) {
        dev.configuration.fields.common.vendor_id = vendor;
        dev.configuration.fields.common.device_id = device;
        dev.configuration.fields.common.revision_id = rev;
        if (is_multi) {
            dev.configuration.fields.common.header_type |= PCI_DEVICE_TYPE_MASK;
        }
        return *this;
    }

    DevBuilder& set_nums(const std::string& pre, const std::string& name,
            bool is_virtual = false) {
        dev.id = SysfsId::from_string(name);
        dev.is_virtual = is_virtual;
        dev.name = name;
        dev.path = pre + name;
        return *this;
    }

    DevBuilder& add_drive(const std::string& name, double size) {
       dev.drive_names.push_back(name);
       dev.drive_sizes.push_back(size);
       return *this;
    }

    DevBuilder& make_invalid_t0_dev(const std::string& pre, const std::string& name) {
        set_nums(pre, name);
        set_common_header(0x1234, 0x2345, 0x34, false);
        set_class(0x12, 0x34, 0x56);
        set_type1_header(0x01);
        return *this;
    }

    DevBuilder& make_invalid_t1_dev(const std::string& pre, const std::string& name) {
        set_nums(pre, name);
        set_common_header(0x1234, 0x2345, 0x34, false);
        set_class(0x12, 0x34, 0x56);
        set_type1_header(0x01);
        return *this;
    }

    DevBuilder& make_switch_vep(const std::string& pre, const std::string& name) {
        set_nums(pre, name);
        set_common_header(VENDOR_PMC_SIERRA, DEVICE_PMC_SIERRA, 0x12, true);
        set_class(CLASS_MEMORY, SUBCLASS_MEMORY, 0x00);
        set_type0_header(VENDOR_PMC_SIERRA, DEVICE_PMC_SIERRA);
        return *this;
    }

    DevBuilder& make_switch_bridge(const std::string& pre, const std::string& name,
            uint8_t sec_bus, bool is_primary_bridge) {
        set_nums(pre, name);
        set_common_header(VENDOR_PMC_SIERRA, DEVICE_PMC_SIERRA, 0x12, is_primary_bridge);
        set_class(CLASS_BRIDGE, SUBCLASS_PCI_BRIDGE, 0x00);
        set_type1_header(sec_bus);
        return *this;
    }

    DevBuilder& make_drive(const std::string& pre, const std::string& name) {
        set_nums(pre, name);
        set_common_header(0x00dd, 0x11dd, 0x12, false);
        set_class(CLASS_STORAGE, SUBCLASS_NVME, PROG_NVME);
        set_type0_header(0x22dd, 0x33dd);
        return *this;
    }

private:
    RawSysfsDevice dev{};
};

/*! Dummy sysfs reader getting test data */
class SysfsDummyReader : public SysfsReaderInterface {
public:

    SysfsDummyReader() = default;
    SysfsDummyReader(const SysfsDummyReader&) = default;
    SysfsDummyReader& operator=(const SysfsDummyReader&) = default;

    virtual ~SysfsDummyReader();

    virtual std::vector<RawSysfsDevice> get_raw_sysfs_devices(const std::string& = "") const override {
        return m_devices;
    }

    void add_device(const RawSysfsDevice& dev) {
        m_devices.push_back(dev);
    }

    void clear() {
        m_devices.clear();
    }

private:
    std::vector<RawSysfsDevice> m_devices{};
};

SysfsDummyReader::~SysfsDummyReader() {}

class SysfsDecoderTest: public ::testing::Test {
protected:

    void SetUp() override {}

    void TearDown() override {
        decoder = SysfsDecoder();
        reader.clear();
    }

    SysfsDummyReader reader{};
    SysfsDecoder decoder{};

public:
    SysfsDecoderTest() {}
    virtual ~SysfsDecoderTest();
};

SysfsDecoderTest::~SysfsDecoderTest() {}


TEST_F(SysfsDecoderTest, NoProblemsWithInvalidDevices) {
    reader.add_device(DevBuilder().make_invalid_t0_dev("/", "0000:00:00.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/", "0000:00:00.1"));
    reader.add_device(DevBuilder().make_invalid_t1_dev("/", "0000:00:00.2"));
    reader.add_device(DevBuilder().make_invalid_t1_dev("/", "0000:00:00.3"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/", "0000:00:01.0"));
    reader.add_device(DevBuilder().make_invalid_t1_dev("/", "0000:00:02.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/", "0000:00:03.0"));
    reader.add_device(DevBuilder().make_invalid_t1_dev("/", "0000:00:04.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/", "0000:01:01.0"));
    reader.add_device(DevBuilder().make_invalid_t1_dev("/", "0000:01:02.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/", "0000:02:03.0"));
    reader.add_device(DevBuilder().make_invalid_t1_dev("/", "0000:03:04.0"));

    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    EXPECT_EQ(decoder.get_switches().size(), 0);
}

TEST_F(SysfsDecoderTest, NoPcieSwitchIfBridgeIsNotPresent) {
    reader.add_device(DevBuilder().make_switch_vep("/","0000:00:00.0"));
    reader.add_device(DevBuilder().make_invalid_t1_dev("/","0000:00:00.1"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/","0000:00:00.2"));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    EXPECT_EQ(decoder.get_switches().size(), 0);
}

TEST_F(SysfsDecoderTest, NoPcieSwitchIfVEPIsNotPresent) {
    reader.add_device(DevBuilder().make_switch_bridge("/","0000:00:00.0", 0xFF, true));
    reader.add_device(DevBuilder().make_invalid_t1_dev("/","0000:00:00.1"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/","0000:00:00.2"));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    EXPECT_EQ(decoder.get_switches().size(), 0);
}

TEST_F(SysfsDecoderTest, NoPcieSwitchIfDifferentPaths) {
    reader.add_device(DevBuilder().make_switch_bridge("/","0000:00:00.0", 0xFF, true));
    reader.add_device(DevBuilder().make_switch_vep("/t/","0000:00:00.1"));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    EXPECT_EQ(decoder.get_switches().size(), 0);
}

TEST_F(SysfsDecoderTest, NoPcieSwitchIfDifferentDomains) {
    reader.add_device(DevBuilder().make_switch_bridge("/","0000:00:00.0", 0xFF, true));
    reader.add_device(DevBuilder().make_switch_vep("/","0001:00:00.1"));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    EXPECT_EQ(decoder.get_switches().size(), 0);
}

TEST_F(SysfsDecoderTest, NoPcieSwitchIfDifferentBuses) {
    reader.add_device(DevBuilder().make_switch_bridge("/","0000:00:00.0", 0xFF, true));
    reader.add_device(DevBuilder().make_switch_vep("/","0000:01:00.1"));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    EXPECT_EQ(decoder.get_switches().size(), 0);
}

TEST_F(SysfsDecoderTest, NoPcieSwitchIfDifferentDevices) {
    reader.add_device(DevBuilder().make_switch_bridge("/","0000:00:00.0", 0xFF, true));
    reader.add_device(DevBuilder().make_switch_vep("/","0000:00:01.1"));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    EXPECT_EQ(decoder.get_switches().size(), 0);
}

TEST_F(SysfsDecoderTest, NoPcieSwitchIfVepHasInvalidClass) {
    reader.add_device(DevBuilder().make_switch_bridge("/","0000:00:00.0", 0xFF, true));
    reader.add_device(DevBuilder().make_switch_vep("/","0000:00:00.1")
        .set_class(0xDD, SUBCLASS_MEMORY, 0x00));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    EXPECT_EQ(decoder.get_switches().size(), 0);
}

TEST_F(SysfsDecoderTest, NoPcieSwitchIfVepHasInvalidSublass) {
    reader.add_device(DevBuilder().make_switch_bridge("/","0000:00:00.0", 0xFF, true));
    reader.add_device(DevBuilder().make_switch_vep("/","0000:00:00.1")
        .set_class(CLASS_MEMORY, 0xDD, 0x00));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    EXPECT_EQ(decoder.get_switches().size(), 0);
}

TEST_F(SysfsDecoderTest, NoPcieSwitchIfVepHasInvalidManufacturerId) {
    reader.add_device(DevBuilder().make_switch_bridge("/","0000:00:00.0", 0xFF, true));
    reader.add_device(DevBuilder().make_switch_vep("/","0000:00:00.1")
        .set_common_header(0xFFFF, DEVICE_PMC_SIERRA, 0x00, true));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    EXPECT_EQ(decoder.get_switches().size(), 0);
}

TEST_F(SysfsDecoderTest, NoPcieSwitchIfVepHasInvalidDeviceId) {
    reader.add_device(DevBuilder().make_switch_bridge("/","0000:00:00.0", 0xFF, true));
    reader.add_device(DevBuilder().make_switch_vep("/","0000:00:00.1")
        .set_common_header(VENDOR_PMC_SIERRA, 0xFFFF, 0x00, true));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    EXPECT_EQ(decoder.get_switches().size(), 0);
}

TEST_F(SysfsDecoderTest, NoPcieSwitchIfBridgeHasInvalidClass) {
    reader.add_device(DevBuilder().make_switch_vep("/","0000:00:00.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/","0000:00:00.0", 0xFF, true)
        .set_class(0xDD, SUBCLASS_PCI_BRIDGE, 0x00));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    EXPECT_EQ(decoder.get_switches().size(), 0);
}

TEST_F(SysfsDecoderTest, NoPcieSwitchIfBridgeHasInvalidSubclass) {
    reader.add_device(DevBuilder().make_switch_vep("/","0000:00:00.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/","0000:00:00.0", 0xFF, true)
        .set_class(CLASS_BRIDGE, 0xDD, 0x00));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    EXPECT_EQ(decoder.get_switches().size(), 0);
}

TEST_F(SysfsDecoderTest, PcieSwitchIsCorrectlyFound) {
    reader.add_device(DevBuilder().make_switch_vep("/","0000:00:00.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/","0000:00:00.0", 0xFF, true));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    EXPECT_EQ(decoder.get_bridges(switches[0]).size(), 0);
}

TEST_F(SysfsDecoderTest, PcieSwitchDataIsCorrectlyRead) {
    reader.add_device(DevBuilder().make_switch_vep("/t/","0000:00:11.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/t/","0000:00:11.0", 0x22, true));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    auto sw = switches[0];
    EXPECT_EQ(SysfsId(0x0000, 0x00, 0x11, 0x01), sw.memory_id);
    EXPECT_EQ(SysfsId(0x0000, 0x00, 0x11, 0x00), sw.bridge_id);
    EXPECT_EQ(sw.vendor_id, VENDOR_PMC_SIERRA);
    EXPECT_EQ(sw.device_id, DEVICE_PMC_SIERRA);
    EXPECT_STREQ(sw.memory_resource.c_str(), "/t/0000:00:11.1/resource0");
    EXPECT_STREQ(sw.bridge_path.c_str(), "/t/0000:00:11.0");
    EXPECT_EQ(sw.sec_bus_num, 0x22);
    EXPECT_EQ(decoder.get_bridges(sw).size(), 0);
}

TEST_F(SysfsDecoderTest, InvalidBridgesAreNotVisible) {
    reader.add_device(DevBuilder().make_switch_vep("/t/","0000:00:11.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/t/","0000:00:11.1", 0x22, true));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.1/","0000:00:00.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.1/","0000:00:00.1"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.1/","0001:00:11.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.1/","0000:01:11.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.1/","0000:01:11.1"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.1/","0000:01:11.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.1/","0001:22:11.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.1/","0000:22:11.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.1/","0000:22:11.1"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.1/","0000:22:00.0"));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    EXPECT_EQ(decoder.get_bridges(switches[0]).size(), 0);
}

TEST_F(SysfsDecoderTest, BridgeDataIsCorrectlyRead) {
    reader.add_device(DevBuilder().make_switch_vep("/t/","0000:00:11.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/t/","0000:00:11.0", 0x22, true));
    reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/","0000:22:11.0", 0x44, false));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    auto bridges = decoder.get_bridges(switches[0]);
    EXPECT_EQ(bridges.size(), 1);
    auto bridge = bridges[0];
    EXPECT_EQ(SysfsId(0x0000, 0x22, 0x11, 0x00), bridge.id);
    EXPECT_STREQ(bridge.path.c_str(), "/t/0000:00:11.0/0000:22:11.0");
    EXPECT_EQ(bridge.sec_bus_num, 0x44);
    EXPECT_EQ(decoder.get_devices(bridge).size(), 0);
}


TEST_F(SysfsDecoderTest, BridgesPresentInvalidDevicesAreNotVisible) {
    reader.add_device(DevBuilder().make_switch_vep("/t/","0000:00:11.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/t/","0000:00:11.0", 0x22, true));
    reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/","0000:22:11.0", 0x44, false));
    reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/","0000:22:12.0", 0x55, false));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/","0000:00:00.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/","0000:00:00.1"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/","0001:00:11.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/","0000:00:11.1"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/","0000:01:11.1"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/","0000:01:11.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/0000:44:11.0/","0001:22:11.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/0000:44:11.0/","0000:22:11.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/0000:44:11.0/","0000:22:11.1"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/0000:44:11.0/","0000:22:33.1"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/0000:44:11.0/","0000:22:22.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/0000:44:11.0/","0001:33:00.1"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/0000:44:11.0/","0000:33:00.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/0000:44:11.0/","0001:44:00.0"));
    reader.add_device(DevBuilder().make_invalid_t0_dev("/t/0000:00:11.0/0000:44:11.0/","0001:44:01.0"));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    auto bridges = decoder.get_bridges(switches[0]);
    EXPECT_EQ(bridges.size(), 2);
    EXPECT_EQ(decoder.get_devices(bridges[0]).size(), 0);
    EXPECT_EQ(decoder.get_devices(bridges[1]).size(), 0);
}

TEST_F(SysfsDecoderTest, PcieDevicesAndFunctionsAreCorrectlyFound) {
    reader.add_device(DevBuilder().make_switch_vep("/t/","0000:00:11.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/t/","0000:00:11.0", 0x22, true));
    reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/","0000:22:33.0", 0x44, false));
    reader.add_device(DevBuilder().make_drive("/t/0000:00:11.0/0000:22:33.0/","0000:44:55.0"));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    auto bridges = decoder.get_bridges(switches[0]);
    EXPECT_EQ(bridges.size(), 1);
    auto devices = decoder.get_devices(bridges[0]);
    EXPECT_EQ(devices.size(), 1);
    auto functions = decoder.get_functions(devices[0]);
    EXPECT_EQ(functions.size(), 1);
}

TEST_F(SysfsDecoderTest, PcieDeviceBasicDataIsCorrectlyRead) {
    reader.add_device(DevBuilder().make_switch_vep("/t/","0000:00:11.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/t/","0000:00:11.0", 0x22, true));
    reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/","0000:22:33.0", 0x44, false));
    reader.add_device(DevBuilder().make_drive("/t/0000:00:11.0/0000:22:33.0/","0000:44:55.0")
        .set_common_header(0x1234, 0x2345, 0xAC, true));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));

    auto switches = decoder.get_switches();
    auto bridges = decoder.get_bridges(switches[0]);
    auto devices = decoder.get_devices(bridges[0]);
    auto dev = devices[0];

    EXPECT_EQ(dev.id, SysfsId(0x0000, 0x44, 0x55, 0x0));
    EXPECT_EQ(dev.bridge_path, "/t/0000:00:11.0/0000:22:33.0");
    EXPECT_EQ(dev.vendor_id, 0x1234);
    EXPECT_EQ(dev.device_id, 0x2345);
    EXPECT_EQ(dev.is_multi_function_device, true);
    EXPECT_EQ(dev.has_pcie_capabilities, false);
    EXPECT_EQ(dev.has_serial_number, false);
}

TEST_F(SysfsDecoderTest, PcieFunctionBasicDataIsCorrectlyRead) {
    reader.add_device(DevBuilder().make_switch_vep("/t/","0000:00:11.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/t/","0000:00:11.0", 0x22, true));
    reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/","0000:22:33.0", 0x44, false));
    reader.add_device(DevBuilder().set_class(CLASS_STORAGE, SUBCLASS_NVME, PROG_NVME)
        .set_nums("/t/0000:00:11.0/0000:22:33:0/", "0000:44:55.6", true)
        .set_common_header(0x1234, 0x2345, 0xAC, false)
        .set_type0_header(0x3456, 0x4567));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    auto bridges = decoder.get_bridges(switches[0]);
    auto devices = decoder.get_devices(bridges[0]);
    auto functions = decoder.get_functions(devices[0]);
    auto fn = functions[0];

    EXPECT_EQ(fn.drives.size(), 0);
    EXPECT_EQ(fn.id, SysfsId(0x0000, 0x44, 0x55, 0x06));
    EXPECT_EQ(fn.path, "/t/0000:00:11.0/0000:22:33:0/0000:44:55.6");
    EXPECT_EQ(fn.is_virtual, true);
    EXPECT_EQ(fn.device_class, CLASS_STORAGE);
    EXPECT_EQ(fn.device_subclass, SUBCLASS_NVME);
    EXPECT_EQ(fn.device_prog_if, PROG_NVME);
    EXPECT_EQ(fn.pci_vendor_id, 0x1234);
    EXPECT_EQ(fn.pci_device_id, 0x2345);
    EXPECT_EQ(fn.pci_revision_id, 0xAC);
    EXPECT_EQ(fn.pci_subsystem_vendor_id, 0x3456);
    EXPECT_EQ(fn.pci_subsystem_id, 0x4567);
}

TEST_F(SysfsDecoderTest, MultifunctionalDevicesAreCorrectlyFound) {
    reader.add_device(DevBuilder().make_switch_vep("/t/","0000:00:11.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/t/","0000:00:11.0", 0x22, true));
    reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/","0000:22:33.0", 0x44, false));
    reader.add_device(DevBuilder().make_drive("/t/0000:00:11.0/0000:22:33.0/", "0000:44:55.0")
        .set_common_header(0x0000, 0x0000, 0x00, true));
    reader.add_device(DevBuilder().make_drive("/t/0000:00:11.0/0000:22:33.0/", "0000:44:55.1")
        .set_common_header(0x0000, 0x0000, 0x01, true));
    reader.add_device(DevBuilder().make_drive("/t/0000:00:11.0/0000:22:33.0/", "0000:44:55.2")
        .set_common_header(0x0000, 0x0000, 0x02, true));

    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    auto bridges = decoder.get_bridges(switches[0]);
    EXPECT_EQ(bridges.size(), 1);
    auto devices = decoder.get_devices(bridges[0]);
    EXPECT_EQ(devices.size(), 1);
    auto functions = decoder.get_functions(devices[0]);
    EXPECT_EQ(functions.size(), 3);
}

TEST_F(SysfsDecoderTest, PcieDrivesAreCorrectlyFound) {
    reader.add_device(DevBuilder().make_switch_vep("/t/","0000:00:11.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/t/","0000:00:11.0", 0x22, true));
    reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/","0000:22:33.0", 0x44, false));
    reader.add_device(DevBuilder().make_drive("/t/0000:00:11.0/0000:22:33.0/", "0000:44:55.6")
        .add_drive("drive1", 122.0)
        .add_drive("drive2", 122.0));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    auto bridges = decoder.get_bridges(switches[0]);
    EXPECT_EQ(bridges.size(), 1);
    auto devices = decoder.get_devices(bridges[0]);
    EXPECT_EQ(devices.size(), 1);
    auto functions = decoder.get_functions(devices[0]);
    EXPECT_EQ(functions.size(), 1);
    EXPECT_EQ(functions[0].drives.size(), 2);
}

TEST_F(SysfsDecoderTest, PcieDrivesDataIsCorrectlyRead) {
  reader.add_device(DevBuilder().make_switch_vep("/t/","0000:00:11.1"));
  reader.add_device(DevBuilder().make_switch_bridge("/t/","0000:00:11.0", 0x22, true));
  reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/","0000:22:33.0", 0x44, false));
  reader.add_device(DevBuilder().make_drive("/t/0000:00:11.0/0000:22:33.0/", "0000:44:55.6")
      .add_drive("drive1", 122.0));
  EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
  auto switches = decoder.get_switches();
  auto bridges = decoder.get_bridges(switches[0]);
  auto devices = decoder.get_devices(bridges[0]);
  auto functions = decoder.get_functions(devices[0]);
  EXPECT_DOUBLE_EQ(functions[0].drives[0].size_bytes, 122.0);
  EXPECT_STREQ(functions[0].drives[0].name.c_str(), "drive1");
}

TEST_F(SysfsDecoderTest, InvalidCapabilitiesAreIgnoredTest1) {
    reader.add_device(DevBuilder().make_switch_vep("/t/", "0000:00:11.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/t/", "0000:00:11.0", 0x22, true));
    reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/", "0000:22:33.0", 0x44, false));
    reader.add_device(DevBuilder().make_drive("/t/0000:00:11.0/0000:22:33.0/", "0000:44:55.0")
        .set_type0_caps(0x50));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    auto bridges = decoder.get_bridges(switches[0]);
    EXPECT_EQ(bridges.size(), 1);
    auto devices = decoder.get_devices(bridges[0]);
    EXPECT_EQ(devices.size(), 1);
    auto dev = devices[0];
    EXPECT_EQ(dev.has_pcie_capabilities, false);
}

TEST_F(SysfsDecoderTest, InvalidCapabilitiesAreIgnoredTest2) {
    reader.add_device(DevBuilder().make_switch_vep("/t/", "0000:00:11.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/t/", "0000:00:11.0", 0x22, true));
    reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/", "0000:22:33.0", 0x44, false));
    reader.add_device(DevBuilder().make_drive("/t/0000:00:11.0/0000:22:33.0/", "0000:44:55.0")
        .set_type0_caps(0x50)
        .add_cap_header(0x50, 0x77, 0x60)
        .add_cap_header(0x60, 0x80, 0x00));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    auto bridges = decoder.get_bridges(switches[0]);
    EXPECT_EQ(bridges.size(), 1);
    auto devices = decoder.get_devices(bridges[0]);
    EXPECT_EQ(devices.size(), 1);
    auto dev = devices[0];
    EXPECT_EQ(dev.has_pcie_capabilities, false);
}

TEST_F(SysfsDecoderTest, PcieCapabilityIsCorrectlyRead) {
  reader.add_device(DevBuilder().make_switch_vep("/t/", "0000:00:11.1"));
  reader.add_device(DevBuilder().make_switch_bridge("/t/", "0000:00:11.0", 0x22, true));
  reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/", "0000:22:33.0", 0x44, false));
  reader.add_device(DevBuilder().make_drive("/t/0000:00:11.0/0000:22:33.0/", "0000:44:55.0")
        .set_type0_caps(0x50)
        .add_cap_header(0x50, 0x77, 0x60)
        .add_cap_header(0x60, PCI_EXPRESS_CAPABILITY_ID, 0x70)
        .add_cap_header(0x70, 0x88, 0x00)
        .add_cap_pcie(0x60, 0x12345678, 0xFEDC));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    auto bridges = decoder.get_bridges(switches[0]);
    EXPECT_EQ(bridges.size(), 1);
    auto devices = decoder.get_devices(bridges[0]);
    EXPECT_EQ(devices.size(), 1);
    auto dev = devices[0];
    EXPECT_EQ(dev.has_pcie_capabilities, true);
    EXPECT_EQ(dev.pcie_link_capability_register, 0x12345678);
    EXPECT_EQ(dev.pcie_link_status_register, 0xFEDC);
}

TEST_F(SysfsDecoderTest, InvalidExtendedCapabilitiesAreIgnoredTest1) {
    reader.add_device(DevBuilder().make_switch_vep("/t/", "0000:00:11.1"));
    reader.add_device(DevBuilder().make_switch_bridge("/t/", "0000:00:11.0", 0x22, true));
    reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/", "0000:22:33.0", 0x44, false));
    reader.add_device(DevBuilder().make_drive("/t/0000:00:11.0/0000:22:33.0/", "0000:44:55.0")
        .set_type0_caps(0x50)
        .add_cap_header(0x50, PCI_EXPRESS_CAPABILITY_ID, 0x00)
        .add_ext_cap_header(PCI_EXTENDED_CAPABILITY_OFFSET, 0xCC, 0x0000));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    auto bridges = decoder.get_bridges(switches[0]);
    EXPECT_EQ(bridges.size(), 1);
    auto devices = decoder.get_devices(bridges[0]);
    EXPECT_EQ(devices.size(), 1);
    auto dev = devices[0];
    EXPECT_EQ(dev.has_pcie_capabilities, true);
    EXPECT_EQ(dev.has_serial_number, false);
}

TEST_F(SysfsDecoderTest, InvalidExtendedCapabilitiesAreIgnoredTest2) {
  reader.add_device(DevBuilder().make_switch_vep("/t/", "0000:00:11.1"));
  reader.add_device(DevBuilder().make_switch_bridge("/t/", "0000:00:11.0", 0x22, true));
  reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/", "0000:22:33.0", 0x44, false));
  reader.add_device(DevBuilder().make_drive("/t/0000:00:11.0/0000:22:33.0/", "0000:44:55.0")
        .set_type0_caps(0x50)
        .add_cap_header(0x50, PCI_EXPRESS_CAPABILITY_ID, 0x00)
        .add_ext_cap_header(PCI_EXTENDED_CAPABILITY_OFFSET, 0xCC, 0x0120)
        .add_ext_cap_header(0x0120, 0xDD, 0x0130)
        .add_ext_cap_header(0x0130, 0xEE, 0x0000));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    auto bridges = decoder.get_bridges(switches[0]);
    EXPECT_EQ(bridges.size(), 1);
    auto devices = decoder.get_devices(bridges[0]);
    EXPECT_EQ(devices.size(), 1);
    auto dev = devices[0];
    EXPECT_EQ(dev.has_pcie_capabilities, true);
    EXPECT_EQ(dev.has_serial_number, false);
}

TEST_F(SysfsDecoderTest, ExtendedSerialNumberCapabilitiesAreCorrectlyRead) {
  reader.add_device(DevBuilder().make_switch_vep("/t/", "0000:00:11.1"));
  reader.add_device(DevBuilder().make_switch_bridge("/t/", "0000:00:11.0", 0x22, true));
  reader.add_device(DevBuilder().make_switch_bridge("/t/0000:00:11.0/", "0000:22:33.0", 0x44, false));
  reader.add_device(DevBuilder().make_drive("/t/0000:00:11.0/0000:22:33.0/", "0000:44:55.0")
        .set_type0_caps(0x50)
        .add_cap_header(0x50, PCI_EXPRESS_CAPABILITY_ID, 0x00)
        .add_ext_cap_header(PCI_EXTENDED_CAPABILITY_OFFSET, 0xCC, 0x0120)
        .add_ext_cap_header(0x0120, PCI_SERIAL_NUMBER_EXTENDED_CAPABILITY_ID, 0x0160)
        .add_ext_cap_header(0x0160, 0xEE, 0x0000)
        .add_ext_cap_sn(0x0120, 0x76543210, 0xFEDCBA98));
    EXPECT_NO_THROW(decoder.read_sysfs_data(reader));
    auto switches = decoder.get_switches();
    EXPECT_EQ(switches.size(), 1);
    auto bridges = decoder.get_bridges(switches[0]);
    EXPECT_EQ(bridges.size(), 1);
    auto devices = decoder.get_devices(bridges[0]);
    EXPECT_EQ(devices.size(), 1);
    auto dev = devices[0];
    EXPECT_EQ(dev.has_pcie_capabilities, true);
    EXPECT_EQ(dev.has_serial_number, true);
    EXPECT_EQ(dev.serial_number, 0xFEDCBA9876543210);
}
