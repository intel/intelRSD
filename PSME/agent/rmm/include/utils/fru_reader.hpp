/*!
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
 * @file rmm/utils/fru_reader.hpp
 */

#pragma once

#include "ipmi/ipmi_controller.hpp"

namespace agent {
namespace rmm {
namespace utils {

class FruReader final {
public:

    FruReader() = default;

    FruReader(const FruReader&) = default;
    FruReader& operator=(const FruReader&) = default;
    FruReader(FruReader&&) = default;
    FruReader& operator=(FruReader&&) = default;

    ~FruReader() = default;

    /*!
     * @brief Reads fru info via ipmi
     * @param ipmi Ipmi controller used for ipmi calls
     * @param bridge_info Bridge data used for ipmi calls
     * @param device_id Id of the queried device
     * @return True if successful
     */
    bool read_and_parse(std::shared_ptr<ipmi::IpmiController> ipmi, const ipmi::BridgeInfo& bridge_info, uint8_t device_id);

    /*!
     * @brief Gets stored serial number
     * @return Stored serial number
     */
    const std::string& get_serial_number() const {
        return m_serial_number;
    }

    /*!
     * @brief Gets stored part number
     * @return Stored part number
     */
    const std::string& get_part_number() const {
        return m_part_number;
    }

    /*!
     * @brief Gets stored model
     * @return Stored model
     */
    const std::string& get_model() const {
        return m_model;
    }

    /*!
     * @brief Gets stored manufacturer
     * @return Stored manufacturer
     */
    const std::string& get_manufacturer() const {
        return m_manufacturer;
    }

private:

    /*! Gets fru info size based on the ipmi responses */
    uint16_t get_fru_size(std::shared_ptr<ipmi::IpmiController>, const ipmi::BridgeInfo&, const uint8_t) const;

    /*! Reads fru data and stores it in m_data variable */
    void read_fru_data(std::shared_ptr<ipmi::IpmiController>, const ipmi::BridgeInfo&, const uint8_t, const uint16_t);

    /*! Parses fru data and fills internal data */
    void parse_fru_data();

    /*! Logs read data */
    void log_fru();

    ipmi::IpmiInterface::ByteBuffer m_data{};

    std::string m_serial_number{};
    std::string m_part_number{};
    std::string m_model{};
    std::string m_manufacturer{};

};

}
}
}
