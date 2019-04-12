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

#include "tgt/request.hpp"

#include <safe-string/safe_lib.hpp>

#include <algorithm>
#include <cstring>

using namespace tgt;

void Request::set_mode(const Mode mode) {
    m_request_data.m_mode = mode;
}

void Request::set_operation(const Operation operation) {
    m_request_data.m_operation = operation;
}

void Request::set_lld(const std::string& lld) {
    strncpy_s(m_request_data.m_lld, sizeof(m_request_data.m_lld),
              lld.c_str(), ::LLD_SIZE);
}

void Request::set_target_id(const std::int32_t target_id) {
    m_request_data.m_target_id = target_id;
}

void Request::set_sid(const std::uint64_t sid) {
    m_request_data.m_sid = sid;
}

void Request::set_lun(const std::uint64_t lun) {
    m_request_data.m_lun = lun;
}

void Request::set_cid(const std::uint32_t cid) {
    m_request_data.m_cid = cid;
}

void Request::set_host_no(const std::uint32_t host_no) {
    m_request_data.m_host_no = host_no;
}

void Request::set_device_type(const std::uint32_t device_type) {
    m_request_data.m_device_type = device_type;
}

void Request::set_ac_dir(const std::uint32_t ac_dir) {
    m_request_data.m_ac_dir = ac_dir;
}

void Request::set_pack(const std::uint32_t pack) {
    m_request_data.m_pack = pack;
}

void Request::set_force(const std::uint32_t force) {
    m_request_data.m_force = force;
}

void Request::add_extra(const std::string& data) {
    if (m_extra_data.size()) {
        m_extra_data.push_back(',');
        m_extra_data.push_back('\0');
    }
    std::copy(data.cbegin(), data.cend(), std::back_inserter(m_extra_data));
    m_extra_data.push_back('\0');
}

Mode Request::get_mode() const {
    return m_request_data.m_mode;
}

Operation Request::get_operation() const {
    return m_request_data.m_operation;
}

std::int32_t Request::get_target_id() const {
    return m_request_data.m_target_id;
}

std::uint32_t Request::get_length() const {
    return m_request_data.m_length;
}

std::uint64_t Request::get_sid() const {
    return m_request_data.m_sid;
}

std::uint64_t Request::get_lun() const {
    return m_request_data.m_lun;
}

std::uint32_t Request::get_cid() const {
    return m_request_data.m_cid;
}

std::uint32_t Request::get_host_no() const {
    return m_request_data.m_host_no;
}

std::uint32_t Request::get_device_type() const {
    return m_request_data.m_device_type;
}

std::uint32_t Request::get_ac_dir() const {
    return m_request_data.m_ac_dir;
}

std::uint32_t Request::get_pack() const {
    return m_request_data.m_pack;
}

std::uint32_t Request::get_force() const {
    return m_request_data.m_force;
}

std::vector<char> Request::get_request_data() {
    m_request_data.m_length =
        static_cast<std::uint32_t>(sizeof(RequestData) + m_extra_data.size());

    std::vector<char> data{};
    data.reserve(m_request_data.m_length);

    auto data_pointer = reinterpret_cast<const char*>(&m_request_data);
    std::copy(data_pointer,
              data_pointer + sizeof(RequestData),
              std::back_inserter(data));

    std::copy(m_extra_data.cbegin(),
              m_extra_data.cend(),
              std::back_inserter(data));

    return data;
}
