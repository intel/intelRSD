/*!
 * @brief Declaration of MdrRegionRead Request/Response.
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation.
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
 * @file command/sdv/mdr_region_read.hpp
 */

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

    const char* get_command_name() const override {
        return "MdrRegionRead";
    }

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

protected:
    /*! @brief Constructor for inheritance purposes.
     *  @param[in] net_fn Net Function with which the command is suppose to be called
     * */
    MdrRegionRead(NetFn net_fn);

    DataRegionId m_data_region_id{};
    std::uint8_t m_data_length{};
    std::uint16_t m_offset{};

private:
    void pack(IpmiInterface::ByteBuffer& data) const override;
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

    const char* get_command_name() const override {
        return "MdrRegionRead";
    }

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
    const IpmiInterface::ByteBuffer& get_data() const {
        return m_data;
    }

protected:
    /*!
     * @brief Constructor.
     * @param[in] net_fn Net function
     * @param[in] data_length Data length to read by command
     * */
    explicit MdrRegionRead(NetFn net_fn, const std::uint16_t data_length);

    std::uint8_t m_length{};
    std::uint8_t m_update_count{};
    IpmiInterface::ByteBuffer m_data{};

private:
    static const constexpr std::size_t OFFSET_LENGTH = 1;
    static const constexpr std::size_t OFFSET_UPDATE_COUNT = 2;
    static const constexpr std::size_t OFFSET_DATA = 3;
    void unpack(const IpmiInterface::ByteBuffer& data) override;
};
}

}
}
}
