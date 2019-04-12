/*!
 * @brief SensorsReader declaration
 *
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file status/sensors_reader.hpp
*/

#pragma once

#include <string>

/*! Agent namspace */
namespace agent {
/*! Chassis namspace */
namespace chassis {

class Bmc;

/*!
 * @brief Reads pwm, inlet temperature and power sensors of given sled.
 */
class SensorsReader {
public:

    /*!
     * @brief Constructor
     * @param manager_uuid Manager uuid
     * @param bmc Bmc
     */
    SensorsReader(const std::string& manager_uuid, Bmc& bmc);

    /*!
     * @brief Invokes ipmi commands and updates model with sensor values.
     */
    void operator()();

private:

    const std::string manager_uuid{};
    Bmc& bmc;
};

}
}
