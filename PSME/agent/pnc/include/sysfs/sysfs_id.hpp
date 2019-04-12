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
 * @file sysfs/sysfs_id.hpp
 * @brief SysfsId representing pci identifiers for various devices
 * */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace agent {
namespace pnc {
namespace sysfs {

/*!
 * Class representing PCI device identifier as visible in sysfs
 */
struct SysfsId final {
    /*! Domain number, AAAA part of the pci bus device name AAAA:BB:CC.D */
    uint16_t domain_num{0x0000};
    /*! Bus number, BB part of the pci bus device name AAAA:BB:CC.D */
    uint8_t bus_num{0x00};
    /*! Device number, CC part of the pci bus device name AAAA:BB:CC.D */
    uint8_t device_num{0x00};
    /*! Function number, D part of the pci bus device name AAAA:BB:CC.D */
    uint8_t function_num{0x00};

    // constructors, destructors and assignement operators
    SysfsId() {}
    SysfsId(uint16_t domain, uint8_t bus, uint8_t device, uint8_t func):
        domain_num(domain), bus_num(bus), device_num(device), function_num(func) {
    }
    ~SysfsId() {}
    SysfsId(const SysfsId& rhs) = default;
    SysfsId(SysfsId&& rhs) = default;
    SysfsId& operator=(const SysfsId& rhs) = default;
    SysfsId& operator=(SysfsId&& rhs) = default;

    /*
     * @brief Opeartor comparing if two SysfsIds are the same
     * @param[in] SysfsIds to compare with
     * @return True, if all four numbers in both objects are the same
     */
    bool operator==(const SysfsId& rhs) const {
        return rhs.domain_num == domain_num && rhs.bus_num == bus_num
            && rhs.device_num == device_num && rhs.function_num == function_num;
    }

    /*
     * @brief Opeartor comparing if two SysfsIds are different
     * @param[in] SysfsIds to compare with
     * @return True, if any of the four numbers in both objects are different
     */
    bool operator!=(const SysfsId& rhs) const {
        return !(rhs == *this);
    }

    /*
     * @brief Compares specific fields of two different ids
     * @param[in] cmp_domain Should domain_num be included in the comparison
     * @param[in] cmp_bus Should bus_num be included in the comparison
     * @param[in] cmp_dev Should device_num be included in the comparison
     * @param[in] cmp_fnc Should function_num be included in the comparison
     * @return True, if all fields included in the comparison are the same for both objects
     */
    bool is_same(const SysfsId& rhs, bool cmp_domain, bool cmp_bus,
            bool cmp_dev, bool cmp_fnc) const {
        if ((cmp_domain && rhs.domain_num != domain_num)
            || (cmp_bus && rhs.bus_num != bus_num)
            || (cmp_dev && rhs.device_num != device_num)
            || (cmp_fnc && rhs.function_num != function_num)) {
            return false;
        }
        return true;
    }

    /*
     * @brief Retrieves all numbers from device name, throws invalid_parameter if string format is wrong
     * @param[in] str Device name as visible in sysfs in the format "xxxx:xx:xx.x"
     * @return SysfsId created from the string
     */
    static SysfsId from_string(const std::string& str);

    /*
     * @brief Converts SysfsId into its string representation
     * @return String in the format "xxxx:xx:xx.x"
     */
    std::string to_string() const;
};

}
}
}
