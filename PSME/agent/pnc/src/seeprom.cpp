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
 * @file seeprom.cpp
 * @brief Seeprom access implementation
 * */

#include "seeprom.hpp"

using namespace agent::pnc;
using namespace agent::pnc::gas;
using namespace agent::pnc::i2c;

Seeprom::~Seeprom(){}

void Seeprom::read(const I2cAccessInterfacePtr i2c) {

    i2c->read(static_cast<uint8_t>(PM85X6TwiPort::PORT0),
              static_cast<uint16_t>(PM85X6TwiDeviceAddress::SEEPROM),
              0,
              reinterpret_cast<uint8_t*>(&fields),
              gas::PM85X6_SEEPROM_DATA_SIZE_BYTES,
              true);

    log_seeprom();
}

void Seeprom::log_seeprom() {

    std::string serial_number(reinterpret_cast<const char*>(fields.pcie_dev.serial_number),
                              SEEPROM_PCIE_DEV_SERIAL_NUMBER_SIZE);
    std::string product_id(reinterpret_cast<const char*>(fields.manufacturer.product_id),
                           SEEPROM_MANUFACTURER_PRODUCT_ID_SIZE);
    std::string product_rev(reinterpret_cast<const char*>(fields.manufacturer.product_rev),
                            SEEPROM_MANUFACTURER_PRODUCT_REV_SIZE);

    log_debug("pnc-seeprom", "SEEPROM Table Revision: " << fields.table_revision);
    log_debug("pnc-seeprom", "SEEPROM Table Size: " << fields.table_size);
    log_debug("pnc-seeprom", "SEEPROM Manufacturer Vendor ID: "
                                   << std::hex << fields.manufacturer.vendor_id[0]
                                   << fields.manufacturer.vendor_id[1]);
    log_debug("pnc-seeprom", "SEEPROM Manufacturer Product ID: " << product_id);
    log_debug("pnc-seeprom", "SEEPROM Manufacturer Product Rev.: " << product_rev);
    log_debug("pnc-seeprom", "SEEPROM Manufacturer Component Vendor ID: "
                                   << std::hex << fields.manufacturer.component_vendor_id[0]
                                   << fields.manufacturer.component_vendor_id[1]);
    log_debug("pnc-seeprom", "SEEPROM Manufacturer Component ID: "
                                   << std::uint32_t(fields.manufacturer.component_id));
    log_debug("pnc-seeprom", "SEEPROM Manufacturer Component Rev.: "
                                   << std::uint32_t(fields.manufacturer.component_rev));
    log_debug("pnc-seeprom", "SEEPROM Manufacturer Vendor Specific: "
                                   << std::hex << fields.manufacturer.vendor_spec[0]
                                   << fields.manufacturer.vendor_spec[1]);
    log_debug("pnc-seeprom", "SEEPROM PCIe Device Vendor ID: " << std::hex
                                   << std::uint32_t(fields.pcie_dev.vendor_id));
    log_debug("pnc-seeprom", "SEEPROM PCIe Device ID: " << std::hex << fields.pcie_dev.device_id);
    log_debug("pnc-seeprom", "SEEPROM PCIe Device Revision ID: " << std::uint32_t(fields.pcie_dev.rev_id));
    log_debug("pnc-seeprom", "SEEPROM PCIe Device Interface: " << std::uint32_t(fields.pcie_dev.iface));
    log_debug("pnc-seeprom", "SEEPROM PCIe Device Sub Class Code: "
                                   << std::hex << std::uint32_t(fields.pcie_dev.sub_class_code));
    log_debug("pnc-seeprom", "SEEPROM PCIe Device Base Class Code: "
                                   << std::hex << std::uint32_t(fields.pcie_dev.base_class_code));
    log_debug("pnc-seeprom", "SEEPROM PCIe Device Subsystem Vendor ID: "
                                   << std::hex << std::uint32_t(fields.pcie_dev.subsys_vendor_id));
    log_debug("pnc-seeprom", "SEEPROM PCIe Device Subsystem ID: "
                                   << std::hex << std::uint32_t(fields.pcie_dev.subsys_id));
    log_debug("pnc-seeprom", "SEEPROM PCIe Device Serial Number: " << std::hex << serial_number);
    log_debug("pnc-seeprom", "SEEPROM CRC Sum: " << std::hex << fields.crc);
}
