/*!
 * @brief Intel Optane DC Persistent Memory GetReadings IPMI command request and response.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file ipmi/command/sdv/nm/get_dcpmem_readings.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "reading.hpp"

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for GetDcpmemReadings command.
 */
class GetDcpmemReadings : public Request {
public:
    /*!
     * @brief Constructor.
     */
    GetDcpmemReadings();


    GetDcpmemReadings(const GetDcpmemReadings&) = default;


    GetDcpmemReadings(GetDcpmemReadings&&) = default;

    GetDcpmemReadings& operator=(const GetDcpmemReadings&) = default;


    virtual ~GetDcpmemReadings();


    const char* get_command_name() const override {
        return "GetDcpmemReadings";
    }

    /*
     * @brief Force Readings Halt action
     * @param force_readings_halt Readings halt byte
     */
    void set_force_readings_halt(std::uint8_t force_readings_halt) {
        m_force_readings_halt = force_readings_halt;
    }

private:
    std::uint8_t m_force_readings_halt{0};

    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief Response message for GetDcpmemReadings command.
 */
class GetDcpmemReadings : public Response {
public:

    /*!
     * @brief Constructor.
     */
    GetDcpmemReadings();


    GetDcpmemReadings(const GetDcpmemReadings&) = default;


    GetDcpmemReadings(GetDcpmemReadings&&) = default;

    GetDcpmemReadings& operator=(const GetDcpmemReadings&) = default;


    virtual ~GetDcpmemReadings();


    const char* get_command_name() const override {
        return "GetDcpmemReadings";
    }

    using RawReadingValues = std::vector<std::uint8_t>;

    /*!
     * @brief Returns data reading collection.
     * @return Collection of metric readings.
     */
    const RawReadingValues& get_readings() const {
        return m_raw_readings;
    }

private:
    static constexpr const std::size_t MIN_RESPONSE_SIZE = 5;
    RawReadingValues m_raw_readings{};

    void unpack(const IpmiInterface::ByteBuffer& data) override;

    void throw_error_on_completion_code(CompletionCode completion_code) const override;
};

}

}
}
}
