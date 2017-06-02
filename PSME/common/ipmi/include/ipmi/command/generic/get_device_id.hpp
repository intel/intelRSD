/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file get_device_id.hpp
 *
 * @brief Implementation of GetDeviceId Request/Response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <string>
#include <map>
#include <vector>

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Represents request message for GetDeviceId command.
 */
class GetDeviceId : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    GetDeviceId();

    /*! Copy constructor */
    GetDeviceId(const GetDeviceId&) = default;

    /*! Assignment operator */
    GetDeviceId& operator=(const GetDeviceId&) = default;

    /*!
     * @brief Destructor.
     */
    virtual ~GetDeviceId();

    /*!
     * @brief Packs data from object to output vector
     * @param[out] data vector where data will be packed.
     */
    virtual void pack(std::vector<std::uint8_t>& data) const;

private:

};
}

namespace response {

/*!
 * @brief Represents response message for GetDeviceId command.
 */
class GetDeviceId : public Response {
public:

    /*!
     * @brief Represents module's type.
     */
    enum PRODUCT_ID : std::uint16_t {
        PRODUCT_ID_INTEL_XEON_BDC_R = 0x2041,
        PRODUCT_ID_INTEL_XEON_BDC_A = 0x3036,
        PRODUCT_ID_INTEL_ATOM = 0x3146,
        PRODUCT_ID_INTEL_BDX_DE_BDC_R = 0xF20A,
        PRODUCT_ID_INTEL_LAST,
        PRODUCT_ID_INTEL_UNKNOWN,
    };

    /*!
     * @brief Represents module's type.
     */
    enum MANUFACTURER_ID : std::uint32_t {
        MANUFACTURER_ID_INTEL = 0x000157,
        MANUFACTURER_ID_QUANTA = 0x001C4C,
        MANUFACTURER_ID_LAST,
        MANUFACTURER_ID_UNKNOWN,
    };

    /*!
     * @brief Default constructor.
     */
    GetDeviceId();

    /*! Copy constructor */
    GetDeviceId(const GetDeviceId&) = default;

    /*! Assignment operator */
    GetDeviceId& operator=(const GetDeviceId&) = default;

    /*!
     * @brief Destructor.
     */
    virtual ~GetDeviceId();

    /*!
     * @brief Unpacks data from vector to object.
     * @param data bytes to be copied to object,
     */
    virtual void unpack(const std::vector<std::uint8_t>& data);

    /*!
     * @brief Gets firmware version: major.minor.aux0.aux1.aux2.aux3
     *
     * @return string with firmware version.
     */
     const std::string& get_firmware_version() const {
         return m_firmware_version;
     }

    /*!
     * @brief Gets product ID.
     * @return PRODUCT_ID
     */
    PRODUCT_ID get_product_id() const {
        return m_product_id;
    }

    /*!
     * @brief Gets manufacturer ID.
     * @return MANUFACTURER_ID.
     */
    MANUFACTURER_ID get_manufacturer_id() const {
        return m_manufacturer_id;
    }

    /*!
     * @brief Returns human readable product name.
     * @return string with product name.
     */
    const std::string& get_product_name() const {
        if (m_product_name_mapping.find(m_product_id) == m_product_name_mapping.end()) {
            return m_product_name_mapping.at(PRODUCT_ID_INTEL_UNKNOWN);
        }
        return m_product_name_mapping.at(m_product_id);
    }

    /*!
     * @brief Returns human readable manufacturer name.
     * @return string with manufacturer name.
     */
    const std::string& get_manufacturer_name() const {
        if (m_manufacturer_name_mapping.find(m_manufacturer_id) == m_manufacturer_name_mapping.end()) {
            return m_manufacturer_name_mapping.at(MANUFACTURER_ID_UNKNOWN);
        }
        return m_manufacturer_name_mapping.at(m_manufacturer_id);
    }

private:
    std::map<PRODUCT_ID, std::string> m_product_name_mapping = {
        {PRODUCT_ID_INTEL_XEON_BDC_A, "Intel Xeon"},
        {PRODUCT_ID_INTEL_XEON_BDC_R, "Intel Xeon"},
        {PRODUCT_ID_INTEL_ATOM, "Intel Atom"},
        {PRODUCT_ID_INTEL_UNKNOWN, "Unknown product"}
    };

    std::map<MANUFACTURER_ID, std::string> m_manufacturer_name_mapping = {
        {MANUFACTURER_ID_INTEL, "Intel Corporation"},
        {MANUFACTURER_ID_QUANTA, "Quanta Computers Incorporated"},
        {MANUFACTURER_ID_UNKNOWN, "Unknown manufacturer"}
    };

    std::string m_firmware_version{};
    PRODUCT_ID m_product_id = PRODUCT_ID_INTEL_UNKNOWN;
    MANUFACTURER_ID m_manufacturer_id = MANUFACTURER_ID_UNKNOWN;

    static constexpr std::size_t OFFSET_FIRMWARE_MAJOR_VERSION = 3;
    static constexpr std::size_t OFFSET_FIRMWARE_MINOR_VERSION = 4;
    static constexpr std::size_t OFFSET_MANUFACTURER_ID = 7;
    static constexpr std::size_t OFFSET_PRODUCT_ID = 10;
    static constexpr std::size_t OFFSET_FIRMWARE_AUXILIARY_VERSION = 12;

    static constexpr std::size_t MASK_MAJOR_VERSION_NUMBER = 0x7F;

    static constexpr std::size_t RESPONSE_SIZE = 16;

    PRODUCT_ID extract_product_id(const std::vector<std::uint8_t>& data) const;
    MANUFACTURER_ID extract_manufacturer_id(const std::vector<std::uint8_t>& data) const;

};
}

}
}
}
