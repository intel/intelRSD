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
 * @file usb_device.hpp
 * @brief USB device
 * */
#pragma once

#include "json-wrapper/json-wrapper.hpp"

#include <string>

namespace agent_framework {
namespace model {
namespace attribute {

/*! UsbDevice */
class UsbDevice {
public:

    explicit UsbDevice();

    UsbDevice(const std::string& vendor_id,
              const std::string& device_id):
              m_vendor_id{vendor_id}, m_device_id{device_id}
              {}

    ~UsbDevice();

    /*! Enable copy */
    UsbDevice(const UsbDevice&) = default;
    UsbDevice& operator=(const UsbDevice&) = default;
    UsbDevice(UsbDevice&&) = default;
    UsbDevice& operator=(UsbDevice&&) = default;

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
     * @brief Convert UsbDevice to JSON object
     *
     * @return JSON representation of UsbDevice object
     * */
    json::Json to_json() const;

    /*!
     * @brief construct an object of class UsbDevice from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed UsbDevice object
     */
    static UsbDevice from_json(const json::Json& json);

private:
    std::string m_vendor_id{};
    std::string m_device_id{};

};

}
}
}
