/*!
 * @brief WarmReset command interface.
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
 * @file warm_reset.hpp
 */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"



namespace ipmi {
namespace command {
namespace generic {

namespace request {

/*!
 * @brief WarmReset request command
 */
class WarmReset : public Request {
public:
    /*!
     * @brief Constructor
     */
    WarmReset();
    WarmReset(const WarmReset&) = default;
    WarmReset(WarmReset&&) = default;


    /*!
     * @brief Destructor
     */
    virtual ~WarmReset();


    const char* get_command_name() const override {
        return "WarmReset";
    }


private:
    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {
/*!
 * @brief WarmReset command response
 */
class WarmReset : public Response {
public:
    /*!
     * @brief Constructor
     */
    WarmReset();
    WarmReset(const WarmReset&) = default;
    WarmReset(WarmReset&&) = default;


    /*!
     * @brief Destructor
     */
    virtual ~WarmReset();


    const char* get_command_name() const override {
        return "WarmReset";
    }


private:
    static constexpr std::size_t RESPONSE_SIZE = 1;


    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

}

}
}
}
