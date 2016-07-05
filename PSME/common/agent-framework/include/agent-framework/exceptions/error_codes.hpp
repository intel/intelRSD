/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 * @file error_codes.hpp
 *
 * @brief Error codes specification
 * */

#ifndef AGENT_FRAMEWORK_EXCEPTIONS_ERROR_CODES_HPP
#define AGENT_FRAMEWORK_EXCEPTIONS_ERROR_CODES_HPP

namespace agent_framework {
namespace exceptions {

/*!
 * @enum ErrorCode
 * @brief Error codes
 *
 * @var ErrorCode::INVALID_PARAMETER
 * Invalid parameter code
 *
 * @var ErrorCode::UNKNOWN_ERROR
 * Error code for unknown error
 *
 * @var ErrorCode::NOT_FOUND
 * Not found code
 *
 * @var ErrorCode::NOT_IMPLEMENTED
 * Error code for not implemented method
 *
 * @var ErrorCode::UNSUPPORTED_VALUE
 * A value is not supported
 *
 * @var ErrorCode::UNSUPPORTED_PARAMETER
 * A parameter is not supported
 *
 * @var ErrorCode::INVALID_UUID
 * UUID is not valid
 *
 * @var ErrorCode::ISCSI
 * iSCSI error code
 *
 * @var ErrorCode::LVM
 * Lvm error code
 *
 * @var ErrorCode::FM10000
 * Fm10000 switch error code
 * */
enum class ErrorCode {
    INVALID_PARAMETER = -32602,
    UNKNOWN_ERROR = -1,

    NOT_FOUND = 10000,
    NOT_IMPLEMENTED = 11000,

    UNSUPPORTED_VALUE = 12000,
    UNSUPPORTED_PARAMETER = 13000,

    INVALID_ARGUMENT = 14000,
    INVALID_UUID = 15000,

    ISCSI = 20000,
    LVM = 30000,
    FM10000 = 40000,
    CERTIFICATE = 50000
};

}
}
#endif	/* AGENT_FRAMEWORK_EXCEPTIONS_ERROR_CODES_HPP */

