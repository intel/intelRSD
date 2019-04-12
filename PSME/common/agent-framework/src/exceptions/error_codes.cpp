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
 * @file error_codes.cpp
 *
 * @brief Error codes
 * */

#include "agent-framework/exceptions/error_codes.hpp"

bool agent_framework::exceptions::is_communication_error(ErrorCode error_code){
    if (
        error_code == ErrorCode::INVALID_PARAMETER ||
        error_code == ErrorCode::INVALID_FIELD ||
        error_code == ErrorCode::DUPLICATED_FIELD ||
        error_code == ErrorCode::UNEXPECTED_FIELD ||
        error_code == ErrorCode::MISSING_FIELD
    ) {
        return true;
    }
    return false;
}
