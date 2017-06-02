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
 * @file lvm_api.cpp
 *
 * @brief C++ wrapper for Liblvm
 * */
#include "storage_config.hpp"
#include "lvm/lvm_api.hpp"

using namespace agent::storage::lvm;

#ifdef LVM2APP_FOUND
#include "lvm_api_impl.cpp"
#else
#include "lvm_api_default.cpp"
#endif

LvmAPI::LvmAPI() : m_impl{new LvmAPI::LvmImpl} {}

LvmAPI::~LvmAPI() {}

LvmAPI::VolumeGroupVec LvmAPI::discovery_volume_groups() {
    return m_impl->discover_volume_groups();
}

void LvmAPI::create_snapshot(const LvmCreateData& data) {
    m_impl->create_snapshot(data);
}

void LvmAPI::create_clone(const LvmCreateData& data) {
    m_impl->create_clone(data);
}

void LvmAPI::remove_logical_volume(const std::string& vg_name,
                                   const std::string& lg_name) {
    m_impl->remove_logical_volume(vg_name, lg_name);
}

