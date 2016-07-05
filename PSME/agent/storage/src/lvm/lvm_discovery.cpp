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
 * @file lvm_discovery.cpp
 *
 * @brief Lvm discovery implementation
 * */

#include "lvm/lvm_discovery.hpp"
#include "lvm/volume_group.hpp"
#include "lvm/logical_volume.hpp"
#include "lvm/physical_volume.hpp"
#include "lvm/lvm_attribute.hpp"
#include "agent-framework/exceptions/exception.hpp"

#include <memory>
#include <lvm2app.h>

using namespace agent::storage::lvm;

using VolumeGroupPtr =
    std::unique_ptr<::volume_group, decltype(&lvm_vg_close)>;

LvmDiscovery::LvmDiscovery(struct lvm* lvm) : m_lvm{lvm} {}

namespace {
    dm_list* get_volume_group_list(lvm_t lvm_handle) {
        auto vgnames = lvm_list_vg_names(lvm_handle);
        if (!vgnames) {
            THROW(agent_framework::exceptions::LvmError, "lvm",
              "Could not open volume group list!");
        }
        return vgnames;
    }

    VolumeGroupPtr
    open_volume_group(::lvm* lvm_handle, const char* name) {
        auto vg_handle = ::lvm_vg_open(lvm_handle,
                    name,
                    agent::storage::lvm::attribute::READ_MODE,
                    agent::storage::lvm::attribute::FLAGS);
        if (!vg_handle) {
            THROW(agent_framework::exceptions::LvmError, "lvm",
              "Could not open volume group!");
        }
        return VolumeGroupPtr{vg_handle, lvm_vg_close};
    }

    void log_volume_group(const VolumeGroup& volume_group) {
        log_debug(GET_LOGGER("lvm"),
                "Found volume group: " << volume_group.get_name());
        log_debug(GET_LOGGER("lvm"),
                "Capacity: " << volume_group.get_capacity_gb());
        log_debug(GET_LOGGER("lvm"),
                "Is protected: " << volume_group.get_protection_status());
        log_debug(GET_LOGGER("lvm"), "Health: " << volume_group.get_health());
        log_debug(GET_LOGGER("lvm"), "Status: " << volume_group.get_status());
    }
}

LvmDiscovery::VolumeGroupVec LvmDiscovery::discovery() {
    LvmDiscovery::VolumeGroupVec groups{};

    lvm_str_list* elem{nullptr};
    auto vg_names = ::get_volume_group_list(m_lvm);

    if(dm_list_empty(vg_names)) {
        return groups;
    }

    dm_list_iterate_items(elem, vg_names) {
        auto vg = ::open_volume_group(m_lvm, elem->str);
        auto volume_group = make_volume_group(vg.get(), elem->str);

        if (lvm_vg_get_pv_count(vg.get())) {
            discovery_physical_volumes(vg.get(), volume_group);
        }

        discovery_logical_volumes(vg.get(), volume_group);

        log_volume_group(volume_group);

        groups.emplace_back(volume_group);
    }
    return groups;
}

VolumeGroup LvmDiscovery::make_volume_group(volume_group* handle,
                                            const char* name) const {
    VolumeGroup volume_group;
    volume_group.set_name(name);
    volume_group.set_capacity_gb(
        double(::lvm_vg_get_size(handle)) * attribute::B_TO_GB);
    lvm_property_value vg_prop =
        lvm_vg_get_property(handle, attribute::VG_ATTR_PROPERTY);
    volume_group.set_protection_status(
            vg_prop.value.string[attribute::VG_RW_ATTR] != attribute::WRITE_MODE_ATTR);
    volume_group.set_health(attribute::HEALTH_OK_STATUS);
    volume_group.set_status(attribute::STATE_ENABLED);
    if (vg_prop.value.string[attribute::VG_STATUS_HEALTH_ATTR] ==
                                                attribute::HEALTH_CRITICAL) {
        volume_group.set_health(attribute::HEALTH_CRITICAL_STATUS);
        volume_group.set_status(attribute::STATE_DISABLED);
    }

    return volume_group;
}

using PhysicalVolumeListPtr =
                    std::unique_ptr<dm_list, decltype(&lvm_list_pvs_free)>;
namespace {
    PhysicalVolumeListPtr get_physical_volume_list(::volume_group* group) {
        auto pvnames = lvm_vg_list_pvs(group);
        if (!pvnames) {
            THROW(agent_framework::exceptions::LvmError, "lvm",
                    "Could not open physical volume list!");
        }
        return PhysicalVolumeListPtr{pvnames, lvm_list_pvs_free};
    }

    void log_physical_volume(const PhysicalVolume& physical_volume) {
        log_debug(GET_LOGGER("lvm"),
                "Found physical volume " << physical_volume.get_name());
        log_debug(GET_LOGGER("lvm"),
                "Capacity: " << physical_volume.get_capacity_gb());
        log_debug(GET_LOGGER("lvm"),
                "Is protected: " << physical_volume.get_protection_status());
        log_debug(GET_LOGGER("lvm"),
                "Health: " << physical_volume.get_health());
        log_debug(GET_LOGGER("lvm"),
                "Status: " << physical_volume.get_status());
        log_debug(GET_LOGGER("lvm"),
                "Volume group: " << physical_volume.get_volume_group());
    }
}

void LvmDiscovery::discovery_physical_volumes(volume_group* vg_handle,
                                              VolumeGroup& group) {
    lvm_pv_list* elem{nullptr};
    auto physical_volume_list = ::get_physical_volume_list(vg_handle);
    dm_list_iterate_items(elem, physical_volume_list.get()) {
        auto physical_volume = make_physical_volume(vg_handle, elem->pv);
        log_physical_volume(physical_volume);
        group.add_physical_volume(physical_volume);
    }
}

