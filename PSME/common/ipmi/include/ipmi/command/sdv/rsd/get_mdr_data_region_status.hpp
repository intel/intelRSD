/*!
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
 * @brief Get mdr data region status command declaration for 2.2 platform
 *
 * @file ipmi/command/sdv/rsd/get_mdr_data_region_status.hpp
 */

#pragma once

#include "ipmi/command/sdv/get_mdr_data_region_status.hpp"
#include "ipmi/command/sdv/enums.hpp"

namespace ipmi {
namespace command {
namespace sdv {
namespace rsd {

using GetMdrStatusLegacyReq = ipmi::command::sdv::request::GetMdrDataRegionStatus;
using GetMdrStatusLegacyRes = ipmi::command::sdv::response::GetMdrDataRegionStatus;

namespace request {
/*!
 * @brief Represents request message for MdrRegionRead command with altered Net Function and Rackscale byte
 */
class GetMdrDataRegionStatus : public GetMdrStatusLegacyReq {
public:
    /*! @brief Default constructor. */
    GetMdrDataRegionStatus();

    GetMdrDataRegionStatus(const GetMdrDataRegionStatus&) = default;
    GetMdrDataRegionStatus(GetMdrDataRegionStatus&&) = default;

    /*! @brief Destructor. */
    virtual ~GetMdrDataRegionStatus();

private:
    static const constexpr std::size_t DATA_REGION_OFFSET = 1;

    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {
/*!
 * @brief Represents response message for MdrRegionRead command with altered Net Function and Rackscale byte
 */
class GetMdrDataRegionStatus : public GetMdrStatusLegacyRes {
public:
    /*! @brief Default constructor. */
    GetMdrDataRegionStatus();

    GetMdrDataRegionStatus(const GetMdrDataRegionStatus&) = default;
    GetMdrDataRegionStatus(GetMdrDataRegionStatus&&) = default;

    /*! @brief Destructor. */
    virtual ~GetMdrDataRegionStatus();

    /*!
     * @brief Check if RackScale extension byte is present in the response.
     * @return True if extension byte is present, false otherwise.
     */
    bool is_rackscale_extension_byte_present() const {
        return m_is_rackscale_extension_byte_present;
    }

private:
    static constexpr std::size_t RESPONSE_SIZE = 12;
    static constexpr std::size_t MDR_VERSION_OFFSET = 2;
    static constexpr std::size_t DATA_REGION_ID_OFFSET = 3;
    static constexpr std::size_t DATA_VALIDATION_OFFSET = 4;
    static constexpr std::size_t DATA_UPDATE_COUNT_OFFSET = 5;
    static constexpr std::size_t LOCK_STATUS_OFFSET = 6;
    static constexpr std::size_t REGION_SIZE_OFFSET = 7;
    static constexpr std::size_t REGION_SIZE_USED_OFFSET = 9;
    static constexpr std::size_t REGION_CHECKSUM_OFFSET = 11;

    void unpack(const IpmiInterface::ByteBuffer& data) override;
    bool m_is_rackscale_extension_byte_present{false};
};
}


}
}
}
}
