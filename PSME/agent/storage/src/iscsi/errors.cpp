/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * @section DESCRIPTION
*/

#include "agent-framework/exceptions/exception.hpp"
#include "iscsi/errors.hpp"

using namespace agent::storage::iscsi::tgt;

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

std::string Errors::get_error_message(const std::uint32_t ec) {
    if (ec < g_error_array.size()) {
        return std::string(g_error_array[ec]);
    }

    return "Unknown request error.";
}

std::string Errors::get_error_message(const Errors::Types error) {
    return Errors::get_error_message(std::uint32_t(error));
}

void Errors::throw_exception(const std::uint32_t ec,
                             const std::string& message) {
    throw_exception(Errors::Types(ec), message);
}

void Errors::throw_exception(const Errors::Types error,
                             const std::string& message) {
    switch (error) {
        case Errors::Types::SUCCESS:
            break;

        case Errors::Types::TARGET_EXIST:
            THROW(agent_framework::exceptions::InvalidValue,
                  "storage-agent", message + get_error_message(error));
        case Errors::Types::TARGET_ACTIVE:
            THROW(agent_framework::exceptions::InvalidValue,
                  "storage-agent", message + get_error_message(error));
        case Errors::Types::LUN_ACTIVE:
            THROW(agent_framework::exceptions::InvalidValue,
                  "storage-agent", message + get_error_message(error));

        case Errors::Types::UNKNOWN_ERROR:
        case Errors::Types::NO_MEM:
        case Errors::Types::NO_DRIVER:
        case Errors::Types::NO_TARGET:
        case Errors::Types::NO_LUN:
        case Errors::Types::NO_SESSION:
        case Errors::Types::NO_CONNECTION:
        case Errors::Types::NO_BINDING:
        case Errors::Types::ACL_EXIST:
        case Errors::Types::ACL_NOEXIST:
        case Errors::Types::USER_EXIST:
        case Errors::Types::NO_USER:
        case Errors::Types::TOO_MANY_USERS:
        case Errors::Types::INVALID_REQUEST:
        case Errors::Types::OUTACCOUNT_EXIST:
        case Errors::Types::DRIVER_ACTIVE:
        case Errors::Types::UNSUPPORTED_OPERATION:
        case Errors::Types::UNKNOWN_PARAM:
        case Errors::Types::PREVENT_REMOVAL:
        default:
            THROW(agent_framework::exceptions::IscsiError,
                  "storage-agent", message + get_error_message(error));
    }
}
