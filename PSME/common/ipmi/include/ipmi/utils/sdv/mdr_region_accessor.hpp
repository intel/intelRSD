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
 * @brief Class for accessing MDR regions
 *
 * @file ipmi/utils/mdr_region_accessor.hpp
 */

#pragma once



#include "ipmi/ipmi_controller.hpp"
#include "ipmi/command/sdv/enums.hpp"
#include "ipmi/command/sdv/get_mdr_data_region_status.hpp"

#include <iterator>



namespace ipmi {
namespace sdv {

/*!
 * @brief Interface for accessing MDR regions
 */
class MdrRegionAccessor {
    friend class MdrRegionAccessorFactory;
public:
    using Ptr = std::unique_ptr<MdrRegionAccessor>;


    /*! Destructor */
    virtual ~MdrRegionAccessor();


    /*!
     * @brief Get mdr region
     * @return Byte buffer (vector of bytes) containing data from desired MDR region
     */
    virtual IpmiInterface::ByteBuffer get_mdr_region() = 0;


    /*!
     * @brief Write mdr region
     * @param buffer Byte buffer (vector of bytes) containing data for the desired MDR region
     */
    virtual void write_mdr_region(const IpmiInterface::ByteBuffer& buffer) = 0;


    /*!
     * Get MDR region checksum, used to determine if the region has changed
     * @return Checksum of the MDR region
     */
    virtual std::uint8_t get_mdr_region_checksum() = 0;

    /*!
     * Get MDR region data contents update count, used to determine number of region changes
     * @return Update count of the MDR region
     */
    virtual std::uint8_t get_mdr_region_update_count() = 0;

protected:
    virtual std::uint16_t get_mdr_region_size_to_read() = 0;


    MdrRegionAccessor(IpmiController& ipmi_controller, const command::sdv::DataRegionId region_id)
        : m_ipmi_ctrl{ipmi_controller}, m_region_id{region_id} {}


    template<typename MdrStatusReq, typename MdrStatusResp>
    MdrStatusResp get_mdr_region_status() {
        MdrStatusReq request{};
        MdrStatusResp response{};
        request.set_data_region_id(m_region_id);
        m_ipmi_ctrl.send(request, response);
        return response;
    }


    template<typename MdrReadReq, typename MdrReadResp>
    ipmi::IpmiInterface::ByteBuffer read_mdr_region(std::uint16_t bytes_to_read) {
        std::uint8_t to_read{0};
        ipmi::IpmiInterface::ByteBuffer mdr_region_data{};
        auto remaining = bytes_to_read;
        MdrReadReq request{};
        request.set_data_region_id(m_region_id);
        while (remaining > 0) {
            // There is a limit to the maximum number of bytes we can read at once
            to_read = std::uint8_t(std::min(std::uint16_t(MdrReadReq::MAX_DATA_COUNT), remaining));
            request.set_data_length(to_read);
            request.set_offset(std::uint16_t(bytes_to_read - remaining));
            MdrReadResp response{to_read};
            m_ipmi_ctrl.send(request, response);
            const auto& response_data = response.get_data();
            mdr_region_data.insert(mdr_region_data.end(), response_data.cbegin(), response_data.cend());
            remaining = std::uint16_t(remaining - response_data.size());
        }
        return mdr_region_data;
    }


    template<typename MdrLockReq, typename MdrLockResp>
    MdrLockResp lock_mdr_region(const command::sdv::LockType& lock_type, uint16_t timeout_ms, uint8_t handle) {
        MdrLockReq request{};
        MdrLockResp response{};
        request.set_data_region_id(m_region_id);
        request.set_lock_type(lock_type);
        request.set_session_lock_handle(handle);
        request.set_timeout(timeout_ms);

        m_ipmi_ctrl.send(request, response);
        return response;
    }


    template<typename MdrWriteReq, typename MdrWriteResp>
    void write_mdr_region_impl(const IpmiInterface::ByteBuffer& buffer, uint16_t mdr_capacity, uint8_t handle) {
        if (buffer.size() > mdr_capacity) {
            throw std::runtime_error("Too much data to write to the MDR region");
        }

        std::uint8_t to_write{0};
        auto remaining = buffer.size();
        auto iter = buffer.cbegin();
        while (remaining > 0) {
            MdrWriteReq request{};
            request.set_data_region_id(m_region_id);
            // There is a limit to the maximum number of bytes we can write at once
            to_write = std::uint8_t(std::min(static_cast<decltype(remaining)>(MdrWriteReq::MAX_DATA_COUNT), remaining));
            request.set_data_length(to_write);
            request.set_data_offset(std::uint16_t(buffer.size() - remaining));
            request.append_data(iter, iter + to_write);
            request.set_session_lock_handle(handle);
            MdrWriteResp response{};
            m_ipmi_ctrl.send(request, response);
            remaining = std::uint16_t(remaining - to_write);
            std::advance(iter, to_write);
        }
    }


    template<typename MdrUpdateCompleteReq, typename MdrUpdateCompleteResp>
    void complete_mdr_update(uint8_t handle) {
        MdrUpdateCompleteReq request{};
        MdrUpdateCompleteResp response{};

        request.set_data_region_id(m_region_id);
        request.set_session_lock_handle(handle);

        m_ipmi_ctrl.send(request, response);
    }


    template<typename MdrStatusResp>
    void check_mdr_region_unlocked_and_valid(const MdrStatusResp& response) {
        if (ipmi::command::sdv::LockStatus::UNLOCKED != response.get_lock_status()) {
            throw ipmi::command::sdv::response::MdrDataRegionLockedError(response);
        }
        if (ipmi::command::sdv::DataValidation::VALID != response.get_data_validation()) {
            throw ipmi::command::sdv::response::MdrDataRegionInvalidError(response);
        }
        if (response.get_region_size_used() == 0) {
            throw ipmi::command::sdv::response::MdrDataRegionEmptyError(response);
        }
    }


private:
    IpmiController& m_ipmi_ctrl;
    const command::sdv::DataRegionId m_region_id;
};

/*!
 * @brief Class for creating MDR Region accessors, depending on the type of platform
 */
class MdrRegionAccessorFactory {
public:

    using Ptr = std::shared_ptr<MdrRegionAccessorFactory>;

    /*!
     * Destructor
     */
    virtual ~MdrRegionAccessorFactory();


    /*!
     * @brief Factory method
     * @param platform_id Platform identifier
     * @param ipmi_controller IpmiController
     * @param region_id Region identifier
     * @return Pointer to MdrRegion for given platform.
     */
    virtual MdrRegionAccessor::Ptr create(std::uint32_t platform_id,
                                          IpmiController& ipmi_controller,
                                          const command::sdv::DataRegionId region_id) const;
};

}
}
