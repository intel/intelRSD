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
 * @file seeprom.hpp
 * @brief Seeprom access implementation
 * */

#pragma once

#include "i2c/i2c_access_interface.hpp"
#include "gas/utils.hpp"

#include "agent-framework/module/enum/enum_builder.hpp"

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {

/*! Seeprom class */
class Seeprom {
public:
    /*! PCIe Device serial number size */
    static constexpr uint8_t SEEPROM_PCIE_DEV_SERIAL_NUMBER_SIZE = 8;
    /*! PCIe Device manufacturer product id size */
    static constexpr uint8_t SEEPROM_MANUFACTURER_PRODUCT_ID_SIZE = 16;
    /*! PCIe Device manufacturer product revision size */
    static constexpr uint8_t SEEPROM_MANUFACTURER_PRODUCT_REV_SIZE = 4;

    /*! Default constructor */
    Seeprom() {}

    /*! Copy constructor */
    Seeprom(const Seeprom&) = default;

    /*! Assignment operator */
    Seeprom& operator=(const Seeprom&) = default;

#pragma pack(push, 1)
    /*! Struct representing manufacturer info data */
    struct manufacturer_info_t {
        std::uint32_t vendor_id[2];
        std::uint8_t product_id[SEEPROM_MANUFACTURER_PRODUCT_ID_SIZE];
        std::uint8_t product_rev[SEEPROM_MANUFACTURER_PRODUCT_REV_SIZE];
        std::uint32_t component_vendor_id[2];
        std::uint16_t component_id;
        std::uint8_t  component_rev;
        std::uint32_t vendor_spec[2];
    };

    /*! Struct representing pcie dev data */
    struct pcie_dev_t {
        std::uint16_t vendor_id;
        std::uint16_t device_id;
        std::uint8_t rev_id;
        std::uint8_t iface;
        std::uint8_t sub_class_code;
        std::uint8_t base_class_code;
        std::uint16_t subsys_vendor_id;
        std::uint16_t subsys_id;
        std::uint8_t serial_number[SEEPROM_PCIE_DEV_SERIAL_NUMBER_SIZE];
    };

    /*! Seeprom memory map */
    struct {
        std::uint32_t table_revision;
        std::uint32_t table_size;
        manufacturer_info_t manufacturer;
        pcie_dev_t pcie_dev;
        std::uint32_t crc;
    } fields{};
#pragma pack(pop)

    /*!
     * @brief Reads Seeprom memory using GAS.
     * @param[in] i2c Interface used as a source of data
     */
    void read(const i2c::I2cAccessInterfacePtr i2c);

    ~Seeprom();

private:
    void log_seeprom();
};

}
}
