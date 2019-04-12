/*!
 * @brief Schema for validation POST requests for Persistent Memory Secure Erase action.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file system_dcpmem_secure_erase.cpp
 */

#include "psme/rest/validators/schemas/system_dcpmem_secure_erase.hpp"
#include "psme/rest/constants/constants.hpp"
#include "agent-framework/module/enum/compute.hpp"

using namespace psme::rest;
using namespace psme::rest::validators::schema;
using namespace agent_framework::model;

const jsonrpc::ProcedureValidator& SystemDcpmemSecureErasePostSchema::get_procedure() {
    static jsonrpc::ProcedureValidator procedure{
        jsonrpc::PARAMS_BY_NAME,
        constants::System::RESET_CONFIGURATION, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        constants::System::ERASE_CONFIGURATION_KEYS, VALID_OPTIONAL(VALID_JSON_BOOLEAN),
        nullptr
    };
    return procedure;
}