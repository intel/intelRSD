/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file get_device_id.cpp
 *
 * @brief Implementation of GetDeviceId Request/Response.
 * */

#include "ipmi/command/generic/get_device_id.hpp"
#include "ipmi/command/generic/enums.hpp"

#include <sstream>
#include <iomanip>

using namespace ipmi;
using namespace ipmi::command::generic;

request::GetDeviceId::GetDeviceId() : Request(generic::NetFn::APP, generic::Cmd::GET_DEVICE_ID) {}
request::GetDeviceId::~GetDeviceId() {}

void request::GetDeviceId::pack(std::vector<std::uint8_t>& data) const {
    (void)data;
}

response::GetDeviceId::GetDeviceId() : Response(generic::NetFn::APP, generic::Cmd::GET_DEVICE_ID, RESPONSE_SIZE) {}
response::GetDeviceId::~GetDeviceId() {}

template<typename T>
static inline unsigned firmware_revision_to_bcd(T data) {
    static constexpr std::uint8_t MASK_HIGH_NIBBLE = 0xF0;
    static constexpr std::uint8_t MASK_LOW_NIBBLE = 0x0F;
    return (10 * ((MASK_HIGH_NIBBLE & unsigned(data)) >> 4)) +
           (MASK_LOW_NIBBLE & unsigned(data));
}

void response::GetDeviceId::unpack(const std::vector<std::uint8_t>& data) {
    std::stringstream stream;
    unsigned major = (MASK_MAJOR_VERSION_NUMBER &
                      data[OFFSET_FIRMWARE_MAJOR_VERSION]);

    const std::uint32_t version[] = {
        firmware_revision_to_bcd(data[OFFSET_FIRMWARE_MINOR_VERSION]),
        data[OFFSET_FIRMWARE_AUXILIARY_VERSION + 0],
        data[OFFSET_FIRMWARE_AUXILIARY_VERSION + 1],
        data[OFFSET_FIRMWARE_AUXILIARY_VERSION + 2],
        data[OFFSET_FIRMWARE_AUXILIARY_VERSION + 3]
    };

    // Saves required major version number without leading zeroes.
    stream << major;

    // For each number. Print it if it's different than 0.
    for (const std::uint32_t& number : version) {
        if (number) {
            stream << "." << std::setw(2) << std::setfill('0') << number;
        }
    }

    m_firmware_version = stream.str();
    m_product_id = extract_product_id(data);
    m_manufacturer_id = extract_manufacturer_id(data);
}

ProductId response::GetDeviceId::extract_product_id(const std::vector<std::uint8_t>& data) const {
    auto id = std::uint16_t((data[OFFSET_PRODUCT_ID + 1] << 8)
                     | (data[OFFSET_PRODUCT_ID + 0] << 0));

    if (id >= PRODUCT_ID_INTEL_LAST) {
        return PRODUCT_ID_INTEL_UNKNOWN;
    }
    return ProductId(id);
}

ManufacturerId response::GetDeviceId::extract_manufacturer_id(const std::vector<std::uint8_t>& data)
const {
    auto id = std::uint32_t((data[OFFSET_MANUFACTURER_ID + 2] << 16) |
                            (data[OFFSET_MANUFACTURER_ID + 1] << 8) |
                            (data[OFFSET_MANUFACTURER_ID + 0] << 0));

    if (id >= MANUFACTURER_ID_LAST) {
        return MANUFACTURER_ID_UNKNOWN;
    }
    return ManufacturerId(id);
}
