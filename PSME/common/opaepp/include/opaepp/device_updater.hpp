/*!
 * @brief OPAE device updater interface. Enables programming AFUs.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
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
 * @file device_updater.hpp
 */

#pragma once

namespace opaepp {

/*!
 * @brief Class for updating AFUs in FPGAs
 */
class DeviceUpdater {

public:

    /*!
     * @brief Default constructor
     */
    DeviceUpdater() = default;


    /*!
     * @brief Default destructor
     */
    virtual ~DeviceUpdater() = default;


    /*!
     * @brief Method for updating the bitstream on particular FPGA
     * @param bus PCIe bus on which the FPGA is represented in the OS
     * @param device PCIe device as which the FPGA is represented in the OS
     * @param function PCIe bus function as which the FPGA is represented in the OS
     * @param bitstream pointer to the bistream data
     * @param bitstream_size size of the bitstream
     * @param slot index of the slot of the FPGA, default 0, since all supported FPGAs have only one slot
     */
    virtual void update(uint8_t bus, uint8_t device, uint8_t function,
                        const uint8_t* bitstream, size_t bitstream_size,
                        uint32_t slot = 0) = 0;
};

}
