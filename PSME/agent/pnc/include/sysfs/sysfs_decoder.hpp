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
 * @file sysfs/sysfs_decoder.hpp
 * @brief SysfsDecoder for decoding raw sysfs data obtained via SysfsReaderInterface
 * */

#pragma once

#include "sysfs/sysfs_reader_interface.hpp"
#include "sysfs/sysfs_dto.hpp"

#include <set>

namespace agent {
namespace pnc {
namespace sysfs {

/*!
 * @brief SysfsDecoder class for decoding raw sysfs data
 */
class SysfsDecoder {
public:

    SysfsDecoder() {}

    SysfsDecoder(const SysfsDecoder&) = default;
    SysfsDecoder& operator=(const SysfsDecoder&) = default;

    virtual ~SysfsDecoder();

    /*!
     * @brief Discovers PCI switches from the sysfs data
     * @param[in] reader Used SysfsReader
     * @return List of found switches
     */
    static SysfsDecoder make_instance(const SysfsReaderInterface& reader) {
        SysfsDecoder decoder = SysfsDecoder{};
        decoder.read_sysfs_data(reader);
        return decoder;
    }

    /*!
     * @brief Reads sysfs devices and drives using provided SysfsReaderInterface
     * @param[in] reader SysfsReader used as a data source
     */
    void read_sysfs_data(const SysfsReaderInterface& reader) {
        m_raw_data.clear();
        m_raw_data = reader.get_raw_sysfs_devices();
    }

    /*!
     * @brief Returns all switches that are found on the device list
     * @return List of all switches
     */
    std::vector<SysfsSwitch> get_switches() const {
        return decode_switches(m_raw_data);
    }

    /*!
     * @brief Returns bridge for a given switch, may throw invalid_argument if not found
     * @param[in] bridge_path Path to the bridge of the switch
     * @param[in] bridge_id Id of the queried bridge
     * @return Bridge object for a found bridge
     */
    SysfsBridge get_bridge_by_switch_path(const std::string bridge_path, uint8_t bridge_id) const;

    /*!
     * @brief Returns all bridges that are found on the switch
     * @param[in] sysfs_switch Parent switch for bridges
     * @return List of all bridges
     */
    std::vector<SysfsBridge> get_bridges(const SysfsSwitch& sysfs_switch) const {
        return decode_bridges_for_switch(sysfs_switch, m_raw_data);
    }

    /*!
     * @brief Returns all devices that are found on the device list for a given switch
     * @param[in] sysfs_bridge Parent bridge of devices
     * @return List of all devices
     */
    std::vector<SysfsDevice> get_devices(const SysfsBridge& sysfs_bridge) const {
        return decode_devices_for_bridge(sysfs_bridge, m_raw_data);
    }

    /*!
     * @brief Returns all functions that are found on the device list for a given device
     * @param[in] sysfs_device Parent device of the functions
     * @return List of all functions
     */
    std::vector<SysfsFunction> get_functions(const SysfsDevice& sysfs_device) const {
        return decode_functions_for_device(sysfs_device, m_raw_data);
    }

private:

    /* Read device information from its primary function data */
    static void decode_device_data(const RawSysfsDevice& primary_function, SysfsDevice& device);

    /* Returns true if dev_under_test is a known memory controller of a PCI switch */
    static bool is_known_switch_memory_controller(const RawSysfsDevice& dev_under_test);

    /* Returns true if dev_under_test is a PCI bridge for provided PCI Switch Memory Controller */
    static bool is_bridge_for_memory_controller(const RawSysfsDevice& memory, const RawSysfsDevice& dev_under_test);

    /* Returns true if dev_under_test is a PCI sub-bridge under a provided PCI switch bridge */
    static bool is_bridge_for_switch(const SysfsSwitch& pci_switch, const RawSysfsDevice& dev_under_test);

    /* Returns true if dev-under_test is a PCI function of the provided PCI device */
    static bool is_device_for_bridge(const SysfsBridge& bridge, const RawSysfsDevice& device_under_test);

    /* Returns true if dev-under_test is a PCI function of the provided PCI device */
    static bool is_function_for_device(const SysfsDevice& device, const RawSysfsDevice& device_under_test);

    /* Creates PCI Switch from raw memory and bridge controllers of the switch */
    static SysfsSwitch make_switch(const RawSysfsDevice& memory, const RawSysfsDevice& bridge);

    /* Creates PCI Bridge from a raw device */
    static SysfsBridge make_bridge(const RawSysfsDevice& bridge);

    /* Creates PCI Device from a raw device, it also requires its bridge's device number */
    static SysfsDevice make_device(const RawSysfsDevice& device);

    /* Creates PCI Function from a raw device */
    static SysfsFunction make_function(const RawSysfsDevice& function);

    /* Retrieves valid PCI switches from the device list, fills switches member vector */
    static std::vector<SysfsSwitch> decode_switches(const std::vector<RawSysfsDevice>& raw_devices);

    /* Retrieves list of PCI subbridges for a given PCI switch bridge */
    static std::vector<SysfsBridge> decode_bridges_for_switch(const SysfsSwitch& pci_switch,
        const std::vector<RawSysfsDevice>& raw_devices);

    /* Retrieves list of PCI devices for a given PCI switch bridge */
    static std::vector<SysfsDevice> decode_devices_for_bridge(const SysfsBridge& bridge,
        const std::vector<RawSysfsDevice>& raw_devices);

    /* Retrieves list of PCI functions for a given PCI device, updates device using its primary function data */
    static std::vector<SysfsFunction> decode_functions_for_device(const SysfsDevice& device,
        const std::vector<RawSysfsDevice>& raw_devices);

    /* Retrieves list of PCI drives for a given PCI function */
    static std::vector<SysfsDrive> decode_drives_for_function(const RawSysfsDevice& function);

    std::vector<RawSysfsDevice> m_raw_data{};
};

}
}
}
