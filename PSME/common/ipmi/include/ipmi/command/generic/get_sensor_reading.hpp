/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * @file get_sensor_reading.hpp
 * */

#ifndef IPMI_COMMAND_GENERIC_GET_SENSOR_READING_HPP
#define IPMI_COMMAND_GENERIC_GET_SENSOR_READING_HPP

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <iostream>

using std::uint8_t;
using std::uint32_t;
using std::vector;
using std::map;
using std::string;

namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief Request message for Get Sensor Reading command.
 */
class GetSensorReading: public Request {
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

    /*!
     * @brief Sets sensor number.
     * @param sensor_number Sensor number.
     */
    void set_sensor_number(uint8_t sensor_number) {
        m_sensor_number = sensor_number;
    }

    virtual void pack(vector<uint8_t>& data) const;

private:
    uint8_t m_sensor_number{};

};

}

namespace response {

/*!
 * @brief Response message for Get Sensor Reading command.
 */
class GetSensorReading: public Response {
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

    /*!
     * @brief Gets sensor reading.
     * @return Sensor reading.
     */
    uint8_t get_sensor_reading() const;

    virtual void unpack(const vector<uint8_t>& data);

private:

    static constexpr size_t RESPONSE_SIZE = 5;

    static constexpr size_t OFFSET_SENSOR_READING = 1;

    uint8_t m_sensor_reading{};
};
}

}
}
}
#endif

