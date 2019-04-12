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
 * @file system_dcpmem_secure_erase.hpp
 */

#pragma once
#include "agent-framework/validators/procedure_validator.hpp"


namespace psme {
namespace rest {
namespace validators {
namespace schema {

/* Schema for validating POST requests on Persistent Memory Secure Erase action. */
class SystemDcpmemSecureErasePostSchema {
public:
    static const jsonrpc::ProcedureValidator& get_procedure();
};

}
}
}
}