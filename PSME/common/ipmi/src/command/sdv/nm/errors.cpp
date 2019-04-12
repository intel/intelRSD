/*!
 * @brief Response Errors for Intel Node Manager (NM).
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
 * @file ipmi/command/sdv/nm/errors.cpp
 */

#include "ipmi/command/sdv/nm/errors.hpp"


using ipmi::CompletionCode;
using namespace ipmi::command::sdv;

constexpr const char OperationInProgressError::ERROR_STRING[];
constexpr const char TriggerProxyAccessHaltForced::ERROR_STRING[];
constexpr const char DcpmemModuleInternalError::ERROR_STRING[];
constexpr const char InvalidPackageGuid::ERROR_STRING[];
constexpr const char InvalidMetricGuid::ERROR_STRING[];
constexpr const char InvalidReportingInterval::ERROR_STRING[];
constexpr const char InvalidPersistencyParameter::ERROR_STRING[];
constexpr const char InvalidGroupingOperation::ERROR_STRING[];
constexpr const char InvalidReadingPackageCommand::ERROR_STRING[];
constexpr const char InvalidReadingIndex::ERROR_STRING[];
constexpr const char InvalidCpuNumber::ERROR_STRING[];
constexpr const char PeciCommandResponseTimeout::ERROR_STRING[];
constexpr const char InvalidPeciInterfaceState::ERROR_STRING[];
constexpr const char PeciBadReadFscInResponse::ERROR_STRING[];
constexpr const char PeciBadWriteFscInResponse::ERROR_STRING[];
constexpr const char SmbusTimeoutError::ERROR_STRING[];
constexpr const char CpuNotPresentError::ERROR_STRING[];
constexpr const char IllegalSmbusAddressError::ERROR_STRING[];
constexpr const char InvalidResponseLength::ERROR_STRING[];
constexpr const char InvalidPlatformState::ERROR_STRING[];
constexpr const char TransactionAbortedError::ERROR_STRING[];
constexpr const CompletionCode OperationInProgressError::ERROR_CODE;
constexpr const CompletionCode TriggerProxyAccessHaltForced::ERROR_CODE;
constexpr const CompletionCode DcpmemModuleInternalError::ERROR_CODE;
constexpr const CompletionCode InvalidPackageGuid::ERROR_CODE;
constexpr const CompletionCode InvalidMetricGuid::ERROR_CODE;
constexpr const CompletionCode InvalidReportingInterval::ERROR_CODE;
constexpr const CompletionCode InvalidPersistencyParameter::ERROR_CODE;
constexpr const CompletionCode InvalidGroupingOperation::ERROR_CODE;
constexpr const CompletionCode InvalidReadingPackageCommand::ERROR_CODE;
constexpr const CompletionCode InvalidReadingIndex::ERROR_CODE;
constexpr const CompletionCode InvalidCpuNumber::ERROR_CODE;
constexpr const CompletionCode PeciCommandResponseTimeout::ERROR_CODE;
constexpr const CompletionCode InvalidPeciInterfaceState::ERROR_CODE;
constexpr const CompletionCode PeciBadReadFscInResponse::ERROR_CODE;
constexpr const CompletionCode PeciBadWriteFscInResponse::ERROR_CODE;
constexpr const CompletionCode SmbusTimeoutError::ERROR_CODE;
constexpr const CompletionCode CpuNotPresentError::ERROR_CODE;
constexpr const CompletionCode IllegalSmbusAddressError::ERROR_CODE;
constexpr const CompletionCode InvalidResponseLength::ERROR_CODE;
constexpr const CompletionCode InvalidPlatformState::ERROR_CODE;
constexpr const CompletionCode TransactionAbortedError::ERROR_CODE;

OperationInProgressError::~OperationInProgressError() {}

TriggerProxyAccessHaltForced::~TriggerProxyAccessHaltForced() {}

DcpmemModuleInternalError::~DcpmemModuleInternalError() {}

InvalidPackageGuid::~InvalidPackageGuid() {}

InvalidMetricGuid::~InvalidMetricGuid() {}

InvalidPersistencyParameter::~InvalidPersistencyParameter() {}

InvalidReadingPackageCommand::~InvalidReadingPackageCommand() {}

InvalidReportingInterval::~InvalidReportingInterval() {}

InvalidGroupingOperation::~InvalidGroupingOperation() {}

InvalidReadingIndex::~InvalidReadingIndex() {}

InvalidCpuNumber::~InvalidCpuNumber() {}

PeciCommandResponseTimeout::~PeciCommandResponseTimeout() {}

InvalidPeciInterfaceState::~InvalidPeciInterfaceState() {}

PeciBadReadFscInResponse::~PeciBadReadFscInResponse() {}

PeciBadWriteFscInResponse::~PeciBadWriteFscInResponse() {}

SmbusTimeoutError::~SmbusTimeoutError() {}

CpuNotPresentError::~CpuNotPresentError() {}

IllegalSmbusAddressError::~IllegalSmbusAddressError() {}

InvalidResponseLength::~InvalidResponseLength() {}

InvalidPlatformState::~InvalidPlatformState() {}

TransactionAbortedError::~TransactionAbortedError() {}
