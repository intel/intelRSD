/*!
 * @brief Declaration of MdrRegionRead Request/Response.
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
 * @file command/sdv/rsd/mdr_region_read.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "ipmi/command/sdv/mdr_region_read.hpp"


namespace ipmi {
namespace command {
namespace sdv {
namespace rsd {

using MdrRegionReadLegacyReq = ipmi::command::sdv::request::MdrRegionRead;
using MdrRegionReadLegacyRes = ipmi::command::sdv::response::MdrRegionRead;

namespace request {

/*!
 * @brief Represents request message for MdrRegionRead command with altered Net Function and Rackscale byte
 */
class MdrRegionRead : public MdrRegionReadLegacyReq {
public:
    /*! @brief Default constructor. */
    MdrRegionRead();

    /*! Copy constructor */
    MdrRegionRead(const MdrRegionRead&) = default;

    /*! Move constructor */
    MdrRegionRead(MdrRegionRead&&) = default;

    /*! Assignment operator */
    MdrRegionRead& operator=(const MdrRegionRead&) = default;

    /*! Rvalue assignment operator */
    MdrRegionRead& operator=(MdrRegionRead&&) = default;

    /*! @brief Destructor. */
    virtual ~MdrRegionRead();


private:
    static const constexpr std::size_t DATA_REGION_OFFSET = 1;
    static const constexpr std::size_t DATA_LENGTH_OFFSET = 2;
    static const constexpr std::size_t WHERE_TO_READ_OFFSET = 3;

    void pack(IpmiInterface::ByteBuffer& data) const override;
};

}

namespace response {
/*!
 * @brief Represents response message for MdrRegionRead command with altered Net Function and Rackscale byte
 */
class MdrRegionRead : public MdrRegionReadLegacyRes {
public:
    /*! @brief Default constructor. */
    explicit MdrRegionRead(const std::uint16_t data_length);

    /*! @brief Destructor. */
    virtual ~MdrRegionRead();

    /*! Copy constructor */
    MdrRegionRead(const MdrRegionRead&) = default;

    /*! Move constructor */
    MdrRegionRead(MdrRegionRead&&) = default;

    /*! Assignment operator */
    MdrRegionRead& operator=(const MdrRegionRead&) = default;

    /*! Rvalue assignment operator */
    MdrRegionRead& operator=(MdrRegionRead&&) = default;

    /*!
     * @brief Check if RackScale extension byte is present in the response.
     * @return True if extension byte is present, false otherwise.
     */
    bool is_rackscale_extension_byte_present() const {
        return m_is_rackscale_extension_byte_present;
    }

protected:
    /*!
     * @brief Throws error derived from ResponseError for command specific non-zero completion codes.
     * @param completion_code completion code (non-zero)
     * @throws ResponseError type exception.
     */
    void throw_error_on_completion_code(CompletionCode completion_code) const override;

private:
    static const constexpr std::size_t LENGTH_OFFSET = 2;
    static const constexpr std::size_t UPDATE_COUNT_OFFSET = 3;
    static const constexpr std::size_t DATA_OFFSET = 4;

    void unpack(const IpmiInterface::ByteBuffer& data) override;
    bool m_is_rackscale_extension_byte_present{false};
};

}

}
}
}
}
