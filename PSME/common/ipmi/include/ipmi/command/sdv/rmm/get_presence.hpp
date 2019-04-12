/*!
 * @brief GetPresence IPMI command request and response.
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
 * @file ipmi/command/sdv/get_presence.hpp
 */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Base class for Get[Chassis,Fan,Psu]Presence request commands.
 */
class GetPresence : public Request {
protected:
    /*!
     * @brief Constructor.
     * @param cmd IPMI command
     */
    explicit GetPresence(Cmd cmd);


private:
    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {

/*!
 * @brief Base class for Get[Chassis,Fan,Psu]Presence response commands.
 */
class GetPresence : public Response {
public:
    /*!
     * @brief Constructor.
     * @param cmd IPMI command
     * @param response_size Expected response size
     */
    explicit GetPresence(Cmd cmd, const size_t response_size = RESP_SIZE);


    /*!
     * @brief Presence mask getter.
     * @return Presence mask.
     */
    std::uint8_t get_presence_mask() const;


    /*!
     * @brief Checks asset presence at given asset index.
     * @param asset_index Asset index in range [0, get_max_index()].
     * @return True if asset is present, false if asset is absent at given
     * index or index is out of range.
     */
    virtual bool is_present(std::uint8_t asset_index) const = 0;


    /*
     * @brief Returns maximum valid asset index.
     * @return maximum valid asset index.
     */
    virtual std::uint8_t get_slot_count() const = 0;


protected:
    static constexpr const std::size_t RESP_SIZE = 2;
    std::uint8_t m_presence_mask{0};

private:
    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

}

}
}
}
