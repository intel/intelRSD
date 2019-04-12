/*!
 * @brief Implementation of raw ipmi Request/Response.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file command/generic/raw_request.cpp
 */

#include "ipmi/command/generic/raw_request.hpp"

using namespace ipmi;
using namespace ipmi::command::generic;

namespace {

constexpr const std::size_t NETFN_CMD_SIZE = 2;

}

request::RawRequest::RawRequest(const std::string& name, IpmiInterface::ByteBuffer request)
    : Request(0, 0), m_name(name), raw(std::move(request)) {
    if (NETFN_CMD_SIZE > raw.size()) {
        throw std::runtime_error("raw command expects at least netfn and cmd");
    }
    set_network_function(raw[0]);
    set_command(raw[1]);
}

request::RawRequest::~RawRequest() {}

void request::RawRequest::pack(IpmiInterface::ByteBuffer& data) const {
    if (NETFN_CMD_SIZE < raw.size()) {
        auto begin = raw.begin();
        // skip NetFn, Cmd
        ++begin;
        ++begin;
        data.insert(data.end(), begin, raw.end());
    }
}

response::RawResponse::RawResponse(const std::string& name): Response(0, 0, 1), m_name(name) {}
response::RawResponse::~RawResponse() {}

void response::RawResponse::unpack(const IpmiInterface::ByteBuffer& data) {
    auto begin = data.begin();
    // skip CompletionCode
    ++begin;
    raw = {begin, data.end()};
}
