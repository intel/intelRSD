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
 */

#include "agent-framework/exceptions/exception.hpp"
#include "agent/utils/iscsi/iscsi_errors.hpp"

using namespace agent::storage::utils;

namespace {

/*! Global array for error string descriptions */
std::array<const char*, 25> g_error_array = {{
     "Success!",
     "Unknown error!",
     "Out of memory!",
     "Can't find the driver!",
     "Can't find the target!",
     "Can't find the logical unit!",
     "Can't find the session!",
     "Can't find the connection!",
     "Can't find the binding!",
     "Target already exists!",
     "Binding already exists!",
     "Logical unit number already exists!",
     "Access control rule already exists!",
     "Access control does not exists!",
     "Account already exists!",
     "Can't find the account!",
     "Too many account!",
     "Invalid request!",
     "Target already has an outgoing account!",
     "Target is still active!",
     "Logical unit is still active!",
     "Drive is busy!",
     "Operation isn't supported!",
     "Unknown parameter!",
     "Device has prevent removal set!"
}};

}

std::string IscsiErrors::get_error_message(const std::uint32_t ec) {
    if (ec < g_error_array.size()) {
        return std::string(g_error_array[ec]);
    }

    return "Unknown request error.";
}

std::string IscsiErrors::get_error_message(const tgt::ErrorType error) {
    return IscsiErrors::get_error_message(std::uint32_t(error));
}

void IscsiErrors::throw_exception(const std::uint32_t ec, const std::string& message) {
    throw_exception(tgt::ErrorType(ec), message);
}

void IscsiErrors::throw_exception(const tgt::ErrorType error, const std::string& message) {
    switch (error) {
        case tgt::ErrorType::SUCCESS:
            break;

        case tgt::ErrorType::TARGET_EXIST:
            THROW(agent_framework::exceptions::InvalidValue,
                  "storage-agent", message + get_error_message(error));
        case tgt::ErrorType::TARGET_ACTIVE:
            THROW(agent_framework::exceptions::InvalidValue,
                  "storage-agent", message + get_error_message(error));
        case tgt::ErrorType::LUN_ACTIVE:
            THROW(agent_framework::exceptions::InvalidValue,
                  "storage-agent", message + get_error_message(error));

        case tgt::ErrorType::UNKNOWN_ERROR:
        case tgt::ErrorType::NO_MEM:
        case tgt::ErrorType::NO_DRIVER:
        case tgt::ErrorType::NO_TARGET:
        case tgt::ErrorType::NO_LUN:
        case tgt::ErrorType::NO_SESSION:
        case tgt::ErrorType::NO_CONNECTION:
        case tgt::ErrorType::NO_BINDING:
        case tgt::ErrorType::ACL_EXIST:
        case tgt::ErrorType::ACL_NOEXIST:
        case tgt::ErrorType::USER_EXIST:
        case tgt::ErrorType::NO_USER:
        case tgt::ErrorType::TOO_MANY_USERS:
        case tgt::ErrorType::INVALID_REQUEST:
        case tgt::ErrorType::OUTACCOUNT_EXIST:
        case tgt::ErrorType::DRIVER_ACTIVE:
        case tgt::ErrorType::UNSUPPORTED_OPERATION:
        case tgt::ErrorType::UNKNOWN_PARAM:
        case tgt::ErrorType::PREVENT_REMOVAL:
        default:
            THROW(agent_framework::exceptions::IscsiError,
                  "storage-agent", message + get_error_message(error));
    }
}
