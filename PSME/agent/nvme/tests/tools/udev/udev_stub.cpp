/*!
 * @brief Implementation of UDev stubs
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file udev_stub.cpp
 */

#include <libudev.h>

struct udev* udev_unref(struct udev*) {
    return nullptr;
}

struct udev* udev_new () {
    static struct udev* ret{nullptr};
    static int u{};
    if (!ret) {
        ret = (struct udev*)&u;
        return nullptr;
    }
    return ret;
}

struct udev_device* udev_device_unref (struct udev_device*) {
    return nullptr;
}

struct udev_enumerate* udev_enumerate_unref(struct udev_enumerate*) {
    return nullptr;
}

struct udev_enumerate* udev_enumerate_new(struct udev*) {
    static struct udev_enumerate* ret{nullptr};
    static int u{};
    if (!ret) {
        ret = (struct udev_enumerate*)&u;
        return nullptr;
    }
    return ret;
}

int udev_enumerate_add_match_subsystem(struct udev_enumerate*, const char*) {
    return 0;
}

int udev_enumerate_add_match_property(struct udev_enumerate*, const char*, const char*) {
    return 0;
}

int udev_enumerate_scan_devices(struct udev_enumerate*) {
    static int ret{1};
    if (ret) {
        ret = 0;
        return 1;
    }
    return ret;
}

struct udev_list_entry* udev_enumerate_get_list_entry(struct udev_enumerate*) {
    static int i{};
    return (struct udev_list_entry*) &i;
}

const char* udev_list_entry_get_name(struct udev_list_entry*) {
    return nullptr;
}

struct udev_device* udev_device_new_from_syspath(struct udev*, const char*) {
    static struct udev_device* ret{nullptr};
    static int u{};
    if (!ret) {
        ret = (struct udev_device*)&u;
        return nullptr;
    }
    return ret;
}

const char* udev_device_get_sysname(struct udev_device*) {
    return "eth0";
}

struct udev_list_entry* udev_list_entry_get_next(struct udev_list_entry*) {
    return nullptr;
}
