/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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

#ifndef AGENT_COMPUTE_TESTS_MOCK_CONST_HPP
#define	AGENT_COMPUTE_TESTS_MOCK_CONST_HPP

namespace mock_const{

    // GET_BIOS_VERSION
    static constexpr const char FULL_VERSION[] = "A20F21_A0";

    // GET_DEVICE_ID
    static constexpr const char FIRMWARE_VERSION[] = "2.58";
    static constexpr const char PRODUCT_ID_INTEL_ATOM[] = "Intel Atom";
    static constexpr const char MANUFACTURER_ID_INTEL[] = "Intel Corporation";

    // GET_PROCESSOR_INFO
    static constexpr const uint16_t CPU_FREQUENCY = 3700;

    // GET_DIMM_INFO
    static constexpr const char DIMM_TYPE[] = "DRAM";
    static constexpr const double DIMM_VOLTAGE = 1.3;
    static constexpr const uint32_t DIMM_SPEED_MHZ = 1600;
    static constexpr const uint32_t DIMM_SIZE_MB = 1024;

    // GET_SYSTEM_MAC_ADDRESS_HASWELL
    static constexpr const char SYSTEM_MAC_ADDRESS_HASWELL[] = "75:d5:35:e5:d5:35";

}

#endif	/* AGENT_COMPUTE_TESTS_MOCK_CONST_HPP */
