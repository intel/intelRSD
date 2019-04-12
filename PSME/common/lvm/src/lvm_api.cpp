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
 * @file lvm_api.cpp
 * @brief C++ wrapper for liblvm
 * */

#include "lvm/lvm_api.hpp"



using namespace storage::lvm;



#include "lvm_api_impl.cpp"



std::mutex LvmApi::g_lvm_mutex{};


LvmApi::LvmApi() : m_impl{new LvmApi::LvmImpl} {}


LvmApi::~LvmApi() {}


LvmApi::VolumeGroups LvmApi::discover_volume_groups(DiscoveryType discovery_type) {
    std::lock_guard<std::mutex> lock{g_lvm_mutex};
    return m_impl->discover_volume_groups(discovery_type);
}


model::LogicalVolume LvmApi::create_snapshot(const model::CreationData& data) {
    std::lock_guard<std::mutex> lock{g_lvm_mutex};
    return m_impl->create_snapshot(data);
}


model::LogicalVolume LvmApi::create_volume(const model::CreationData& data) {
    std::lock_guard<std::mutex> lock{g_lvm_mutex};
    return m_impl->create_volume(data);
}

model::LogicalVolume LvmApi::resize_volume(const std::string& vg_name, const std::string& lv_name, std::uint64_t size) {
    std::lock_guard<std::mutex> lock{g_lvm_mutex};
    return m_impl->resize_volume(vg_name, lv_name, size);
}

void LvmApi::remove_logical_volume(const std::string& vg_name, const std::string& lg_name) {
    std::lock_guard<std::mutex> lock{g_lvm_mutex};
    m_impl->remove_logical_volume(vg_name, lg_name);
}


void LvmApi::add_logical_volume_tag(const std::string& vg_name, const std::string& lv_name, const std::string& tag) {
    std::lock_guard<std::mutex> lock{g_lvm_mutex};
    m_impl->add_lv_tag(vg_name, lv_name, tag);
}


void LvmApi::remove_logical_volume_tag(const std::string& vg_name, const std::string& lv_name, const std::string& tag) {
    std::lock_guard<std::mutex> lock{g_lvm_mutex};
    m_impl->remove_lv_tag(vg_name, lv_name, tag);
}
