/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file ipmb/command/get_aggregated_thermal_sensor.hpp
 * @brief GetAggregatedHealthSensor IPMI command.
 * */

#pragma once
#include <ipmb/command/command.hpp>

#include <vector>

/*! Agent namesapce */
namespace agent {
/*! Chassis namesapce */
namespace chassis {
/*! IPMB namesapce */
namespace ipmb {
/*! Command namesapce */
namespace command {

/*! GetAggregatedHealthSensor command */
class GetAggregatedHealthSensor final : public Command {
public:
    /*! Default constructor */
    GetAggregatedHealthSensor(){}
    /*! Copy constructor */
    GetAggregatedHealthSensor(const GetAggregatedHealthSensor&) = default;
    /*! Assigment construcor */
    GetAggregatedHealthSensor& operator=(const GetAggregatedHealthSensor&) = default;

    /*!
     * Unpacks IPMI Message
     * @param msg IPMI Message
     * */
    void unpack(IpmiMessage& msg);

    /*!
     * Packs IPMI Message
     * @param msg IPMI Message
     * */
    void pack(IpmiMessage& msg);

    ~GetAggregatedHealthSensor();

private:
    static const constexpr uint8_t CMD_RESPONSE_DATA_LENGHT = 2;

    static const constexpr uint8_t OFFSET_CC = 0;
    static const constexpr uint8_t OFFSET_STATUS = 1;

    static const constexpr uint8_t CMD_STATUS_OK = 0x00;
    static const constexpr uint8_t CMD_STATUS_FAILURE = 0x80;

    class Response final : public Command::Response {
        uint8_t m_health{};
    public:
        /*!
         * Fill data buffer
         * @param msg IPMI Message
         * */
        void add_data(IpmiMessage& msg);

        /*!
         * Sets health status
         * @param health Sealth status
         * */
        void set_health(const uint8_t health) {
            m_health = health;
        }

        /*!
         * Gets health status
         * @return Health status
         * */
        uint8_t get_health() const {
            return m_health;
        }
    };

    Response m_response{};

};

}
}
}
}

