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
 * @file ipmi/command/sdv/nm/errors.hpp
 */

#pragma once



#include "ipmi/response_error.hpp"



namespace ipmi {
namespace command {
namespace sdv {

/*!
 * @brief Exception type for Operation in progress
 */
class OperationInProgressError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x82;
    static constexpr const char ERROR_STRING[] = "Operation in progress";


    explicit OperationInProgressError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    OperationInProgressError(const OperationInProgressError&) = default;


    OperationInProgressError(OperationInProgressError&&) = default;


    virtual ~OperationInProgressError();
};

/*!
 * @brief Exception type for Trigger Proxy Access operation halt was forced.
 */
class TriggerProxyAccessHaltForced : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x83;
    static constexpr const char ERROR_STRING[] = "Trigger Proxy Access operation halt was forced";


    explicit TriggerProxyAccessHaltForced(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    TriggerProxyAccessHaltForced(const TriggerProxyAccessHaltForced&) = default;


    TriggerProxyAccessHaltForced(TriggerProxyAccessHaltForced&&) = default;


    virtual ~TriggerProxyAccessHaltForced();
};

/*!
 * @brief Exception type for DCPMEM Module internal error.
 */
class DcpmemModuleInternalError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x84;
    static constexpr const char ERROR_STRING[] = "DCPMEM Module internal error";


    explicit DcpmemModuleInternalError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    DcpmemModuleInternalError(const DcpmemModuleInternalError&) = default;


    DcpmemModuleInternalError(DcpmemModuleInternalError&&) = default;


    virtual ~DcpmemModuleInternalError();
};

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
 * @brief Exception type for PECI Command response timeout.
 */
class PeciCommandResponseTimeout : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xA2;
    static constexpr const char ERROR_STRING[] = "PECI Command response timeout";


    explicit PeciCommandResponseTimeout(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    PeciCommandResponseTimeout(const PeciCommandResponseTimeout&) = default;


    PeciCommandResponseTimeout(PeciCommandResponseTimeout&&) = default;


    virtual ~PeciCommandResponseTimeout();
};

/*!
 * @brief Exception type for inband PECI interface is down.
 */
class InvalidPeciInterfaceState : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xA3;
    static constexpr const char ERROR_STRING[] = "Inband PECI interface is down, response data invalid";


    explicit InvalidPeciInterfaceState(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidPeciInterfaceState(const InvalidPeciInterfaceState&) = default;


    InvalidPeciInterfaceState(InvalidPeciInterfaceState&&) = default;


    virtual ~InvalidPeciInterfaceState();
};

/*!
 * @brief Exception type for PECI bad read FSC in the response.
 */
class PeciBadReadFscInResponse : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xA4;
    static constexpr const char ERROR_STRING[] = "PECI bad read FSC in the response";


    explicit PeciBadReadFscInResponse(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    PeciBadReadFscInResponse(const PeciBadReadFscInResponse&) = default;


    PeciBadReadFscInResponse(PeciBadReadFscInResponse&&) = default;


    virtual ~PeciBadReadFscInResponse();
};

/*!
 * @brief Exception type for PECI bad write FSC in the response.
 */
class PeciBadWriteFscInResponse : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xA5;
    static constexpr const char ERROR_STRING[] = "PECI bad write FSC in the response";


    explicit PeciBadWriteFscInResponse(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    PeciBadWriteFscInResponse(const PeciBadWriteFscInResponse&) = default;


    PeciBadWriteFscInResponse(PeciBadWriteFscInResponse&&) = default;


    virtual ~PeciBadWriteFscInResponse();
};

/*!
 * @brief Exception type for SMBus error: SMBus timeout.
 */
class SmbusTimeoutError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xAA;
    static constexpr const char ERROR_STRING[] = "SMBus error: SMBus timeout";


    explicit SmbusTimeoutError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    SmbusTimeoutError(const SmbusTimeoutError&) = default;


    SmbusTimeoutError(SmbusTimeoutError&&) = default;


    virtual ~SmbusTimeoutError();
};

/*!
 * @brief Exception type for CPU not present.
 */
class CpuNotPresentError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xAC;
    static constexpr const char ERROR_STRING[] = "CPU not present";


    explicit CpuNotPresentError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    CpuNotPresentError(const CpuNotPresentError&) = default;


    CpuNotPresentError(CpuNotPresentError&&) = default;


    virtual ~CpuNotPresentError();
};

/*!
 * @brief Exception type for SMBus error: illegal SMBus address.
 */
class IllegalSmbusAddressError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xAD;
    static constexpr const char ERROR_STRING[] = "SMBus error: illegal SMBus address";


    explicit IllegalSmbusAddressError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    IllegalSmbusAddressError(const IllegalSmbusAddressError&) = default;


    IllegalSmbusAddressError(IllegalSmbusAddressError&&) = default;


    virtual ~IllegalSmbusAddressError();
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
 * @brief Exception type for Smart & CLST event happened and the transaction has been aborted - retry needed.
 */
class TransactionAbortedError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xD7;
    static constexpr const char ERROR_STRING[] =
        "Smart & CLST event happened and the transaction has been aborted - retry needed";


    explicit TransactionAbortedError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    TransactionAbortedError(const TransactionAbortedError&) = default;


    TransactionAbortedError(TransactionAbortedError&&) = default;


    virtual ~TransactionAbortedError();
};

}
}
}
