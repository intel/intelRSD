/*!
 * @brief OPAE device reader interface
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file device_reader.hpp
 */

#pragma once



#include "device.hpp"
#include "opae/cxx/core/properties.h"



namespace opaepp {

struct PcieDeviceAddress {

    PcieDeviceAddress() = default;


    PcieDeviceAddress(uint8_t bus_num,
                      uint8_t device_num,
                      uint8_t func_num) : m_bus_num(bus_num),
                                          m_device_num(device_num),
                                          m_func_num(func_num) {}


    PcieDeviceAddress& operator=(const PcieDeviceAddress& pcie_device_address) {
        m_bus_num = pcie_device_address.m_bus_num;
        m_device_num = pcie_device_address.m_device_num;
        m_func_num = pcie_device_address.m_func_num;

        return *this;
    }


    bool operator==(const PcieDeviceAddress& pcie_device_address) const {
        return (m_bus_num == pcie_device_address.m_bus_num &&
                m_device_num == pcie_device_address.m_device_num &&
                m_func_num == pcie_device_address.m_func_num);
    }


    uint8_t m_bus_num = 0;
    uint8_t m_device_num = 0;
    uint8_t m_func_num = 0;
};

class DeviceReader {

public:

    using Devices = std::vector<Device>;
    using Properties = std::vector<opae::fpga::types::properties::ptr_t>;


    /*! Default constructor. */
    DeviceReader() = default;


    /*! Default destructor */
    virtual ~DeviceReader() = default;


protected:

    /*!
     * @brief Get all OPAE devices, logs if discovery of any particular device failed.
     * @throws runtime_error if it was unable to get tokens for discovery of devices
     * @return vector of discovered devices
     */
    virtual Devices get_devices() = 0;


    /*!
     * @brief Get OPAE devices for given BDF, logs if discovery of any particular device failed.
     * @param pcie_device_nums PCIe device address
     * @throws runtime_error if it was unable to get tokens for discovery of devices
     * @return vector of discovered devices
     */
    virtual Devices get_devices(const PcieDeviceAddress& pcie_device_nums) = 0;


    /*!
     * @brief Get OPAE devices for given BDF and object type, logs if discovery of any particular device failed.
     * @param pcie_device_address PCIe device address
     * @param obj_type type of objects which will be read
     * @throws runtime_error if it was unable to get tokens for discovery of devices
     * @return vector of discovered devices
     */
    virtual Devices get_devices(const PcieDeviceAddress& pcie_device_address,
                                fpga_objtype obj_type) = 0;
};

}
