/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 * */

#include "agent-framework/exceptions/exception.hpp"
#include "lvm/lvm_attribute.hpp"
#include "lvm/lvm_discovery.hpp"
#include "lvm/lvm_create_data.hpp"

extern "C" {
#include <lvm2app.h>
}

using LvmPtr = std::unique_ptr<lvm, decltype(&lvm_quit)>;
using VolumeGroupPtr = std::unique_ptr<volume_group, decltype(&lvm_vg_close)>;

class LvmAPI::LvmImpl {
public:
    explicit LvmImpl();

    LvmAPI::VolumeGroupVec discover_volume_groups();
    void create_snapshot(const LvmCreateData&);
    void create_clone(const LvmCreateData&);
    void remove_logical_volume(const std::string& vg_name,
                               const std::string& lv_name);
private:
    LvmPtr m_lvm;

    /* Private functions */
    VolumeGroupPtr open_volume_group(lvm_t lvm_handle, const std::string& name);
    lv_t open_logical_volume(vg_t vg_handle, const std::string& name);
    lv_t create_snapshot(lv_t lv_handle, const std::string& name,
                                         const std::uint64_t size,
                                         const bool bootable);
    lv_t create_clone(vg_t vg_handle, const std::string& name,
                                      const std::uint64_t size,
                                      const bool bootable);
    std::uint64_t get_lv_size(const lv_t lv_handle) const;
    void remove_lvm(lv_t lvm);
    void save_lvm_changes(vg_t vg_handle);
};

LvmAPI::LvmImpl::LvmImpl() : m_lvm{lvm_init(nullptr), lvm_quit} {
    if (!m_lvm) {
        THROW(::agent_framework::exceptions::LvmError, "lvm",
          "Could not create lvm handle!");
    }
}

LvmAPI::VolumeGroupVec LvmAPI::LvmImpl::discover_volume_groups() {
    LvmDiscovery ld{m_lvm.get()};
    return ld.discovery();
}

void LvmAPI::LvmImpl::create_snapshot(const LvmCreateData& data) {
    auto vg_handle = open_volume_group(m_lvm.get(), data.get_volume_group());
    create_snapshot(open_logical_volume(vg_handle.get(),
                                          data.get_logical_volume()),
                      data.get_create_name(),
                      data.get_size(),
                      data.get_bootable());
    lvm_vg_write(vg_handle.get());
}

void LvmAPI::LvmImpl::create_clone(const LvmCreateData& data) {
    auto vg_handle = open_volume_group(m_lvm.get(), data.get_volume_group());
    if (data.get_size() <
                    lvm_lv_get_size(open_logical_volume(vg_handle.get(),
                                          data.get_logical_volume()))) {
    }
    create_clone(vg_handle.get(), data.get_create_name(), data.get_size(),
                data.get_bootable());
    //writing changes to VG just in case we set the bootable flag
    lvm_vg_write(vg_handle.get());
}

void LvmAPI::LvmImpl::remove_logical_volume(const std::string& vg_name,
                                            const std::string& lv_name) {
    auto vg_handle = open_volume_group(m_lvm.get(), vg_name);
    remove_lvm(open_logical_volume(vg_handle.get(), lv_name));
    save_lvm_changes(vg_handle.get());
}

/* Private functions */
VolumeGroupPtr
LvmAPI::LvmImpl::open_volume_group(lvm_t lvm_handle, const std::string& name) {
    auto vg_handle = lvm_vg_open(lvm_handle,
                name.c_str(),
                agent::storage::lvm::attribute::WRITE_MODE,
                agent::storage::lvm::attribute::FLAGS);
    if (!vg_handle) {
        THROW(agent_framework::exceptions::LvmError, "lvm",
          "Could not open volume group!");
    }
    return VolumeGroupPtr{vg_handle, lvm_vg_close};
}

lv_t LvmAPI::LvmImpl::open_logical_volume(vg_t vg_handle,
                                          const std::string& name) {
    auto lv_handle = lvm_lv_from_name(vg_handle,
                                      name.c_str());
    if (!lv_handle) {
        THROW(agent_framework::exceptions::LvmError, "lvm",
              "Could not open logical volume!");
    }

    return lv_handle;
}

lv_t LvmAPI::LvmImpl::create_snapshot(lv_t lv_handle,
                     const std::string& name, const std::uint64_t size,
                     const bool bootable) {
    auto snapshot_handle = lvm_lv_snapshot(lv_handle,
                                           name.c_str(),
                                           size);
    if (!snapshot_handle) {
        THROW(agent_framework::exceptions::LvmError, "lvm",
              "Could not create snapshot!");
    }
    if(bootable) {
        lvm_lv_add_tag(snapshot_handle, attribute::LV_BOOTABLE_ATTR);
    }

    return snapshot_handle;
}

lv_t LvmAPI::LvmImpl::create_clone(vg_t vg_handle, const std::string& name,
                                                   const std::uint64_t size,
                                                   const bool bootable) {
    auto clone_handle = lvm_vg_create_lv_linear(vg_handle, name.c_str(), size);
    if (!clone_handle) {
        THROW(agent_framework::exceptions::LvmError, "lvm",
              "Could not create clone!");
    }
    if (bootable) {
        lvm_lv_add_tag(clone_handle, attribute::LV_BOOTABLE_ATTR);
    }

    return clone_handle;
}

void LvmAPI::LvmImpl::remove_lvm(lv_t lvm) {
    auto ret = lvm_vg_remove_lv(lvm);
    if (ret) {
        THROW(agent_framework::exceptions::LvmError, "lvm",
              "Could not remove lvm!");
    }
}

void LvmAPI::LvmImpl::save_lvm_changes(vg_t vg_handle) {
    auto ret = lvm_vg_write(vg_handle);
    if (ret) {
        THROW(agent_framework::exceptions::LvmError, "lvm",
              "Could not save changes to volume group!");
    }
}

