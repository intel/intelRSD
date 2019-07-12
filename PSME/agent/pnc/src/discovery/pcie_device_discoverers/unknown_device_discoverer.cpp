/*!
 * @brief Implementation of discoverer for PCIe devices not recognized by PSME PNC agent
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
 * @file unknown_device_discoverer.cpp
 */

#include "discovery/device_discoverers/unknown_device_discoverer.hpp"



using namespace agent::pnc::discovery::device_discoverers;


bool
UnknownDeviceDiscoverer::oob_port_device_discovery(const gas::GlobalAddressSpaceRegisters& gas,
                                                   const Uuid& dsp_port_uuid) const {

    log_error("pnc-discovery", "Out-of-band device discovery failed or device is not a drive or processor");

    Uuid device_uuid{};

    discover_target_endpoint(gas, device_uuid, dsp_port_uuid);

    return true;
}


bool
UnknownDeviceDiscoverer::ib_port_device_discovery(const Uuid& /*switch_uuid*/,
                                                  const Uuid& /*dsp_port_uuid*/,
                                                  uint8_t /*bridge_id*/,
                                                  const Uuid& /*device_uuid*/) const {

    log_error("pnc-discovery", "In-band device discovery failed or device is not a drive or processor");

    return true;
}


Uuid
UnknownDeviceDiscoverer::discover_oob_add_and_stabilize(const Uuid&) const {
    // No device to add and stabilize, do nothing
    return Uuid{};
}


void UnknownDeviceDiscoverer::sync_device_properties_with_db(const Uuid& /*device_uuid*/) const {

}


void
UnknownDeviceDiscoverer::critical_state_device_discovery(const Uuid& /*device_uuid*/) const {

}


void
UnknownDeviceDiscoverer::set_functional_device(agent_framework::model::PcieFunction& /*pcie_function*/,
                                               const Uuid& /*device_uuid*/) const {

}


void
UnknownDeviceDiscoverer::sysfs_discovery(const Uuid& /*device_uuid*/,
                                         const agent::pnc::sysfs::SysfsDevice& /*sysfs_device*/) const {

}


Uuid
UnknownDeviceDiscoverer::add_from_sysfs(const Uuid& /*dsp_port_uuid*/,
                                        const std::string& /*serial_number*/) const {

    return Uuid{};
}