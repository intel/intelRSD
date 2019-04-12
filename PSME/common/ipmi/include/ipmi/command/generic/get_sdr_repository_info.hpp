/*!
 * @brief Declaration of Get SDR Repository Info Request/Response.
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
 * @file command/generic/get_sdr_repository_info.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Request message for Get SDR Repository Info command.
 */
class GetSdrRepositoryInfo : public Request {
public:
    /*!
     * @brief Default constructor.
     */
    GetSdrRepositoryInfo();

    /*! Copy constructor. */
    GetSdrRepositoryInfo(const GetSdrRepositoryInfo&) = default;

    /*! Assignment operator */
    GetSdrRepositoryInfo& operator=(const GetSdrRepositoryInfo&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetSdrRepositoryInfo();

    const char* get_command_name() const override {
        return "GetSdrRepositoryInfo";
    }

private:
    void pack(IpmiInterface::ByteBuffer&) const override {
        /* no request data */
    }
};

}

namespace response {

/*!
 * @brief Response message for Get SDR Repository Info command.
 */
class GetSdrRepositoryInfo : public Response {
public:
    static constexpr const std::uint8_t IPMI2_SDR_VERSION = 0x51;
    static constexpr const std::uint16_t AVAILABLE_64KB_MINUS_2 = 0xFFFE;
    static constexpr const std::uint16_t UNSPECIFIED_FREE_SPACE = 0xFFFF;

    /*! @brief SDR supported update modes */
    enum class UpdateMode : std::uint8_t {
        UNSPECIFIED = 0,
        NON_MODAL,
        MODAL,
        BOTH
    };

    /*!
     * @brief Default constructor.
     */
    GetSdrRepositoryInfo();

    /*! Copy constructor. */
    GetSdrRepositoryInfo(const GetSdrRepositoryInfo&) = default;

    /*! Assignment operator */
    GetSdrRepositoryInfo& operator=(const GetSdrRepositoryInfo&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetSdrRepositoryInfo();

    const char* get_command_name() const override {
        return "GetSdrRepositoryInfo";
    }

    /*!
     * @brief Gets number of records.
     * @return Number of records.
     */
    std::uint16_t get_record_count() const {
        return m_record_count;
    }

    /*!
     * @brief Gets SDR version number.
     * @return SDR version number.
     */
    std::uint8_t get_sdr_version() const {
        return m_sdr_version;
    }

    /*!
     * @brief Gets free records.
     * @return Free space in bytes.
     */
    std::uint16_t get_free_space() const {
        return m_free_space_in_bytes;
    }

    /*!
     * @brief Gets most recent addition timestamp.
     * @return Most recent addition timestamp.
     */
    std::uint32_t get_last_addition_timestamp() const {
        return m_last_addition_timestamp;
    }

    /*!
     * @brief Gets most recent delete or clear timestamp.
     * @return Most recent delete or clear timestamp.
     */
    std::uint32_t get_last_delete_timestamp() const {
        return m_last_delete_timestamp;
    }

    /*!
     * @brief Indicates if SDR could not be written due to lack of space
     * in the SDR Repository.
     * @return True if overflow occurred, false otherwise.
     */
    bool is_overflow_flag_set() const {
        return m_supported_operations & OVERFLOW_MASK;
    }

    /*!
     * @brief Indicates if Delete SDR command is supported
     * @return True if Delete SDR command is supported, false otherwise.
     */
    bool is_delete_sdr_supported() const {
        return m_supported_operations & DELETE_SDR_MASK;
    }

    /*!
     * @brief Indicates if Partial Add SDR command is supported
     * @return True if Partial Add SDR command is supported, false otherwise.
     */
    bool is_partial_add_supported() const {
        return m_supported_operations & PARTIAL_ADD_MASK;
    }

    /*!
     * @brief Indicates if Reserve SDR command is supported
     * @return True if Reserve SDR command is supported, false otherwise.
     */
    bool is_reserve_sdr_supported() const {
        return m_supported_operations & RESERVE_SDR_MASK;
    }

    /*!
     * @brief Indicates if Get SDR Allocation command is supported
     * @return True if Get SDR Allocation command is supported, false otherwise.
     */
    bool is_get_sdr_allocation_supported() const {
        return m_supported_operations & GET_SDR_ALLOCATION_MASK;
    }

    /*!
     * @brief Gets supported SDR update mode
     * @return SDR supported UpdateMode
     */
    UpdateMode get_supported_update_mode() const {
        return static_cast<UpdateMode>((m_supported_operations & UPDATE_TYPE_MASK) >> 5);
    }

private:
    static constexpr std::size_t RESPONSE_SIZE = 15;
    static constexpr std::uint8_t OVERFLOW_MASK = 1 << 7;
    static constexpr std::uint8_t UPDATE_TYPE_MASK = 0x60;
    static constexpr std::uint8_t DELETE_SDR_MASK = 1 << 3;
    static constexpr std::uint8_t PARTIAL_ADD_MASK = 1 << 2;
    static constexpr std::uint8_t RESERVE_SDR_MASK = 1 << 1;
    static constexpr std::uint8_t GET_SDR_ALLOCATION_MASK = 1 << 0;

    std::uint8_t m_sdr_version{};
    std::uint16_t m_record_count{};
    std::uint16_t m_free_space_in_bytes{};
    std::uint32_t m_last_addition_timestamp{};
    std::uint32_t m_last_delete_timestamp{};
    std::uint8_t m_supported_operations{};

    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

}

}
}
}

