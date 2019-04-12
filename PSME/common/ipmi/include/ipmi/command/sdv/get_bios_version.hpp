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
 * @file get_bios_version.hpp
 *
 * @brief Get Bios Version request and response.
 * */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

#include <cstdint>
#include <vector>
#include <string>

namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for Get Bios Info command.
 */
class GetBiosVersion : public Request {
public:

    /*!
     * @brief Constructor.
     */
    GetBiosVersion();

    /*! Copy constructor. */
    GetBiosVersion(const GetBiosVersion&) = default;

    /*! Assignment operator */
    GetBiosVersion& operator=(const GetBiosVersion&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetBiosVersion();

    virtual void pack(std::vector<std::uint8_t>& data) const override {
        // Request without parameters. Pack is empty.
        (void)data;
    }

    virtual const char* get_command_name() const override {
        return "GetBiosVersion";
    }

private:


};
}

namespace response {

/*!
 * @brief Response message for Get Bios Info command. Gets BIOS version information.
 */
class GetBiosVersion : public Response {
public:

    /*!
     * @brief Constructor.
     */
    GetBiosVersion();

    /*! Copy constructor. */
    GetBiosVersion(const GetBiosVersion&) = default;

    /*! Assignment operator */
    GetBiosVersion& operator=(const GetBiosVersion&) = default;

    /*!
     * @brief Default destructor.
     */
    virtual ~GetBiosVersion();

    virtual void unpack(const std::vector<std::uint8_t>& data) override;

    const std::string& get_version() const {
        return m_full_version;
    }

    virtual const char* get_command_name() const override {
        return "GetBiosVersion";
    }

private:
    static constexpr std::size_t RESPONSE_SIZE = 17;
    static constexpr std::size_t OFFSET_ASCII_CODE = 1;
    static constexpr std::size_t OFFSET_VERSION = 5;

    std::string m_full_version{};
    std::string extract_bios_version(const std::vector<std::uint8_t>& data);
};
}

}
}
}
