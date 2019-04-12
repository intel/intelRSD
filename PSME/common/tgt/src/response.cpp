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

#include "tgt/response.hpp"

using namespace tgt;

char* Response::data() {
    return reinterpret_cast<char*>(&m_response_data);
}

std::size_t Response::get_response_pod_size() const {
    return sizeof(m_response_data);
}

ErrorType Response::get_error() const {
    return ErrorType(m_response_data.m_error);
}

bool Response::is_valid() const {
    return ErrorType::SUCCESS == get_error();
}

std::uint32_t Response::get_length() const {
    return m_response_data.m_length - static_cast<std::uint32_t>(get_response_pod_size());
}

Response::extra_data_t& Response::get_extra_data() {
    return m_extra_data;
}
