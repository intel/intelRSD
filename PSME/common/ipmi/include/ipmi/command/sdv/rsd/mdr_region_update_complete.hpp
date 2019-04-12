/*!
 * @brief Declaration of MdrRegionUpdateComplete Request/Response.
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
 * @file command/sdv/rsd/mdr_region_update_complete.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/rsd/constants.hpp"
#include "ipmi/command/sdv/enums.hpp"

namespace ipmi {
namespace command {
namespace sdv {
namespace rsd {

namespace request {

/*!
 * @brief Represents request message for MdrRegionUpdateComplete command.
 */
class MdrRegionUpdateComplete : public Request {
public:
    static constexpr size_t SESSION_LOCK_HANDLE_OFFSET = 1;
    static constexpr size_t DATA_REGION_OFFSET = 2;

    /*! @brief Default constructor. */
    MdrRegionUpdateComplete();

    /*! Copy constructor */
    MdrRegionUpdateComplete(const MdrRegionUpdateComplete&) = default;

    /*! Move constructor */
    MdrRegionUpdateComplete(MdrRegionUpdateComplete&&) = default;

    /*! Assignment operator */
    MdrRegionUpdateComplete& operator=(const MdrRegionUpdateComplete&) = default;

    /*! Rvalue assignment operator */
    MdrRegionUpdateComplete& operator=(MdrRegionUpdateComplete&&) = default;

    /*! @brief Destructor. */
    virtual ~MdrRegionUpdateComplete();

    /*! @brief Returns command name. */
    const char* get_command_name() const override {
        return "MdrRegionUpdateComplete";
    }

    /*!
    * @brief Sets Data Region ID
    * @param[in] data_region_id Data Region ID
    */
    void set_data_region_id(const DataRegionId data_region_id) {
        m_data_region_id = data_region_id;
    }

    /*!
     * @brief Gets Data Region ID
     * @return Data Region ID
     */
    DataRegionId get_data_region_id() const {
        return m_data_region_id;
    }

    /*!
    * @brief Gets session lock handle
    * @return session lock handle
    */
    std::uint8_t get_session_lock_handle() const {
        return m_session_lock_handle;
    }

    /*!
    * @brief Sets session lock handle
    * @param[in] handle session lock handle that can be obtained from Get MDR region lock
    */
    void set_session_lock_handle(std::uint8_t handle) {
        m_session_lock_handle = handle;
    }

private:
    DataRegionId m_data_region_id{};
    std::uint8_t m_session_lock_handle{};

    void pack(IpmiInterface::ByteBuffer& data) const override;
};
}

namespace response {

class MdrRegionUpdateComplete : public Response {
public:

    /*! @brief Default constructor. */
    MdrRegionUpdateComplete();

    /*! Copy constructor */
    MdrRegionUpdateComplete(const MdrRegionUpdateComplete&) = default;

    /*! Move constructor */
    MdrRegionUpdateComplete(MdrRegionUpdateComplete&&) = default;

    /*! Assignment operator */
    MdrRegionUpdateComplete& operator=(const MdrRegionUpdateComplete&) = default;

    /*! Rvalue assignment operator */
    MdrRegionUpdateComplete& operator=(MdrRegionUpdateComplete&&) = default;

    /*! @brief Destructor. */
    virtual ~MdrRegionUpdateComplete();

    /*! @brief Returns command name. */
    const char* get_command_name() const override {
        return "MdrRegionUpdateComplete";
    }

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
    static constexpr size_t RESPONSE_SIZE = 2;
    bool m_is_rackscale_extension_byte_present{false};
    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

}


}
}
}
}
