/*!
 * @brief "serial_oem" IpmiController configuration data.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file ipmitool/serial_connection_data.hpp
 */

#pragma once



#include <ipmi/connection_data.hpp>



namespace ipmi {
namespace manager {
namespace ipmitool {

class SerialConnectionData : public ipmi::ConnectionData {
public:
    static constexpr const char* INTF_TYPE = "serial-oem";


    /*!
     * @brief Build connection data on device path
     * @param device path to the device to open
     */
    SerialConnectionData(const std::string& device);


    SerialConnectionData(const SerialConnectionData&) = default;
    SerialConnectionData(SerialConnectionData&&) = default;

    /*!
     * @brief Default constructor
     */
    SerialConnectionData();


    /*!
     * @brief Default destructor
     */
    virtual ~SerialConnectionData();


    /*!
     * @brief Set device name
     * @param device proper full-path device name (eg. "/dev/ttyCm1IPMI")
     */
    void set_device(const std::string& device);


    /*!
     * @brief Get device name
     * @return full-path device name (eg. "/dev/ttyCm1IPMI")
     */
    const std::string& get_device() const {
        return device;
    }


    /*!
     * @brief Set baud rate
     * @param rate baud rate
     */
    void set_baud_rate(unsigned rate);


    /*!
     * @brief Get baud rate
     * @return baud rate
     */
    unsigned get_baud_rate() const {
        return baud_rate;
    }


    /*!
     * @brief Set number of data bits
     * @param bits number of bits, must be 5..9
     */
    void set_data_bits(unsigned bits);


    /*!
     * @brief Get number of data bits
     * @return Number of data bits
     */
    unsigned get_data_bits() const {
        return data_bits;
    }


    /*!
     * @brief Set parity description
     * @param parity must be "none", "even", or "odd"
     */
    void set_parity(const std::string& parity);


    /*!
     * @brief Get parity description
     * @return Parity description
     */
    const std::string& get_parity() const {
        return parity;
    }


    /*!
     * @brief Set stop length
     * @param bits must be 1, 1.5, or 2
     */
    void set_stop_bits(float bits);


    /*!
     * @brief Get stop length
     * @return Stop length
     */
    float get_stop_bits() const {
        return stop_bits;
    }


    /*!
     * @brief Set flow control
     * @param control must be "none", "rts_cts", "xon_xoff"
     */
    void set_flow_control(const std::string& control);


    /*!
     * @brief Get flow control
     * @return Flow control type
     */
    const std::string& get_flow_control() const {
        return flow_control;
    }


    /*!
     * @brief Get "composite" string to be passed to ipmitool as device path
     *
     * "composite device" is the only option to keep settings grouped.
     * It uses variable to keep concatenated string suitable for ipmitool library.
     * Pointer passed to the library must be valid when ipmi_intf is alive.
     */
    const std::string& get_composite_device() const {
        return composite;
    }


    std::string get_info() const override;

private:
    std::string device{};
    unsigned baud_rate{115200};
    unsigned data_bits{8};
    std::string parity{"none"};
    float stop_bits{1};
    std::string flow_control{"none"};


    void set_composite_device();


    std::string composite{};
};

}
}
}
