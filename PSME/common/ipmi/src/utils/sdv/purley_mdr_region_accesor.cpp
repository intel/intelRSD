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
 * @brief
 *
 * @file purley_mdr_region_accesor.cpp
 */


#include "ipmi/utils/sdv/purley_mdr_region_accessor.hpp"
#include "ipmi/command/sdv/rsd/mdr_region_read.hpp"
#include "ipmi/command/sdv/rsd/mdr_region_write.hpp"
#include "ipmi/command/sdv/rsd/mdr_region_lock.hpp"
#include "ipmi/command/sdv/rsd/mdr_region_update_complete.hpp"
#include "ipmi/command/sdv/rsd/get_mdr_data_region_status.hpp"
#include "ipmi/utils/sdv/mdr_region_guard.hpp"

#include <mutex>
#include <type_traits>

using namespace ipmi;
using namespace ipmi::sdv;


using LockRequest = ipmi::command::sdv::rsd::request::MdrRegionLock;
using LockResponse = ipmi::command::sdv::rsd::response::MdrRegionLock;
using GetMdrDataRegionStatusReq = ipmi::command::sdv::rsd::request::GetMdrDataRegionStatus;
using GetMdrDataRegionStatusRes = ipmi::command::sdv::rsd::response::GetMdrDataRegionStatus;


IpmiInterface::ByteBuffer PurleyMdrRegionAccessor::get_mdr_region() {
    using Request = ipmi::command::sdv::rsd::request::MdrRegionRead;
    using Response = ipmi::command::sdv::rsd::response::MdrRegionRead;

    const auto bytes_to_read = get_mdr_region_size_to_read();

    return read_mdr_region<Request, Response>(bytes_to_read);
}

std::uint16_t PurleyMdrRegionAccessor::get_mdr_region_size_to_read() {
    auto status = get_mdr_region_status<GetMdrDataRegionStatusReq, GetMdrDataRegionStatusRes>();
    check_mdr_region_unlocked_and_valid(status);
    return status.get_region_size_used();
}

std::uint8_t PurleyMdrRegionAccessor::get_mdr_region_checksum() {
    auto status = get_mdr_region_status<GetMdrDataRegionStatusReq, GetMdrDataRegionStatusRes>();
    check_mdr_region_unlocked_and_valid(status);
    return status.get_region_checksum();
}

std::uint8_t PurleyMdrRegionAccessor::get_mdr_region_update_count() {
    auto status = get_mdr_region_status<GetMdrDataRegionStatusReq, GetMdrDataRegionStatusRes>();
    check_mdr_region_unlocked_and_valid(status);
    return status.get_data_update_count();
}

void PurleyMdrRegionAccessor::write_mdr_region(const IpmiInterface::ByteBuffer& buffer) {
    using Request = ipmi::command::sdv::rsd::request::MdrRegionWrite;
    using Response = ipmi::command::sdv::rsd::response::MdrRegionWrite;

    const auto bytes_to_write =
        get_mdr_region_status<GetMdrDataRegionStatusReq, GetMdrDataRegionStatusRes>().get_region_size();

    MdrRegionGuard<LockResponse> lock_impl(
        [this](uint16_t a, uint8_t b) -> LockResponse {
            return this->lock_mdr_region<LockRequest, LockResponse>(command::sdv::LockType::PREEMPTIBLE_LOCK, a, b);},
        [this](uint8_t handle) {
            complete_mdr_update<ipmi::command::sdv::rsd::request::MdrRegionUpdateComplete,
            ipmi::command::sdv::rsd::response::MdrRegionUpdateComplete>(handle);},
        LockRequest::DEFAULT_TIMEOUT_MS, LockRequest::DEFAULT_RETRIES);
    // use C-style locking for interaction with BMC
    lock_impl.lock();
    write_mdr_region_impl<Request, Response>(buffer, bytes_to_write, lock_impl.get_handle());
    // safe in case of an exception, because lock timeout will drop the lock and invalidate the region
    lock_impl.unlock_except();
}
