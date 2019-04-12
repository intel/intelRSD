/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file lvm_api_impl.hpp
 * @brief LVM API implementation
 * */

#pragma once



#include "lvm/lvm_api.hpp"
#include "lvm.hpp"



namespace storage {
namespace lvm {

class LvmApi::LvmImpl {
public:
    using LvmPtr = std::unique_ptr<::lvm, decltype(&lvm_quit)>;
    using VolumeGroupPtr = std::unique_ptr<::volume_group, decltype(&lvm_vg_close)>;


    explicit LvmImpl();


    LvmApi::VolumeGroups discover_volume_groups(DiscoveryType discovery_type);


    model::LogicalVolume create_snapshot(const model::CreationData&);


    model::LogicalVolume create_volume(const model::CreationData&);


    model::LogicalVolume resize_volume(const std::string& vg_name,
                                       const std::string& lv_name,
                                       std::uint64_t size);


    void remove_logical_volume(const std::string& vg_name, const std::string& lv_name);


    void add_lv_tag(const std::string& vg_name, const std::string& lv_name, const std::string& tag);


    void remove_lv_tag(const std::string& vg_name, const std::string& lv_name, const std::string& tag);


private:
    LvmPtr m_lvm;


    VolumeGroupPtr open_volume_group(lvm_t lvm_handle, const std::string& name);


    lv_t open_logical_volume(vg_t vg_handle, const std::string& name);


    std::uint64_t get_lv_size(const lv_t lv_handle) const;


    void save_lvm_changes(vg_t vg_handle);
};

}
}