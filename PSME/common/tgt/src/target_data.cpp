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

#include "tgt/target_data.hpp"

using namespace tgt;

void LunData::set_lun(const std::uint64_t lun) {
    m_lun = lun;
}

std::uint64_t LunData::get_lun() const {
    return m_lun;
}

void LunData::set_device_path(const std::string& device_path) {
    m_device_path = device_path;
}

const std::string& LunData::get_device_path() const {
    return m_device_path;
}

void TargetData::set_target_id(const std::int32_t target_id) {
    m_target_id = target_id;
}

std::int32_t TargetData::get_target_id() const {
    return m_target_id;
}

void TargetData::set_target_iqn(const std::string& target_iqn) {
    m_target_iqn = target_iqn;
}

const std::string& TargetData::get_target_iqn() const {
    return m_target_iqn;
}

void TargetData::set_target_initiator(const std::string& initiator) {
    m_target_initiator = initiator;
}

const std::string& TargetData::get_target_initiator() const {
    return m_target_initiator;
}

void TargetData::set_authentication_method(const std::string& authentication_method) {
    m_authentication_method = authentication_method;
}

const std::string& TargetData::get_authentication_method() const {
    return m_authentication_method;
}

void TargetData::set_chap_username(const std::string& username) {
    m_chap_username = username;
}

const std::string& TargetData::get_chap_username() const {
    return m_chap_username;
}

void TargetData::set_mutual_chap_username(const std::string& username) {
    m_mutual_chap_username = username;
}

const std::string& TargetData::get_mutual_chap_username() const {
    return m_mutual_chap_username;
}

void TargetData::set_chap_password(const std::string& password) {
    m_chap_password = password;
}

void TargetData::set_mutual_chap_password(const std::string& password) {
    m_mutual_chap_password = password;
}

void TargetData::add_lun_data(std::shared_ptr<LunData> lun) {
    m_luns.push_back(lun);
}

const LunDataCollection& TargetData::get_luns() const {
    return m_luns;
}
