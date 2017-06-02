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
 * @file mdr_region_read.hpp
 * @brief Declaration of MdrRegionRead Request/Response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Represents request message for MdrRegionRead command.
 */
class MdrRegionRead : public Request {
public:
    static const constexpr std::uint8_t MAX_DATA_COUNT = 255;

    /*! @brief Default constructor. */
    MdrRegionRead();

    /*! Copy constructor */
    MdrRegionRead(const MdrRegionRead&) = default;

    /*! Move constructor */
    MdrRegionRead(MdrRegionRead&&) = default;

    /*! Assignment operator */
    MdrRegionRead& operator=(const MdrRegionRead&) = default;

    /*! Rvalue assignment operator */
    MdrRegionRead& operator=(MdrRegionRead&&) = default;

    /*! @brief Destructor. */
    virtual ~MdrRegionRead();

    /*!
     * @brief Packs data from object to output vector
     * @param[out] data Vector where data will be packed.
     * */
    virtual void pack(std::vector<std::uint8_t>& data) const;

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
     * @brief Sets data length to read by command
     * @param[in] data_length Data length to read
     */
    void set_data_length(const std::uint8_t data_length) {
        m_data_length = data_length;
    }

    /*!
     * @brief Gets data length
     * @return Data length
     */
    std::uint8_t get_data_length() const {
        return m_data_length;
    }

    /*!
     * @brief Sets offset to read
     * @param[in] offset Offset to read
     */
    void set_offset(const std::uint16_t offset) {
        m_offset = offset;
    }

    /*!
     * @brief Gets offset
     * @return Offset to read
     */
    std::uint16_t get_offset() const {
        return m_offset;
    }

private:
    DataRegionId m_data_region_id{};
    std::uint8_t m_data_length{};
    std::uint16_t m_offset{};
};
}

namespace response {

/*! @brief Represents response message for GetMdrDataRegionStatus command. */
class MdrRegionRead : public Response {
public:

    /*!
     * @brief Constructor.
     * @param[in] data_length Data length to read by command
     * */
    explicit MdrRegionRead(const std::uint16_t data_length);

    /*! Copy constructor */
    MdrRegionRead(const MdrRegionRead&) = default;

    /*! Assignment operator */
    MdrRegionRead& operator=(const MdrRegionRead&) = default;

    /*! Move constructor */
    MdrRegionRead(MdrRegionRead&&) = default;

    /*! Rvalue assignment operator */
    MdrRegionRead& operator=(MdrRegionRead&&) = default;

    /*! @brief Destructor. */
    virtual ~MdrRegionRead();

    /*!
     * @brief Unpacks data from vector to object.
     * @param data bytes to be copied to object,
     */
    virtual void unpack(const std::vector<std::uint8_t>& data);

    /*!
     * @brief Gets read length
     * @return Read length
     */
    std::uint8_t get_length() const {
        return m_length;
    }

    /*!
     * @brief Gets update count
     * @return Update count
     */
    std::uint8_t get_update_count() const {
        return m_update_count;
    }

    /*!
     * @brief Gets data
     * @return MDR region data
     */
    const std::vector<std::uint8_t> get_data() const {
        return m_data;
    }

private:
    static const constexpr std::size_t OFFSET_LENGTH = 1;
    static const constexpr std::size_t OFFSET_UPDATE_COUNT = 2;
    static const constexpr std::size_t OFFSET_DATA = 3;

    std::uint8_t m_length{};
    std::uint8_t m_update_count{};
    std::vector<std::uint8_t> m_data{};
};
}

}
}
}
