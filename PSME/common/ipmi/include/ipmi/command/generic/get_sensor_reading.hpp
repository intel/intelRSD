/*!
 * @brief Declaration of GetSensorReading Request/Response.
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
 * @file command/generic/get_sensor_reading.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <iostream>

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Request message for Get Sensor Reading command.
 */
class GetSensorReading : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    GetSensorReading();

    /*! Copy constructor. */
    GetSensorReading(const GetSensorReading&) = default;

    /*! Assignment operator */
    GetSensorReading& operator=(const GetSensorReading&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetSensorReading();

    const char* get_command_name() const override {
        return "GetSensorReading";
    }

    /*!
     * @brief Sets sensor number.
     * @param sensor_number Sensor number.
     */
    void set_sensor_number(std::uint8_t sensor_number) {
        m_sensor_number = sensor_number;
    }

private:
    std::uint8_t m_sensor_number{};

    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief Response message for Get Sensor Reading command.
 */
class GetSensorReading : public Response {
public:

    /*!
     * @brief Default constructor.
     */
    GetSensorReading();

    /*! Copy constructor. */
    GetSensorReading(const GetSensorReading&) = default;

    /*! Assignment operator */
    GetSensorReading& operator=(const GetSensorReading&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetSensorReading();

    const char* get_command_name() const override {
        return "GetSensorReading";
    }

    /*!
     * @brief Gets sensor reading.
     * @return Sensor reading.
     */
    std::uint8_t get_sensor_reading() const;

    /*!
     * @brief Indicates if the reading is valid.
     * @return true if reading is valid, false otherwise.
     */
    bool is_valid_reading() const;

    /*!
     * @brief Indicates if sensor scanning is disabled.
     * @return true if sensor scanning is disabled, false otherwise.
     */
    bool is_scanning_disabled() const;

    /*!
     * @brief Get first data byte
     * @return First data byte (for threshold or discrete sensors)
     */
    std::uint8_t get_data_1() const;

    /*!
     * @brief Get second data byte
     * @return Second data byte (for discrete sensors only, optional field)
     */
    std::uint8_t get_data_2() const;

private:
    static constexpr std::size_t MIN_RESPONSE_SIZE = 3;
    static constexpr std::size_t OFFSET_READING_VALIDITY = OFFSET_DATA + 1;
    static constexpr std::size_t OFFSET_DATA_1 = OFFSET_DATA + 2;
    static constexpr std::size_t OFFSET_DATA_2 = OFFSET_DATA + 3;

    std::uint8_t m_sensor_reading{0};
    std::uint8_t m_reading_validity{0};
    std::uint8_t m_data1{0}; // for treshold-based || discrete
    std::uint8_t m_data2{0}; // for discrete (optional)

    void unpack(const IpmiInterface::ByteBuffer& data) override;
};
}

}
}
}
