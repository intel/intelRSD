/*!
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
 *
 * @file get_sensor_reading_factors.hpp
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <cstdint>

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Request message for Get Sensor Reading Factors command.
 */
class GetSensorReadingFactors : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    GetSensorReadingFactors();

    /*! Copy constructor. */
    GetSensorReadingFactors(const GetSensorReadingFactors&) = default;

    /*! Assignment operator */
    GetSensorReadingFactors& operator=(const GetSensorReadingFactors&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetSensorReadingFactors();

    /*!
     * @brief Sets sensor number.
     * @param sensor_number Sensor number.
     */
    void set_sensor_number(std::uint8_t sensor_number) {
        m_sensor_number = sensor_number;
    }

    /*!
     * @brief Sets reading byte
     * @param reading_byte Sensor reading byte
     */
    void set_reading_byte(std::uint8_t reading_byte) {
        m_reading_byte = reading_byte;
    }

    virtual void pack(IpmiInterface::ByteBuffer& data) const override;

    virtual const char* get_command_name() const override {
        return "GetSensorReadingFactors";
    }

private:
    std::uint8_t m_sensor_number{};
    std::uint8_t m_reading_byte{};
};

}

namespace response {

/*!
 * @brief Response message for Get Sensor Reading Factors command.
 */
class GetSensorReadingFactors : public Response {
public:

    /*!
     * @brief Default constructor.
     */
    GetSensorReadingFactors();

    /*! Copy constructor. */
    GetSensorReadingFactors(const GetSensorReadingFactors&) = default;

    /*! Assignment operator */
    GetSensorReadingFactors& operator=(const GetSensorReadingFactors&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetSensorReadingFactors();

    /*!
     * @brief Gets reading multiplier.
     * @return Sensor reading multiplier.
     */
    int16_t get_multiplier() const {
        return m_multiplier;
    }

    /*!
     * @brief Gets additive offset (B).
     * @return Additive offset (B).
     */
    int32_t get_additive_offset() const {
        return m_additive_offset;
    }

    /*!
     * @brief Gets exponent (K1).
     * @return Exponent. Sets 'decimal point' location for B.
     */
    int32_t get_exponent() const {
        return m_exponent;
    }

    /*!
     * @brief Gets result exponent (K2).
     * @return Result exponent.
     */
    int32_t get_result_exponent() const {
        return m_result_exponent;
    }

    virtual void unpack(const IpmiInterface::ByteBuffer& data) override;

    virtual const char* get_command_name() const override {
        return "GetSensorReadingFactors";
    }

private:

    static constexpr std::size_t RESPONSE_SIZE = 8;

    int16_t m_multiplier{};
    int32_t m_additive_offset{};
    int32_t m_exponent{};
    int32_t m_result_exponent{};
};
}

}
}
}
