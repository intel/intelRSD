/*!
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
 * */

#pragma once

namespace mock {

    // IPMI COMPLETION CODES
    static const constexpr std::uint8_t CC_OK = 0x0;
    static const constexpr std::uint8_t CC_TIMEOUT = 0xc3;

    // GET_BIOS_VERSION
    static constexpr const char FULL_VERSION[] = "A20_21A0";

    // GET_DEVICE_ID
    static constexpr const std::uint8_t FIRMWARE_VERSION_MAJOR = 0x02;
    static constexpr const std::uint8_t FIRMWARE_VERSION_MINOR = 0x08;
    static constexpr const char PRODUCT_ID_INTEL_ATOM[] = "Intel Atom";
    static constexpr const char MANUFACTURER_ID_INTEL[] = "Intel Corporation";

    // GET_PROCESSOR_INFO
    static constexpr const uint16_t CPU_FREQUENCY = 3700;

    // GET_MEMORY_INFO
    static constexpr const char MEMORY_TYPE[] = "DRAM";
    static constexpr const double MEMORY_VOLTAGE = 1.5;
    static constexpr const std::uint32_t MEMORY_SPEED_MHZ = 1600;
    static constexpr const std::uint32_t MEMORY_SIZE_MIB = 1024;

    // GET_SYSTEM_MAC_ADDRESS_HASWELL
    static constexpr const char SYSTEM_MAC_ADDRESS_HASWELL[] = "75:d5:35:e5:d5:35";

    // GET_CABLE_ID
    static constexpr const char PCIE_PORT_CABLE_ID[] = "123ABC987";

    // GET_MDR_REGION_STATUS
    static const constexpr std::uint8_t MDR_VERSION = 0x4;
    static const constexpr std::uint8_t MDR_DATA_UPDATE_COUNT = 0x1;
    static const constexpr std::uint16_t MDR_REGION_SIZE = 146;
    static const constexpr std::uint16_t MDR_REGION_SIZE_USED = 100;
    static const constexpr std::uint8_t MDR_CHECKSUM = 0x86;

}

