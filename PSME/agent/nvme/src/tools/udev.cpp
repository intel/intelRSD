/*!
 * @brief Implementation of UDev class.
 *
 * @copyright Copyright (c) 2015-2019 Intel Corporation
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

#include "tools/udev.hpp"

using namespace agent::nvme::tools;
using namespace std;

const vector<string> UDev::get_interface_list(const string& driver,
                                              const string& match_prefix) const {
    vector<string> list = get_driver_entries(driver);
    vector<string> ret{};

    if (match_prefix.empty()) {
        return list;
    }

    // filter ports by prefix name
    for (auto port : list) {
        if (match_prefix == port.substr(0, match_prefix.length())) {
            ret.push_back(port);
        }
    }
    /* return port list */
    return ret;
}

const vector<string> UDev::get_driver_entries(const string& driver) const {
    vector<string> entry_list{};
    struct udev_list_entry* dev_entry = nullptr;
    static const constexpr char* SUBSYSTEM = "net";

    /* create udev context */
    Udev ctx(udev_new());
    if (!ctx) {
        throw runtime_error("udev_new() error");
    }

    /* create an enumeration context to scan /sys */
    UdevEnum udev_enum(udev_enumerate_new(ctx.get()));
    if (!udev_enum) {
        throw runtime_error("udev_enumerate_new() error");
    }

    /* add match criteria */
    udev_enumerate_add_match_subsystem(udev_enum.get(), SUBSYSTEM);
    udev_enumerate_add_match_property(udev_enum.get(), "ID_NET_DRIVER", driver.c_str());

    /* lookup network devices */
    if (0 != udev_enumerate_scan_devices(udev_enum.get())) {
        throw runtime_error("udev_enumerate_add_match_subsystem() error");
    }

    /* get entry list */
    dev_entry = udev_enumerate_get_list_entry(udev_enum.get());
    while (nullptr != dev_entry) {

        /* get kernel sys device info */
        const char* path = udev_list_entry_get_name(dev_entry);
        UdevDevice dev(udev_device_new_from_syspath(ctx.get(), path));
        if (!dev) {
            throw runtime_error("udev_device_new_from_syspath() error");
        }

        /* add entry to the list */
        string name{udev_device_get_sysname(dev.get())};
        entry_list.push_back(name);

        /* get next entry */
        dev_entry = udev_list_entry_get_next(dev_entry);
    }

    /* return entry list */
    return entry_list;
}
