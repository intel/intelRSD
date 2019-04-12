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
 *
 * @file pnc.hpp
 * @brief Contains constants used by PNC agents
 * */

#pragma once

namespace agent_framework {
namespace model {
namespace literals {

/*!
 * @brief Class consisting of literals for Switch
 */
class Switch {
public:
    static constexpr const char SWITCH[] = "switch";
    static constexpr const char STATUS[] = "status";
    static constexpr const char SWITCH_ID[] = "switchId";
    static constexpr const char CHASSIS[] = "chassis";
    static constexpr const char ASSET_TAG[] = "assetTag";
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char SKU[] = "sku";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
    static constexpr const char INDICATOR_LED[] = "indicatorLED";
    static constexpr const char POWER_STATE[] = "powerState";
    static constexpr const char ALLOWED_ACTIONS[] = "allowedActions";
    static constexpr const char PROTOCOL[] = "protocol";
};

/*!
 * @brief Class consisting of literals for Fabric
 */
class Fabric {
public:
    static constexpr const char FABRIC[] = "fabric";
    static constexpr const char STATUS[] = "status";
    static constexpr const char PROTOCOL[] = "protocol";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for Port
 */
class Port {
public:
    static constexpr const char PORT[] = "port";
    static constexpr const char STATUS[] = "status";
    static constexpr const char PORT_ID[] = "portId";
    static constexpr const char PORT_TYPE[] = "portType";
    static constexpr const char CABLE_IDS[] = "cableIds";
    static constexpr const char SPEED_GBPS[] = "speedGBps";
    static constexpr const char WIDTH[] = "width";
    static constexpr const char MAX_SPEED_GBPS[] = "maxSpeedGBps";
    static constexpr const char MAX_WIDTH[] = "maxWidth";
    static constexpr const char OEM[] = "oem";
    static constexpr const char ALLOWED_ACTIONS[] = "allowedActions";
    static constexpr const char POWER_STATE[] = "powerState";
    static constexpr const char PROTOCOL[] = "protocol";
};

/*!
 * @brief Class consisting of literals for PcieDevice
 */
class PcieDevice {
public:
    static constexpr const char DEVICE[] = "device";
    static constexpr const char STATUS[] = "status";
    static constexpr const char DEVICE_ID[] = "deviceId";
    static constexpr const char DEVICE_CLASS[] = "deviceClass";
    static constexpr const char SPEED_GBPS[] = "speedGBps";
    static constexpr const char WIDTH[] = "width";
    static constexpr const char MAX_SPEED_GBPS[] = "maxSpeedGBps";
    static constexpr const char MAX_WIDTH[] = "maxWidth";
    static constexpr const char FIRMWARE_VERSION[] = "firmwareVersion";
    static constexpr const char CHASSIS[] = "chassis";
    static constexpr const char ASSET_TAG[] = "assetTag";
    static constexpr const char FRU_INFO[] = "fruInfo";
    static constexpr const char SKU[] = "sku";
    static constexpr const char COLLECTIONS[] = "collections";
    static constexpr const char OEM[] = "oem";
};

/*!
 * @brief Class consisting of literals for PcieFunction
 */
class PcieFunction {
public:
    static constexpr const char FUNCTION[] = "function";
    static constexpr const char STATUS[] = "status";
    static constexpr const char FUNCTION_ID[] = "functionId";
    static constexpr const char FUNCTION_TYPE[] = "functionType";
    static constexpr const char DEVICE_CLASS[] = "deviceClass";
    static constexpr const char PCI_DEVICE_ID[] = "pciDeviceId";
    static constexpr const char PCI_VENDOR_ID[] = "pciVendorId";
    static constexpr const char PCI_CLASS_CODE[] = "pciClassCode";
    static constexpr const char PCI_REVISION_ID[] = "pciRevisionId";
    static constexpr const char PCI_SUBSYSTEM_ID[] = "pciSubsystemId";
    static constexpr const char PCI_SUBSYSTEM_VENDOR_ID[] = "pciSubsystemVendorId";
    static constexpr const char FUNCTIONAL_DEVICE[] = "functionalDevice";
    static constexpr const char OEM[] = "oem";
};

}
}
}
