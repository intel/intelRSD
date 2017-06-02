/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file exception.hpp
 *
 * @brief LLDP API exception
 * */

#include "api/lldp/exceptions/api_error.hpp"
#include "api/lldp/exceptions/err_code.hpp"

using std::string;
using std::runtime_error;
using namespace agent::network::api::lldp::exception;

ApiError::ApiError(const string& what) : runtime_error{what} { }

ApiError::ApiError(ErrCode code, const string& what) :
    m_code{code}, runtime_error{what} { }

ApiError::~ApiError() { }
