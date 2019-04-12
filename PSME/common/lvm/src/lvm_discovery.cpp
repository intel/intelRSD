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
 * @file lvm_discovery.cpp
 * @brief Lvm discovery implementation
 * */

#include "lvm/model/volume_group.hpp"
#include "lvm/model/logical_volume.hpp"
#include "lvm/model/physical_volume.hpp"
#include "lvm/model/discovery_type.hpp"
#include "lvm/lvm_attribute.hpp"

/* Private header */
#include "lvm_discovery.hpp"
#include "lvm.hpp"

/* Other */
#include "logger/logger_factory.hpp"
#include <memory>


using namespace storage::lvm;

LvmDiscovery::LvmDiscovery(struct lvm* lvm) : m_lvm{lvm} {}


namespace {

void throw_if_errno(lvm_t lvm_handle, const std::string& message) {
    auto error = ::lvm_errno(lvm_handle);
    if (error) {
        log_error("lvm", "LVM internal error occurred! Errno("
            << std::to_string(error) << ": " << ::lvm_errmsg(lvm_handle) << ")");
        throw std::runtime_error(message);
    }
}

struct PhysicalVolumeDeleter {
    void operator()(dm_list* pvs_list) {
        try {
            lvm_list_pvs_free(pvs_list);
            throw_if_errno(lvm_handle, "Could not deallocate PVs structures.");
        }
        catch (const std::exception& exception) {
            log_warning("lvm", exception.what());
        }
    }

    lvm_t lvm_handle{};
};

using VolumeGroupPtr = std::unique_ptr<::volume_group, decltype(&lvm_vg_close)>;
using PhysicalVolumeListPtr = std::unique_ptr<dm_list, PhysicalVolumeDeleter>;



dm_list* get_volume_group_list(lvm_t lvm_handle) {
    auto vg_names = ::lvm_list_vg_names(lvm_handle);
    if (!vg_names) {
        throw std::runtime_error("Could not open volume group list!");
    }
    return vg_names;
}


VolumeGroupPtr open_volume_group(::lvm* lvm_handle, const char* name) {
    auto vg_handle = ::lvm_vg_open(lvm_handle,
                                   name,
                                   storage::lvm::attribute::READ_MODE,
                                   storage::lvm::attribute::FLAGS);
    if (!vg_handle) {
        throw std::runtime_error("Could not open volume group!");
    }
    return VolumeGroupPtr{vg_handle, ::lvm_vg_close};
}


bool skip_logical_volume(logical_volume* handle) {
    lvm_property_value lv_prop = ::lvm_lv_get_property(handle, attribute::LV_ATTR_PROPERTY);
    // to avoid showing snapshots twice
    return lv_prop.value.string[attribute::LV_TYPE_ATTR] == attribute::LV_VIRTUAL_TYPE;
}


bool check_bootable_tag(logical_volume* handle) {
    dm_list* lv_tags = ::lvm_lv_get_tags(handle);
    lvm_str_list* strl = nullptr;
    dm_list_iterate_items(strl, lv_tags) {
        if (0 == strncmp(attribute::LV_BOOTABLE_ATTR, strl->str, strlen(attribute::LV_BOOTABLE_ATTR))) {
            return true;
        }
    }
    return false;
}

}


LvmDiscovery::VolumeGroups LvmDiscovery::discover(DiscoveryType discovery_type) {
    LvmDiscovery::VolumeGroups groups{};

    lvm_str_list* elem{nullptr};
    auto vg_names = ::get_volume_group_list(m_lvm);

    if (::dm_list_empty(vg_names)) {
        return groups;
    }

    dm_list_iterate_items(elem, vg_names) {
        auto vg = ::open_volume_group(m_lvm, elem->str);
        auto volume_group = make_volume_group(vg.get(), elem->str);

        if (discovery_type == DiscoveryType::VG_PV || discovery_type == DiscoveryType::VG_PV_LV) {
            if (::lvm_vg_get_pv_count(vg.get())) {
                discovery_physical_volumes(vg.get(), volume_group);
            }
        }
        if (discovery_type == DiscoveryType::VG_LV || discovery_type == DiscoveryType::VG_PV_LV) {
            discovery_logical_volumes(vg.get(), volume_group);
        }
        groups.emplace_back(volume_group);
    }
    return groups;
}


model::VolumeGroup LvmDiscovery::make_volume_group(volume_group* handle, const char* name) const {
    model::VolumeGroup volume_group{};
    volume_group.set_name(name);
    volume_group.set_capacity_b(::lvm_vg_get_size(handle));
    volume_group.set_free_b(::lvm_vg_get_free_size(handle));

    lvm_property_value vg_prop = ::lvm_vg_get_property(handle, attribute::VG_ATTR_PROPERTY);
    volume_group.set_protection_status(vg_prop.value.string[attribute::VG_RW_ATTR] != attribute::WRITE_MODE_ATTR);

    volume_group.set_health(attribute::HEALTH_OK_STATUS);
    volume_group.set_status(attribute::STATE_ENABLED);
    if (vg_prop.value.string[attribute::VG_STATUS_HEALTH_ATTR] == attribute::HEALTH_CRITICAL) {
        volume_group.set_health(attribute::HEALTH_CRITICAL_STATUS);
        volume_group.set_status(attribute::STATE_DISABLED);
    }

    return volume_group;
}


