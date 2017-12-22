/*!
 * @brief Response Errors for TelemetryHub.
 *
 * @header{License}
 * @copyright Copyright (c) 2017 Intel Corporation.
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
 * @header{Filesystem}
 * @file ipmi/command/sdv/nm/errors.cpp
 */

#include "ipmi/command/sdv/nm/errors.hpp"


using ipmi::CompletionCode;
using namespace ipmi::command::sdv;

constexpr const char InvalidPackageGuid::ERROR_STRING[];
constexpr const char InvalidMetricGuid::ERROR_STRING[];
constexpr const char InvalidReportingInterval::ERROR_STRING[];
constexpr const char InvalidPersistencyParameter::ERROR_STRING[];
constexpr const char InvalidGroupingOperation::ERROR_STRING[];
constexpr const char InvalidReadingPackageCommand::ERROR_STRING[];
constexpr const char InvalidReadingIndex::ERROR_STRING[];
constexpr const char InvalidCpuNumber::ERROR_STRING[];
constexpr const char InvalidPciInterfaceState::ERROR_STRING[];
constexpr const char InvalidPlatformState::ERROR_STRING[];
constexpr const char InvalidResponseLength::ERROR_STRING[];
constexpr const CompletionCode InvalidPackageGuid::ERROR_CODE;
constexpr const CompletionCode InvalidMetricGuid::ERROR_CODE;
constexpr const CompletionCode InvalidReportingInterval::ERROR_CODE;
constexpr const CompletionCode InvalidPersistencyParameter::ERROR_CODE;
constexpr const CompletionCode InvalidGroupingOperation::ERROR_CODE;
constexpr const CompletionCode InvalidReadingPackageCommand::ERROR_CODE;
constexpr const CompletionCode InvalidReadingIndex::ERROR_CODE;
constexpr const CompletionCode InvalidCpuNumber::ERROR_CODE;
constexpr const CompletionCode InvalidPciInterfaceState::ERROR_CODE;
constexpr const CompletionCode InvalidPlatformState::ERROR_CODE;
constexpr const CompletionCode InvalidResponseLength::ERROR_CODE;

InvalidPackageGuid::~InvalidPackageGuid() {}

InvalidMetricGuid::~InvalidMetricGuid() {}

InvalidPersistencyParameter::~InvalidPersistencyParameter() {}

InvalidReadingPackageCommand::~InvalidReadingPackageCommand() {}

InvalidReportingInterval::~InvalidReportingInterval() {}

InvalidGroupingOperation::~InvalidGroupingOperation() {}

InvalidReadingIndex::~InvalidReadingIndex() {}

InvalidCpuNumber::~InvalidCpuNumber() {}

InvalidPciInterfaceState::~InvalidPciInterfaceState() {}

InvalidPlatformState::~InvalidPlatformState() {}

InvalidResponseLength::~InvalidResponseLength() {}
