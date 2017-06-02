/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file response.cpp
 *
 * @brief Response object from IPMI.
 * */
#include "ipmi/response.hpp"

using namespace ipmi;

Response::Response(NetFn fn, Cmd cmd, std::size_t rsp_size) : Message(fn, cmd), DATA_SIZE(rsp_size) {}

Response::~Response() {}

Response::COMPLETION_CODE Response::get_completion_code() const {
    return m_completion_code;
}

bool Response::is_response_correct(const std::vector<std::uint8_t>& data) {
    m_completion_code = COMPLETION_CODE(data[OFFSET_COMPLETION_CODE]);

    bool retval = true;
    if(ERROR_DATA_SIZE == data.size()) {
        retval = false;
    }
    else if (data.size() < DATA_SIZE) {
        throw std::runtime_error(("Cannot unpack response. Data length too short."
                             " Expected: ") + std::to_string(DATA_SIZE)
                           + " Received: " + std::to_string(data.size()));
    }
    return retval;
}

