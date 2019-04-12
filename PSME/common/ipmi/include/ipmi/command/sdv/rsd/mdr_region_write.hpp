/*!
 * @brief MdrRegionWrite command interface.
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
 * @file mdr_region_write.hpp
 */


#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"

#include <iterator>
#include <algorithm>

namespace ipmi {
namespace command {
namespace sdv {
namespace rsd {

namespace request {

class MdrRegionWrite : public Request {
public:
    /* The number of bytes in IPMI message is strongly limited. For LAN 140 is allowed.
     * Headers and request should be substracted. It depends on the BMC version, it was tested that 160 works as well).
     */
    static const constexpr std::uint8_t MAX_DATA_COUNT = 120;

    static constexpr const uint8_t HANDLE_OFFSET = 1;
    static constexpr const uint8_t DATA_REGION_ID_OFFSET = 2;
    static constexpr const uint8_t DATA_LENGTH_OFFSET = 3;
    static constexpr const uint8_t WHERE_TO_WRITE_OFFSET = 4;
    static constexpr const uint8_t WHAT_TO_WRITE_OFFSET = 6;


    /*! Default constructor. */
    MdrRegionWrite();


    /*! Copy constructor. */
    MdrRegionWrite(const MdrRegionWrite&) = default;


    /*! Move constructor */
    MdrRegionWrite(MdrRegionWrite&&) = default;


    /*! Assignment operator */
    MdrRegionWrite& operator=(const MdrRegionWrite&) = default;


    /*! Rvalue assignment operator */
    MdrRegionWrite& operator=(MdrRegionWrite&&) = default;


    /*! Destructor */
    virtual ~MdrRegionWrite();

    /*!
     * @brief Sets Data Region ID
     * @param[in] data_region_id Data Region ID
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

    /*!
     * @brief Gets Data length
     * @return Data length to be written in bytes
     */
    std::uint8_t get_data_length() const {
        return m_data_length;
    }

    /*!
     * @brief Sets Data Length
     * @param[in] data_length Data length to be written
     */
    void set_data_length(std::uint8_t data_length) {
        m_data_length = data_length;
    }

    /*!
     * @brief Gets Data offset
     * @return offset where data should be written to
     */
    std::uint16_t get_data_offset() const {
        return m_data_offset;
    }

    /*!
     * @brief Sets data offset
     * @param[in] offset offset where data should be written to
     */
    void set_data_offset(std::uint16_t offset) {
        m_data_offset = offset;
    }

    /*!
     * @brief Gets session lock handle
     * @return session lock handle
     */
    std::uint8_t get_session_lock_handle() const {
        return m_session_lock_handle;
    }

    /*!
     * @brief Sets session lock handle
     * @param[in] handle handle to the session
     */
    void set_session_lock_handle(std::uint8_t handle) {
        m_session_lock_handle = handle;
    }

    virtual const char* get_command_name() const override {
        return "MdrRegionWrite";
    }

    /*!
     * @brief Append data to be sent.
     *
     * @param first Input iterator to the first element to be copied.
     * @param last Input iterator to the first element after the last element to be copied.
     */
    template<typename Iter>
    void append_data(const Iter& first, const Iter& last) {
        std::copy(first, last, std::back_inserter(m_data));
    }

private:
    DataRegionId m_data_region_id{};
    std::uint8_t m_data_length{};
    std::uint8_t m_session_lock_handle{};
    std::uint16_t m_data_offset{};
    IpmiInterface::ByteBuffer m_data{};

    void pack(IpmiInterface::ByteBuffer& data) const override;

};
}

namespace response {
class MdrRegionWrite : public Response {
public:
    static constexpr const uint8_t RESPONSE_SIZE = 9;
    static constexpr const uint8_t DATA_REGION_ID_OFFSET = 2;
    static constexpr const uint8_t DATA_VALIDATION_OFFSET = 3;
    static constexpr const uint8_t LOCK_STATUS_OFFSET = 4;
    static constexpr const uint8_t MAX_REGION_LENGTH_OFFSET = 5;
    static constexpr const uint8_t SIZE_OF_REGION_USED_OFFSET = 7;

    /*! Default constructor. */
    MdrRegionWrite();


    /*! Copy constructor. */
    MdrRegionWrite(const MdrRegionWrite&) = default;


    /*! Move constructor */
    MdrRegionWrite(MdrRegionWrite&&) = default;


    /*! Assignment operator */
    MdrRegionWrite& operator=(const MdrRegionWrite&) = default;


    /*! Rvalue assignment operator */
    MdrRegionWrite& operator=(MdrRegionWrite&&) = default;


    /*! Destructor */
    virtual ~MdrRegionWrite();

    /*!
     * @brief Sets Data Region ID
     * @param[in] data_region_id Data Region ID
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

    /*!
     * @brief Sets valid data flag
     * @param[in] data_valid informs whether data is valid or not
     */
    void set_valid_data(DataValidation data_valid) {
        m_data_validation = data_valid;
    }

    /*!
     * @brief Gets valid data flag
     * @return enum informing whether data is valid or not
     */
    DataValidation get_valid_data() const {
        return m_data_validation;
    }

    /*!
     * @brief Sets max region length
     * @param[in] max_length Max region length
     */
    void set_max_region_length(std::uint16_t max_length) {
        m_max_region_length = max_length;
    }

    /*!
    * @brief Gets max region length
    * @return Max region length
    */
    std::uint16_t get_max_region_length() const {
        return m_max_region_length;
    }

    /*!
     * @brief Sets size of region used
     * @param[in] size size of region used
     */
    void set_size_of_region_used(std::uint16_t size) {
        m_size_of_region_used = size;
    }

    /*!
     * @brief Gets size of region used
     * @return size of region used
     */
    std::uint16_t get_size_of_region_used() const {
        return m_size_of_region_used;
    }

    virtual const char* get_command_name() const override {
        return "MdrRegionWrite";
    }

protected:
    /*!
     * @brief Throws error derived from ResponseError for command specific non-zero completion codes.
     * @param completion_code completion code (non-zero)
     * @throws ResponseError type exception.
     */
    void throw_error_on_completion_code(CompletionCode completion_code) const override;

private:
    DataRegionId m_data_region_id{};
    DataValidation m_data_validation{};
    LockStatus m_lock_status{};
    std::uint16_t m_max_region_length{};
    std::uint16_t m_size_of_region_used{};

    bool m_is_rackscale_extension_byte_present{false};
    void unpack(const IpmiInterface::ByteBuffer& data) override;
};



}
}
}
}
}


