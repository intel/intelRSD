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
 *
 *
 * @file pci_device.hpp
 * @brief PCI device
 * */
#pragma once


#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! PciDevice */
class PciDevice {
public:

    explicit PciDevice();

    PciDevice(const std::string& vendor_id,
              const std::string& device_id):
              m_vendor_id{vendor_id}, m_device_id{device_id}
              {}

    ~PciDevice();

    /*! Enable copy */
    PciDevice(const PciDevice&) = default;
    PciDevice& operator=(const PciDevice&) = default;
    PciDevice(PciDevice&&) = default;
    PciDevice& operator=(PciDevice&&) = default;

    /*!
     * @brief Set vendor id
     *
     * @param vendor_id String with 4 digit hex number
     * */
    void set_vendor_id(const std::string& vendor_id) {
        m_vendor_id = vendor_id;
    }

    /*!
     * @brief Get vendor id
     *
     * @return vendor id
     * */
    const std::string get_vendor_id() const {
        return m_vendor_id;
    }

    /*!
     * @brief Set device id
     *
     * @param device_id String with 4 digit hex number
     * */
    void set_device_id(const std::string& device_id) {
        m_device_id = device_id;
    }

    /*!
     * @brief Get device id
     *
     * @return device id
     * */
    const std::string get_device_id() const {
        return m_device_id;
    }

    /*!
     * @brief Convert PciDevice to JSON object
     *
     * @return JSON representation of PciDevice object
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class PciDevice from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed PciDevice object
     */
    static PciDevice from_json(const json::Json& json);

private:
    std::string m_vendor_id{};
    std::string m_device_id{};

};

}
}
}
