/*!
 * @brief Reset command interface.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file reset.hpp
 */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Reset command request
 */
class Reset : public Request {
public:
    /*!
     * @brief This enum specifies the exact hardware component that should be reset.
     */
    enum class ComponentToReset : std::uint8_t {
        MMP = 0x00,
        SLED_1 = 0x01,
        SLED_2 = 0x02,
        SLED_3 = 0x03,
        SLED_4 = 0x04,
        RRC = 0x05,
        SWITCH = 0x06
    };


    /*!
     * @brief Constructor
     */
    Reset();

    /*!
     * @brief Constructor with component to reset
     * @param component_to_reset NMP reset enum value
     */
    Reset(ComponentToReset component_to_reset);

    /*!
     * @brief Constructor with component to reset
     * @param component_to_reset value to be passed in the message
     */
    Reset(std::uint8_t component_to_reset);

    /*!
     * @brief Destructor
     */
    virtual ~Reset();


    const char* get_command_name() const override {
        return "Reset";
    }


    /*!
     * @brief Select the component that should be reset
     * @param component_to_reset Component to reset
     */
    void set_component_to_reset(const ComponentToReset component_to_reset) {
        m_component_to_reset = static_cast<std::uint8_t>(component_to_reset);
    }


private:
    void pack(IpmiInterface::ByteBuffer& data) const override;

    /*!
     * @brief value to be put in the request
     *
     * There are two "enums" which are to be passed: either MBP or NMP.
     * MBP (24.x) handles requests to MCU (0), drawers (1, 3, 5, 7) and switch (9).
     * NMP (directly to CM) handles Snowflake (0), Sleds (1-4), RRC (5), Switch (6).
     * It is not exactly stated what is hidden under "sled".
     */
    std::uint8_t m_component_to_reset{};
};

}

namespace response {

/*!
 * @brief Reset command response
 */
class Reset : public Response {
public:
    /*!
     * @brief Constructor
     */
    Reset();


    /*!
     * @brief Desctructor
     */
    virtual ~Reset();

    virtual const char* get_command_name() const override {
        return "Reset";
    }

private:
    static constexpr std::size_t RESPONSE_SIZE = 1;


    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

}

}
}
}
