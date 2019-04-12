/*!
 * @brief Response Errors for Set and Get OOB Boot Options.
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
 * @file ipmi/command/sdv/iscsi_oob_boot/errors.hpp
 */

#pragma once

#include "ipmi/response_error.hpp"

namespace ipmi {
namespace command {
namespace sdv {

/*!
 * @brief Exception type for Invalid iSCSI OOB Parameter Version.
 */
class InvalidParameterVersion : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x00;
    static constexpr const char ERROR_STRING[] = "Invalid iSCSI OOB Parameter Version. Received version is unsupported";


    explicit InvalidParameterVersion(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidParameterVersion(const InvalidParameterVersion&) = default;


    InvalidParameterVersion(InvalidParameterVersion&&) = default;


    virtual ~InvalidParameterVersion();
};

/*!
 * @brief Exception type for Invalid Get iSCSI OOB Options Response Length.
 */
class InvalidIscsiOobResponseLength : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x00;
    static constexpr const char ERROR_STRING[] = "Unexpected data length for v2 Get iSCSI OOB Options Response packet.";


    explicit InvalidIscsiOobResponseLength(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidIscsiOobResponseLength(const InvalidIscsiOobResponseLength&) = default;


    InvalidIscsiOobResponseLength(InvalidIscsiOobResponseLength&&) = default;


    virtual ~InvalidIscsiOobResponseLength();
};


/*!
 * @brief Exception type for mismatching length field and actual response length in Get iSCSI OOB Heap Options response
 */
class HeapResponseLengthMismatch : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x00;
    static constexpr const char ERROR_STRING[] = "Data length retrieved for for Get iSCSI OOB Heap Options "
        "doesn't match length on record.";


    explicit HeapResponseLengthMismatch(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    HeapResponseLengthMismatch(const HeapResponseLengthMismatch&) = default;


    HeapResponseLengthMismatch(HeapResponseLengthMismatch&&) = default;


    virtual ~HeapResponseLengthMismatch();
};



}
}
}