PhysicalVolume
LvmDiscovery::make_physical_volume(volume_group* vg_handle,
                                   physical_volume* handle) const {
    PhysicalVolume physical_volume;

    physical_volume.set_name(::lvm_pv_get_name(handle));
    physical_volume.set_capacity_gb(
            double(lvm_pv_get_size(handle)) * attribute::B_TO_GB);
    physical_volume.set_health(attribute::HEALTH_OK_STATUS);
    physical_volume.set_status(attribute::STATE_ENABLED);

    lvm_property_value pv_prop = lvm_pv_get_property(handle,
                                                attribute::PV_ATTR_PROPERTY);
    if (pv_prop.value.string[attribute::PV_STATUS_HEALTH_ATTR] ==
                                    attribute::PV_STATUS_HEALTH_CRITICAL) {
        physical_volume.set_health(attribute::HEALTH_CRITICAL_STATUS);
        physical_volume.set_status(attribute::STATE_DISABLED);
    }

    physical_volume.set_volume_group(lvm_vg_get_name(vg_handle));

    return physical_volume;
}

namespace {
    dm_list* get_logical_volumes_list(vg_t vg_handle) {
        auto lv_names = lvm_vg_list_lvs(vg_handle);
        if (!lv_names) {
            /* TODO: use lvm_errno() to check if this is en error */
            log_warning(GET_LOGGER("lvm"),
                    "Could not open logical volume list!");
        }
        return lv_names;
    }

    void log_logical_volume(const LogicalVolume& logical_volume) {
        log_debug(GET_LOGGER("lvm"),
                "Found logical volume: " << logical_volume.get_name());
        log_debug(GET_LOGGER("lvm"),
                "Capacity: " << logical_volume.get_capacity_gb());
        log_debug(GET_LOGGER("lvm"),
                "Is protected: " << logical_volume.get_protection_status());
        log_debug(GET_LOGGER("lvm"),
                "Health: " << logical_volume.get_health());
        log_debug(GET_LOGGER("lvm"),
                "Status: " << logical_volume.get_status());
        log_debug(GET_LOGGER("lvm"),
                "Snapshot: " << logical_volume.is_snapshot());
        if (logical_volume.is_snapshot()) {
            log_debug(GET_LOGGER("lvm"),
                    "Master LV name: " << logical_volume.get_master_name());
        }
        log_debug(GET_LOGGER("lvm"),
                "Volume group: " << logical_volume.get_volume_group());
    }

    bool skip_logical_volume(logical_volume* handle) {
        lvm_property_value lv_prop = lvm_lv_get_property(handle,
                                        attribute::LV_ATTR_PROPERTY);
        //to avoid showing snapshots twice
        return lv_prop.value.string[attribute::LV_TYPE_ATTR] ==
               attribute::LV_VIRTUAL_TYPE;
    }

    bool check_bootable_tag(logical_volume* handle){
        dm_list* lv_tags = lvm_lv_get_tags(handle);
        lvm_str_list *strl = nullptr;
        dm_list_iterate_items(strl, lv_tags) {
            if (0 == strncmp(attribute::LV_BOOTABLE_ATTR, strl->str,
                    strlen(attribute::LV_BOOTABLE_ATTR))) {
                return true;
            }
        }
        return false;
    }
}

void LvmDiscovery::discovery_logical_volumes(volume_group* vg_handle,
                                             VolumeGroup& group) {
    lvm_lv_list* lv_list{nullptr};
    auto lv_names = ::get_logical_volumes_list(vg_handle);
    dm_list_iterate_items(lv_list, lv_names) {
        if (skip_logical_volume(lv_list->lv)) {
            continue;
        }
        auto logical_volume = make_logical_volume(vg_handle, lv_list->lv);
        log_logical_volume(logical_volume);
        group.add_logical_volume(logical_volume);
    }
}

LogicalVolume LvmDiscovery::make_logical_volume(volume_group* vg_handle,
                                                logical_volume* handle) const {

    auto lv_name = lvm_lv_get_name(handle);
    auto master_name = lvm_lv_get_origin(handle);
    auto vg_name = ::lvm_vg_get_name(vg_handle);
    if (!lv_name || !vg_name) {
        THROW(agent_framework::exceptions::LvmError, "lvm",
            "Could not read logical volume properties!");
    }

    lvm_property_value lv_prop = ::lvm_lv_get_property(handle,
                                            attribute::LV_ATTR_PROPERTY);
    LogicalVolume logical_volume;
    logical_volume.set_capacity_gb(
            double(lvm_lv_get_size(handle)) * attribute::B_TO_GB);
    logical_volume.set_name(lv_name);
    logical_volume.set_protection_status(
            lv_prop.value.string[attribute::LV_RW_ATTR] != attribute::WRITE_MODE_ATTR);
    logical_volume.set_health(
            lv_prop.value.string[attribute::LV_HEALTH_ATTR] ==
                                attribute::HEALTH_CRITICAL ?
                                attribute::HEALTH_CRITICAL_STATUS :
                                attribute::HEALTH_OK_STATUS);
    logical_volume.set_status(
            lv_prop.value.string[attribute::LV_STATUS_ATTR] ==
                                            attribute::STATE_ACTIVE ?
                                            attribute::STATE_ENABLED :
                                            attribute::STATE_DISABLED);
    if (lv_prop.value.string[attribute::LV_TYPE_ATTR] == attribute::LV_SNAPSHOT_TYPE){
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
