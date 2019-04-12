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
 * @file get_thermal_state.hpp
 *
 * @brief Get Thermal State request and response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Sends thermal state request to BMC.
 * */
class GetThermalState : public Request {
public:

    /*!
     * @brief Default constructor.
     */
    GetThermalState();

    /*! Copy constructor. */
    GetThermalState(const GetThermalState&) = default;

    /*! Assignment operator */
    GetThermalState& operator=(const GetThermalState&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetThermalState();

    virtual void pack(std::vector<std::uint8_t>& data) const override;

};

}

namespace response {

/*!
 * @brief Gets SLED Thermal state.
 */
class GetThermalState : public Response {
public:

    enum THERMAL_STATE : std::uint8_t {
        POWERED_OFF,
        BELOW_NORMAL,
        NORMAL,
        WARM,
        ABOVE_WARM,
        HOT,
        VERY_HOT,
        CRITICAL
    };

    /*!
     * @brief Default constructor.
     */
    GetThermalState();

    /*! Copy constructor. */
    GetThermalState(const GetThermalState&) = default;

    /*! Assignment operator */
    GetThermalState& operator=(const GetThermalState&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetThermalState();

    /*!
     * @brief Gets thermal state
     * @return thermal state
     * */
    THERMAL_STATE get_thermal_state() {
        return m_thermal_state;
    }


    virtual void unpack(const std::vector<std::uint8_t>& data) override;

private:
    THERMAL_STATE m_thermal_state{};

    static constexpr std::size_t RESPONSE_SIZE = 1;
    static constexpr std::size_t OFFSET_THERMAL_STATE = 1;

};

}
}
}
}
