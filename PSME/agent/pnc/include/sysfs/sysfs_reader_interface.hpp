/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file sysfs/sysfs_reader_interface.hpp
 * @brief Generic interface providing capabilities of reading raw sysfs device data
 * */

#pragma once

#include <vector>

#include "sysfs/raw_sysfs_device.hpp"

namespace agent {
namespace pnc {
namespace sysfs {

/*!
 * SysfsReaderInterface describing interface of a generic sysfs reader
 */
class SysfsReaderInterface {
public:

    SysfsReaderInterface() = default;
    SysfsReaderInterface(const SysfsReaderInterface&) = default;
    SysfsReaderInterface& operator=(const SysfsReaderInterface&) = default;

    virtual ~SysfsReaderInterface();

    /*!
     * @brief Reads list of all pci devices in the sysfs
     * @param[in] path String describing parents of the devices e.g. "0000:00:00.0/0000:01:00.0/"
     *                 Path should consist of series of devices formatted as 'xxxx:xx:xx.x/'
     * @return List of all PCI devices found in the system
     */
    virtual std::vector<RawSysfsDevice> get_raw_sysfs_devices(const std::string& path = "") const = 0;

};

}
}
}