void LvmDiscovery::discovery_physical_volumes(volume_group* vg_handle, model::VolumeGroup& group) {
    lvm_pv_list* elem{nullptr};
    auto physical_volume_list = ::lvm_vg_list_pvs(vg_handle);
    if (!physical_volume_list) {
        throw std::runtime_error("Could not open physical volume list!");
    }
    dm_list_iterate_items(elem, physical_volume_list) {
        auto physical_volume = make_physical_volume(vg_handle, elem->pv);
        group.add_physical_volume(physical_volume);
    }
}


model::PhysicalVolume LvmDiscovery::make_physical_volume(volume_group* vg_handle, physical_volume* pv_handle) const {
    auto pv_name = ::lvm_pv_get_name(pv_handle);
    auto vg_name = ::lvm_vg_get_name(vg_handle);
    if (!pv_name || !vg_name) {
        throw std::runtime_error("Could not read physical volume properties.");
    }
    model::PhysicalVolume physical_volume;

    physical_volume.set_name(pv_name);
    physical_volume.set_capacity_b(::lvm_pv_get_size(pv_handle));
    physical_volume.set_free_b(::lvm_pv_get_free(pv_handle));
    physical_volume.set_health(attribute::HEALTH_OK_STATUS);
    physical_volume.set_status(attribute::STATE_ENABLED);

    lvm_property_value pv_prop = ::lvm_pv_get_property(pv_handle, attribute::PV_ATTR_PROPERTY);
    if (pv_prop.value.string[attribute::PV_STATUS_HEALTH_ATTR] == attribute::PV_STATUS_HEALTH_CRITICAL) {
        physical_volume.set_health(attribute::HEALTH_CRITICAL_STATUS);
        physical_volume.set_status(attribute::STATE_DISABLED);
    }

    physical_volume.set_volume_group(vg_name);

    return physical_volume;
}


void LvmDiscovery::discovery_logical_volumes(volume_group* vg_handle, model::VolumeGroup& group) {
    lvm_lv_list* lv_list{nullptr};

    auto lv_names = ::lvm_vg_list_lvs(vg_handle);
    if (!lv_names) {
        throw_if_errno(m_lvm, "Logical volume list is not accessible!");

        log_warning("lvm", "Logical volume list is empty!");
        return;
    }

    dm_list_iterate_items(lv_list, lv_names) {
        if (::skip_logical_volume(lv_list->lv)) {
            continue;
        }
        auto logical_volume = make_logical_volume(vg_handle, lv_list->lv);
        group.add_logical_volume(logical_volume);
    }
}


model::LogicalVolume LvmDiscovery::make_logical_volume(volume_group* vg_handle, logical_volume* handle) const {
    auto lv_name = ::lvm_lv_get_name(handle);
    auto master_name = ::lvm_lv_get_origin(handle);
    auto vg_name = ::lvm_vg_get_name(vg_handle);
    if (!lv_name || !vg_name) {
        throw std::runtime_error("Could not read logical volume properties.");
    }

    lvm_property_value lv_prop = ::lvm_lv_get_property(handle, attribute::LV_ATTR_PROPERTY);
    model::LogicalVolume logical_volume{};
    logical_volume.set_capacity_b(::lvm_lv_get_size(handle));
    logical_volume.set_name(lv_name);
    logical_volume.set_protection_status(lv_prop.value.string[attribute::LV_RW_ATTR] != attribute::WRITE_MODE_ATTR);
    logical_volume.set_health(lv_prop.value.string[attribute::LV_HEALTH_ATTR] == attribute::HEALTH_CRITICAL ?
                              attribute::HEALTH_CRITICAL_STATUS : attribute::HEALTH_OK_STATUS);
    logical_volume.set_status(lv_prop.value.string[attribute::LV_STATUS_ATTR] == attribute::STATE_ACTIVE ?
                              attribute::STATE_ENABLED : attribute::STATE_DISABLED);
    if (lv_prop.value.string[attribute::LV_TYPE_ATTR] == attribute::LV_SNAPSHOT_TYPE) {
        logical_volume.set_snapshot(true);
        logical_volume.set_master_name(master_name);
    }
    else {
        logical_volume.set_snapshot(false);
    }

    check_bootable_tag(handle) ? logical_volume.set_bootable(true) : logical_volume.set_bootable(false);

    logical_volume.set_volume_group(vg_name);
    return logical_volume;
}
