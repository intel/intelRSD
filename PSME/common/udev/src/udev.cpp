/*!
 * @brief Implementation of UDev class.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file udev.cpp
 */

#include "udev/udev.hpp"



using namespace net_udev;


const std::vector<std::string> UDev::get_interface_list(const std::string& driver_name,
                                                        const std::string& match_prefix) const {
    std::vector<std::string> list = get_driver_entries(driver_name);
    std::vector<std::string> result{};

    if (match_prefix.empty()) {
        return list;
    }

    /* Filter ports by prefix name */
    for (auto port : list) {
        if (match_prefix == port.substr(0, match_prefix.length())) {
            result.push_back(port);
        }
    }

    /* Return port list */
    return result;
}


const std::vector<std::string> UDev::get_driver_entries(const std::string& driver) const {
    std::vector<std::string> entry_list{};
    struct udev_list_entry* dev_entry = nullptr;
    static const constexpr char* SUBSYSTEM = "net";

    /* Create udev context */
    Udev ctx(udev_new());
    if (!ctx) {
        throw std::runtime_error("Failed to create UDev context [udev_new()].");
    }

    /* Create an enumeration context to scan /sys */
    UdevEnum udev_enum(udev_enumerate_new(ctx.get()));
    if (!udev_enum) {
        throw std::runtime_error("Failed to create UDev enumeration context [udev_enumerate_new()].");
    }

    /* Add match criteria */
    udev_enumerate_add_match_subsystem(udev_enum.get(), SUBSYSTEM);
    udev_enumerate_add_match_property(udev_enum.get(), "ID_NET_DRIVER", driver.c_str());

    /* Lookup network devices */
    if (0 != udev_enumerate_scan_devices(udev_enum.get())) {
        throw std::runtime_error("Could not lookup network interfaces [udev_enumerate_scan_devices()].");
    }

    /* Get entry list */
    dev_entry = udev_enumerate_get_list_entry(udev_enum.get());
    while (nullptr != dev_entry) {

        /* Get kernel sys device info */
        const char* path = udev_list_entry_get_name(dev_entry);
        UdevDevice dev(udev_device_new_from_syspath(ctx.get(), path));
        if (!dev) {
            throw std::runtime_error("Failed to initialize device [udev_device_new_from_syspath()]");
        }

        /* Add entry to the list */
        std::string name{udev_device_get_sysname(dev.get())};
        entry_list.push_back(name);

        /* Get next entry */
        dev_entry = udev_list_entry_get_next(dev_entry);
    }

    /* Return entry list */
    return entry_list;
}
