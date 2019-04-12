/*!
 * @brief OPAE proxy device reader class declaration
 *
 * @copyright Copyright (c) 2019 Intel Corporation
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
 * @file opae_proxy_device_reader.hpp
 */

#pragma once



#include "opaepp/device_reader.hpp"
#include "opae_proxy_context.hpp"



namespace opaepp {

/*!
 * OpaeProxyDeviceReader declaration
 */
class OpaeProxyDeviceReader : public DeviceReader {

public:
    /*!
     * @brief Default constructor
     */
    OpaeProxyDeviceReader(std::shared_ptr<OpaeProxyContext> opae_proxy) : m_proxy_context(opae_proxy) {}


    /*!
     * @brief Reads all OPAE devices, logs if discovery of any particular device failed and stores mapping of
     * unique uuid from FPGA to FPGA opae fpga token in related proxy context based on enumerated devices
     * @throws runtime_error if it was unable to get tokens for discovery of devices
     * @return reference to DeviceReader object
     */
    DeviceReader& read_devices();


    /*!
     * @brief Reads OPAE device for given BDF, logs if discovery of any particular device failed.
     * @param pcie_device_nums pcie device address
     * @throws runtime_error if it was unable to get tokens for discovery of devices
     * @return reference to DeviceReader object
     */
    DeviceReader& read_devices(const PcieDeviceAddress& pcie_device_nums);


private:
    /*!
     * @brief Default constructor not allowed
     */
    OpaeProxyDeviceReader() = delete;


    /*!
     * @brief Performs discovery of Device object based on given token
     * @param token_ptr pointer to OPAE token
     * @return Device object with properties discovered
     */
    Device discover(fpga_token& token);


    /*!
     * @brief Reads properties and stores it in given device object
     * @param device device object to be filled with properties
     */
    void read_properties(Device& device, fpga_token& token);


    std::shared_ptr<OpaeProxyContext> m_proxy_context{};
};

}
