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
 * @file chassis_control_command.hpp
 *
 * @brief IPMI Chassis Control Command.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace generic {

namespace request {
/*!
 * @brief Request message for IPMI Chassis Control Command.
 *
 * It's needed to set PowerStatus you expected to enforce.
 * Default behavior is to power down chassis/platform.
 */
class ChassisControlCommand : public Request {
public:

    /*!
     * @brief Represents Power Status to be set.
     */
    enum class PowerState : std::uint8_t {
        POWER_DOWN = 0x00,
        POWER_UP,
        POWER_CYCLE,
        HARD_RESET,
        DIAGNOSTIC_INTERRUPT,
        ACPI_SOFT_SHUTDOWN,
        LAST,
        UNKNOWN
    };

    /*!
     * @brief Default constructor.
     */
    ChassisControlCommand();

    /*! @brief Copy constructor. */
    ChassisControlCommand(const ChassisControlCommand&) = default;

    /*! @brief Assignment operator */
    ChassisControlCommand& operator=(const ChassisControlCommand&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~ChassisControlCommand();

    /*!
     * @brief Sets requested power status to be send to MC.
     * @param status new power status to enforce on chassis.
     */
    void set_power_state(PowerState status);

    virtual void pack(std::vector<std::uint8_t>& data) const override;

    virtual const char* get_command_name() const override {
        return "ChassisControlCommand";
    }

private:
    PowerState m_power_state = PowerState::POWER_DOWN;
};

}

namespace response {
/*!
 * @brief Request message for IPMI Chassis Control Command.
 */
class ChassisControlCommand : public Response {
public:
    /*!
     * @brief Default constructor.
     */
    ChassisControlCommand();

    /*! Copy constructor. */
    ChassisControlCommand(const ChassisControlCommand&) = default;

    /*! Assignment operator */
    ChassisControlCommand& operator=(const ChassisControlCommand&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~ChassisControlCommand();

    virtual void unpack(const std::vector<std::uint8_t>& data) override;

    virtual const char* get_command_name() const override {
        return "ChassisControlCommand";
    }

private:
    static constexpr std::size_t RESPONSE_SIZE = 1;
};

}

}
}
}
