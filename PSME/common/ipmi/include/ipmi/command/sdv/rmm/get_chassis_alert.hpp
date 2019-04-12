/*!
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
 *
 * @file get_presence.hpp
 *
 * @brief GetChassisAlert IPMI command request and response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief GetChassisAlerts request command.
 */
class GetChassisAlert : public Request {
public:
    /*!
     * @brief Constructor.
     */
    explicit GetChassisAlert();

    /*!
     * @brief destructor.
     */
    virtual ~GetChassisAlert();

    void pack(std::vector<std::uint8_t>& data) const override;

    virtual const char* get_command_name() const override {
        return "GetChassisAlert";
    }
};

}

namespace response {

/*!
 * @brief GetChassisAlerts response command.
 */
class GetChassisAlert : public Response {
public:
    static constexpr std::uint8_t SLOT_COUNT = 8;

    /*!
     * @brief Constructor.
     */
    explicit GetChassisAlert();

    void unpack(const std::vector<std::uint8_t>& data) override;

    /*!
     * @brief Chassis alerts mask getter.
     * @return Chassis alerts mask.
     */
    std::uint8_t get_alerts_mask() const;

    /*!
     * @brief Checks if chassis at given asset index is alerted.
     * @param chassis_index Chassis index in range [0, get_max_index()].
     * @return True if chassis is alerted, false otherwise or index is out of range.
     */
    bool is_alerted(std::uint8_t chassis_index) const;

    /*
     * @brief Returns maximum valid chassis index.
     * @return maximum valid asset index.
     */
    std::uint8_t get_slot_count() const {
        return SLOT_COUNT;
    }

    virtual const char* get_command_name() const override {
        return "GetChassisAlert";
    }

protected:
    static constexpr std::size_t RESPONSE_SIZE = 2;
    std::uint8_t m_alerts_mask{0};
};

}

}
}
}
