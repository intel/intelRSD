/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file master_write_read.hpp
 *
 * @brief Master Write Read command.
 *
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "enums.hpp"

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Request message for IPMI Master Write Read Command.
 */
class MasterWriteRead : public Request {
public:

    /*!
     * @brief Constructor destructor.
     * @param[in] bus I2C bus
     * @param[in] address I2C device address
     * @param[in] read_count Number of bytes to read_count
     * @param[in] data Bytes to write
     */
    MasterWriteRead(std::uint8_t bus, std::uint8_t address,
        std::uint8_t read_count, std::vector<std::uint8_t> data);


    /*! Copy constructor */
    MasterWriteRead(const MasterWriteRead&) = delete;


    /*! Assignment operator */
    MasterWriteRead& operator=(const MasterWriteRead&) = delete;


    /*!
     * @brief Default destructor.
     */
    virtual ~MasterWriteRead();


    void pack(std::vector<std::uint8_t>& data) const override;

    virtual const char* get_command_name() const override {
        return "MasterWriteRead";
    }

private:
    std::uint8_t m_bus{0};
    std::uint8_t m_address{0};
    std::uint8_t m_read_count{0};
    std::vector<std::uint8_t> m_data{};
};

}

namespace response {

/*!
 * @brief Response message for IPMI Master Write Read command.
 */
class MasterWriteRead : public Response {
public:

    /*!
     * @brief Default constructor.
     * @param[in] data_size Number of bytes that are to be read
     */
    MasterWriteRead(uint8_t data_size);


    /*! Copy constructor */
    MasterWriteRead(const MasterWriteRead&) = default;


    /*! Assignment operator */
    MasterWriteRead& operator=(const MasterWriteRead&) = default;


    /*!
     * @brief Default destructor.
     */
    virtual ~MasterWriteRead();


    void unpack(const std::vector<std::uint8_t>& data) override;

    /*!
     * @brief Returns read data
     * @return Read data
     * */
    std::vector<std::uint8_t> get_data() const { return m_response; }

    virtual const char* get_command_name() const override {
        return "MasterWriteRead";
    }

private:
    std::vector<std::uint8_t> m_response{};
};

}

}
}
}
