/*!
 * @brief Response object from IPMI.
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation.
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
 * @file ipmi/response.cpp
 */

#include "ipmi/response.hpp"
#include "ipmi/response_error.hpp"

#include <sstream>
#include <iomanip>

using namespace ipmi;

Response::Response(IpmiInterface::NetFn fn, IpmiInterface::Cmd cmd, std::size_t min_rsp_size):
    Message(fn, cmd), minimal_response_length(min_rsp_size) { }

Response::~Response() { }

void Response::validate_completion_code(const IpmiInterface::ByteBuffer& data) const {
    if (data.empty()) {
        throw ResponseLengthError(*this, minimal_response_length, 0);
    }
    /*
     * response has completion code as first byte.
     */
    const auto completion_code = data[ipmi::Response::OFFSET_COMPLETION_CODE];
    if (completion_code) {
        throw_error_on_completion_code(completion_code);
        Response::throw_error_on_completion_code(completion_code);
    }
}

void Response::validate_response_size(const IpmiInterface::ByteBuffer& data) const {
    if (data.size() < minimal_response_length) {
        throw ResponseLengthError(*this, minimal_response_length, data.size());
    }
}

void Response::do_unpack(const IpmiInterface::ByteBuffer& data) {
    validate_completion_code(data);
    validate_response_size(data);
    unpack(data);
}

void Response::throw_error_on_completion_code(CompletionCode completion_code) const {
    switch (completion_code) {
    case NodeBusyError::ERROR_CODE:
        throw NodeBusyError(*this);
    case InvalidCommandError::ERROR_CODE:
        throw InvalidCommandError(*this);
    case TimeOutError::ERROR_CODE:
        throw TimeOutError(*this);
    case OutOfSpaceError::ERROR_CODE:
        throw OutOfSpaceError(*this);
    case RequestedRecordNotPresentError::ERROR_CODE:
        throw RequestedRecordNotPresentError(*this);
    case InvalidDataFieldError::ERROR_CODE:
        throw InvalidDataFieldError(*this);
    case DestinationUnavailableError::ERROR_CODE:
        throw DestinationUnavailableError(*this);
    case CommandNotSupportedInPresentStateError::ERROR_CODE:
        throw CommandNotSupportedInPresentStateError(*this);
    case RequestDataLengthInvalidError::ERROR_CODE:
        throw RequestDataLengthInvalidError(*this);
    case RequestDataTruncatedError::ERROR_CODE:
        throw RequestDataTruncatedError(*this);
    case ReservationError::ERROR_CODE:
        throw ReservationError(*this);
    case UnspecifiedError::ERROR_CODE:
        throw UnspecifiedError(*this);
    case ParameterNotSupportedError::ERROR_CODE:
        throw ParameterNotSupportedError(*this);
    case UpdateInProgressError::ERROR_CODE:
        throw UpdateInProgressError(*this);
    default:
        std::ostringstream error{};
        error << " completion code = 0x" << std::hex << static_cast<unsigned>(completion_code);
        throw ResponseError(*this, error.str(), completion_code);
    }
}
