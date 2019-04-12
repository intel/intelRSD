/*!
 * @brief Intel Optane DC Persistent Memory GetReadings IPMI command request and response implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file sdv/nm/get_dcpmem_readings.cpp
 */

#include "ipmi/command/sdv/nm/get_dcpmem_readings.hpp"
#include "ipmi/command/sdv/nm/errors.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::GetDcpmemReadings::GetDcpmemReadings()
    : Request(NetFn::INTEL_NM, Cmd::GET_DCPMEM_READINGS) {}


request::GetDcpmemReadings::~GetDcpmemReadings() {}

void request::GetDcpmemReadings::pack(IpmiInterface::ByteBuffer& data) const {
    data.insert(data.end(), INTEL_IANA_LSB_FIRST.begin(), INTEL_IANA_LSB_FIRST.end());
    data.push_back(m_force_readings_halt);
}

response::GetDcpmemReadings::GetDcpmemReadings()
    : Response(NetFn::INTEL_NM + 1, Cmd::GET_DCPMEM_READINGS, MIN_RESPONSE_SIZE) {}


response::GetDcpmemReadings::~GetDcpmemReadings() {}

namespace {
constexpr const std::uint8_t OFFSET_READINGS = 4;
}

void response::GetDcpmemReadings::unpack(const IpmiInterface::ByteBuffer& data) {
    m_raw_readings = {data.begin() + OFFSET_READINGS, data.end()};
}

void response::GetDcpmemReadings::throw_error_on_completion_code(CompletionCode completion_code) const {
    switch (completion_code) {
    case OperationInProgressError::ERROR_CODE:
        throw OperationInProgressError(*this);
    case TriggerProxyAccessHaltForced::ERROR_CODE:
        throw TriggerProxyAccessHaltForced(*this);
    case DcpmemModuleInternalError::ERROR_CODE:
        throw DcpmemModuleInternalError(*this);
    case PeciCommandResponseTimeout::ERROR_CODE:
        throw PeciCommandResponseTimeout(*this);
    case PeciBadReadFscInResponse::ERROR_CODE:
        throw PeciBadReadFscInResponse(*this);
    case PeciBadWriteFscInResponse::ERROR_CODE:
        throw PeciBadWriteFscInResponse(*this);
    case CpuNotPresentError::ERROR_CODE:
        throw CpuNotPresentError(*this);
    case IllegalSmbusAddressError::ERROR_CODE:
        throw IllegalSmbusAddressError(*this);
    case SmbusTimeoutError::ERROR_CODE:
        throw SmbusTimeoutError(*this);
    case TransactionAbortedError::ERROR_CODE:
        throw TransactionAbortedError(*this);
    case UnspecifiedError::ERROR_CODE:
        throw UnspecifiedError(*this);
    default:
        break;
    };
}
