/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * */

#include "agent-framework/exceptions/exception.hpp"
#include "lvm/lvm_attribute.hpp"
#include "lvm/lvm_discovery.hpp"
#include "lvm/model/creation_data.hpp"



extern "C" {
#include <lvm2app.h>
}

using LvmPtr = std::unique_ptr<lvm, decltype(&lvm_quit)>;
using VolumeGroupPtr = std::unique_ptr<volume_group, decltype(&lvm_vg_close)>;

class LvmApi::LvmImpl {
public:
    explicit LvmImpl();


    LvmApi::VolumeGroups discover_volume_groups(DiscoveryType discovery_type);


    model::LogicalVolume create_snapshot(const model::CreationData&);


    model::LogicalVolume create_volume(const model::CreationData&);


    void remove_logical_volume(const std::string& vg_name,
                               const std::string& lv_name);


    void add_lv_tag(const std::string& vg_name, const std::string& lv_name, const std::string& tag);


    void remove_lv_tag(const std::string& vg_name, const std::string& lv_name, const std::string& tag);


private:
    LvmPtr m_lvm;


    VolumeGroupPtr open_volume_group(lvm_t lvm_handle, const std::string& name);


    lv_t open_logical_volume(vg_t vg_handle, const std::string& name);


    std::uint64_t get_lv_size(const lv_t lv_handle) const;


    void save_lvm_changes(vg_t vg_handle);
};


LvmApi::LvmImpl::LvmImpl() : m_lvm{lvm_init(nullptr), lvm_quit} {
    if (!m_lvm) {
        THROW(agent_framework::exceptions::LvmError, "lvm", "Could not create lvm handle!");
    }
}


LvmApi::VolumeGroups LvmApi::LvmImpl::discover_volume_groups(DiscoveryType discovery_type) {
    LvmDiscovery ld{m_lvm.get()};
    return ld.discover(discovery_type);
}


model::LogicalVolume LvmApi::LvmImpl::create_snapshot(const model::CreationData& data) {
    auto vg_handle = open_volume_group(m_lvm.get(), data.get_volume_group());
    auto lv_handle = open_logical_volume(vg_handle.get(), data.get_logical_volume());

    auto snapshot_handle = lvm_lv_snapshot(lv_handle, data.get_create_name().c_str(), data.get_size());
    if (!snapshot_handle) {
        THROW(agent_framework::exceptions::LvmError, "lvm",
              "Could not create snapshot " + data.get_create_name() + ".");
    }

    if (data.get_bootable()) {
        lvm_lv_add_tag(snapshot_handle, attribute::LV_BOOTABLE_ATTR);
    }

    // writing changes to VG just in case we set the bootable flag
    save_lvm_changes(vg_handle.get());
    return LvmDiscovery{m_lvm.get()}.make_logical_volume(vg_handle.get(), snapshot_handle);
}


model::LogicalVolume LvmApi::LvmImpl::create_volume(const model::CreationData& data) {
    auto vg_handle = open_volume_group(m_lvm.get(), data.get_volume_group());

    auto name = data.get_create_name();
    auto new_volume_handle = lvm_vg_create_lv_linear(vg_handle.get(), name.c_str(), data.get_size());
    if (!new_volume_handle) {
        THROW(agent_framework::exceptions::LvmError, "lvm", "Could not create new volume " + name + "!");
    }

    if (data.get_bootable()) {
        lvm_lv_add_tag(new_volume_handle, attribute::LV_BOOTABLE_ATTR);
    }

    // writing changes to VG just in case we set the bootable flag
    save_lvm_changes(vg_handle.get());
    return LvmDiscovery{m_lvm.get()}.make_logical_volume(vg_handle.get(), new_volume_handle);
}


void LvmApi::LvmImpl::remove_logical_volume(const std::string& vg_name,
                                            const std::string& lv_name) {
    auto vg_handle = open_volume_group(m_lvm.get(), vg_name);
    auto lv_handle = open_logical_volume(vg_handle.get(), lv_name);

    auto ret = lvm_vg_remove_lv(lv_handle);
    if (ret) {
        THROW(agent_framework::exceptions::LvmError, "lvm", "Could not remove lvm " + lv_name + "!");
    }
    save_lvm_changes(vg_handle.get());
}


/* Private functions */
VolumeGroupPtr LvmApi::LvmImpl::open_volume_group(lvm_t lvm_handle, const std::string& name) {
    auto vg_handle = lvm_vg_open(lvm_handle,
                                 name.c_str(),
                                 agent::storage::lvm::attribute::WRITE_MODE,
                                 agent::storage::lvm::attribute::FLAGS);
    if (!vg_handle) {
        THROW(agent_framework::exceptions::LvmError, "lvm",
              "Could not open volume group " + name + "!");
    }
    return VolumeGroupPtr{vg_handle, lvm_vg_close};
}


lv_t LvmApi::LvmImpl::open_logical_volume(vg_t vg_handle, const std::string& name) {
    auto lv_handle = lvm_lv_from_name(vg_handle, name.c_str());
    if (!lv_handle) {
        THROW(agent_framework::exceptions::LvmError, "lvm",
              "Could not open logical volume " + name + "!");
    }

    return lv_handle;
}


void LvmApi::LvmImpl::save_lvm_changes(vg_t vg_handle) {
    auto ret = lvm_vg_write(vg_handle);
    if (ret) {
        THROW(agent_framework::exceptions::LvmError, "lvm", "Could not save changes to volume group!");
    }
}


void LvmApi::LvmImpl::add_lv_tag(const std::string& vg_name, const std::string& lv_name, const std::string& tag) {
    auto vg_handle = open_volume_group(m_lvm.get(), vg_name);
    auto lv_handle = open_logical_volume(vg_handle.get(), lv_name);
    auto ret = lvm_lv_add_tag(lv_handle, tag.c_str());
    if (ret) {
        THROW(agent_framework::exceptions::LvmError, "lvm", "Could not add tag to logical volume " + lv_name + "!");
    }
    save_lvm_changes(vg_handle.get());
}


void LvmApi::LvmImpl::remove_lv_tag(const std::string& vg_name, const std::string& lv_name, const std::string& tag) {
    auto vg_handle = open_volume_group(m_lvm.get(), vg_name);
    auto lv_handle = open_logical_volume(vg_handle.get(), lv_name);
    auto ret = lvm_lv_remove_tag(lv_handle, tag.c_str());
    if (ret) {
        THROW(agent_framework::exceptions::LvmError, "lvm", "Could not remove tag from logical volume " + lv_name + "!");
    }
    save_lvm_changes(vg_handle.get());
}
