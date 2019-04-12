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
 * @file command/generic/get_fru_inventory_area_info.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Request message for GetFruInventoryAreaInfo.
 */
class GetFruInventoryAreaInfo : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    GetFruInventoryAreaInfo();

    /*! Copy constructor. */
    GetFruInventoryAreaInfo(const GetFruInventoryAreaInfo&) = default;

    /*! Assignment operator */
    GetFruInventoryAreaInfo& operator=(const GetFruInventoryAreaInfo&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetFruInventoryAreaInfo();

    const char* get_command_name() const override {
        return "GetFruInventoryAreaInfo";
    }

    /*!
     * @brief Sets device id.
     * @param device_id device id.
     */
    void set_device_id(std::uint8_t device_id) {
        m_device_id = device_id;
    }

private:
    std::uint8_t m_device_id{0};

    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief Response message for GetFruInventoryAreaInfo.
 */
class GetFruInventoryAreaInfo : public Response {
public:

    enum class FruAccessMode : uint8_t {
        BYTE = 0,
        WORD = 1
    };

    /*!
     * @brief Default constructor.
     */
    GetFruInventoryAreaInfo();

    /*! Copy constructor. */
    GetFruInventoryAreaInfo(const GetFruInventoryAreaInfo&) = default;

    /*! Assignment operator */
    GetFruInventoryAreaInfo& operator=(const GetFruInventoryAreaInfo&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetFruInventoryAreaInfo();

    const char* get_command_name() const override {
        return "GetFruInventoryAreaInfo";
    }

    /*!
     * @brief Gets fru area size.
     * @return Fru Inventory Area size
     */
    std::uint16_t get_size() const {
        return m_size;
    }

    /*!
     * @brief Gets fru access mode
     * @return Fru access mode
     */
    FruAccessMode get_access_mode() const {
        return m_access_mode;
    }

private:
    static constexpr std::size_t RESPONSE_SIZE = 4;
    static constexpr std::uint8_t DATA_OFFSET_SIZE_LSB = 1;
    static constexpr std::uint8_t DATA_OFFSET_SIZE_MSB = 2;
    static constexpr std::uint8_t DATA_OFFSET_ACCESS_MODE = 3;
    static constexpr std::uint8_t DATA_MASK_ACCESS_MODE = 1;

    std::uint16_t m_size{};
    FruAccessMode m_access_mode{FruAccessMode::BYTE};

    void unpack(const IpmiInterface::ByteBuffer& data) override;
};
}

}
}
}
