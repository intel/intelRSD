/*!
 * @brief Intel Optane DC Persistent Memory Trigger Proxy Access IPMI command request and response implementation.
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
 * @file sdv/nm/trigger_dcpmem_proxy_access.cpp"
 */

#include "ipmi/command/sdv/nm/trigger_dcpmem_proxy_access.hpp"
#include "ipmi/command/sdv/nm/errors.hpp"
#include "ipmi/command/sdv/enums.hpp"

using namespace ipmi;
using namespace ipmi::command::sdv;

request::TriggerDcpmemProxyAccess::TriggerDcpmemProxyAccess()
    : Request(NetFn::INTEL_NM, Cmd::TRIGGER_DCPMEM_PROXY_ACCESS) {}

request::TriggerDcpmemProxyAccess::~TriggerDcpmemProxyAccess() {}

void request::TriggerDcpmemProxyAccess::pack(IpmiInterface::ByteBuffer& data) const {
    data.insert(data.end(), INTEL_IANA_LSB_FIRST.begin(), INTEL_IANA_LSB_FIRST.end());
    data.push_back(std::uint8_t(m_readings_id));
    data.push_back(m_readings_subid);
    data.push_back(m_opcode);
    data.push_back(m_subopcode);
    data.push_back(m_write_data_offset);
    data.push_back(m_write_data_size);
    data.push_back(m_read_data_offset);
    data.push_back(m_read_data_size);
}

response::TriggerDcpmemProxyAccess::TriggerDcpmemProxyAccess()
    : Response(NetFn::INTEL_NM + 1, Cmd::TRIGGER_DCPMEM_PROXY_ACCESS, RESPONSE_SIZE) {}


response::TriggerDcpmemProxyAccess::~TriggerDcpmemProxyAccess() {}

void response::TriggerDcpmemProxyAccess::unpack(const IpmiInterface::ByteBuffer&) {
    // TODO: anything to do here? only INTEL_IANA in response
}

void response::TriggerDcpmemProxyAccess::throw_error_on_completion_code(CompletionCode completion_code) const {
    switch (completion_code) {
    case OperationInProgressError::ERROR_CODE:
        throw OperationInProgressError(*this);
    case InvalidCpuNumber::ERROR_CODE:
        throw InvalidCpuNumber(*this);
    case InvalidResponseLength::ERROR_CODE:
        throw InvalidResponseLength(*this);
    case UnspecifiedError::ERROR_CODE:
        throw UnspecifiedError(*this);
    default:
        break;
    };
}