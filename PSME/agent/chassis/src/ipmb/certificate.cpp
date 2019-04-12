/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 *
 * @file certificate.cpp
 * @brief Certificate
 * */

#include "agent-framework/exceptions/exception.hpp"
#include <ipmb/certificate.hpp>
#include <algorithm>

extern "C" {
#include <md5/md5.h>
}

using namespace agent::chassis::ipmb;

namespace {
    static const constexpr char PODM_TYPE[] = "PODM";
    static const constexpr char RMM_TYPE[] = "RMM";
}

void Certificate::add_chunk(size_t cert_length, size_t chunk_number,
                            const std::vector<uint8_t>& data) {

    std::lock_guard<std::mutex> lock(m_mutex);

    if (0 == chunk_number) {
        m_length = cert_length;
        m_received = 0;
        m_chunks.clear();
    }

    if (m_chunks.find(chunk_number) != m_chunks.end()) {
        m_received -= m_chunks[chunk_number].size();
    }

    m_chunks[chunk_number] = data;
    m_received += data.size();
}

const std::vector<uint8_t>& Certificate::get_chunk(size_t chunk_number) const {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_chunks.find(chunk_number) == m_chunks.end()) {
        throw std::runtime_error("Certificate chunk not found");
    }

    return m_chunks.at(chunk_number);
}

void Certificate::make_md5_hash(const std::vector<std::uint8_t>& input,
                    std::vector<std::uint8_t>& output) const {

    output.resize(16);

    MD5_CTX context;
    MD5_Init(&context);
    MD5_Update(&context, input.data(), input.size());
    MD5_Final(output.data(), &context);
}

std::vector<uint8_t> Certificate::get_hash() const {
    auto data = get_data();
    std::vector<std::uint8_t> hash{};
    log_debug(LOGUSR, "md5 data in size: " << data.size());
    make_md5_hash(data, hash);
    return hash;
}

std::vector<std::uint8_t> Certificate::get_data() const {
    std::vector<std::uint8_t> data;
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& chunk : m_chunks) {
        std::copy(chunk.second.begin(), chunk.second.end(), std::back_inserter(data));
    }
    return data;
}

Certificate::Type Certificate::from_string(const std::string& type) {
    if (0 == type.compare(::PODM_TYPE)) {
        return Type::PODM;
    }
    else if (0 == type.compare(::RMM_TYPE)) {
        return Type::RMM;
    }
    else if (type.empty()) {
        THROW(agent_framework::exceptions::InvalidValue,
              "chassis-agent", "Empty certificate type is not valid.");
    }

    THROW(agent_framework::exceptions::InvalidValue,
          "chassis-agent", "'" + type + "' is not a valid certificate type.");
}

bool Certificate::is_complete() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return (m_received == m_length);
}
