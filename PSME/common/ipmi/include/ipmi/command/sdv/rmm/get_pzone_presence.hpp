/*!
 * @brief GetPzonePresence IPMI command request and response.
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
 * @file ipmi/command/sdv/get_pzone_presence.hpp
 */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/rmm/get_presence.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Request message for GetPzonePresence command.
 */
class GetPzonePresence : public GetPresence {
public:
    /*!
     * @brief Constructor.
     */
    GetPzonePresence();


    GetPzonePresence(const GetPzonePresence&) = default;


    GetPzonePresence(GetPzonePresence&&) = default;


    virtual ~GetPzonePresence();


    const char* get_command_name() const override {
        return "GetPzonePresence";
    }
};

}

namespace response {

/*!
 * @brief Response message for GetPzonePresence command.
 */
class GetPzonePresence : public GetPresence {
public:
    /*!
     * @brief Constructor.
     */
    GetPzonePresence();


    GetPzonePresence(const GetPzonePresence&) = default;


    GetPzonePresence(GetPzonePresence&&) = default;


    virtual ~GetPzonePresence();


    const char* get_command_name() const override {
        return "GetPzonePresence";
    }


    bool is_present(std::uint8_t asset_index = SLOT_COUNT - 1) const override;


    std::uint8_t get_slot_count() const override {
        return SLOT_COUNT;
    }


private:
    static constexpr const std::uint8_t SLOT_COUNT = 1;
};

}

}
}
}
