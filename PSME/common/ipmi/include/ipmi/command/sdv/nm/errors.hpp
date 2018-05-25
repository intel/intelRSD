/*!
 * @brief Response Errors for TelemetryHub.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @file ipmi/command/sdv/nm/errors.hpp
 */

#pragma once



#include "ipmi/response_error.hpp"



namespace ipmi {
namespace command {
namespace sdv {

/*!
 * @brief Exception type for Invalid Package GUID.
 */
class InvalidPackageGuid : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x90;
    static constexpr const char ERROR_STRING[] = "Invalid Package GUID";


    explicit InvalidPackageGuid(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidPackageGuid(const InvalidPackageGuid&) = default;


    InvalidPackageGuid(InvalidPackageGuid&&) = default;


    virtual ~InvalidPackageGuid();
};

/*!
 * @brief Exception type for Invalid Metric GUID.
 */
class InvalidMetricGuid : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x91;
    static constexpr const char ERROR_STRING[] = "Invalid Metric GUID";


    explicit InvalidMetricGuid(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidMetricGuid(const InvalidMetricGuid&) = default;


    InvalidMetricGuid(InvalidMetricGuid&&) = default;


    virtual ~InvalidMetricGuid();
};

/*!
 * @brief Exception type for Invalid reporting interval.
 */
class InvalidReportingInterval : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x93;
    static constexpr const char ERROR_STRING[] = "Invalid Reporting Interval";


    explicit InvalidReportingInterval(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidReportingInterval(const InvalidReportingInterval&) = default;


    InvalidReportingInterval(InvalidReportingInterval&&) = default;


    virtual ~InvalidReportingInterval();
};

/*!
 * @brief Exception type for Invalid grouping operation.
 */
class InvalidGroupingOperation : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x94;
    static constexpr const char ERROR_STRING[] = "Invalid Grouping Operation";


    explicit InvalidGroupingOperation(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidGroupingOperation(const InvalidGroupingOperation&) = default;


    InvalidGroupingOperation(InvalidGroupingOperation&&) = default;


    virtual ~InvalidGroupingOperation();
};

/*!
 * @brief Exception type for Invalid reading package command.
 */
class InvalidReadingPackageCommand : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x95;
    static constexpr const char ERROR_STRING[] = "Invalid Reading Package Command";


    explicit InvalidReadingPackageCommand(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidReadingPackageCommand(const InvalidReadingPackageCommand&) = default;


    InvalidReadingPackageCommand(InvalidReadingPackageCommand&&) = default;


    virtual ~InvalidReadingPackageCommand();
};

/*!
 * @brief Exception type for Invalid persistency parameter.
 */
class InvalidPersistencyParameter : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x96;
    static constexpr const char ERROR_STRING[] = "Invalid Persistency Parameter";


    explicit InvalidPersistencyParameter(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidPersistencyParameter(const InvalidPersistencyParameter&) = default;


    InvalidPersistencyParameter(InvalidPersistencyParameter&&) = default;


    virtual ~InvalidPersistencyParameter();
};

/*!
 * @brief Exception type for Invalid reading index.
 */
class InvalidReadingIndex : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x92;
    static constexpr const char ERROR_STRING[] = "Reading index exceeds the number of elements";


    explicit InvalidReadingIndex(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidReadingIndex(const InvalidReadingIndex&) = default;


    InvalidReadingIndex(InvalidReadingIndex&&) = default;


    virtual ~InvalidReadingIndex();
};

/*!
 * @brief Exception type for wrong CPU number.
 */
class InvalidCpuNumber : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xA1;
    static constexpr const char ERROR_STRING[] = "Wrong CPU number";


    explicit InvalidCpuNumber(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidCpuNumber(const InvalidCpuNumber&) = default;


    InvalidCpuNumber(InvalidCpuNumber&&) = default;


    virtual ~InvalidCpuNumber();
};

/*!
 * @brief Exception type for inband PCI interface is down.
 */
class InvalidPciInterfaceState : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xA3;
    static constexpr const char ERROR_STRING[] = "Inband PCI interface is down, response data invalid";


    explicit InvalidPciInterfaceState(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidPciInterfaceState(const InvalidPciInterfaceState&) = default;


    InvalidPciInterfaceState(InvalidPciInterfaceState&&) = default;


    virtual ~InvalidPciInterfaceState();
};

/*!
 * @brief Exception type for platform not in S0/S1 state.
 */
class InvalidPlatformState : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xD5;
    static constexpr const char ERROR_STRING[] = "Platform not in S0/S1 state";


    explicit InvalidPlatformState(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidPlatformState(const InvalidPlatformState&) = default;


    InvalidPlatformState(InvalidPlatformState&&) = default;


    virtual ~InvalidPlatformState();
};

/*!
 * @brief Exception type for unsupported length of response for underlying transport
 */
class InvalidResponseLength : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xAD;
    static constexpr const char ERROR_STRING[] =
        "Response cannot be delivered because its length is not supported for underlying transport";


    explicit InvalidResponseLength(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidResponseLength(const InvalidResponseLength&) = default;


    InvalidResponseLength(InvalidResponseLength&&) = default;


    virtual ~InvalidResponseLength();
};

}
}
}
