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
 * @brief MDR region lock command declaration
 *
 * @file ipmi/command/sdv/rsd/mdr_region_lock.hpp
 */

#pragma once

#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"


namespace ipmi {
namespace command {
namespace sdv {
namespace rsd {

namespace request {
class MdrRegionLock : public Request {
public:
    static constexpr uint16_t DEFAULT_TIMEOUT_MS = 1000;
    static constexpr int DEFAULT_RETRIES = 3;

    static constexpr size_t SESSION_LOCK_HANDLE_OFFSET = 1;
    static constexpr size_t DATA_REGION_OFFSET = 2;
    static constexpr size_t LOCK_TYPE_OFFSET = 3;
    static constexpr size_t TIMEOUT_OFFSET = 4;

    /*! @brief Default constructor */
    MdrRegionLock();

    /*! @brief Copy constructor */
    MdrRegionLock(const MdrRegionLock&) = default;

    /*! @brief Move constructor */
    MdrRegionLock(MdrRegionLock&&) = default;

    /*! @brief Assignment operator */
    MdrRegionLock& operator=(const MdrRegionLock&) = default;

    /*! @brief Rvalue assignment operator */
    MdrRegionLock& operator=(MdrRegionLock&&) = default;

    /*! @brief Destructor. */
    virtual ~MdrRegionLock();

    virtual const char* get_command_name() const override {
        return "MdrRegionLock";
    }

    /*!
     * @brief Set session lock handle
     * @param[in] handle session lock handle
     */
    void set_session_lock_handle(std::uint8_t handle) {
        m_session_lock_handle = handle;
    }

    /*!
     * @brief Get session lock handle
     * @return session lock handle
     */
    std::uint8_t get_session_lock_handle() const {
        return m_session_lock_handle;
    }

    /*!
     * @brief Set data region id
     * @param[in] data_region_id New value
     */
    void set_data_region_id(DataRegionId data_region_id) {
        m_data_region_id = data_region_id;
    }

    /*!
     * @brief Get data region id
     * @return data region id New value
     */
    DataRegionId get_data_region_id() const {
        return m_data_region_id;
    }

    /*!
     * @brief Set lock type
     * @param lock_type New value
     */
    void set_lock_type(LockType lock_type) {
        m_lock_type = lock_type;
    }

    /*!
     * Get lock type
     * @return lock type New value
     */
    LockType get_lock_type() const {
        return m_lock_type;
    }

    /*!
     * @brief Set timeout
     * @param timeout New value
     */
    void set_timeout(uint16_t timeout) {
        m_timeout = timeout;
    }

    /*!
     * Get timeout
     * @return timeout New value
     */
    uint16_t get_timeout() const {
        return m_timeout;
    }

private:
    std::uint8_t m_session_lock_handle{};
    DataRegionId m_data_region_id{};
    LockType m_lock_type{};
    uint16_t m_timeout{};

    virtual void pack(IpmiInterface::ByteBuffer& data) const override;
};
}

namespace response {
class MdrRegionLock : public Response {
public:
    /*! @brief Default constructor */
    MdrRegionLock();

    /*! @brief Copy constructor */
    MdrRegionLock(const MdrRegionLock&) = default;

    /*! @brief Move constructor */
    MdrRegionLock(MdrRegionLock&&) = default;

    /*! @brief Assignment operator */
    MdrRegionLock& operator=(const MdrRegionLock&) = default;

    /*! @brief Rvalue assignment operator */
    MdrRegionLock& operator=(MdrRegionLock&&) = default;

    /*! @brief Destructor. */
    virtual ~MdrRegionLock();

    virtual const char* get_command_name() const override {
        return "MdrRegionLock";
    }

    /*!
     * @brief Set session lock handle
     * @param[in] handle session lock handle
     */
    void set_session_lock_handle(std::uint8_t handle) {
        m_session_lock_handle = handle;
    }

    /*!
     * @brief Get session lock handle
     * @return session lock handle
     */
    std::uint8_t get_session_lock_handle() const {
        return m_session_lock_handle;
    }

protected:
    /*!
     * @brief Throws error derived from ResponseError for command specific non-zero completion codes.
     * @param completion_code completion code (non-zero)
     * @throws ResponseError type exception.
     */
    void throw_error_on_completion_code(CompletionCode completion_code) const override;

private:
    static constexpr size_t SESSION_LOCK_HANDLE_OFFSET = 2;
    static constexpr size_t RESPONSE_SIZE = 3;

    std::uint8_t m_session_lock_handle{};
    bool m_is_rackscale_extension_byte_present{false};

    void unpack(const IpmiInterface::ByteBuffer& data) override;
};
}

}
}
}
}
