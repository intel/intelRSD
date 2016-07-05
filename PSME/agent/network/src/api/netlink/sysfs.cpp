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
 * @file sysfs.cpp
 *
 * @brief Get switch port attribute via sysfs.
 * */

#include "api/netlink/sysfs.hpp"
#include <iostream>

using namespace agent::network::api::netlink;

SysFs::SysFs(const std::string& name) {
    /* create sw port attribute map */
    attrs[SwPortAttribute::SW_PORT_ATTR_MAX_FRAME_SIZE] =
        "switch/max_frame_size";
    attrs[SwPortAttribute::SW_PORT_ATTR_AUTONEG] =
        "switch/autoneg";
    ifname = name;
}

SysFs::~SysFs() { }

uint32_t SysFs::get_autoneg() const {
    return uint32_t(get_switch_port_attribute_int(
                    SwPortAttribute::SW_PORT_ATTR_AUTONEG));
}

uint32_t SysFs::get_max_frame_size() const {
    return uint32_t(get_switch_port_attribute_int(
                    SwPortAttribute::SW_PORT_ATTR_MAX_FRAME_SIZE));
}

int SysFs::get_switch_port_attribute_int(SwPortAttribute attr) const {
    int rval{0};

    /* get switch attribute value */
    std::string value = get_switch_port_attribute(attr);
    try {
        rval = std::stoi(std::string(value));
    } catch (const std::logic_error& ia) {
        throw std::runtime_error("fail to convert value: " + std::string(ia.what()));
    }
    return rval;
}

std::string SysFs::get_switch_port_attribute(SwPortAttribute attr) const {
    struct udev_list_entry* first_entry = nullptr;

    /* check if interface name is set */
    if (ifname.empty()) {
        throw std::runtime_error("fail to get port attribute for unknown port");
    }

    /* create udev context */
    Udev ctx(udev_new());
    if (!ctx) {
        throw std::runtime_error("udev_new() error");
    }

    /* create an enumeration context to scan /sys */
    UdevEnum udev_enum(udev_enumerate_new(ctx.get()));
    if (!udev_enum) {
        throw std::runtime_error("udev_enumerate_new() error");
    }

    /* add match criteria */
    int rc = udev_enumerate_add_match_subsystem(udev_enum.get(), SUBSYSTEM);
    rc += udev_enumerate_add_match_property(udev_enum.get(), "ID_NET_DRIVER", DRIVER);
    rc += udev_enumerate_add_match_property(udev_enum.get(), "ID_NET_DRIVER", TEAM);
    rc += udev_enumerate_add_match_sysname(udev_enum.get(), ifname.c_str());
    if (0 != rc) {
        throw std::runtime_error("udev_enumerate_add_match_*() error");
    }

    /* lookup network devices */
    if (0 != udev_enumerate_scan_devices(udev_enum.get())) {
        throw std::runtime_error("udev_enumerate_add_match_subsystem() error");
    }
    first_entry = udev_enumerate_get_list_entry(udev_enum.get());
    if (nullptr == first_entry) {
        throw std::runtime_error(ifname + " interface doesn't exist");
    }

    /* get kernel sys device info */
    const char* path = udev_list_entry_get_name(first_entry);
    UdevDevice dev(udev_device_new_from_syspath(ctx.get(), path));
    if (!dev) {
        throw std::runtime_error("udev_device_new_from_syspath() error");
    }
    const char* val = udev_device_get_sysattr_value(dev.get(), attrs.at(attr));
    if (nullptr == val) {
        throw std::runtime_error("failed to get value of sys attribute ["
                                 + std::string(attrs.at(attr)) + "]");
    }
    return val;
}

uint32_t SysFs::get_port_count() const {
    uint32_t count{0};
    struct udev_list_entry* dev_entry = nullptr;

    /* create udev context */
    Udev ctx(udev_new());
    if (!ctx) {
        throw std::runtime_error("udev_new() error");
    }

    /* create an enumeration context to scan /sys */
    UdevEnum udev_enum(udev_enumerate_new(ctx.get()));
    if (!udev_enum) {
        throw std::runtime_error("udev_enumerate_new() error");
    }

    /* add match criteria */
    int rc = udev_enumerate_add_match_subsystem(udev_enum.get(), SUBSYSTEM);
    rc += udev_enumerate_add_match_property(udev_enum.get(), "ID_NET_DRIVER", DRIVER);
    rc += udev_enumerate_add_match_property(udev_enum.get(), "ID_NET_DRIVER", TEAM);
    if (0 != rc) {
        throw std::runtime_error("udev_enumerate_add_match_*() error");
    }

    /* lookup network devices */
    if (0 != udev_enumerate_scan_devices(udev_enum.get())) {
        throw std::runtime_error("udev_enumerate_add_match_subsystem() error");
    }
    dev_entry = udev_enumerate_get_list_entry(udev_enum.get());
    if (nullptr == dev_entry) {
        throw std::runtime_error("interface doesn't exist");
    }

    /* get port count */
    while (nullptr != dev_entry) {
        dev_entry = udev_list_entry_get_next(dev_entry);
        count++;
    }

    /* return port count */
    return ((0 < count) ? (count - 1) : 0);
}

const std::vector<std::string> SysFs::get_port_list() const {
    std::vector<std::string> list = get_driver_entries(DRIVER);
    /* remove CPU port form the port list */
    for (auto port_iter = list.begin(); port_iter != list.end(); ++port_iter) {
        if (CPU_PORT == (*port_iter)) {
            list.erase(port_iter);
            break;
        }
    }
    /* return port list */
    return list;
}

const std::vector<std::string> SysFs::get_team_list() const {
    std::vector<std::string> list = get_driver_entries(TEAM);
    return list;
}

const std::vector<std::string>
SysFs::get_driver_entries(const std::string& driver) const {
    std::vector<std::string> entry_list{};
    struct udev_list_entry* dev_entry = nullptr;

    /* create udev context */
    Udev ctx(udev_new());
    if (!ctx) {
        throw std::runtime_error("udev_new() error");
    }

    /* create an enumeration context to scan /sys */
    UdevEnum udev_enum(udev_enumerate_new(ctx.get()));
    if (!udev_enum) {
        throw std::runtime_error("udev_enumerate_new() error");
    }

    /* add match criteria */
    udev_enumerate_add_match_subsystem(udev_enum.get(), SUBSYSTEM);
    udev_enumerate_add_match_property(udev_enum.get(), "ID_NET_DRIVER", driver.c_str());

    /* lookup network devices */
    if (0 != udev_enumerate_scan_devices(udev_enum.get())) {
        throw std::runtime_error("udev_enumerate_add_match_subsystem() error");
    }

    /* get entry list */
    dev_entry = udev_enumerate_get_list_entry(udev_enum.get());
    while (nullptr != dev_entry) {

        /* get kernel sys device info */
        const char* path = udev_list_entry_get_name(dev_entry);
        UdevDevice dev(udev_device_new_from_syspath(ctx.get(), path));
        if (!dev) {
            throw std::runtime_error("udev_device_new_from_syspath() error");
        }

        /* add entry to the list */
        std::string name{udev_device_get_sysname(dev.get())};
        entry_list.push_back(name);

        /* get next entry */
        dev_entry = udev_list_entry_get_next(dev_entry);
    }

    /* return entry list */
    return entry_list;
}
