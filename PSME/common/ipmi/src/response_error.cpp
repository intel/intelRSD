/*!
 * @brief Response Errors defined by IPMI specification.
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
 * @file ipmi/response_error.cpp
 */

#include "ipmi/response_error.hpp"
#include "ipmi/response.hpp"



using namespace ipmi;

constexpr const char NodeBusyError::ERROR_STRING[];
constexpr const char InvalidCommandError::ERROR_STRING[];
constexpr const char TimeOutError::ERROR_STRING[];
constexpr const char OutOfSpaceError::ERROR_STRING[];
constexpr const char InvalidDataFieldError::ERROR_STRING[];
constexpr const char DestinationUnavailableError::ERROR_STRING[];
constexpr const char CommandNotSupportedInPresentStateError::ERROR_STRING[];
constexpr const char RequestDataLengthInvalidError::ERROR_STRING[];
constexpr const char RequestDataTruncatedError::ERROR_STRING[];
constexpr const char ReservationError::ERROR_STRING[];
constexpr const char UnspecifiedError::ERROR_STRING[];
constexpr const char ParameterNotSupportedError::ERROR_STRING[];
constexpr const char UpdateInProgressError::ERROR_STRING[];
constexpr const CompletionCode NodeBusyError::ERROR_CODE;
constexpr const CompletionCode InvalidCommandError::ERROR_CODE;
constexpr const CompletionCode TimeOutError::ERROR_CODE;
constexpr const CompletionCode OutOfSpaceError::ERROR_CODE;
constexpr const CompletionCode InvalidDataFieldError::ERROR_CODE;
constexpr const CompletionCode DestinationUnavailableError::ERROR_CODE;
constexpr const CompletionCode CommandNotSupportedInPresentStateError::ERROR_CODE;
constexpr const CompletionCode RequestDataLengthInvalidError::ERROR_CODE;
constexpr const CompletionCode RequestDataTruncatedError::ERROR_CODE;
constexpr const CompletionCode ReservationError::ERROR_CODE;
constexpr const CompletionCode UnspecifiedError::ERROR_CODE;
constexpr const CompletionCode ParameterNotSupportedError::ERROR_CODE;
constexpr const CompletionCode UpdateInProgressError::ERROR_CODE;


ResponseError::ResponseError(const Response& response,
                             const std::string& what_arg,
                             CompletionCode completion_code)
    : std::runtime_error(response.get_command_name() + std::string(" failed: ") + what_arg),
      m_completion_code(completion_code),
      m_command(response.get_command()),
      m_network_function(response.get_network_function()) {}


ResponseError::~ResponseError() {}


ResponseLengthError::~ResponseLengthError() {}


NodeBusyError::~NodeBusyError() {}


InvalidCommandError::~InvalidCommandError() {}


TimeOutError::~TimeOutError() {}


OutOfSpaceError::~OutOfSpaceError() {}


constexpr const CompletionCode RequestedRecordNotPresentError::ERROR_CODE;

constexpr const char RequestedRecordNotPresentError::ERROR_STRING[];

RequestedRecordNotPresentError::~RequestedRecordNotPresentError() {}


InvalidDataFieldError::~InvalidDataFieldError() {}


DestinationUnavailableError::~DestinationUnavailableError() {}


CommandNotSupportedInPresentStateError::~CommandNotSupportedInPresentStateError() {}


RequestDataLengthInvalidError::~RequestDataLengthInvalidError() {}


RequestDataTruncatedError::~RequestDataTruncatedError() {}


ReservationError::~ReservationError() {}


UnspecifiedError::~UnspecifiedError() {}


ParameterNotSupportedError::~ParameterNotSupportedError() {}


UpdateInProgressError::~UpdateInProgressError() {}

RequestedDataLengthError::~RequestedDataLengthError() {}


constexpr const char IncorrectResponseVersionError::ERROR_STRING[];
constexpr const CompletionCode IncorrectResponseVersionError::ERROR_CODE;

IncorrectResponseVersionError::IncorrectResponseVersionError(const Response& response)
    : ResponseError(response, ERROR_STRING, ERROR_CODE) {}

IncorrectResponseVersionError::~IncorrectResponseVersionError() {}


constexpr const char SelEraseInProgressError::ERROR_STRING[];
constexpr const CompletionCode SelEraseInProgressError::ERROR_CODE;

SelEraseInProgressError::SelEraseInProgressError(const Response& response)
    : ResponseError(response, ERROR_STRING, ERROR_CODE) {}

SelEraseInProgressError::~SelEraseInProgressError() {}


constexpr const char WrongEntityError::ERROR_STRING[];
constexpr const CompletionCode WrongEntityError::ERROR_CODE;

WrongEntityError::WrongEntityError(const Response& response)
    : ResponseError(response, ERROR_STRING, ERROR_CODE) {}

WrongEntityError::~WrongEntityError() {}


constexpr const char InvalidMdrRegion::ERROR_STRING[];
constexpr const CompletionCode InvalidMdrRegion::ERROR_CODE;

InvalidMdrRegion::InvalidMdrRegion(const Response& response)
    : ResponseError(response, ERROR_STRING, ERROR_CODE) {}

InvalidMdrRegion::~InvalidMdrRegion() {}


constexpr const char MdrRegionInUse::ERROR_STRING[];
constexpr const CompletionCode MdrRegionInUse::ERROR_CODE;

MdrRegionInUse::MdrRegionInUse(const Response& response)
    : ResponseError(response, ERROR_STRING, ERROR_CODE) {}

MdrRegionInUse::~MdrRegionInUse() {}


constexpr const char MdrRegionLocked::ERROR_STRING[];
constexpr const CompletionCode MdrRegionLocked::ERROR_CODE;

MdrRegionLocked::MdrRegionLocked(const Response& response)
    : ResponseError(response, ERROR_STRING, ERROR_CODE) {}

MdrRegionLocked::~MdrRegionLocked() {}


constexpr const char MdrRegionNotLocked::ERROR_STRING[];
constexpr const CompletionCode MdrRegionNotLocked::ERROR_CODE;

MdrRegionNotLocked::MdrRegionNotLocked(const Response& response)
    : ResponseError(response, ERROR_STRING, ERROR_CODE) {}

MdrRegionNotLocked::~MdrRegionNotLocked() {}


constexpr const char MdrDataLengthError::ERROR_STRING[];

MdrDataLengthError::~MdrDataLengthError() {}


constexpr const char MdrInvalidPayloadSize::ERROR_STRING[];

MdrInvalidPayloadSize::~MdrInvalidPayloadSize() {}
