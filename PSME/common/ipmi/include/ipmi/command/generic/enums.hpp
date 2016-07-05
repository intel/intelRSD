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
 * @file enums.hpp
 *
 * @brief IPMI generic enums.
 * */

#ifndef IPMI_COMMAND_GENERIC_ENUMS_HPP
#define IPMI_COMMAND_GENERIC_ENUMS_HPP

namespace ipmi {
namespace command {
namespace generic {

/*
 * @brief Represents codes for Network Function tailored to IPMI spec.
 */
enum class NetFn: uint8_t {
    CHASSIS = 0x00,
    SENSOR_EVENT = 0x04,
    APP = 0x06,
};

/*!
 * @brief Represents codes for IPMI Command.
 */
enum class Cmd: uint8_t {
    GET_CHASSIS_STATUS = 0x01,
    GET_DEVICE_ID = 0x01,
    GET_THERMAL_STATE = 0x08,
    SET_BOOT_ORDER = 0x08,
    GET_BOOT_ORDER = 0x09,
    GET_SENSOR_READING_FACTORS = 0x23,
    GET_SENSOR_READING = 0x2D,
    CHASSIS_CONTROLL_COMMAND = 0x02,
    GET_SYSTEM_GUID = 0x37,
    GET_DEVICE_GUID = 0x08
};

}
}
}
#endif /* IPMI_COMMAND_GENERIC_ENUMS_HPP */
