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
 * @file get_mdr_data_region_status.hpp
 * @brief Implementation of GetMdrDataRegionStatus Request/Response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*! @brief Represents request message for GetMdrDataRegionStatus command. */
class GetMdrDataRegionStatus : public Request {
public:

    /*! @brief Default constructor. */
    GetMdrDataRegionStatus();

    /*! @brief Copy constructor */
    GetMdrDataRegionStatus(const GetMdrDataRegionStatus&) = default;

    /*! @brief Move constructor */
    GetMdrDataRegionStatus(GetMdrDataRegionStatus&&) = default;

    /*! @brief Assignment operator */
    GetMdrDataRegionStatus& operator=(const GetMdrDataRegionStatus&) = default;

    /*! @brief Rvalue assignment operator */
    GetMdrDataRegionStatus& operator=(GetMdrDataRegionStatus&&) = default;

    /*! @brief Destructor. */
    virtual ~GetMdrDataRegionStatus();

    /*!
     * @brief Packs data from object to output vector
     * @param[out] data vector where data will be packed.
     */
    virtual void pack(std::vector<std::uint8_t>& data) const;

    /*!
     * @brief Sets Data Region ID
     * @param data_region_id Data Region ID
     */
    void set_data_region_id(const DataRegionId data_region_id) {
        m_data_region_id = data_region_id;
    }

    /*!
     * @brief Gets Data Region ID
     * @return Data Region ID
     */
    DataRegionId get_data_region_id() const {
        return m_data_region_id;
    }

private:
    DataRegionId m_data_region_id{};
};
}

namespace response {

/*! @brief Represents response message for GetMdrDataRegionStatus command. */
class GetMdrDataRegionStatus : public Response {
public:
    enum DataValidation : std::uint8_t {
        INVALID = 0x0,
        VALID = 0x1
    };

    enum LockStatus : std::uint8_t {
        UNLOCKED = 0x0,
        STRICT_LOCK = 0x1,
        PREEMPTABLE_LOCK = 0x2
    };

    /*! @brief Default constructor. */
    GetMdrDataRegionStatus();

    /*! @brief Copy constructor */
    GetMdrDataRegionStatus(const GetMdrDataRegionStatus&) = default;

    /*! @brief Assignment operator */
    GetMdrDataRegionStatus& operator=(const GetMdrDataRegionStatus&) = default;

    /*! @brief Move constructor */
    GetMdrDataRegionStatus(GetMdrDataRegionStatus&&) = default;

    /*! @brief Rvalue assignment operator */
    GetMdrDataRegionStatus& operator=(GetMdrDataRegionStatus&&) = default;

    /*! @brief Destructor. */
    virtual ~GetMdrDataRegionStatus();

    /*!
     * @brief Unpacks data from vector to object.
     * @param data bytes to be copied to object,
     */
    virtual void unpack(const std::vector<std::uint8_t>& data);

    /*!
     * @brief Gets MDR Version
     * @return MDR Version
     */
    std::uint8_t get_mdr_version() const {
        return m_mdr_version;
    }

    /*!
     * @brief Gets Data Region ID
     * @return Data Region ID
     */
    DataRegionId get_data_region_id() const {
        return m_data_region_id;
    }

    /*!
     * @brief Gets Data Validation
     * @return Data Validation
     */
    DataValidation get_data_validation() const {
        return m_data_validation;
    }

    /*!
     * @brief Gets Data Update Count
     * @return Data Update Count
     */
    std::uint8_t get_data_update_count() const {
        return m_data_update_count;
    }

    /*!
     * @brief Gets Lock Status
     * @return Lock Status
     */
    LockStatus get_lock_status() const {
        return m_lock_status;
    }

    /*!
     * @brief Gets Region Size
     * @return Region Size
     */
    std::uint16_t get_region_size() const {
        return m_region_size;
    }

    /*!
     * @brief Gets Region Size Used
     * @return Region Size Used
     */
    std::uint16_t get_region_size_used() const {
        return m_region_size_used;
    }

    /*!
     * @brief Gets Region Checksum
     * @return Region Checksum
     */
    std::uint8_t get_region_checksum() const {
        return m_region_checksum;
    }

private:
    static constexpr std::size_t RESPONSE_SIZE = 10;
    static constexpr std::size_t OFFSET_MDR_VERSION = 1;
    static constexpr std::size_t OFFSET_DATA_REGION_ID = 2;
    static constexpr std::size_t OFFSET_DATA_VALIDATION = 3;
    static constexpr std::size_t OFFSET_DATA_UPDATE_COUNT = 4;
    static constexpr std::size_t OFFSET_LOCK_STATUS = 5;
    static constexpr std::size_t OFFSET_REGION_SIZE = 6;
    static constexpr std::size_t OFFSET_REGION_SIZE_USED = 8;
    static constexpr std::size_t OFFSET_REGION_CHECKSUM = 10;

    std::uint8_t m_mdr_version{};
    DataRegionId m_data_region_id{};
    DataValidation m_data_validation{};
    std::uint8_t m_data_update_count{};
    LockStatus m_lock_status{};
    std::uint16_t m_region_size{};
    std::uint16_t m_region_size_used{};
    std::uint8_t m_region_checksum{};
};
}

}
}
}
