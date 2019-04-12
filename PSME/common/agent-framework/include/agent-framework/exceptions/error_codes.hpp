/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @brief GAMI error codes specification
 * */

#pragma once

namespace agent_framework {
namespace exceptions {

/*!
 * @enum ErrorCode
 * @brief Error codes
 *
 * @var ErrorCode::INVALID_PARAMETER
 * Invalid parameter error code provided by JSON RPC library
 *
 * @var ErrorCode::METHOD_NOT_FOUND
 * RPC method not found error code provided by JSON RPC library
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
 * @var ErrorCode::METHOD_NOT_ALLOWED
 * Error code for not allowed method
 *
 * @var ErrorCode::METHOD_CONFLICT
 * Error code for when a method conflicts with an ongoing asynchronous action
 *
 * @var ErrorCode::UNSUPPORTED_VALUE
 * A value is not supported
 *
 * @var ErrorCode::UNSUPPORTED_FIELD
 * A field is not supported
 *
 * @var ErrorCode::INVALID_VALUE
 * A value is invalid
 *
 * @var ErrorCode::INVALID_FIELD
 * A field is invalid
 *
 * @var ErrorCode::INVALID_UUID
 * Passed UUID is not valid
 *
 * @var ErrorCode::COMPUTE
 * Compute Agent errors
 *
 * @var ErroCode::IPMI
 * Communication error via IPMI
 *
 * @var ErrorCode::NETWORK
 * Network Agent error
 *
 * @var ErrorCode::FM10000
 * Fm10000 switch error code
 *
 * @var ErrorCode::STORAGE
 * Storage Agent error
 *
 * @var ErrorCode::ISCSI
 * iSCSI error code
 *
 * @var ErrorCode::LVM
 * Lvm error code
 *
 * @var ErrorCode::PCIE_FABRIC
 * PCIe Fabric (PNC) Agent error
 *
 * @var ErrorCode::CHASSIS
 * Chassis Agent error
 *
 * @var ErrorCode::CERTIFICATE
 * Problems with certificate
 * */
enum class ErrorCode {
    INVALID_PARAMETER = -32602,
    METHOD_NOT_FOUND = -32601,
    UNKNOWN_ERROR = -1,

    NOT_FOUND = 10000,
    NOT_IMPLEMENTED = 11000,
    METHOD_NOT_ALLOWED = 12000,
    METHOD_CONFLICT = 13000,

    INVALID_VALUE = 14000,
    UNSUPPORTED_VALUE = 14100,
    INVALID_COLLECTION = 14200,
    INVALID_ENUM = 14300,
    INVALID_VALUE_FORMAT = 14400,
    INVALID_UUID = 14500,

    INVALID_FIELD = 15000,
    UNSUPPORTED_FIELD = 15100,
    DUPLICATED_FIELD = 15200,
    INVALID_FIELD_TYPE = 15300,
    UNEXPECTED_FIELD = 15400,
    MISSING_FIELD = 15500,

    COMPUTE = 20000,
    IPMI = 21000,

    NETWORK = 30000,
    FM10000 = 31000,

    STORAGE = 40000,
    ISCSI = 41000,
    LVM = 42000,
    NVME = 43000,
    SPDK = 44000,

    PCIE_FABRIC = 50000,

    CHASSIS = 60000,
    CERTIFICATE = 61000,

    FPGAOF = 70000,
};

/*!
 * @brief Check if the error code indicates an exception that should be hidden by the REST API,
 * because it refers to internal GAMI-REST communication errors.
 *
 * @param error_code error code to be checked
 * @return is it an error code of a communication error
 */
bool is_communication_error(ErrorCode error_code);

}
}

